#include "fastbit/bitvector.h"

#include <iostream>
#include <sstream>
#include <boost/filesystem.hpp>
#include <sys/mman.h>
#include <queue>
#include "fastbit/bitvector.h"

#include "table.h"
//#include "util.h"

#ifdef LINUX
#include <omp.h>
#endif

using namespace ub;
using namespace std;

// NOTE: The original code uses n_rows, which has not been initialized here.
// We replaced it with the constant value 10000000 on May 22, 2022.
// const ibis::bitvector::word_t UB_PAD_BITS = (n_rows / 310) * 31;
const ibis::bitvector::word_t UB_PAD_BITS = (10000000/ 31) * 31;

Table::Table(Table_config *config) : BaseTable(config), number_of_rows(config->n_rows) 
{    
    if (config->encoding == EE)
        num_bitmaps = config->g_cardinality;
    else if (config->encoding == RE)
        num_bitmaps = config->g_cardinality - 1;
    else {
        cout << "Bitmap UpBit only spports the EE and RE encoding schemes" << endl;
        assert(0);
    }

    bitmaps.reserve(num_bitmaps);
    update_bitmaps.reserve(num_bitmaps);
    lk_bitmaps.reserve(num_bitmaps);
    total_rows = config->n_rows + UB_PAD_BITS;

    config->nThreads_for_getval = 16;   // FIXME: In our evaluation, UpBit use the best argument 16.

    int n_threads = (config->nThreads_for_getval > num_bitmaps) ? num_bitmaps : config->nThreads_for_getval;
    int n_btv_per_thread = num_bitmaps / n_threads;
    int n_left = num_bitmaps % n_threads;          
    thread* threads = new thread[n_threads];

    assert(n_btv_per_thread >= 1);

    vector<int>begin(n_threads + 1, 0);   
    /// store bitmap ID running in corresponding thread
    for (int i = 1; i <= n_left; i++)       
        begin[i] = begin[i - 1] + n_btv_per_thread + 1;
    for (int i = n_left + 1; i <= n_threads; i++)
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
        bitmaps[i]->adjustSize(0, total_rows);
        if (config->enable_fence_pointer) {
            bitmaps[i]->index.clear();
            bitmaps[i]->buildIndex();
        }
        update_bitmaps[i] = new ibis::bitvector();
        update_bitmaps[i]->adjustSize(0, total_rows);
        lk_bitmaps[i] = new shared_mutex();
    }
}

int Table::append(int tid, int val) 
{
    lock_guard<shared_mutex> guard_number_of_rows(lk_append);

    if (config->encoding == EE) {
        lock_guard<shared_mutex> guard_bitvec(*lk_bitmaps[val]);
        update_bitmaps[val]->setBit(number_of_rows, 1, config);
    }
    else if (config->encoding == RE) {
        // ibis::bitvector is not thread-safe.
        // We thus first lock the bitvectors to avoid conflicting with concurrent queries.
        for (int idx = val; idx < num_bitmaps; idx++) {
            lk_bitmaps[idx]->lock();
        }

        for (int idx = val; idx < num_bitmaps; idx++) {
            update_bitmaps[idx]->setBit(number_of_rows, 1, config);
        }

        for (int idx = val; idx < num_bitmaps; idx++) {
            lk_bitmaps[idx]->unlock();
        }
    }
    else {
        assert(0);
    }

    number_of_rows += 1;

    return 0;
}

int Table::__get_point_val(unsigned int column, unsigned int row) 
{
    auto bit1 = bitmaps[column]->getBitWithIndex(row);
    auto bit2 = update_bitmaps[column]->getBit(row, config);

    return (bit1 ^ bit2);
}

int Table::update(int tid, uint64_t rowid, int to_val) 
{
    int from_val = get_value(rowid);

    if (from_val == -1 || to_val == from_val)
        return -ENOENT;

    if (config->encoding == EE) {
        pair<int, int> lk_order = (from_val < to_val) ?
                                    make_pair(from_val, to_val) : make_pair(to_val, from_val);
        lock_guard<shared_mutex> guard_first_bitvec(*lk_bitmaps[lk_order.first]);
        lock_guard<shared_mutex> guard_second_bitvec(*lk_bitmaps[lk_order.second]);

        //Double-check after locking
        if (__get_point_val(from_val, rowid) != 1 ||
            __get_point_val(to_val, rowid) != 0) {
            cout << "WARNING: [Update] Bitvector has been changed "
                << "in between I checking it and grabing the locks." 
                << " So I'm leaving the bitvector untouched." << endl;
            return -EPERM;
        }

        update_bitmaps[to_val]->setBit(rowid, !update_bitmaps[to_val]->getBit(rowid, config), config);
        update_bitmaps[from_val]->setBit(rowid, !update_bitmaps[from_val]->getBit(rowid, config), config);
    }
    else if (config->encoding == RE) {
        int min, max, bit_value_old;
        if (to_val > from_val) {
            min = from_val;
            max = to_val - 1;
            bit_value_old = 1;
        }
        else {
            min = to_val;
            max = from_val - 1;
            bit_value_old = 0;
        }

        // for (int idx = min; idx <= max; idx++) causes conflicts among concurrent updates.
        for (int idx = 0; idx < num_bitmaps; idx++) {
            lk_bitmaps[idx]->lock();
        }

        //Double-check after locking
        for (int idx = min; idx <= max; idx++) {
            if (__get_point_val(idx, rowid) != bit_value_old) {
                cout << "WARNING: [Update] Bitvector has been changed "
                    << "in between I checking it and grabing the locks." 
                    << " So I'm leaving the bitvector untouched." << endl;
                for (int idx = 0; idx < num_bitmaps; idx++) {
                    lk_bitmaps[idx]->unlock();
                }
                return -EPERM;
            }
        }

        for (int idx = min; idx <= max; idx++) {
            update_bitmaps[idx]->setBit(rowid, !update_bitmaps[idx]->getBit(rowid, config), config);
        }

        for (int idx = 0; idx < num_bitmaps; idx++) {
            lk_bitmaps[idx]->unlock();
        }
    }
    else {
        assert(0);
    }

    return 0;
}

int Table::remove(int tid, uint64_t rowid) 
{
    int val = get_value(rowid);
    if (val == -1)
        return -ENOENT;

    if (config->encoding == EE) {
        lock_guard<shared_mutex> guard_bitv(*lk_bitmaps[val]);

        //Double-check after locking
        if (__get_point_val(val, rowid) != 1) {
            cout << "WARNING: [Remove] Bitvector has been changed "
                << "in between I checking it and grabing the locks." 
                << " So I'm leaving the bitvector untouched." << endl;
            return -EPERM;
        }

        update_bitmaps[val]->setBit(rowid, !update_bitmaps[val]->getBit(rowid, config), config);
    }
    else if (config->encoding == RE) {
        for (int idx = 0; idx < num_bitmaps; idx++) {
            lk_bitmaps[idx]->lock();
        }

        //Double-check after locking
        for (int idx = val; idx < num_bitmaps; idx++) {
            if (__get_point_val(idx, rowid) != 1) {
                cout << "WARNING: [Remove] Bitvector has been changed "
                    << "in between I checking it and grabing the locks." 
                    << " So I'm leaving the bitvector untouched." << endl;
                for (int idx = 0; idx < num_bitmaps; idx++) {
                    lk_bitmaps[idx]->unlock();
                }
                return -EPERM;
            }
        }

        for (int idx = val; idx < num_bitmaps; idx++) {
            update_bitmaps[idx]->setBit(rowid, 0, config);
        }

        for (int idx = 0; idx < num_bitmaps; idx++) {
            lk_bitmaps[idx]->unlock();
        }
    }
    else {
        assert(0);
    }

    return 0;
}

int Table::evaluate(int tid, uint32_t val) 
{
    ibis::bitvector res;

    lk_append.lock_shared();        // To synchronize with appends.
    lk_bitmaps[val]->lock_shared(); // To synchronize with updates and deletes.

    ibis::bitvector *btv_tmp = __atomic_load_n(&bitmaps[val], MM_RELAXED);
    uint32_t l_nor = __atomic_load_n(&number_of_rows, MM_RELAXED);

    lk_append.unlock_shared(); 

    auto cnt = update_bitmaps[val]->cnt();
    if (cnt == 0) {
        res.copy(*btv_tmp);
    } else {
        res.copy(*update_bitmaps[val]);
        res ^= *btv_tmp;
    }
    if (enable_fence_pointer) {
        res.index.clear();
        res.buildIndex();
    }

    lk_bitmaps[val]->unlock_shared();

    // There are two possible approaches to synchronizing merge and others.
    // (1) Upgrade the read lock lk_bitmaps to write lock. This approach,
    //     however, has not been supported by STL yet, and can lead to tricky
    //     concurrency issues. I.e., two evaluate()s attempt to upgrade at the same time.
    // (2) Each thread constructs its own bitmaps[val], index, and update_bitmaps[val]
    //     in private memory space, and then propagates by using a single atomic CAS instruction.
    //     This approach, however, requires to add an extra level of pointer indirection.
    //     Moreover, memory management mechanism (RCU or hazard pointers) is required,
    //     which fundamentally changes the current design of UpBit.
    //  We thus currently first release the shared_lock, and then grab a write_lock.

    if (cnt > config->n_merge_threshold) {
        lk_append.lock_shared();
        lk_bitmaps[val]->lock();

        // Double-check after locking
        if (l_nor == __atomic_load_n(&number_of_rows, MM_RELAXED)) {

            // We can release the locks in the same order because
            // we will not acquire any locks.
            lk_append.unlock_shared();

            // Double-check after locking
            btv_tmp = __atomic_load_n(&bitmaps[val], MM_RELAXED);
            ibis::bitvector res2;
            res2.copy(*update_bitmaps[val]);
            res2 ^= *btv_tmp;
            if (res == res2)
            { 
                // bitmaps[val] has not been changed
                bitmaps[val]->copy(res);
                if (enable_fence_pointer) {
                    bitmaps[val]->index.clear();
                    bitmaps[val]->buildIndex();
                }
                update_bitmaps[val]->clear();
                update_bitmaps[val]->adjustSize(0, total_rows);
            }
        }
        else {
            lk_append.unlock_shared();
        }

        lk_bitmaps[val]->unlock();
    }

    int ret;
    if (config->decode) {
        std::vector<uint32_t> dummy;
        res.decode(dummy, config);
        ret = 0;
    } else {
        ret = res.do_cnt();
    }

    return ret;
}

void Table::_get_value(uint64_t rowid, int begin, int range, bool *flag, int *result)
{
    int ret = -1;

    for(int i = 0; i < range; i++) {
        int curVal = begin + i;

        if (config->encoding == EE) {
            if (__atomic_load_n(flag, MM_CST))
                break;

            lk_bitmaps[curVal]->lock_shared();
        }

        //lock_guard<shared_mutex> guard_bitvec(*lk_bitmaps[curVal]);

        auto bit1 = bitmaps[curVal]->getBitWithIndex(rowid);
        auto bit2 = update_bitmaps[curVal]->getBit(rowid, config);
        if ((bit1 ^ bit2) == 1) {
            if (config->encoding == EE) {
                ret = curVal;
                __atomic_store_n(flag, true, MM_CST);
                // break;
            }
            else if (config->encoding == RE) {
                if (ret == -1) {
                    ret = curVal;
                } else {
                    ret = (curVal < ret) ? curVal : ret;
                }
            }
        } 

        if (config->encoding == EE) {
            lk_bitmaps[curVal]->unlock_shared();
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

    if (config->encoding == RE) {
        for (int i = 0; i < num_bitmaps; i++)
            lk_bitmaps[i]->lock_shared();
    }

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

    if (config->encoding == RE) {
        for (int i = 0; i < num_bitmaps; i++)
            lk_bitmaps[i]->unlock_shared();
    }

    delete[] local_results;
    delete[] getval_threads;

    return ret;
}

void Table::printMemory() {
    uint64_t bitmap = 0, updateable_bitmap = 0, fence_pointers = 0;
    for (int i = 0; i < num_bitmaps; ++i) {
        bitmap += bitmaps[i]->getSerialSize();
        fence_pointers += bitmaps[i]->index.size() * sizeof(int) * 2;
        updateable_bitmap += update_bitmaps[i]->getSerialSize();
    }
    std::cout << "M FP " << fence_pointers << std::endl;
    std::cout << "M UB " << updateable_bitmap << std::endl;
    std::cout << "M BM " << bitmap << std::endl;
}

void Table::printUncompMemory() {
    uint64_t bitmap = 0, updateable_bitmap = 0, fence_pointers = 0;
    for (int i = 0; i < num_bitmaps; ++i) {
        bitmaps[i]->appendActive();
        bitmaps[i]->decompress();
        bitmap += bitmaps[i]->getSerialSize();
        bitmaps[i]->compress();
        update_bitmaps[i]->decompress();
        updateable_bitmap += update_bitmaps[i]->getSerialSize();
        update_bitmaps[i]->compress();
        fence_pointers += bitmaps[i]->index.size() * sizeof(int) * 2;
    }
    std::cout << "UncM FP " << fence_pointers << std::endl;
    std::cout << "UncM UB " << updateable_bitmap << std::endl;
    std::cout << "UncM BM " << bitmap << std::endl;
}

int Table::range(int tid, uint32_t start, uint32_t range) {
    ibis::bitvector res;
    res.set(0, total_rows);
    if (config->range_algo == "naive") {
        for (uint32_t i = 0; i < range; ++i) {
            res |= *(bitmaps[start + i]);
        }
    } else if (config->range_algo == "pq") {
        typedef std::pair<ibis::bitvector*, bool> _elem;
        // put all bitmaps in a priority queue
        std::priority_queue<_elem> que;
        _elem op1, op2, tmp;
        tmp.first = 0;

        // populate the priority queue with the original input
        for (uint32_t i = 0; i < range; ++i) {
            op1.first = bitmaps[start + i];
            op1.second = false;
            que.push(op1);
        }

        while (! que.empty()) {
            op1 = que.top();
            que.pop();
            if (que.empty()) {
                res.copy(*(op1.first));
                if (op1.second) delete op1.first;
                break;
            }

            op2 = que.top();
            que.pop();
            tmp.second = true;
            tmp.first = *(op1.first) | *(op2.first);

            if (op1.second)
                delete op1.first;
            if (op2.second)
                delete op2.first;
            if (! que.empty()) {
                que.push(tmp);
                tmp.first = 0;
            }
        }
        if (tmp.first != 0) {
            if (tmp.second) {
                res |= *(tmp.first);
                delete tmp.first;
                tmp.first = 0;
            }
            else {
                res |= *(tmp.first);
            }
        }
    } else {
        auto end = start + range;
        while (start < end && bitmaps[start] == 0)
            ++ start;
        if (start < end) {
            res |= *(bitmaps[start]);
            ++ start;
        }
        res.decompress();
        for (uint32_t i = start; i < end; ++ i) {
            res |= *(bitmaps[i]);
        }
    }
    if (config->decode) {
        std::vector<uint32_t> dummy;
        res.decode(dummy, config);
        return 0;
    } else {
        return res.do_cnt();
    }
}
