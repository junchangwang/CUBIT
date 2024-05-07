#include <iostream>
#include <tuple>
#include <atomic>

#include "table.h"
#include "fastbit/bitvector.h"
#include "nicolas/util.h"
#include "nicolas/segBtv.h"

using namespace cubit;
using namespace std;

// NOTE: DBx1000 selects 1 as the starting value of timestamp, so we stick to this.
#define TIMESTAMP_INIT_VAL (1UL)

Cubit::Cubit(Table_config *config) : 
    BaseTable(config), g_timestamp(TIMESTAMP_INIT_VAL), g_number_of_rows(config->n_rows)
{
    autoCommit = config->autoCommit;
    merge_threshold = config->n_merge_threshold;
    db_control = config->db_control;

    if (config->encoding == EE)
        num_bitmaps = config->g_cardinality;
    else if (config->encoding == AE || config->encoding == RE) 
        num_bitmaps = config->g_cardinality - 1;
    else 
        assert(0);

    TransDesc *trans_dummy = new TransDesc();

    trans_dummy->l_start_ts = 0UL;
    if (!db_control) {
        // Stand-alone CUBIT
        uint128_t data_t = __atomic_load_n((uint128_t *)&g_timestamp, MM_RELAXED);
        trans_dummy->l_commit_ts = (uint64_t)data_t;
        trans_dummy->l_number_of_rows = (uint64_t)(data_t>>64);
    } else {
        // Serve as a library in DBMS systems like DBx1000
        uint64_t data_t = __atomic_load_n(&db_timestamp, MM_RELAXED);
        trans_dummy->l_commit_ts = data_t;
        trans_dummy->l_number_of_rows = db_number_of_rows;
    } 

    assert(trans_dummy->l_commit_ts == TIMESTAMP_INIT_VAL);
    assert(trans_dummy->l_number_of_rows == config->n_rows);

    total_rows = config->n_rows + NBUB_PAD_BITS;
    
    int n_threads = (config->nThreads_for_getval > num_bitmaps) ? num_bitmaps : config->nThreads_for_getval;
    int n_btv_per_thread = num_bitmaps / n_threads;
    int n_left = num_bitmaps % n_threads;
    thread* threads = new thread[n_threads];

    assert(n_btv_per_thread >= 1);
    
    bitmaps = new Bitmap*[num_bitmaps];

    vector<int>begin (n_threads + 1, 0); 
    for(int i = 1; i <= n_left; i++)
        begin[i] = begin[i - 1] + n_btv_per_thread + 1;
    for(int i = n_left + 1; i <= n_threads; i++)
        begin[i] = begin[i - 1] + n_btv_per_thread;

    
    for (int i = 0; i < n_threads; i++) {
        threads[i] = thread(&Cubit::_read_btv, this, begin[i], begin[i + 1], trans_dummy, config);        
    } 
    for (int t = 0; t < n_threads; t++) {
        threads[t].join();
    }
    delete[] threads;

    n_trans_pool = config->n_workers*(config->n_udis+config->n_queries)*2;
    trans_pool = new TransDesc[n_trans_pool] {};
    assert(trans_pool);
    __atomic_store_n(&cnt_trans_used, 0, MM_RELAXED);

    if (config->segmented_btv) {
        uint64_t btv_size_t = 0;
        for (int i = 0; i < num_bitmaps; i++) {
            for (const auto &[id_t, seg_t] : bitmaps[i]->seg_btv->seg_table) {
                btv_size_t += seg_t.btv->getSerialSize();
            }
        }
        cout << "Bitmap size (MB): " << btv_size_t/1000000 << endl;
    }

    merge_req_queues = new queue<struct merge_req*>[config->n_workers];
    lk_merge_req_queues = new mutex[config->n_workers];

    cout << "=== Size of TransDesc pool: " << n_trans_pool 
        << "  autoCommit: " << autoCommit
        << "  merge_threshold: " << merge_threshold 
        << "  db_control: " << db_control
        << "  Segmented: " << config->segmented_btv
        << "  Segment Size: " << config->rows_per_seg
        << "  Enable Parallel CNT: " << config->enable_parallel_cnt
        << "  Para CNT threads: " << config->nThreads_for_getval
        << " ==="<< endl;
}

void Cubit::_read_btv(int begin, int end, TransDesc* trans_dummy, Table_config *config) 
{
    for (int i = begin; i < end; i++) {
        bitmaps[i] = new Bitmap{};
        bitmaps[i]->l_commit_ts = TIMESTAMP_INIT_VAL;
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
        if (config->segmented_btv) {
            // std::cout << "M BM " << btv_t->getSerialSize() << endl;

            bitmaps[i]->seg_btv = new SegBtv(config, btv_t);
            if (config->enable_fence_pointer) {
                bitmaps[i]->seg_btv->buildAllIndex();
            }

            // uint64_t bitmap_t = 0;
            // for (const auto &[id_t, seg_t] : bitmaps[i]->seg_btv->seg_table)
            //     bitmap_t += seg_t.btv->getSerialSize();
            // std::cout << "Seg M BM " << btv_t->getSerialSize() << endl << endl;

            delete btv_t;
        }
        else {
            __atomic_store_n(&bitmaps[i]->btv, btv_t, MM_RELEASE);
        }

        bitmaps[i]->next = NULL;
    } 
}

/*************************************
 *       Transaction Semantics       *
 ************************************/

TransDesc * Cubit::trans_begin(int tid, uint64_t db_timestamp_t)
{
    ThreadInfo *th = &g_ths_info[tid];

    if (READ_ONCE(th->active_trans))
        return (TransDesc *)th->active_trans;

    TransDesc *trans = allocate_trans();
    trans->l_end_trans = __atomic_load_n(&trans_queue->tail, MM_ACQUIRE);
    trans->l_commit_ts = INV_TIMESTAMP;

    if (!db_control) {
        uint128_t data_t = __atomic_load_n((uint128_t *)&g_timestamp, MM_RELAXED);
        trans->l_start_ts = (uint64_t)data_t;
        trans->l_number_of_rows = (uint64_t)(data_t>>64);

        // We need to retrieve a snapshot of the <trans_queue->tail, g_timestamp> pair.
        // However, new trans may have been inserted into TRX_LOG between accessing trans_queue->tail and g_timestamp.
        // We can use the following logic to detect and prevent this, 
        // because CUBIT guarantees to update tail and then timestamp, in order.
        while (trans->l_start_ts > READ_ONCE(trans->l_end_trans->l_commit_ts)) {
            assert(READ_ONCE(trans->l_end_trans->next));
            trans->l_end_trans = __atomic_load_n(&trans->l_end_trans->next, MM_ACQUIRE);
        }
    }
    else {
        if(db_timestamp_t != UINT64_MAX) {
            trans->l_start_ts = db_timestamp_t;
        }
        else {
            trans->l_start_ts = __atomic_load_n(&db_timestamp, MM_RELAXED); 
        }
        trans->l_number_of_rows = db_number_of_rows;
    }

    if (trans->l_start_ts < READ_ONCE(trans->l_end_trans->l_commit_ts)) {
        // New trans have completed
        trans->l_start_ts = READ_ONCE(trans->l_end_trans->l_commit_ts);
    }

    trans->l_inc_rows = 0;
    trans->next = NULL;
    __atomic_store_n(&th->active_trans, trans, MM_RELEASE);

    return (TransDesc *)th->active_trans;
}

/*************************************
 *         Buffer Management         *
 ************************************/

TransDesc * Cubit::allocate_trans() 
{
    uint64_t pos = __atomic_fetch_add(&cnt_trans_used, 1, MM_CST);
    assert(pos < n_trans_pool);

    return &trans_pool[pos];
}

int Cubit::delete_trans(int tid, TransDesc *trans) 
{
    // FIXME: No recycle yet
    //
    return 0;
}

/* Get RUBs in between (tsp_begin, tsp_end] */
TransDesc * Cubit::get_rubs_on_btv(uint64_t tsp_begin, uint64_t tsp_end, 
                        TransDesc *trans, uint32_t val, map<uint64_t, RUB> &rubs)
{
    assert(tsp_begin < tsp_end);

    // Check trans in between (tsp_begin, tsp_end].
    TransDesc *trans_prev = NULL;
    while (__atomic_load_n(&trans, MM_ACQUIRE) && (READ_ONCE(trans->l_commit_ts) <= tsp_end)) {
        if (READ_ONCE(trans->l_commit_ts) > tsp_begin) 
        {   // The normal case
            for (const auto & [row_id_t, rub_t] : trans->rubs)
            {
                if (rub_t.pos.count(val)) {
                    rubs[row_id_t] = rub_t;
                }
                else {
                    rubs.erase(row_id_t);
                }

            }
        } else if (READ_ONCE(trans->l_commit_ts) == tsp_begin) {
            ; // Skip the first trans
        } else {
            assert(trans->l_commit_ts == trans->l_start_ts);
            // assert(false);
        }

        trans_prev = trans;
        trans = READ_ONCE(trans->next);
    }

    return trans ? trans : trans_prev;
}

/* Get the last RUB in between (tsp_begin, tsp_end] of the specified row. */
TransDesc * Cubit::get_rub_on_row(uint64_t tsp_begin, uint64_t tsp_end, 
                        TransDesc *trans, uint64_t row_id, RUB &rub, uint64_t &rub_tsp)
{
    assert(tsp_begin < tsp_end);

    // Check trans in between (tsp_begin, tsp_end].
    TransDesc *trans_prev = NULL;
    while (__atomic_load_n(&trans, MM_ACQUIRE) && (READ_ONCE(trans->l_commit_ts) <= tsp_end)) {
        if (READ_ONCE(trans->l_commit_ts) > tsp_begin) 
        {
            for (const auto & [row_id_t, rub_t] : trans->rubs) 
            {
                if (row_id == row_id_t) {
                    rub = rub_t;
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
int Cubit::check_conflicts(TransDesc *trans, TransDesc *tail)
{
    TransDesc *end_trans_t = __atomic_load_n(&trans->l_end_trans, MM_ACQUIRE);
    TransDesc *end_trans_t_2 = __atomic_load_n(&end_trans_t->next, MM_ACQUIRE);

    while (end_trans_t_2 && READ_ONCE(end_trans_t_2->l_commit_ts) <= READ_ONCE(tail->l_commit_ts)) 
    {
        for (const auto & [row_t, rub_t] : trans->rubs) {
            // No need to check the rubs of insert operations since they will be updated 
            // to the correct row ids when submitting.
            if (rub_t.type != TYPE_INSERT) {
                if (end_trans_t_2->rubs.count(row_t)) {
                    #if defined(VERIFY_RESULTS)
                    cout << "NOTE: In check_conflict(): Other transactions have committed. " <<
                        " My ts: " << trans->l_commit_ts << " Conflicting ts: " << end_trans_t_2->l_commit_ts << 
                        ". Conflicting rows: " << row_t << endl;
                    #endif
                    return -1;
                }
            }
        }
        end_trans_t = end_trans_t_2;
        end_trans_t_2 = __atomic_load_n(&end_trans_t->next, MM_ACQUIRE);
    }
    // No conflict such that move l_end_trans forward.
    __atomic_store_n(&trans->l_end_trans, end_trans_t, MM_CST);

    return 0;
}

int Cubit::pos2RE(int start, int end, RLE_map &pos_re)
{
    for (int i = start; i <= end; i++)
        pos_re[i] = 1;

    return 0;
}

int Cubit::pos2AE(int start, int end, RLE_map &pos_AE)
{
    RLE_map pos_RE{};
    RLE_map pos_Anchors{};
    RLE_map pos_Curves_of_the_Anchors{};
    
    pos2RE(start, end, pos_RE);

    // Select the Anchors in pos_RE (the result of range encoding).
    // Record these Anchors in pos_AE because we first flip them.
    for (const auto & [pos_t, len_t] : pos_RE) {
        if (config->AE_anchors.count(pos_t)) {
            pos_Anchors[pos_t] = 1;
            pos_AE[pos_t] = 1;
        }
    }

    for (const auto & [pos_t, len_t] : pos_Anchors) {
        pos_RE.erase(pos_t);
    }

    // Record the corresponding Curves of each Anchor in pos_Anchors.
    for (const auto & [pos_t, len_t] : pos_Anchors) {
        for (int j = 1; j <= config->AE_interval && config->AE_curves.count(pos_t + j); j++)
            pos_Curves_of_the_Anchors[pos_t+j] = pos_t;
    }

    // For any Curves in pos_Curves_of_the_Anchors,
    // if they do not appear in pos_RE, record them in pos_AE
    // because they also need to be flipped.
    for (const auto & [pos_t, len_t] : pos_Curves_of_the_Anchors) {
        if ( !pos_RE.count(pos_t) ) {
            pos_AE[pos_t] = 1;
        }
        else {
            pos_RE.erase(pos_t);
        }
    }

    // For any Curves in pos_RE,
    // if they do not appear in pos_Curves_of_the_Anchors,
    // they need to be flipped becasue we didn't change the corresponding Anchors.
    for (const auto & [pos_t, len_t] : pos_RE) {
        assert( !pos_Curves_of_the_Anchors.count(pos_t) );
        pos_AE[pos_t] = 1;
    }

    return 0;
}

/*************************************
 *       Index manipulation          *
 ************************************/

int Cubit::append(int tid, int val, uint64_t row_id) 
{
    ThreadInfo *th = &g_ths_info[tid];
    assert(val < num_bitmaps);

    TransDesc *trans = (TransDesc *)__atomic_load_n(&th->active_trans, MM_ACQUIRE);

    if (!trans)
        trans = trans_begin(tid);

    RLE_map pos_map{};
    if (config->encoding == EE) {
        pos_map[val] = 1;
    } 
    else if (config->encoding == RE) {
        pos2RE(val, num_bitmaps-1, pos_map);
    } 
    else if (config->encoding == AE) {
        pos2AE(val, num_bitmaps-1, pos_map);
    }

    // We temporarily set row_id values to prevent the rubs of a transactions' insert operations 
    // from overwriting each other.
    if(row_id != UINT64_MAX) {
        trans->rubs[row_id] = 
                    RUB{row_id, TYPE_INSERT, pos_map};  
    }
    else {
        trans->rubs[trans->l_number_of_rows + trans->l_inc_rows] = 
                    RUB{trans->l_number_of_rows + trans->l_inc_rows, TYPE_INSERT, pos_map};
    }
    trans->l_inc_rows ++;

    if (autoCommit) {
        trans_commit(tid);
    }

    return 0;
}

int Cubit::update(int tid, uint64_t row_id, int to_val)
{
    ThreadInfo *th = &g_ths_info[tid];
    assert(to_val < num_bitmaps);

    rcu_read_lock();

    TransDesc * trans = (TransDesc *)__atomic_load_n(&th->active_trans, MM_ACQUIRE);

    // rcu_read_lock() must be invoked before accessing g_timestamp in trans_begin(),
    // to guarantee that the corresponding btv cannot be reclaimed before
    // we access it in get_value_rcu().

    if (!trans)
        trans = trans_begin(tid);

    RUB last_rub = RUB{0, TYPE_INV, {}};
    int from_val = get_value_rcu(row_id, trans->l_start_ts, last_rub);
    assert(from_val < num_bitmaps);

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

    RLE_map pos_map{};
    int min, max;
    if (from_val < to_val) {
        min = from_val;
        max = to_val - 1;
    } else {
        min = to_val;
        max = from_val - 1;
    }
    if (config->encoding == EE) {
        pos_map[from_val] = 1;
        pos_map[to_val] = 1;
    }
    else if (config->encoding == RE) {
        pos2RE(min, max, pos_map);
    }
    else if (config->encoding == AE) {
        pos2AE(min, max, pos_map);
    }

    for (const auto & [pos_t, len_t] : last_rub.pos) {
        if (pos_map.count(pos_t)) {
            pos_map.erase(pos_t);
        } else {
            pos_map[pos_t] = 1;
        }
    }

    trans->rubs[row_id] = RUB{row_id, TYPE_UPDATE, pos_map};

    if (autoCommit) {
        trans_commit(tid);
    }

    return 0;
}

int Cubit::remove(int tid, uint64_t row_id)
{
    struct ThreadInfo *th = &g_ths_info[tid];
    assert(row_id < g_number_of_rows);

    rcu_read_lock();

    struct TransDesc * trans = (TransDesc *)__atomic_load_n(&th->active_trans, MM_ACQUIRE);

    if (!trans)
        trans = trans_begin(tid);

    RUB last_rub = RUB{0, TYPE_INV, {}};
    int val = get_value_rcu(row_id, trans->l_start_ts, last_rub);
    assert(val < num_bitmaps);

    rcu_read_unlock();

    if (val == -1) {
        #if defined(VERIFY_RESULTS)
        cout << "Remove: The value at row " << row_id << " has been deleted." <<
            "I will return." << endl;
        #endif
        return -ENOENT;
    }

    RLE_map pos_map{};
    if (config->encoding == EE) {
        pos_map[val] = 1;
    }    
    else if (config->encoding == RE) {
        pos2RE(val, num_bitmaps-1, pos_map);
    }
    else if (config->encoding == AE) {
        pos2AE(val, num_bitmaps-1, pos_map);
    }

    for (const auto & [pos_t, len_t] : last_rub.pos) {
        if (pos_map.count(pos_t)) {
            // May generate RUBs containing zero pos, which is what we want.
            pos_map.erase(pos_t);
        } else {
            pos_map[pos_t] = 1;
        }
    }

    trans->rubs[row_id] = RUB{row_id, TYPE_DELETE, pos_map};

    if (autoCommit) {
        trans_commit(tid);
    }

    return 0;
}

int Cubit::evaluate_Curve(int tid, uint32_t val)
{
    ThreadInfo *th = &g_ths_info[tid];
    assert(val < num_bitmaps);

    rcu_read_lock();

    TransDesc *trans = (TransDesc *)__atomic_load_n(&th->active_trans, MM_ACQUIRE);
    bool reuse_bitmap_curve = true;

    if (!trans)
        trans = trans_begin(tid);

    auto t1 = std::chrono::high_resolution_clock::now();

    // Processing val_curve
    auto val_curve = val;
    Bitmap *bitmap_old_curve = __atomic_load_n(&bitmaps[val_curve], MM_ACQUIRE);

    /* Need to check RUBs in between (tsp_begin, tsp_end]. */
    uint64_t tsp_begin_curve = READ_ONCE(bitmap_old_curve->l_commit_ts);
    uint64_t tsp_end_curve = READ_ONCE(trans->l_start_ts);
    while (tsp_begin_curve > tsp_end_curve) {
        #if defined(VERIFY_RESULTS)
        assert(READ_ONCE(bitmap_old_curve->next));
        cout << "Note: MVCC evaluate() moves from ts " << bitmap_old_curve->l_commit_ts << 
                " to ts " << bitmap_old_curve->next->l_commit_ts << endl;
        #endif
        bitmap_old_curve = __atomic_load_n(&bitmap_old_curve->next, MM_ACQUIRE);
        tsp_begin_curve = READ_ONCE(bitmap_old_curve->l_commit_ts); 
        reuse_bitmap_curve = false;
    }

    map<uint64_t, RUB> rubs_curve{};
    if (tsp_begin_curve < tsp_end_curve) {
        get_rubs_on_btv(tsp_begin_curve, tsp_end_curve, READ_ONCE(bitmap_old_curve->l_start_trans), val_curve, rubs_curve);
    }

    // Processing val_anchor
    auto val_anchor = config->AE_curves[val_curve];
    Bitmap *bitmap_old_anchor = __atomic_load_n(&bitmaps[val_anchor], MM_ACQUIRE);

    /* Need to check RUBs in between (tsp_begin, tsp_end]. */
    uint64_t tsp_begin_anchor = READ_ONCE(bitmap_old_anchor->l_commit_ts);
    uint64_t tsp_end_anchor = READ_ONCE(trans->l_start_ts);
    while (tsp_begin_anchor > tsp_end_anchor) {
        #if defined(VERIFY_RESULTS)
        assert(READ_ONCE(bitmap_old_anchor->next));
        cout << "Note: MVCC evaluate() moves from ts " << bitmap_old_anchor->l_commit_ts << 
                " to ts " << bitmap_old_anchor->next->l_commit_ts << endl;
        #endif
        bitmap_old_anchor = __atomic_load_n(&bitmap_old_anchor->next, MM_ACQUIRE);
        tsp_begin_anchor = READ_ONCE(bitmap_old_anchor->l_commit_ts);
    }

    map<uint64_t, RUB> rubs_anchor{};
    if (tsp_begin_anchor < tsp_end_anchor) {
        get_rubs_on_btv(tsp_begin_anchor, tsp_end_anchor, READ_ONCE(bitmap_old_anchor->l_start_trans), val_anchor, rubs_anchor);
    }

    auto t2 = std::chrono::high_resolution_clock::now();

    uint64_t cnt = 0UL;
    #if defined(VERIFY_RESULTS)
        uint64_t cnt_test = 0UL;
    #endif
    SegBtv *my_seg_btv = nullptr;
    ibis::bitvector *my_btv = nullptr;

    if (config->segmented_btv) {
        my_seg_btv = new SegBtv(*bitmap_old_curve->seg_btv);
        my_seg_btv->deepCopy(*bitmap_old_curve->seg_btv);
        *my_seg_btv ^= *bitmap_old_anchor->seg_btv;
        #if defined(VERIFY_RESULTS)
            cnt_test = my_seg_btv->do_cnt();
        #endif
    } else {
        my_btv = *bitmap_old_curve->btv ^ *bitmap_old_anchor->btv;
    }

    auto t3 = std::chrono::high_resolution_clock::now();

    if (config->decode) {
        std::vector<uint32_t> dummy;
        if (config->segmented_btv)
            my_seg_btv->decode(dummy, config);
        else
            my_btv->decode(dummy, config);
        cnt = 0;
    } else {
        if (config->segmented_btv) {
            if (config->enable_parallel_cnt) {
                cnt = my_seg_btv->do_cnt_parallel(config);
                #if defined(VERIFY_RESULTS)
                    assert(cnt == cnt_test);
                #endif
            }
            else {
                cnt = my_seg_btv->do_cnt();
            }
        }
        else{
            cnt = my_btv->do_cnt();
        }
    }

    auto t4 = std::chrono::high_resolution_clock::now();

    for (const auto & [row_id_t, rub_t] : rubs_curve) {
        assert(rub_t.pos.count(val_curve));
        if (config->segmented_btv) {
            cnt += my_seg_btv->getBit(row_id_t, config) ? (-1) : (1);
        }
        else {
            cnt += my_btv->getBit(row_id_t, config) ? (-1) : (1);
        }
    }

    for (const auto & [row_id_t, rub_t] : rubs_anchor) {
        assert(rub_t.pos.count(val_anchor));
        if (config->segmented_btv) {
            cnt += my_seg_btv->getBit(row_id_t, config) ? (-1) : (1);
        }
        else {
            cnt += my_btv->getBit(row_id_t, config) ? (-1) : (1);
        }
    }

    auto t5 = std::chrono::high_resolution_clock::now();
    auto t6 = t5;

    struct Bitmap *bitmap_new_curve = new Bitmap{};
    __atomic_store_n(&bitmap_new_curve->btv, my_btv, MM_CST);
    __atomic_store_n(&bitmap_new_curve->seg_btv, my_seg_btv, MM_CST);

    if ((rubs_curve.size() >= merge_threshold) && reuse_bitmap_curve) 
    {
        if (config->segmented_btv) {
            my_seg_btv->deepCopy(*bitmap_old_curve->seg_btv);
            for (const auto & [row_id_t, rub_t] : rubs_curve) {
                assert(rub_t.pos.count(val_curve));
                my_seg_btv->setBit(row_id_t, !my_seg_btv->getBit(row_id_t, config), config);
            }
        } else {
            my_btv->copy(*bitmap_old_curve->btv);
            for (const auto & [row_id_t, rub_t] : rubs_curve) {
                assert(rub_t.pos.count(val_curve));
                my_btv->setBit(row_id_t, !my_btv->getBit(row_id_t, config), config);
            }
        }

        map<uint64_t, RUB> *rubs_t = new map<uint64_t, RUB>(rubs_curve);
        struct merge_req *req = new merge_req{tid, val_curve, trans, bitmap_old_curve, bitmap_new_curve, rubs_t};

        // FIXME: In case that academically wait-free is acquired,
        //        Michael's wait-free SPSC queue can be used.
        lock_guard<mutex> lock(lk_merge_req_queues[tid]);
        merge_req_queues[tid].push(req);

        t6 = std::chrono::high_resolution_clock::now();

        // cout << "[Evaluate(ms)] [Curve column] [Merge]" <<
        //     "    Total: " << to_string(std::chrono::duration_cast<std::chrono::microseconds>(t6-t1).count()) <<
        //     "    Get btvs: " << to_string(std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count()) <<
        //     "    XOR: " << to_string(std::chrono::duration_cast<std::chrono::microseconds>(t3-t2).count()) <<
        //     "    Count: " << to_string(std::chrono::duration_cast<std::chrono::microseconds>(t4-t3).count()) <<
        //     "    Process Rubs: " << to_string(std::chrono::duration_cast<std::chrono::microseconds>(t5-t4).count()) <<
        //     "    MergeReq: " << to_string(std::chrono::duration_cast<std::chrono::microseconds>(t6-t5).count()) << endl;
    } 
    else {
        call_rcu(&bitmap_new_curve->head, free_bitmap_cb);
        t6 = std::chrono::high_resolution_clock::now();

        // cout << "[Evaluate(ms)] [Curve column] [No Merge]" <<
        //     "    Total: " << to_string(std::chrono::duration_cast<std::chrono::microseconds>(t6-t1).count()) <<
        //     "    Get btvs: " << to_string(std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count()) <<
        //     "    XOR: " << to_string(std::chrono::duration_cast<std::chrono::microseconds>(t3-t2).count()) <<
        //     "    Count: " << to_string(std::chrono::duration_cast<std::chrono::microseconds>(t4-t3).count()) <<
        //     "    Process Rubs: " << to_string(std::chrono::duration_cast<std::chrono::microseconds>(t5-t4).count()) <<
        //     "    MergeReq: " << to_string(std::chrono::duration_cast<std::chrono::microseconds>(t6-t5).count()) << endl;
    }

    rcu_read_unlock();

    if (autoCommit) {
        assert(trans_commit(tid) == -ENOENT);
    }

    return cnt;
}

int Cubit::evaluate_common(int tid, uint32_t val)
{   
    ThreadInfo *th = &g_ths_info[tid];
    assert(val < num_bitmaps);

    rcu_read_lock();

    TransDesc *trans = (TransDesc *)__atomic_load_n(&th->active_trans, MM_ACQUIRE);
    bool reuse_bitmap = true;

    if (!trans)
        trans = trans_begin(tid);

    auto t1 = std::chrono::high_resolution_clock::now();

    Bitmap *bitmap_old = __atomic_load_n(&bitmaps[val], MM_ACQUIRE);

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

    map<uint64_t, RUB> rubs{};
    if (tsp_begin < tsp_end) {
        get_rubs_on_btv(tsp_begin, tsp_end, READ_ONCE(bitmap_old->l_start_trans), val, rubs);
    }

    auto t2 = std::chrono::high_resolution_clock::now();

    uint64_t cnt = 0UL;
    #if defined(VERIFY_RESULTS)
        uint64_t cnt_test = 0UL;
    #endif
    ibis::bitvector *old_btv = READ_ONCE(bitmap_old->btv);
    SegBtv *old_seg_btv = READ_ONCE(bitmap_old->seg_btv);
    
    if ((rubs.size() < merge_threshold) || !reuse_bitmap) 
    {   // will not merge
        if (config->decode) {
            std::vector<uint32_t> dummy;
            if (config->segmented_btv)
                old_seg_btv->decode(dummy, config);
            else
                old_btv->decode(dummy, config);
            cnt = 0;
        } else {
            // cnt = config->segmented_btv ? old_seg_btv->do_cnt() : old_btv->do_cnt();
            if (config->segmented_btv) {
                if (config->enable_parallel_cnt) {
                    cnt = old_seg_btv->do_cnt_parallel(config);
                    #if defined(VERIFY_RESULTS)
                        cnt_test = old_seg_btv->do_cnt();
                        assert(cnt == cnt_test);
                    #endif
                }
                else {
                    cnt = old_seg_btv->do_cnt();
                }
            }
            else {
                cnt = old_btv->do_cnt();
            }
        }

        auto t3 = std::chrono::high_resolution_clock::now();

        for (const auto &[row_id_t, rub_t] : rubs) {
            assert(rub_t.pos.count(val));
            if (config->segmented_btv) {
                cnt += old_seg_btv->getBit(row_id_t, config) ? (-1) : (1);
            }
            else {
                cnt += old_btv->getBit(row_id_t, config) ? (-1) : (1);
            }
        }

        auto t4 = std::chrono::high_resolution_clock::now();

        // cout << "[Evaluate(ms)] [Non-Curve] [No merge]" <<
        //     "    Total: " << to_string(std::chrono::duration_cast<std::chrono::microseconds>(t4-t1).count()) <<
        //     "    Get btvs: " << to_string(std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count()) <<
        //     "    Count: " << to_string(std::chrono::duration_cast<std::chrono::microseconds>(t3-t2).count()) <<
        //     "    Process Rubs: " << to_string(std::chrono::duration_cast<std::chrono::microseconds>(t4-t3).count()) << endl;
    }
    else { // To merge
        auto t3 = t2;
        SegBtv *new_seg_btv = nullptr;
        ibis::bitvector *new_btv = nullptr; 
        if (config->segmented_btv) {
            new_seg_btv = new SegBtv(*bitmap_old->seg_btv);
            
            new_seg_btv->adjustSize(0, bitmap_old->seg_btv->get_rows());

            for (auto const & [row_id_t, rub_t] : rubs) {
                assert(rub_t.pos.count(val));
                //new_seg_btv->setBit(row_id_t, !new_seg_btv->getBit(row_id_t, config), config);
                new_seg_btv->setBit(row_id_t, 1, config);
            }

            *new_seg_btv ^= *bitmap_old->seg_btv;

            t3 = std::chrono::high_resolution_clock::now();

            if (config->decode) {
                std::vector<uint32_t> dummy;
                new_seg_btv->decode(dummy, config);
                cnt = 0;
            } else {
                if (config->enable_parallel_cnt) {
                    cnt = new_seg_btv->do_cnt_parallel(config);
                }
                else {
                    cnt = new_seg_btv->do_cnt();
                }
            }
        }
        else {
            new_btv = new ibis::bitvector{};
            *new_btv ^= *bitmap_old->btv;

            for (auto const & [row_id_t, rub_t] : rubs) {
                assert(rub_t.pos.count(val));
                // new_btv->setBit(row_id_t, !new_btv->getBit(row_id_t, config), config);
                new_btv->setBit(row_id_t, 1, config);
            }

            // *new_btv ^= *bitmap_old->btv;

            t3 = std::chrono::high_resolution_clock::now();

            if (config->decode) {
                std::vector<uint32_t> dummy;
                new_btv->decode(dummy, config);
                cnt = 0;
            } else {
                cnt = new_btv->do_cnt();
            }
        }

        auto t4 = std::chrono::high_resolution_clock::now();

        struct Bitmap *bitmap_new = new Bitmap{};
        __atomic_store_n(&bitmap_new->btv, new_btv, MM_RELEASE);
        __atomic_store_n(&bitmap_new->seg_btv, new_seg_btv, MM_RELEASE);

        map<uint64_t, RUB> *rubs_t = new map<uint64_t, RUB>(rubs);
        struct merge_req *req = new merge_req{tid, val, trans, bitmap_old, bitmap_new, rubs_t};

        // FIXME: In case that academically wait-free is acquired,
        //        Michael's wait-free SPSC queue can be used.
        lock_guard<mutex> lock(lk_merge_req_queues[tid]);
        merge_req_queues[tid].push(req);

        auto t5 = std::chrono::high_resolution_clock::now();

        // cout << "[Evaluate(ms)] [Non-Curve] [Merge]" <<
        //     "    Total: " << to_string(std::chrono::duration_cast<std::chrono::microseconds>(t5-t1).count()) <<
        //     "    Get btvs: " << to_string(std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count()) <<
        //     "    XOR: " << to_string(std::chrono::duration_cast<std::chrono::microseconds>(t3-t2).count()) <<
        //     "    Count: " << to_string(std::chrono::duration_cast<std::chrono::microseconds>(t4-t3).count()) <<
        //     "    RegMerge: " << to_string(std::chrono::duration_cast<std::chrono::microseconds>(t5-t4).count()) << endl;
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

int Cubit::evaluate(int tid, uint32_t val)
{
    if (config->encoding == EE || config->encoding == RE) 
    {
        return evaluate_common(tid, val);
    }
    else if (config->encoding == AE) 
    {  
        if ( config->AE_curves.count(val) ) {
            return evaluate_Curve(tid, val);
        }

        return evaluate_common(tid, val);
    }
    else {
        assert(0);
    }
}

void Cubit::_get_value(uint64_t row_id, int begin, int range, uint64_t l_timestamp,
        bool *flag, int *result, RUB *rub, uint64_t *rub_tsp)
{
    int ret = -1;

    if (config->encoding == EE) {
        if (__atomic_load_n(flag, MM_CST)) 
            goto out;
    }

    for(int i = 0; i < range; i++) {
        int curVal = begin + i;
        RUB rub_t = RUB{0, TYPE_INV, {}};
        uint64_t rub_tsp_t = 0UL;
        
        if (config->encoding == EE) {
            if (__atomic_load_n(flag, MM_CST))
                break;
        }

        Bitmap *bitmap = __atomic_load_n(&bitmaps[curVal], MM_ACQUIRE);
        uint64_t tsp_begin = READ_ONCE(bitmap->l_commit_ts);
        uint64_t tsp_end = l_timestamp;

        // Search the bitvector chain.
        while (tsp_begin > tsp_end) {
            #if defined(VERIFY_RESULTS)
            cout << "MVCC _get_value() moves from ts: " << bitmap->l_commit_ts << 
               " to ts: " << bitmap->next->l_commit_ts << 
               "   tsp_end: " << tsp_end <<
               "   g_timestamp: " << READ_ONCE(g_timestamp) << endl;
            #endif
            bitmap = __atomic_load_n(&bitmap->next, MM_ACQUIRE);
            assert(bitmap);
            assert(READ_ONCE(bitmap->l_commit_ts) < tsp_begin);
        
            tsp_begin = READ_ONCE(bitmap->l_commit_ts); 
        }

        int bit1 = 0;
        int bit2 = 0;
        if (config->segmented_btv) {
            SegBtv *p = __atomic_load_n(&bitmap->seg_btv, MM_ACQUIRE);
            bit1 = p->getBit(row_id, config);
        }
        else {
            ibis::bitvector *p = __atomic_load_n(&bitmap->btv, MM_ACQUIRE);
            bit1 = p->getBit(row_id, config);
        }

        if (tsp_begin < tsp_end) { 
            get_rub_on_row(tsp_begin, tsp_end, READ_ONCE(bitmap->l_start_trans),
                    row_id, rub_t, rub_tsp_t);
        }

        if (rub_t.type != TYPE_INV && rub_t.pos.count(curVal))
            bit2 = 1;

        // Record the rub value with the largest timestamp value.
        if (rub_t.type != TYPE_INV) {
            #if defined(VERIFY_RESULTS)
            if (rub_tsp_t == *rub_tsp) {
                assert(rub_t.pos == rub->pos);
            }
            #endif
            if (rub_tsp_t > *rub_tsp) {
                *rub = rub_t;
                *rub_tsp = rub_tsp_t;
            }
        }

        auto result = bit1 ^ bit2;

        if (config->encoding == AE && config->AE_curves.count(curVal)) {
            auto val_anchor = config->AE_curves[curVal];
            Bitmap *bitmap_anchor = __atomic_load_n(&bitmaps[val_anchor], MM_ACQUIRE);

            /* Need to check RUBs in between (tsp_begin, tsp_end]. */
            uint64_t tsp_begin_anchor = READ_ONCE(bitmap_anchor->l_commit_ts);
            uint64_t tsp_end_anchor = READ_ONCE(l_timestamp);
            while (tsp_begin_anchor > tsp_end_anchor) {
                assert(READ_ONCE(bitmap_anchor->next));
                bitmap_anchor = __atomic_load_n(&bitmap_anchor->next, MM_ACQUIRE);
                tsp_begin_anchor = READ_ONCE(bitmap_anchor->l_commit_ts);
            }

            int bit1_anchor = config->segmented_btv ? bitmap_anchor->seg_btv->getBit(row_id, config) :
                        bitmap_anchor->btv->getBit(row_id, config);
            int bit2_anchor = 0;

            rub_t = RUB{0, TYPE_INV, {}};
            rub_tsp_t = 0UL;
            if (tsp_begin_anchor < tsp_end_anchor) {
                get_rub_on_row(tsp_begin_anchor, tsp_end_anchor, READ_ONCE(bitmap_anchor->l_start_trans),
                        row_id, rub_t, rub_tsp_t);
            }

            if (rub_t.type != TYPE_INV && rub_t.pos.count(val_anchor))
                bit2_anchor = 1;

            result ^= bit1_anchor;
            result ^= bit2_anchor;
        }

        if (result == 1) {
            if (config->encoding == EE) {
                ret = curVal;
                __atomic_store_n(flag, true, MM_CST);
                break;
            }
            else if (config->encoding == RE || config->encoding == AE) {
                if (ret == -1) {
                    ret = curVal;
                } else {
                    ret = (curVal < ret) ? curVal : ret;
                }
            }
        }
    }

out:
    __atomic_store_n(result, ret, MM_RELEASE);
}

int Cubit::get_value_rcu(uint64_t row_id, uint64_t l_timestamp, RUB &last_rub) 
{
    bool flag = false;
    int n_threads = (config->nThreads_for_getval > num_bitmaps) ? num_bitmaps : config->nThreads_for_getval;
    int begin = 0;
    int offset = num_bitmaps / n_threads;
    thread * getval_threads = new thread[n_threads];
    int * local_results = new int[n_threads];
    fill_n(local_results, n_threads, -2);
    RUB *last_rub_t = new RUB[n_threads]{};
    uint64_t *last_rub_tsp_t = new uint64_t[n_threads];
    fill_n(last_rub_tsp_t, n_threads, 0UL);
    uint64_t last_rub_tsp = 0UL;

    assert(offset >= 1); 

    for (int i = 0; i < n_threads; i++) {
        int begin = i * offset;
        int range = offset;
        if ((i == (n_threads-1)) && (num_bitmaps > n_threads))
            range += (num_bitmaps % n_threads);

        getval_threads[i] = thread(&Cubit::_get_value, this, row_id, begin, range, l_timestamp,
                                &flag, &local_results[i], &last_rub_t[i], &last_rub_tsp_t[i]);
    }

    int ret = -1; 
    for (int t = 0; t < n_threads; t++) {
        getval_threads[t].join();
        int tmp = __atomic_load_n(&local_results[t], MM_CST);
        if (tmp != -1) {
            if (config->encoding == EE) {
                assert(ret == -1);
                ret = tmp;
            }
            else if (config->encoding == RE || config->encoding == AE) {
                if (ret == -1) {
                    ret = tmp;
                } else {
                    ret = (tmp < ret) ? tmp : ret;
                }
            }
        }

        if (last_rub_t[t].type != TYPE_INV) {
            #if defined(VERIFY_RESULTS)
            if (last_rub_tsp_t[t] == last_rub_tsp) {
                assert(last_rub_t[t].pos == last_rub.pos);
            }
            #endif
            if (last_rub_tsp_t[t] > last_rub_tsp) {
                last_rub = last_rub_t[t];
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

void Cubit::printMemory() {
    uint64_t bitmap = 0, updateable_bitmap = 0, fence_pointers = 0;
    for (int i = 0; i < num_bitmaps; ++i) {
        bitmap += bitmaps[i]->btv->getSerialSize();
        fence_pointers += bitmaps[i]->btv->index.size() * sizeof(int) * 2;
    }
    std::cout << "M FP " << fence_pointers << std::endl;
    std::cout << "M BM " << bitmap << std::endl;
}

void Cubit::printMemorySeg() {
    uint64_t bitmap = 0, updateable_bitmap = 0, fence_pointers = 0;
    for (int i = 0; i < num_bitmaps; ++i) {
        for (const auto & [id_t, seg_t] : bitmaps[i]->seg_btv->seg_table) {
            bitmap += seg_t.btv->getSerialSize();
            fence_pointers += seg_t.btv->index.size() * sizeof(int) * 2;
        }
    }
    std::cout << "Seg M FP " << fence_pointers << std::endl;
    std::cout << "Seg M BM " << bitmap << std::endl;
}

void Cubit::printUncompMemory() {
    uint64_t bitmap = 0, fence_pointers = 0;
    for (int i = 0; i < num_bitmaps; ++i) {
        bitmaps[i]->btv->appendActive();
        bitmaps[i]->btv->decompress();
        bitmap += bitmaps[i]->btv->getSerialSize();
        bitmaps[i]->btv->compress();
        fence_pointers += bitmaps[i]->btv->index.size() * sizeof(int) * 2;
    }
    std::cout << "UncM FP " << fence_pointers << std::endl;
    std::cout << "UncM BM " << bitmap << std::endl;
}

void Cubit::printUncompMemorySeg() {
    uint64_t bitmap = 0, fence_pointers = 0;
    for (int i = 0; i < num_bitmaps; ++i) {
        for (const auto & [id_t, seg_t] : bitmaps[i]->seg_btv->seg_table) {
            seg_t.btv->appendActive();
            seg_t.btv->decompress();
            bitmap += seg_t.btv->getSerialSize();
            seg_t.btv->compress();
            fence_pointers += seg_t.btv->index.size() * sizeof(int) * 2;
        }
    }
    std::cout << "Seg UncM FP " << fence_pointers << std::endl;
    std::cout << "Seg UncM BM " << bitmap << std::endl;
}

int Cubit::range(uint32_t start, uint32_t range) {
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
int Cubit::__init_append(int tid, int rowID, int val) 
{
    static mutex g_lock;

    lock_guard<mutex> guard(g_lock);
    if (config->on_disk) { }
    else {

        if (rowID >= g_number_of_rows) {
            g_number_of_rows = rowID + 1;

            for (int i = 0; i < num_bitmaps; ++i) {
        	    bitmaps[i]->btv->adjustSize(0, g_number_of_rows);
                if (config->segmented_btv) {
                    bitmaps[i]->seg_btv->adjustSize(0, g_number_of_rows);
                }
            }
        }

        bitmaps[val]->btv->setBit(rowID, 1, config);
        if (config->segmented_btv) {
            bitmaps[val]->seg_btv->setBit(rowID, 1, config);
        }

        // if (config->enable_fence_pointer) {
        //     bitmaps[val]->btv->index.clear();
        //     bitmaps[val]->btv->buildIndex();
        // }
    }

    return 0;
} 


// int Cubit::evaluate_common(int tid, uint32_t val)
// {   
//     ThreadInfo *th = &g_ths_info[tid];
//     assert(val < cardinality);

//     rcu_read_lock();

//     TransDesc *trans = (TransDesc *)__atomic_load_n(&th->active_trans, MM_ACQUIRE);
//     bool reuse_bitmap = true;

//     if (!trans)
//         trans = trans_begin(tid);

//     Bitmap *bitmap_old = __atomic_load_n(&bitmaps[val], MM_ACQUIRE);

//     /* Need to check RUBs in between (tsp_begin, tsp_end]. */
//     uint64_t tsp_begin = READ_ONCE(bitmap_old->l_commit_ts);
//     uint64_t tsp_end = READ_ONCE(trans->l_start_ts);
//     while (tsp_begin > tsp_end) {
//         #if defined(VERIFY_RESULTS)
//         assert(READ_ONCE(bitmap_old->next));
//         cout << "Note: MVCC evaluate() moves from ts " << bitmap_old->l_commit_ts << 
//                 " to ts " << bitmap_old->next->l_commit_ts << endl;
//         #endif
//         bitmap_old = __atomic_load_n(&bitmap_old->next, MM_ACQUIRE);
//         tsp_begin = READ_ONCE(bitmap_old->l_commit_ts); 
//         reuse_bitmap = false;
//     }

//     map<uint64_t, RUB> rubs{};
//     if (tsp_begin < tsp_end) {
//         get_rubs_on_btv(tsp_begin, tsp_end, READ_ONCE(bitmap_old->l_start_trans), val, rubs);
//     }

//     int cnt = 0;
//     ibis::bitvector *my_btv = NULL;
//     if (rubs.size() == 0) {
//         // Reuse old bitvector. No need to allocate a new one.
//         my_btv = READ_ONCE(bitmap_old->btv);

//         if (config->decode) {
//             std::vector<uint32_t> dummy;
//             my_btv->decode(dummy, config);
//             cnt = 0;
//         } else {
//             cnt = my_btv->do_cnt();
//         }
//     }
//     else {

// /********** Straight forward approach **********/
// //        my_btv = new ibis::bitvector();
// //        my_btv->copy(*bitmap_old->btv);
// //        // Building index has been delegated to the merge daemon.
// //
// //        for (auto const &[row_id_t, rub_t] : rubs) {
// //            auto [row_id_t_2, idx1, idx2, type] = decode_rub(rub_t);
// //            assert(idx1 == val || idx2 == val);
// //            // vb = vb ^ 1 = !vb
// //            // NOTE:: do not use getBitWithIndex() to avoid
// //            //        rebuilding index for each iteration
// //            my_btv->setBit(row_id_t, !my_btv->getBit(row_id_t, config), config);  
// //        }
        
// /********** Optimized approach **********/

//         auto t1 = std::chrono::high_resolution_clock::now();

//         my_btv = new ibis::bitvector{};

//         // map<uint32_t, uint64_t>::iterator it;
//         // for (it = rubs.begin(); it != rubs.end(); it++) {
//             // auto [row_id_t_2, idx1, idx2, type] = decode_rub(it->second);
//             // assert(idx1 == val || idx2 == val);
//             // NOTE:: do not use getBitWithIndex() to avoid
//             //        rebuilding index for each iteration
//             // my_btv->setBit(it->first, 1, config);  
//         // }

//         for (auto const & [row_id_t, rub_t] : rubs) {
//             // FIXME: RLE
//             assert(rub_t.pos.count(val));
//             my_btv->setBit(row_id_t, 1, config);
//         }

//         auto t2 = std::chrono::high_resolution_clock::now();

//         *my_btv ^= *bitmap_old->btv;

//         auto t3 = std::chrono::high_resolution_clock::now();

//         if (config->decode) {
//             std::vector<uint32_t> dummy;
//             my_btv->decode(dummy, config);
//             cnt = 0;
//         } else {
//             cnt = my_btv->do_cnt();
//         }

//         auto t4 = std::chrono::high_resolution_clock::now();

//         struct Bitmap *bitmap_new = new Bitmap{};
//         __atomic_store_n(&bitmap_new->btv, my_btv, MM_RELEASE);

//         // Perform Merge operation
//         if (reuse_bitmap && (rubs.size() >= MERGE_THRESH)) 
//         {
//             // map<uint32_t, uint64_t> *rubs_t = new map<uint32_t, uint64_t>(rubs);
//             map<uint64_t, RUB> *rubs_t = new map<uint64_t, RUB>(rubs);
//             struct merge_req *req = new merge_req{tid, val, trans, bitmap_old, bitmap_new, rubs_t};

//             // FIXME: In case that academically wait-free is acquired,
//             //        Michael's wait-free SPSC queue can be used.
//             lock_guard<mutex> lock(lk_merge_req_queues[tid]);
//             merge_req_queues[tid].push(req);

//             auto t5 = std::chrono::high_resolution_clock::now();

//             cout << "[Query time (ms)]" <<
//                 "    Total: " << to_string(std::chrono::duration_cast<std::chrono::microseconds>(t5-t1).count()) <<
//                 "    Setbits: " << to_string(std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count()) <<
//                 "    XOR: " << to_string(std::chrono::duration_cast<std::chrono::microseconds>(t3-t2).count()) <<
//                 "    Count: " << to_string(std::chrono::duration_cast<std::chrono::microseconds>(t4-t3).count()) <<
//                 "    RegMerge: " << to_string(std::chrono::duration_cast<std::chrono::microseconds>(t5-t4).count()) << endl;
//         } 
//         else {
//             call_rcu(&bitmap_new->head, free_bitmap_cb);
//         }
//     }

//     // This barrier can be moved forward.
//     // However, performance improvement is negligibal since
//     // the memory reclamation has been delegated to background threads.
//     // We leave it here to make the code easy to understand.
//     rcu_read_unlock();

//     if (autoCommit) {
//         assert(trans_commit(tid) == -ENOENT);
//     }

//     return cnt;
// }
