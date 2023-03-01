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
// Junchang replaced it with the constant value 10000000 on May 22, 2022.
// const ibis::bitvector::word_t UB_PAD_BITS = (n_rows / 310) * 31;
const ibis::bitvector::word_t UB_PAD_BITS = (10000000/ 31) * 31;

Table::Table(Table_config *config) : BaseTable(config->g_cardinality), number_of_rows(config->n_rows) 
{
    this->config = config;
    
    bitmaps.reserve(config->g_cardinality);
    update_bitmaps.reserve(config->g_cardinality);
    lk_bitmaps.reserve(config->g_cardinality);
    total_rows = config->n_rows + UB_PAD_BITS;
    for (uint32_t i = 0; i < config->g_cardinality; ++i) {
        bitmaps[i] = new ibis::bitvector();
        if (config->INDEX_PATH != "")
            bitmaps[i]->read(getBitmapName(i).c_str());
        bitmaps[i]->adjustSize(0, total_rows);
        if (config->enable_fence_pointer)
            bitmaps[i]->buildIndex();
        update_bitmaps[i] = new ibis::bitvector();
        update_bitmaps[i]->adjustSize(0, total_rows);
        lk_bitmaps[i] = new shared_mutex();
    }
}

int Table::append(int tid, int val) 
{
    lock_guard<shared_mutex> guard_number_of_rows(lk_append);
    lock_guard<shared_mutex> guard_bitvec(*lk_bitmaps[val]);

    update_bitmaps[val]->setBit(number_of_rows, 1, config);
    number_of_rows += 1;

    return 0;
}

int Table::__get_point_val(unsigned int column, unsigned int row) 
{
    auto bit1 = bitmaps[column]->getBitWithIndex(row);
    auto bit2 = update_bitmaps[column]->getBit(row, config);

    return (bit1 ^ bit2);
}

int Table::update(int tid, unsigned int rowid, int to_val) 
{
    int from_val = get_value(rowid);
#if defined(VERIFY_RESULTS)
    cout << "Updating row " << rowid << " old: " << from_val << " to: " << to_val << endl;
#endif

    if (from_val == -1 || to_val == from_val)
        return -ENOENT;

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

    return 0;
}

int Table::remove(int tid, unsigned int rowid) 
{
    int val = get_value(rowid);
    if (val == -1)
        return -ENOENT;

    lock_guard<shared_mutex> guard_bitv(*lk_bitmaps[val]);

    //Double-check after locking
    if (__get_point_val(val, rowid) != 1) {
        cout << "WARNING: [Remove] Bitvector has been changed "
             << "in between I checking it and grabing the locks." 
             << " So I'm leaving the bitvector untouched." << endl;
        return -EPERM;
    }

    update_bitmaps[val]->setBit(rowid, !update_bitmaps[val]->getBit(rowid, config), config);

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
    if (config->enable_fence_pointer) {
        res.index.clear();
        res.buildIndex();
    }

    // There are two possible approaches to synchronizing merge and others.
    // (1) Upgrade the read lock guard_bitvec to write lock. This approach,
    //     however, has not been supported by STL yet, and can lead to tricky
    //     concurrency issues. I.e., two evaluate()s attempt to upgrade at the same time.
    // (2) Each thread constructs its own bitmaps[val], index, and update_bitmaps[val]
    //     in private memory space, and then propagates by using a single atomic CAS instruction.
    //     This approach, however, requires to add an extra level of pointer indirection.
    //     Moreover, memory management mechanism (RCU or hazard pointers) is required,
    //     which fundamentally changes the current design of UpBit.
    //  We thus currently first release the shared_lock, and then grab a write_lock.

    lk_bitmaps[val]->unlock_shared();

    if (cnt > config->n_merge) {
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

void Table::_get_value(uint32_t rowid, int begin, int range, bool *flag, int *result)
{
    int ret = -1; 

    for(int i = 0; i < range; i++) {
        int curVal = begin + i;

        if (__atomic_load_n(flag, MM_CST))
            break;

        lock_guard<shared_mutex> guard_bitvec(*lk_bitmaps[curVal]);

        auto bit1 = bitmaps[curVal]->getBitWithIndex(rowid);
        auto bit2 = update_bitmaps[curVal]->getBit(rowid, config);
        if ((bit1 ^ bit2) == 1) {
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

    return ret;
}

void Table::printMemory() {
    uint64_t bitmap = 0, updateable_bitmap = 0, fence_pointers = 0;
    for (int i = 0; i < cardinality; ++i) {
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
    for (int i = 0; i < cardinality; ++i) {
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

int Table::range(uint32_t start, uint32_t range) {
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
