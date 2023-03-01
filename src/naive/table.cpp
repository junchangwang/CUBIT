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

Table::Table(Table_config *config) : BaseTable(config->g_cardinality), number_of_rows(config->n_rows)
{
    this->config = config;

    bitmaps.reserve(config->g_cardinality);
    for (uint32_t i = 0; i < config->g_cardinality; ++i) {
        bitmaps[i] = new ibis::bitvector();
        if (config->INDEX_PATH != "")
            bitmaps[i]->read(getBitmapName(i).c_str());
        if (config->enable_fence_pointer)
            bitmaps[i]->buildIndex();
    }

    /*
    for (uint32_t i = 0; i < number_of_rows; i++) {
        int ret = get_value(i);
        assert((ret >= 0) && (ret < cardinality));
        if ( i % 10000 == 0)
            cout << "Passed data integrity check: " << i << endl;
    }
    cout << " === Passed data integrity check. === " << endl;
    */
}

int Table::append(int tid, int val) 
{
    lock_guard<shared_mutex> guard(g_lock);

    if (config->on_disk) { }
    else {
        bitmaps[val]->setBit(number_of_rows, 1, config);
        if (config->enable_fence_pointer) {
            bitmaps[val]->index.clear();
            bitmaps[val]->buildIndex();
        }

        number_of_rows += 1;
        
        for (int i = 0; i < cardinality; ++i) {
            bitmaps[i]->adjustSize(0, number_of_rows);
        }
    }
    return 0;
}

int Table::update(int tid, uint32_t rowid, int to_val) 
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

        // if (verbose)
        //     cout << "U SETBIT " << time_diff(before , after) << endl;
    }

    return 0;
}

int Table::remove(int tid, uint32_t rowid) 
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
        bitmaps[val]->setBit(rowid, 0, config);
        if (config->enable_fence_pointer) {
            bitmaps[val]->index.clear();
            bitmaps[val]->buildIndex();
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
            if (config->enable_fence_pointer) {
                res.index.clear();
                res.buildIndex();
            }
            // if (verbose)
            //     cout << "Q COPY " << time_diff(before , after) << endl;
        }
    }
    auto cnt = res.do_cnt();
    // if (verbose)
    //     cout << "Q DEC " << time_diff(before , after) << endl;
    return cnt;
}

void Table::_get_value(uint32_t rowid, int begin, int range, bool *flag, int *result)
{
    int ret = -1;

    for(int i = 0; i < range; i++) {
        int curVal = begin + i;

        if (__atomic_load_n(flag, MM_CST))
            break;

        auto bit1 = bitmaps[curVal]->getBit(rowid, config);
        if (bit1 == 1) {
#if defined(VERIFY_RESULTS)
            assert(ret == -1);
            assert(!__atomic_load_n(flag, MM_CST));
#endif
            ret = curVal;
            __atomic_store_n(flag, true, MM_CST);

            break;
        }
    }

    __atomic_store_n(result, ret, MM_RELEASE);
}

int Table::get_value(uint32_t rowid) {
    bool flag = false;
    int n_threads = (config->nThreads_for_getval > cardinality) ? cardinality : config->nThreads_for_getval;
    int begin = 0;
    int offset = cardinality / n_threads;
    thread * getval_threads = new thread[n_threads];
    int * local_results = new int[n_threads];
    fill_n(local_results, n_threads, -2);

    assert(offset >= 1);

    for (int i = 0; i < n_threads; i++) {
        int begin = i * offset;
        int range = offset;
        if ((i == (n_threads-1)) && (cardinality > n_threads))
            range += (cardinality % n_threads);

        getval_threads[i] = thread(&Table::_get_value, this, 
                rowid, begin, range, &flag, &local_results[i]);
    }

    int ret = -1;
    for (int t = 0; t < n_threads; t++) {
        getval_threads[t].join();
        int tmp = __atomic_load_n(&local_results[t], MM_RELAXED);
        if (tmp != -1) {
            assert(ret == -1);
            ret = tmp;
        }
    }

    delete[] local_results;
    delete[] getval_threads;

#if defined(VERIFY_RESULTS)
    if (ret != -1) {
        assert(bitmaps[ret]->getBit(rowid, config) == 1);
    }
#endif

    return ret;
}

void Table::printMemory() {
    uint64_t bitmap = 0;
    for (int i = 0; i < cardinality; ++i) {
        bitmaps[i]->appendActive();
        bitmap += bitmaps[i]->getSerialSize();
    }
    cout << "M BM " << bitmap << std::endl;
}

void Table::printUncompMemory() {
    uint64_t bitmap = 0;
    for (int i = 0; i < cardinality; ++i) {
        bitmaps[i]->appendActive();
        bitmaps[i]->decompress();
        bitmap += bitmaps[i]->getSerialSize();
        bitmaps[i]->compress();
    }
    cout << "UncM BM " << bitmap << std::endl;
}
