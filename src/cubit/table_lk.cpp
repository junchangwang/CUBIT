#include <iostream>
#include <tuple>
#include <atomic>

#include "table_lk.h"
#include "fastbit/bitvector.h"
#include "nicolas/util.h"
#include "nicolas/base_table.h"

using namespace std;
using namespace cubit_lk;

CubitLK::CubitLK(Table_config *config) : Cubit(config)
{
    trans_queue = new cubit::queue_t{};
    __atomic_store_n(&trans_queue->head, bitmaps[0]->l_start_trans, MM_RELAXED);
    __atomic_store_n(&trans_queue->tail, bitmaps[0]->l_start_trans, MM_RELAXED);
}

/*************************************
 *       Transaction Semantics       *
 ************************************/

int CubitLK::trans_commit(int tid, uint64_t db_timestamp_t, uint64_t db_row_nums) 
{
    ThreadInfo *th = &g_ths_info[tid];
    TransDesc *trans = (TransDesc *)__atomic_load_n(&th->active_trans, MM_ACQUIRE);

    if (!trans) {
#if defined(VERIFY_RESULTS)
        cout << "ERROR: committing an empty trans." << endl;
#endif
        return -EPERM;
    }

    // No updates in this transaction
    if (trans->rubs.empty()) {
        delete_trans(tid, trans);
        __atomic_store_n(&th->active_trans, NULL, MM_RELAXED);
        return -ENOENT;
    }

    {   lock_guard<shared_mutex> guard(g_trans_lk);

        // Check after locking

        struct TransDesc *tail_t = __atomic_load_n(&trans_queue->tail, MM_ACQUIRE);
        if (check_conflicts(trans, tail_t) != 0) {
            delete_trans(tid, trans);
            __atomic_store_n(&th->active_trans, NULL, MM_RELAXED);
            return -EPERM;
        }

        // Passed check
        if (!db_control) {
            // Stand-alone CUBIT
            uint128_t data_t = __atomic_load_n((uint128_t *)&g_timestamp, MM_CST);
            trans->l_commit_ts = (uint64_t)data_t + 1;
            trans->l_number_of_rows = (uint64_t)(data_t>>64);
        } else {
            // Serve as a library in DBMS systems like DBx1000
            if(db_timestamp_t != UINT64_MAX) {
                assert(db_row_nums != UINT64_MAX);
                trans->l_commit_ts = db_timestamp_t;
                trans->l_number_of_rows = db_row_nums;
            }
            else {
                uint64_t data_t = __atomic_load_n(&db_timestamp, MM_CST);
                trans->l_commit_ts = data_t + 1;
                trans->l_number_of_rows = db_number_of_rows;
            }
        } 

        // Fill missing row_id's
        if (trans->l_inc_rows) {
            int n_inc_rows = 0;
            map<uint64_t, RUB> row_id_v{};

            // The follwing code snippet can be included in a single for-loop body in the future.
            for (const auto & [row_id_t, rub_t] : trans->rubs) {
                if (rub_t.type == TYPE_INSERT) {
                    row_id_v[row_id_t] = rub_t;
                }
            }

            for (const auto & [row_id_t, rub_t] : row_id_v)
                trans->rubs.erase(row_id_t);

            for (const auto & [row_id_t, rub_t] : row_id_v) {
                auto actual_row_id = trans->l_number_of_rows + n_inc_rows;
                trans->rubs[actual_row_id] = RUB{actual_row_id, rub_t.type, rub_t.pos};
                n_inc_rows ++;
            }
            if(db_timestamp_t == UINT64_MAX) {
                assert(n_inc_rows == trans->l_inc_rows);
            }
        }

        // Link the new trans to trans_queue->tail
        // It is safe to update trans_queue->tail because we are holding the lock.
        __atomic_store_n(&trans_queue->tail->next, trans, MM_CST);
        __atomic_store_n(&trans_queue->tail, trans, MM_CST);

        // Update g_number_of_rows
        __atomic_add_fetch(&g_number_of_rows, trans->l_inc_rows, __ATOMIC_SEQ_CST);

        // Update local timestamp and g_timestamp
        __atomic_add_fetch(&g_timestamp, 1, __ATOMIC_SEQ_CST);

        // NOTE: When CUBIT is controlled by a DBMS (i.e., db_control == true),
        //       it's the DBMS system's responsibility to increment db_timestamp.

        __atomic_store_n(&th->active_trans, NULL, MM_RELAXED);
    }

    return 0;
}

int CubitLK::merge_bitmap(int tid, uint32_t val, TransDesc *trans,
        cubit::Bitmap *bitmap_old, cubit::Bitmap *bitmap_new, map<uint64_t, RUB> *rubs)
{
    /* Prepare trans_merge */
    struct TransDesc *trans_merge = allocate_trans();
    trans_merge->l_start_ts = trans->l_start_ts;
    trans_merge->l_commit_ts = trans->l_start_ts;
    trans_merge->l_end_trans = trans->l_end_trans;

    for (auto const &[row_id_t, rub_t] : *rubs) {
        assert(rub_t.pos.count(val));
        RUB rub_t_t = RUB(rub_t);
        rub_t_t.pos.erase(val);
        trans_merge->rubs[row_id_t] = RUB{row_id_t, TYPE_MERGE, rub_t_t.pos};
    }
    assert(trans_merge->rubs.size() == rubs->size());

    bitmap_new->l_start_trans = trans_merge;
    bitmap_new->next = __atomic_load_n(&bitmaps[val], MM_RELAXED);

    {   lock_guard<shared_mutex> guard(g_trans_lk);

        // Double-check after locking
        if (__atomic_load_n(&bitmaps[val], MM_RELAXED) != bitmap_old) {
            goto merge_error;
        }

        // Double-check after locking
        if (check_conflicts(trans_merge, READ_ONCE(trans_queue->tail)) != 0)
            goto merge_error;

        // Passed check

        // auto ts_tmp = db_control ? __atomic_load_n(&db_timestamp, MM_CST) : __atomic_load_n(&g_timestamp, MM_CST);
        // trans_merge->l_commit_ts = bitmap_new->l_commit_ts = ts_tmp + 1;
        auto ts_tmp = db_control ? trans->l_start_ts : __atomic_load_n(&g_timestamp, MM_CST);
        trans_merge->l_commit_ts = bitmap_new->l_commit_ts = ts_tmp;
        if(!db_control) {
            trans_merge->l_commit_ts++;
            bitmap_new->l_commit_ts++;
        }

        __atomic_store_n(&bitmaps[val], bitmap_new, MM_RELEASE);

        __atomic_store_n(&trans_queue->tail->next, trans_merge, MM_CST);
        __atomic_store_n(&trans_queue->tail, trans_merge, MM_CST);

        __atomic_add_fetch(&g_timestamp, 1, __ATOMIC_SEQ_CST);

        goto out_merge;
    }

merge_error:
    delete_trans(tid, trans_merge);
    return -1;

out_merge:
    return 0;
}

