#include <iostream>
#include <tuple>
#include <atomic>

#include "table_lf.h"
#include "fastbit/bitvector.h"
#include "nicolas/util.h"
#include "nicolas/base_table.h"

using namespace std;
using namespace cubit_lf;

CubitLF::CubitLF(Table_config *config) : Cubit(config)
{
    trans_queue = new cubit::queue_t{};
    __atomic_store_n(&trans_queue->head, bitmaps[0]->l_start_trans, MM_RELAXED);
    __atomic_store_n(&trans_queue->tail, bitmaps[0]->l_start_trans, MM_RELAXED);
    
    if (config->db_control) {
        cout << "ERROR: The lock-free version (CUBIT-lf) cannot be used by DBMSs yet. "
                << "Please use CUBIT-lk instead." << endl;
        assert(false);
    }
}

int CubitLF::help_insert_trans(TransDesc *tail, TransDesc *trans)
{ 
    // Insert new bitmap if necessary
    if ((trans->bitmap_new_h != NULL) &&
        (__atomic_load_n(&bitmaps[trans->val_h], MM_RELAXED) != (cubit::Bitmap *)trans->bitmap_new_h)) {
        if (cmpxchg(&bitmaps[trans->val_h], (cubit::Bitmap *)trans->bitmap_old_h, (cubit::Bitmap *)trans->bitmap_new_h) 
                != (cubit::Bitmap *)trans->bitmap_old_h) {
            #if defined(VERIFY_RESULTS)
            cout << "NOTE: In help_insert_trans(): bitmap_new has been set by other helpers." << endl;
            #endif
        }
    }

    // Update g_timestamp and g_number_of_rows atomically
    uint64_t tsp_t = trans->l_commit_ts-1;
    uint64_t nor_t = trans->l_number_of_rows;
    if (!CAS2(&g_timestamp, &tsp_t, &nor_t, trans->l_commit_ts, (trans->l_number_of_rows + trans->l_inc_rows))) {
        #if defined(VERIFY_RESULTS)
        cout << "Note in help_insert_trans(): g_timestamp has been updated by other helpers." 
             << "Current value: " << tsp_t << " : " <<  nor_t 
             << " . My attempt: " << trans->l_commit_ts << " : " << trans->l_number_of_rows + trans->l_inc_rows
             << endl;
        #endif
    }

    // Update tail
    if (cmpxchg(&trans_queue->tail, tail, trans) != tail) {
        #if defined(VERIFY_RESULTS)
	    cout << "Note in help_insert_trans(): trans_queue->tail has been updated by other helpers." << endl;
        #endif
    }

    return 0;
}

/*************************************
 *       Transaction Semantics       *
 ************************************/

int CubitLF::trans_commit(int tid, uint64_t db_timestamp_t, uint64_t db_row_nums) 
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
        __atomic_store_n(&th->active_trans, NULL, MM_RELEASE);
        return -ENOENT;
    }

    // enqueue and call help if necessary
    TransDesc *tail_t, *next_t;

    while(true) {
        tail_t = __atomic_load_n(&trans_queue->tail, MM_ACQUIRE);
        next_t = __atomic_load_n(&tail_t->next, MM_ACQUIRE);

        if (tail_t != READ_ONCE(trans_queue->tail)) continue;

        if (next_t != NULL) {
            help_insert_trans(tail_t, next_t);
            continue;
        }

        // Hold the valid (tail_t, next_t) snapshot, then double-check.
        if (check_conflicts(trans, tail_t) != 0) {
            delete_trans(tid, trans);
            __atomic_store_n(&th->active_trans, NULL, MM_RELEASE);
            return -EPERM;
        }

        // Prepare a new trans.
        uint128_t global_val = __atomic_load_n((uint128_t *)&g_timestamp, MM_CST);
        trans->l_commit_ts = (uint64_t)global_val;
        trans->l_commit_ts += 1;
        trans->l_number_of_rows = (uint64_t)(global_val>>64);
        trans->bitmap_new_h = NULL;

        // Fill missing row id's
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

            assert(n_inc_rows == trans->l_inc_rows);
        }

        if (cmpxchg(&tail_t->next, next_t, trans) == next_t) 
            break;  /* Linearization point */
    }

    help_insert_trans(tail_t, trans);

    __atomic_store_n(&th->active_trans, NULL, MM_RELEASE);
    return 0;
}

int CubitLF::merge_bitmap(int tid, uint32_t val, TransDesc *trans,
        cubit::Bitmap *bitmap_old, cubit::Bitmap *bitmap_new, map<uint64_t, RUB> *rubs)
{
    /* Prepare trans_merge */
    TransDesc *trans_merge = allocate_trans();
    trans_merge->l_start_ts = trans->l_start_ts;
    trans_merge->l_commit_ts = trans->l_start_ts;
    trans_merge->l_end_trans = trans->l_end_trans;

    for (auto const &[row_id_t, rub_t] : *rubs) {
        assert(rub_t.pos.count(val));
        RUB rub_t_t = RUB{rub_t};
        rub_t_t.pos.erase(val);
        trans_merge->rubs[row_id_t] = RUB{row_id_t, TYPE_MERGE, rub_t_t.pos};
    }
    assert(trans_merge->rubs.size() == rubs->size());

    bitmap_new->l_start_trans = trans_merge;
    bitmap_new->next = bitmap_old;

    {   /* enqueue and call help if necessary */
        TransDesc *tail_t, *next_t;

        while(1) {
            tail_t = __atomic_load_n(&trans_queue->tail, MM_ACQUIRE);
            next_t = __atomic_load_n(&tail_t->next, MM_ACQUIRE);

            if (tail_t != READ_ONCE(trans_queue->tail)) continue;

            if (next_t != NULL) {
                help_insert_trans(tail_t, next_t);
                continue;
            }

            // Check after holding the valid (tail_t, next_t) snapshot.

            if (READ_ONCE(bitmaps[val]) != bitmap_old) {
                #if defined(VERIFY_RESULTS)
                cout << "WARNING: A new bitmap has been set to bitmaps[" << val << "], with timestamp being "
                        << bitmaps[val]->l_commit_ts <<" . Old timestamp value: " << trans_merge->l_start_ts << " Giving up." << endl;
                #endif
                goto merge_error;
            }

            if (check_conflicts(trans_merge, tail_t) != 0)
                goto merge_error;

            // Prepare trans.
            uint128_t global_val = __atomic_load_n((uint128_t *)&g_timestamp, MM_CST);
            trans_merge->l_commit_ts = bitmap_new->l_commit_ts = (uint64_t)global_val + 1;
            trans_merge->l_number_of_rows = (uint64_t)(global_val>>64);
            // Set information for helping insert bitmap_new
            trans_merge->val_h = val;
            trans_merge->bitmap_old_h = bitmap_old;
            trans_merge->bitmap_new_h = bitmap_new;

            if (cmpxchg(&tail_t->next, next_t, trans_merge) == next_t) 
                break;  /* Linearization point */
        }

        help_insert_trans(tail_t, trans_merge);
    }
    goto out_merge;

merge_error:
    delete_trans(tid, trans_merge);
    return -1;

out_merge:
    return 0;
}

