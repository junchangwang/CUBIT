#include "fastbit/bitvector.h"
#include <boost/filesystem.hpp>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <thread>

#include "naive/table.h"
#include "nicolas/util.h"

#ifdef LINUX
#include <omp.h>
#endif

using namespace naive;
using namespace std;

Table::Table(Table_config *config) : BaseTable(config), number_of_rows(config->n_rows)
{
    if (config->encoding == EE)
        num_bitmaps = config->g_cardinality;
    else if (config->encoding == RE)
        num_bitmaps = config->g_cardinality - 1;
    else {
        cout << "Bitmap In-place only spports the EE and RE encoding schemes" << endl;
        assert(0);
    }
    bitmaps.reserve(num_bitmaps);

    int n_threads = (config->nThreads_for_getval > num_bitmaps) ? num_bitmaps : config->nThreads_for_getval;
    int n_btv_per_thread = num_bitmaps / n_threads;
    int n_left = num_bitmaps % n_threads;
    thread* threads = new thread[n_threads];

    assert(n_btv_per_thread >= 1);
    
    vector<int>begin (n_threads + 1, 0); 
    for(int i = 1; i <= n_left; i++)
        begin[i] = begin[i - 1] + n_btv_per_thread + 1;
    for(int i = n_left + 1; i <= n_threads; i++)
        begin[i] = begin[i - 1] + n_btv_per_thread;

    for (int i = 0; i < n_threads; i++) {
        threads[i] = thread(&Table::_read_btv, this, begin[i], begin[i + 1]);        
    }     
    for (int t = 0; t < n_threads; t++) {
        threads[t].join();
    }

    delete[] threads;
}

void Table::_read_btv(int begin, int end) {
    for (int i = begin; i < end; i++) {      
        bitmaps[i] = new ibis::bitvector();
        if (config->INDEX_PATH != "")
            bitmaps[i]->read(getBitmapName(i).c_str());
        if (config->enable_fence_pointer) {
            bitmaps[i]->index.clear();
            bitmaps[i]->buildIndex();
        }
    }
}

int Table::append(int tid, int val) 
{
    lock_guard<shared_mutex> guard(g_lock);

    if (config->on_disk) { }
    else {
        if (config->encoding == EE) {
            bitmaps[val]->setBit(number_of_rows, 1, config);
            if (config->enable_fence_pointer) {
                bitmaps[val]->index.clear();
                bitmaps[val]->buildIndex();
            }

            number_of_rows += 1;
            
            for (int i = 0; i < num_bitmaps; i++) {
                bitmaps[i]->adjustSize(0, number_of_rows);
            }
        }
        else if (config->encoding == RE) {
            for (int idx = val; idx < num_bitmaps; idx++) {
                bitmaps[idx]->setBit(number_of_rows, 1, config);
                if (config->enable_fence_pointer) {
                    bitmaps[idx]->index.clear();
                    bitmaps[idx]->buildIndex();
                }
            }

            number_of_rows += 1;
            
            for (int idx = 0; idx < val; idx++) {
                bitmaps[idx]->adjustSize(0, number_of_rows);
            }
        }
        else {
            assert(0);
        }
    }

    return 0;
}

int Table::update(int tid, uint64_t rowid, int to_val) 
{
    lock_guard<shared_mutex> guard(g_lock);

    int from_val = get_value(rowid);
    #if defined(VERIFY_RESULTS)
    cout << "Updating row " << rowid << " old: " << from_val << " to: " << to_val << endl;
    #endif
    if ((from_val == to_val) || (from_val == -1))
        return -ENOENT;

    if (config->on_disk) {
        ibis::bitvector bitvector1, bitvector2;

        bitvector1.read(getBitmapName(from_val).c_str());
        bitvector2.read(getBitmapName(to_val).c_str());
        // if (verbose)
        //    cout << "U READ " << time_diff(before , after) << endl;

        bitvector1.setBit(rowid, 0, config);
        bitvector2.setBit(rowid, 1, config);
        // if (verbose)
        //     cout << "U SETBIT " << time_diff(before , after) << endl;

        bitvector1.write(getBitmapName(from_val).c_str());
        bitvector2.write(getBitmapName(to_val).c_str());
        // if (verbose)
        //     cout << "U WRITE " << time_diff(before , after) << endl;
    }
    else {
        if (config->encoding == EE) 
        {
            #if defined(VERIFY_RESULTS)
            assert(bitmaps[from_val]->getBit(rowid, config) == 1);
            assert(bitmaps[to_val]->getBit(rowid, config) == 0);
            #endif

            bitmaps[from_val]->setBit(rowid, 0, config);
            if (config->enable_fence_pointer) {
                bitmaps[from_val]->index.clear();
                bitmaps[from_val]->buildIndex();
            }

            bitmaps[to_val]->setBit(rowid, 1, config);
            if (config->enable_fence_pointer) {
                bitmaps[to_val]->index.clear();
                bitmaps[to_val]->buildIndex();
            }

            #if defined(VERIFY_RESULTS)
            assert(bitmaps[from_val]->getBit(rowid, config) == 0);
            assert(bitmaps[to_val]->getBit(rowid, config) == 1);
            assert(get_value(rowid) == to_val);
            #endif
        }
        else if (config->encoding == RE) {
            int min, max;
            if (to_val > from_val) {
                min = from_val;
                max = to_val - 1;
            }
            else {
                min = to_val;
                max = from_val - 1;
            }
            for (int idx = min; idx <= max; idx++) {
                bitmaps[idx]->setBit(rowid, !bitmaps[idx]->getBit(rowid, config), config);
                if (config->enable_fence_pointer) {
                    bitmaps[idx]->index.clear();
                    bitmaps[idx]->buildIndex();
                }
            }
        }
        else {
            assert(0);
        }

        // if (verbose)
        //     cout << "U SETBIT " << time_diff(before , after) << endl;
    }

    return 0;
}

int Table::remove(int tid, uint64_t rowid) 
{
    lock_guard<shared_mutex> guard(g_lock);

    auto val = get_value(rowid);
    if (val == -1)
        return -ENOENT;

    if (config->on_disk) {
        ibis::bitvector bitvector;
        bitvector.read(getBitmapName(val).c_str());
        bitvector.setBit(rowid, 0, config);
        bitvector.write(getBitmapName(val).c_str());
    }
    else {
        if (config->encoding == EE) {
            bitmaps[val]->setBit(rowid, 0, config);
            if (config->enable_fence_pointer) {
                bitmaps[val]->index.clear();
                bitmaps[val]->buildIndex();
            }
        }
        else if (config->encoding == RE) {
            for (int idx = val; idx < num_bitmaps; idx++) {
                bitmaps[idx]->setBit(rowid, 0, config);
                if (config->enable_fence_pointer) {
                    bitmaps[idx]->index.clear();
                    bitmaps[idx]->buildIndex();
                }
            }
        }
        else {
            assert(0);
        }

        #if defined(VERIFY_RESULTS)
        assert(bitmaps[val]->getBit(rowid, config) == 0);
        assert(get_value(rowid) == -1);
        #endif
    }

    return 0;
}

int Table::evaluate(int tid, uint32_t val) 
{
    ibis::bitvector res;
    {
        shared_lock<shared_mutex> guard(g_lock);

        if (config->on_disk) {
            string name = getBitmapName(val);
            res.read(name.c_str());
            // if (verbose)
            //     cout << "Q READ " << time_diff(before , after) << endl;
        } else {
            res.copy(*bitmaps[val]);
            // if (config->enable_fence_pointer) {
            //     res.index.clear();
            //     res.buildIndex();
            // }
            // if (verbose)
            //     cout << "Q COPY " << time_diff(before , after) << endl;
        }
    }
    auto cnt = res.do_cnt();
    // if (verbose)
    //     cout << "Q DEC " << time_diff(before , after) << endl;
    return cnt;
}

void Table::_get_value(uint64_t rowid, int begin, int range, bool *flag, int *result)
{
    int ret = -1;

    for(int i = 0; i < range; i++) {
        int curVal = begin + i;

        if (config->encoding == EE) {
            if (__atomic_load_n(flag, MM_CST))
                break;
        }
        else if (config->encoding == RE) {
            ;
        }

        auto bit1 = bitmaps[curVal]->getBit(rowid, config);
        if (bit1 == 1) {
            if (config->encoding == EE) {
                ret = curVal;
                __atomic_store_n(flag, true, MM_CST);
                break;
            }
            else if (config->encoding == RE) {
                if (ret == -1) {
                    ret = curVal;
                } else {
                    ret = (curVal < ret) ? curVal : ret;
                }
            }
        }
    }

    __atomic_store_n(result, ret, MM_RELEASE);
}

int Table::get_value(uint64_t rowid) {
    bool flag = false;
    int n_threads = (config->nThreads_for_getval > num_bitmaps) ? num_bitmaps : config->nThreads_for_getval;
    int begin = 0;
    int offset = num_bitmaps / n_threads;
    thread * getval_threads = new thread[n_threads];
    int * local_results = new int[n_threads];
    fill_n(local_results, n_threads, -2);

    assert(offset >= 1);

    for (int i = 0; i < n_threads; i++) {
        int begin = i * offset;
        int range = offset;
        if ((i == (n_threads-1)) && (num_bitmaps > n_threads))
            range += (num_bitmaps % n_threads);

        getval_threads[i] = thread(&Table::_get_value, this, 
                rowid, begin, range, &flag, &local_results[i]);
    }

    int ret = -1;
    for (int t = 0; t < n_threads; t++) {
        getval_threads[t].join();
        int tmp = __atomic_load_n(&local_results[t], MM_RELAXED);
        if (tmp != -1) {
            if (config->encoding == EE) {
                assert(ret == -1);
                ret = tmp;
            }
            else if (config->encoding == RE) {
                if (ret == -1) {
                    ret = tmp;
                } else {
                    ret = (tmp < ret) ? tmp : ret;
                }
            }
        }
    }

    delete[] local_results;
    delete[] getval_threads;

    return ret;
}

void Table::printMemory() {
    uint64_t bitmap = 0;
    for (int i = 0; i < num_bitmaps; ++i) {
        bitmaps[i]->appendActive();
        bitmap += bitmaps[i]->getSerialSize();
    }
    cout << "M BM " << bitmap << std::endl;
}

void Table::printUncompMemory() {
    uint64_t bitmap = 0;
    for (int i = 0; i < num_bitmaps; ++i) {
        bitmaps[i]->appendActive();
        bitmaps[i]->decompress();
        bitmap += bitmaps[i]->getSerialSize();
        bitmaps[i]->compress();
    }
    cout << "UncM BM " << bitmap << std::endl;
}
