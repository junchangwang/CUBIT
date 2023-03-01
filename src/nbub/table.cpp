#include <iostream>
#include <tuple>
#include <atomic>

#include "table.h"
#include "fastbit/bitvector.h"
#include "nicolas/util.h"
#include "config.h"

using namespace nbub;
using namespace std;

Nbub::Nbub(Table_config *config) : 
    BaseTable(config->g_cardinality), g_timestamp(1UL), g_number_of_rows(config->n_rows)
{
    this->config = config;

    autoCommit = config->autoCommit;
    MERGE_THRESH = config->n_merge_threash;
    db_control = config->db_control;

    TransDesc *trans_dummy = new TransDesc();
    uint128_t data_t;
    if (db_control) {
        data_t = __atomic_load_n((uint128_t *)&db_timestamp, MM_RELAXED);
        trans_dummy->l_start_ts = trans_dummy->l_commit_ts = (uint64_t)data_t;
        trans_dummy->l_number_of_rows = db_number_of_rows;
    } 
    else { 
        data_t = __atomic_load_n((uint128_t *)&g_timestamp, MM_RELAXED);
        trans_dummy->l_start_ts = trans_dummy->l_commit_ts = (uint64_t)data_t;
        trans_dummy->l_number_of_rows = (uint64_t)(data_t>>64);
    }

    // NOTE: DBx1000 selects 1 as the starting value of timestamp, so we stick to this.
    assert(trans_dummy->l_commit_ts == 1);
    assert(trans_dummy->l_number_of_rows == config->n_rows);

    total_rows = config->n_rows + NBUB_PAD_BITS;
    bitmaps = new Bitmap *[config->g_cardinality];
    for (uint32_t i = 0; i < config->g_cardinality; ++i) {
        bitmaps[i] = new Bitmap{};
        bitmaps[i]->l_commit_ts = 0UL;
        __atomic_store_n(&bitmaps[i]->l_start_trans, trans_dummy, MM_RELEASE);
        ibis::bitvector *btv_t = new ibis::bitvector();
        assert(btv_t);
        if (config->INDEX_PATH != "")
            btv_t->read(getBitmapName(i).c_str());
        btv_t->adjustSize(0, total_rows);
        if (config->enable_fence_pointer) {
            btv_t->index.clear();
            btv_t->buildIndex();
        }
        __atomic_store_n(&bitmaps[i]->btv, btv_t, MM_RELEASE);
        bitmaps[i]->next = NULL;
    }

    n_trans_pool = config->n_workers*(config->n_deletes+config->n_queries)*2;
    trans_pool = new TransDesc[n_trans_pool] {};
    assert(trans_pool);
    __atomic_store_n(&cnt_trans_used, 0, MM_RELAXED);

// #if defined(VERIFY_RESULTS)
    cout << "=== Size of TransDesc pool: " << n_trans_pool 
            << "  autoCommit: " << autoCommit
            << "  MERGE_THRESH: " << MERGE_THRESH 
            << "  db_control: " << db_control
            << " ==="<< endl;
// #endif
    
    merge_req_queues = new queue<struct merge_req*>[config->n_workers];
    lk_merge_req_queues = new mutex[config->n_workers];
}

/*************************************
 *       Transaction Semantics       *
 ************************************/

struct TransDesc * Nbub::trans_begin(int tid) 
{
    struct ThreadInfo *th = &g_ths_info[tid];

    if (READ_ONCE(th->active_trans))
        return th->active_trans;

    TransDesc *trans = allocate_trans();
    trans->l_end_trans = __atomic_load_n(&trans_queue->tail, MM_ACQUIRE);
    trans->l_commit_ts = INV_TIMESTAMP;

    // NOTE: 
    // The original CC in CUBIT is light-weight; only transactions including updates increment timestamp.
    // In contrast, the CC in DBMS (e.g., DBx1000) increases timestamp for both queries and updates.
    // In the latter case, if (trans->l_start_ts > READ_ONCE(trans->l_end_trans->l_commit_ts)),
    // we cannot distinguish whether some queries have been successfully committed or a new trans has been
    // inserted into the list. So we simply assume no new trans has been inserted.
    if (! db_control) {
        trans->l_start_ts = __atomic_load_n(&g_timestamp, MM_RELAXED);
        // New trans have been inserted in between accessing trans_queue->tail and g_timestamp,
        // so we need to get a snapshot of them.
        while (trans->l_start_ts > READ_ONCE(trans->l_end_trans->l_commit_ts)) {
            assert(READ_ONCE(trans->l_end_trans->next));
            trans->l_end_trans = __atomic_load_n(&trans->l_end_trans->next, MM_ACQUIRE);
        }
    }
    else {
        trans->l_start_ts = __atomic_load_n(&db_timestamp, MM_RELAXED);
    }

    if (trans->l_start_ts < READ_ONCE(trans->l_end_trans->l_commit_ts)) {
        // New trans have completed
        trans->l_start_ts = READ_ONCE(trans->l_end_trans->l_commit_ts);
    }

    trans->l_inc_rows = 0;
    trans->next = NULL;
    __atomic_store_n(&th->active_trans, trans, MM_RELEASE);

    return th->active_trans;
}

/*************************************
 *         Buffer Management         *
 ************************************/

struct TransDesc * Nbub::allocate_trans() 
{
    uint64_t pos = __atomic_fetch_add(&cnt_trans_used, 1, MM_CST);
    assert(pos < n_trans_pool);

    return &trans_pool[pos];
}

int Nbub::delete_trans(int tid, struct TransDesc *trans) 
{
    // FIXME: No recycle yet
    //
    return 0;
}


/*************************************
 *         Helper functions          *
 ************************************/

uint64_t Nbub::encode_rub(uint32_t pos, uint16_t idx1, uint16_t idx2, uint8_t type) {
    uint64_t tmp = ((uint64_t)pos << OFFSET_POS) | ((uint32_t)idx1 << OFFSET_IDX1) |
                    ((uint32_t)idx2 << OFFSET_IDX2) | type;
    return tmp;
}

tuple<uint32_t, uint16_t, uint16_t, uint8_t> Nbub::decode_rub(uint64_t rub) {
    uint32_t pos = rub >> OFFSET_POS;
    uint16_t idx1 = (rub << N_BITS_POS) >> (N_BITS_POS + N_BITS_PER_IDX + N_BITS_TYPE);
    uint16_t idx2 = (rub << (N_BITS_POS + N_BITS_PER_IDX)) >> (N_BITS_POS + N_BITS_PER_IDX + N_BITS_TYPE);
    uint8_t type = (uint8_t) rub;

    return make_tuple(pos, idx1, idx2, type);
}

/* Get RUBs in between (tsp_begin, tsp_end] */
TransDesc * Nbub::get_rubs_on_btv(uint64_t tsp_begin, uint64_t tsp_end, 
                                        TransDesc *trans, uint32_t val, 
                                        map<uint32_t, uint64_t> &rubs)
{
    assert(tsp_begin < tsp_end);

    // Check trans in between (tsp_begin, tsp_end].
    TransDesc *trans_prev = NULL;
    while (__atomic_load_n(&trans, MM_ACQUIRE) && (READ_ONCE(trans->l_commit_ts) <= tsp_end)) {
        if (READ_ONCE(trans->l_commit_ts) > tsp_begin) {
            uint64_t rub_tmp = 0UL;
            for (int i = 0; i < trans->rubs.size(); i++) {
                rub_tmp = trans->rubs[i];
                auto [row_id_t, idx1, idx2, type] = decode_rub(rub_tmp);
                if ((idx1 == val) || (idx2 == val)) {
                    rubs[row_id_t] = rub_tmp;
                }
                else {
                    // In C++, map.erase() implies "remove if exist".
                    // row_id_t was recorded. But now we get a newer rub on this row
                    // and this rub does not contain val. We then remove the older rub.
                    rubs.erase(row_id_t);
                }
            }
        } else if (READ_ONCE(trans->l_commit_ts) == tsp_begin) {
            ; // Skip the first trans
        } else {
            assert(false);
        }

        trans_prev = trans;
        trans = READ_ONCE(trans->next);
    }

    return trans ? trans : trans_prev;
}

/* Get the last RUB in between (tsp_begin, tsp_end] of the specified row. */
TransDesc * Nbub::get_rub_on_row(uint64_t tsp_begin, uint64_t tsp_end, 
                                    TransDesc *trans, uint32_t row_id,
                                    uint64_t &rub, uint64_t &rub_tsp)
{
    assert(tsp_begin < tsp_end);  

    // Check trans in between (tsp_begin, tsp_end].
    TransDesc *trans_prev = NULL;
    while (__atomic_load_n(&trans, MM_ACQUIRE) && (READ_ONCE(trans->l_commit_ts) <= tsp_end)) {
        if (READ_ONCE(trans->l_commit_ts) > tsp_begin) {
            uint64_t rub_tmp = 0UL;
            for (int i = 0; i < trans->rubs.size(); i++) {
                rub_tmp = trans->rubs[i];
                auto [row_id_t, idx1, idx2, type] = decode_rub(rub_tmp);
                if (row_id == row_id_t) {
                    rub = rub_tmp;
                    rub_tsp = READ_ONCE(trans->l_commit_ts);
                    // Note: There is no break, because we want to
                    //       record the last rub in between (tsp_begin, tsp_end].
                }
            }
        } else if (trans->l_commit_ts == tsp_begin) {
            ; // Skip the first trans
        } else {
            assert(false);
        }

        trans_prev = trans;
        trans = READ_ONCE(trans->next);
    }

    return trans ? trans : trans_prev;
}


// Check if trans conflicts with others in between (trans->l_end_trans, tail].
// Return 0 if no conflict; Otherwise -1.
// Side effect: Assign tail to trans->l_end_trans if there is no conflict,
//          such that the next invocation of this function on the same trans can be accelerated.
//          No concurrency issue because this function is invoked sequentially.
int Nbub::check_conflicts(struct TransDesc *trans, struct TransDesc *tail)
{
    struct TransDesc *end_trans_t = __atomic_load_n(&trans->l_end_trans, MM_ACQUIRE);
    struct TransDesc *end_trans_t_2 = __atomic_load_n(&end_trans_t->next, MM_ACQUIRE);

    if (end_trans_t_2) { 
	while (end_trans_t_2 && READ_ONCE(end_trans_t_2->l_commit_ts) <= READ_ONCE(tail->l_commit_ts)) {
            vector<uint64_t> intersection;
	    set_intersection(end_trans_t_2->updated_rows.begin(), end_trans_t_2->updated_rows.end(),
		    trans->updated_rows.begin(), trans->updated_rows.end(),
		    back_inserter(intersection));
	    if (!intersection.empty()) {
#if defined(VERIFY_RESULTS)
		cout << "NOTE: In check_conflict(): Other transactions have committed. " <<
		    " My ts: " << trans->l_commit_ts << " Conflicting ts: " << end_trans_t_2->l_commit_ts << 
		    ". Conflicting rubs: " << intersection[0] << endl;
#endif
		return -1;
	    }
	    end_trans_t = end_trans_t_2;
	    end_trans_t_2 = __atomic_load_n(&end_trans_t->next, MM_ACQUIRE);
	}
	// No conflict such that move l_end_trans forward.
        __atomic_store_n(&trans->l_end_trans, end_trans_t, MM_CST);
    }

    return 0;
}


/*************************************
 *       Index manipulation          *
 ************************************/

int Nbub::append(int tid, int val) 
{    
    struct ThreadInfo *th = &g_ths_info[tid];
    assert(val < cardinality);

    struct TransDesc *trans = __atomic_load_n(&th->active_trans, MM_ACQUIRE);

    if (!trans)
        trans = trans_begin(tid);

    // row_id will be generated when committed.
    uint64_t rub = encode_rub(INV_ROW_ID, val, INV_IDX, TYPE_INSERT);
    trans->rubs.push_back(rub);
    trans->l_inc_rows ++;

    if (autoCommit) {
        trans_commit(tid);
    }

    return 0;
}

int Nbub::update(int tid, unsigned int row_id, int to_val)
{
    struct ThreadInfo *th = &g_ths_info[tid];
    assert(to_val < cardinality);

    rcu_read_lock();

    struct TransDesc * trans = __atomic_load_n(&th->active_trans, MM_ACQUIRE);

    // rcu_read_lock() must be invoked before accessing g_timestamp in trans_begin(),
    // to guarantee that the corresponding btv cannot be reclaimed before
    // we access it in get_value_rcu().

    if (!trans)
        trans = trans_begin(tid);

    uint64_t last_rub = 0UL;
    int from_val = get_value_rcu(row_id, trans->l_start_ts, &last_rub);
    assert(from_val < cardinality);

    rcu_read_unlock();

    if (from_val == -1) {
#if defined(VERIFY_RESULTS)
        cout << "NOTE in update(): the value at row " << row_id <<
            " has been deleted. I will return." << endl;
#endif
        return -ENOENT;
    }
    else if (to_val == from_val) {
#if defined(VERIFY_RESULTS)
        cout << "NOTE in update(): from_val == to_val (" << to_val <<
            "). I will return." << endl;
#endif
        return -ENOENT;
    }

    // Build new rub 
    int i = 0;
    uint16_t my_idx[2];
    auto [row_id_t, idx1, idx2, type] = decode_rub(last_rub);
    if (!last_rub) {
        idx1 = idx2 = INV_IDX;
    }

    if ((from_val != idx1) && (from_val != idx2))
        my_idx[i++] = from_val;
    if ((to_val != idx1) && (to_val != idx2))
        my_idx[i++] = to_val;

    if (i < 2) {
        if ((idx1 != INV_IDX) && (idx1 != from_val) && (idx1 != to_val))
            my_idx[i++] = idx1;
        if ((idx2 != INV_IDX) && (idx2 != from_val) && (idx2 != to_val))
            my_idx[i++] = idx2;
    }

    assert(i <= 2);
    for(; i < 2; i++) {
        my_idx[i] = INV_IDX;
    }
    uint64_t my_rub = encode_rub(row_id, my_idx[0], my_idx[1], TYPE_UPDATE);

    trans->rubs.push_back(my_rub);
    trans->updated_rows.push_back(row_id);

    if (autoCommit) {
        trans_commit(tid);
    }

    return 0;
}

int Nbub::remove(int tid, unsigned int row_id)
{
    struct ThreadInfo *th = &g_ths_info[tid];
    assert(row_id < g_number_of_rows);

    rcu_read_lock();

    struct TransDesc * trans = __atomic_load_n(&th->active_trans, MM_ACQUIRE);

    if (!trans)
        trans = trans_begin(tid);

    uint64_t last_rub = 0UL;
    int val = get_value_rcu(row_id, trans->l_start_ts, &last_rub);
    assert(val < cardinality);

    rcu_read_unlock();

    if (val == -1) {
#if defined(VERIFY_RESULTS)
        cout << "Remove: The value at row " << row_id << " has been deleted." <<
            "I will return." << endl;
#endif
        return -ENOENT;
    }

    // Fill rub 
    int i = 0;
    uint16_t my_idx[2];
    auto [row_id_t, idx1, idx2, type] = decode_rub(last_rub);
    if (!last_rub) {
        idx1 = idx2 = INV_IDX;
    }

    if ((val != idx1) && (val != idx2))
        my_idx[i++] = val;

    if ((idx1 != INV_IDX) && (idx1 != val))
        my_idx[i++] = idx1;
    if ((idx2 != INV_IDX) && (idx2 != val))
        my_idx[i++] = idx2;

    assert(i <= 2);
    for(; i < 2; i++) {
        my_idx[i] = INV_IDX;
    }
    uint64_t my_rub = encode_rub(row_id, my_idx[0], my_idx[1], TYPE_DELETE);

    trans->rubs.push_back(my_rub);
    trans->updated_rows.push_back(row_id);

    if (autoCommit) {
        trans_commit(tid);
    }

    return 0;
}

int Nbub::evaluate(int tid, uint32_t val)
{   
    struct ThreadInfo *th = &g_ths_info[tid];
    assert(val < cardinality);

    rcu_read_lock();

    struct TransDesc *trans = __atomic_load_n(&th->active_trans, MM_ACQUIRE);
    bool reuse_bitmap = true;

    if (!trans)
        trans = trans_begin(tid);

    struct Bitmap *bitmap_old = __atomic_load_n(&bitmaps[val], MM_ACQUIRE);

    /* Need to check RUBs in between (tsp_begin, tsp_end]. */
    uint64_t tsp_begin = READ_ONCE(bitmap_old->l_commit_ts);
    uint64_t tsp_end = READ_ONCE(trans->l_start_ts);
    while (tsp_begin > tsp_end) {
#if defined(VERIFY_RESULTS)
        assert(READ_ONCE(bitmap_old->next));
        cout << "Note: MVCC evaluate() moves from ts " << bitmap_old->l_commit_ts << 
                " to ts " << bitmap_old->next->l_commit_ts << endl;
#endif
        bitmap_old = __atomic_load_n(&bitmap_old->next, MM_ACQUIRE);
        tsp_begin = READ_ONCE(bitmap_old->l_commit_ts); 
        reuse_bitmap = false;
    }

    map<uint32_t, uint64_t> rubs{};
    if (tsp_begin < tsp_end) {
        get_rubs_on_btv(tsp_begin, tsp_end, READ_ONCE(bitmap_old->l_start_trans), val, rubs);
    }

    int cnt = 0;
    ibis::bitvector *my_btv = NULL;
    if (rubs.size() == 0) {
        // Reuse old bitvector. No need to allocate a new one.
        my_btv = READ_ONCE(bitmap_old->btv);

        if (config->decode) {
            std::vector<uint32_t> dummy;
            my_btv->decode(dummy, config);
            cnt = 0;
        } else {
            cnt = my_btv->do_cnt();
        }
    }
    else {

/********** Straight forward approach **********/
//        my_btv = new ibis::bitvector();
//        my_btv->copy(*bitmap_old->btv);
//        // Building index has been delegated to the merge daemon.
//
//        for (auto const &[row_id_t, rub_t] : rubs) {
//            auto [row_id_t_2, idx1, idx2, type] = decode_rub(rub_t);
//            assert(idx1 == val || idx2 == val);
//            // vb = vb ^ 1 = !vb
//            // NOTE:: do not use getBitWithIndex() to avoid
//            //        rebuilding index for each iteration
//            my_btv->setBit(row_id_t, !my_btv->getBit(row_id_t, config), config);  
//        }
        
/********** Optimized approach **********/

        auto t1 = std::chrono::high_resolution_clock::now();

        my_btv = new ibis::bitvector();

        map<uint32_t, uint64_t>::iterator it;
        for (it = rubs.begin(); it != rubs.end(); it++) {
            auto [row_id_t_2, idx1, idx2, type] = decode_rub(it->second);
            assert(idx1 == val || idx2 == val);
            // NOTE:: do not use getBitWithIndex() to avoid
            //        rebuilding index for each iteration
            my_btv->setBit(it->first, 1, config);  
        }

        auto t2 = std::chrono::high_resolution_clock::now();

        *my_btv ^= *bitmap_old->btv;

        auto t3 = std::chrono::high_resolution_clock::now();

        if (config->decode) {
            std::vector<uint32_t> dummy;
            my_btv->decode(dummy, config);
            cnt = 0;
        } else {
            cnt = my_btv->do_cnt();
        }

        auto t4 = std::chrono::high_resolution_clock::now();

        struct Bitmap *bitmap_new = new Bitmap{};
        __atomic_store_n(&bitmap_new->btv, my_btv, MM_RELEASE);

        // Perform Merge operation
        if (reuse_bitmap && (rubs.size() >= MERGE_THRESH)) 
        {
            map<uint32_t, uint64_t> *rubs_t = new map<uint32_t, uint64_t>(rubs);
            struct merge_req *req = new merge_req{tid, val, trans, bitmap_old, bitmap_new, rubs_t};

            // FIXME: In case that academically wait-free is acquired,
            //        Michael's wait-free SPSC queue can be used.
            lock_guard<mutex> lock(lk_merge_req_queues[tid]);
            merge_req_queues[tid].push(req);

            // auto t5 = std::chrono::high_resolution_clock::now();

            // cout << "[Query time (ms)]" <<
            //     "    Setbits: " << to_string(std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count()) <<
            //     "    XOR: " << to_string(std::chrono::duration_cast<std::chrono::microseconds>(t3-t2).count()) <<
            //     "    Count: " << to_string(std::chrono::duration_cast<std::chrono::microseconds>(t4-t3).count()) <<
            //     "    RegMerge: " << to_string(std::chrono::duration_cast<std::chrono::microseconds>(t5-t4).count()) << endl;
        } 
        else {
            call_rcu(&bitmap_new->head, free_bitmap_cb);
        }

    }

    // This barrier can be moved forward.
    // However, performance improvement is negligibal since
    // the memory reclamation has been delegated to background threads.
    // We leave it here to make the code easy to understand.
    rcu_read_unlock();

    if (autoCommit) {
        assert(trans_commit(tid) == -ENOENT);
    }

    return cnt;
}

void Nbub::_get_value(uint32_t rowid, int begin, int range, uint64_t l_timestamp,
        bool *flag, int *result, uint64_t *rub, uint64_t *rub_tsp)
{
    int ret = -1;

#if !defined(VERIFY_RESULTS)
    if (__atomic_load_n(flag, MM_CST)) 
        goto out;
#endif

    for(int i = 0; i < range; i++) {
        int curVal = begin + i;
        uint64_t rub_t = 0UL;
        uint64_t rub_tsp_t = 0UL;
        
#if !defined(VERIFY_RESULTS)
        if (__atomic_load_n(flag, MM_CST))
            break;
#endif

        struct Bitmap *bitmap = __atomic_load_n(&bitmaps[curVal], MM_ACQUIRE);
        uint64_t tsp_begin = READ_ONCE(bitmap->l_commit_ts);
        uint64_t tsp_end = l_timestamp;

        while (tsp_begin > tsp_end) {
            bitmap = __atomic_load_n(&bitmap->next, MM_ACQUIRE);
#if defined(VERIFY_RESULTS)
            //cout << "MVCC _get_value() moves from ts " << bitmap->l_commit_ts << 
            //    " to ts " << bitmap->next->l_commit_ts << endl;
            assert(bitmap);
            assert(READ_ONCE(bitmap->l_commit_ts) < tsp_begin);
#endif
            tsp_begin = READ_ONCE(bitmap->l_commit_ts); 
        }

        int bit1 = bitmap->btv->getBitWithIndex(rowid);
        int bit2 = 0;

        if (tsp_begin < tsp_end) { 
            get_rub_on_row(tsp_begin, tsp_end, READ_ONCE(bitmap->l_start_trans),
                    rowid, rub_t, rub_tsp_t);
        }

        if (rub_t) {
            auto [row_id_t, idx1, idx2, type] = decode_rub(rub_t);
            if ((idx1 == curVal) || (idx2 == curVal)) {
                bit2 = 1;
            }
        } 

        // Record the rub value with the largest timestamp value.
        if (rub_t) {
#if defined(VERIFY_RESULTS)
            if (rub_tsp_t == *rub_tsp) {
                assert(rub_t == *rub);
            }
#endif
            if (rub_tsp_t > *rub_tsp) {
                *rub = rub_t;
                *rub_tsp = rub_tsp_t;
            }
        }

        if ((bit1 ^ bit2) == 1) {
#if defined(VERIFY_RESULTS)
            assert(ret == -1);
            assert(!__atomic_load_n(flag, MM_CST));
#endif
            ret = curVal;
            __atomic_store_n(flag, true, MM_CST);

#if !defined(VERIFY_RESULTS)
            break;
#endif
        }
    }

out:
    __atomic_store_n(result, ret, MM_RELEASE);
}


int Nbub::get_value_rcu(uint32_t rowid, uint64_t l_timestamp, uint64_t *last_rub) 
{
    bool flag = false;
    int n_threads = (config->nThreads_for_getval > cardinality) ? cardinality : config->nThreads_for_getval;
    int begin = 0;
    int offset = cardinality / n_threads;
    thread * getval_threads = new thread[n_threads];
    int * local_results = new int[n_threads];
    fill_n(local_results, n_threads, -2);
    uint64_t *last_rub_t = new uint64_t[n_threads];
    fill_n(last_rub_t, n_threads, 0UL);
    uint64_t *last_rub_tsp_t = new uint64_t[n_threads];
    fill_n(last_rub_tsp_t, n_threads, 0UL);
    uint64_t last_rub_tsp = 0UL;

    assert(offset >= 1); 

    for (int i = 0; i < n_threads; i++) {
        int begin = i * offset;
        int range = offset;
        if ((i == (n_threads-1)) && (cardinality > n_threads))
            range += (cardinality % n_threads);

        getval_threads[i] = thread(&Nbub::_get_value, this, rowid, begin, range, l_timestamp,
                                &flag, &local_results[i], &last_rub_t[i], &last_rub_tsp_t[i]);
    }

    int ret = -1; 
    for (int t = 0; t < n_threads; t++) {
        getval_threads[t].join();
        int tmp = __atomic_load_n(&local_results[t], MM_CST);
        if (tmp != -1) {
#if defined(VERIFY_RESULTS)
            assert(ret == -1);
#endif
            ret = tmp;
        }

        if (last_rub_t[t]) {
#if defined(VERIFY_RESULTS)
            if (last_rub_tsp_t[t] == last_rub_tsp) {
                assert(last_rub_t[t] == *last_rub);
            }
#endif
            if (last_rub_tsp_t[t] > last_rub_tsp) {
                *last_rub = last_rub_t[t];
                last_rub_tsp = last_rub_tsp_t[t];
            }
        }
    }

    delete[] local_results;
    delete[] getval_threads;
    delete[] last_rub_t;
    delete[] last_rub_tsp_t;

    return ret;
}

void Nbub::printMemory() {
    uint64_t bitmap = 0, updateable_bitmap = 0, fence_pointers = 0;
    for (int i = 0; i < cardinality; ++i) {
        bitmap += bitmaps[i]->btv->getSerialSize();
        fence_pointers += bitmaps[i]->btv->index.size() * sizeof(int) * 2;

        // We assume there are on average two bitvectors in each version chain.
        // We can optimize this in the future.
        bitmap += bitmaps[i]->btv->getSerialSize();
        fence_pointers += bitmaps[i]->btv->index.size() * sizeof(int) * 2;
    }
    std::cout << "M FP " << fence_pointers << std::endl;
    std::cout << "M BM " << bitmap << std::endl;
}

void Nbub::printUncompMemory() {
    uint64_t bitmap = 0, fence_pointers = 0;
    for (int i = 0; i < cardinality; ++i) {
        bitmaps[i]->btv->appendActive();
        bitmaps[i]->btv->decompress();
        bitmap += bitmaps[i]->btv->getSerialSize();
        bitmaps[i]->btv->compress();
        fence_pointers += bitmaps[i]->btv->index.size() * sizeof(int) * 2;
    }
    std::cout << "UncM FP " << fence_pointers << std::endl;
    std::cout << "UncM BM " << bitmap << std::endl;
}

int Nbub::range(uint32_t start, uint32_t range) {
    ibis::bitvector res;
    res.set(0, total_rows);
    /*
    if (range_algo == "naive") {
        for (uint32_t i = 0; i < range; ++i) {
            res |= *(bitmaps[start + i]);
        }
    } else if (range_algo == "pq") {
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
    if (decode) {
        std::vector<uint32_t> dummy;
        res.decode(dummy);
        return 0;
    } else {
        return res.do_cnt();
    }
    */

   //// FIXME: remove this
    return res.do_cnt();
}

// This is a helper function
// which allows external applications (e.g., DBx1000 and MonetDB) to initialize the bitmap index.
// This function shouldn't be used as usual.
int Nbub::__init_append(int tid, int rowID, int val) 
{
    static mutex g_lock;

    lock_guard<mutex> guard(g_lock);
    if (config->on_disk) { }
    else {

        if (rowID >= g_number_of_rows) {
            g_number_of_rows = rowID + 1;

            for (int i = 0; i < config->g_cardinality; ++i) {
        	    bitmaps[i]->btv->adjustSize(0, g_number_of_rows);
            }
        }

        bitmaps[val]->btv->setBit(rowID, 1, config);

        // if (config->enable_fence_pointer) {
        //     bitmaps[val]->btv->index.clear();
        //     bitmaps[val]->btv->buildIndex();
        // }
    }

    return 0;
} 
