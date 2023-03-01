#include <iostream>
#include <tuple>
#include <atomic>

#include "table_lk.h"
#include "fastbit/bitvector.h"
#include "nicolas/util.h"

using namespace std;
using namespace nbub_lk;

NbubLK::NbubLK(Table_config *config) : Nbub(config)
{
    trans_queue = new queue_t();
    __atomic_store_n(&trans_queue->head, bitmaps[0]->l_start_trans, MM_RELAXED);
    __atomic_store_n(&trans_queue->tail, bitmaps[0]->l_start_trans, MM_RELAXED);

#if defined(VERIFY_RESULTS)
    assert(init_sequential_test());
#endif
}

/*************************************
 *       Transaction Semantics       *
 ************************************/

int NbubLK::trans_commit(int tid) 
{
    struct ThreadInfo *th = &g_ths_info[tid];
    struct TransDesc *trans = __atomic_load_n(&th->active_trans, MM_ACQUIRE);

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

        // Sanity check

        struct TransDesc *tail_t = __atomic_load_n(&trans_queue->tail, MM_ACQUIRE);
        if (check_conflicts(trans, tail_t) != 0) {
            delete_trans(tid, trans);
            __atomic_store_n(&th->active_trans, NULL, MM_RELAXED);
            return -EPERM;
        }

        // Passed check
        auto ts_tmp = db_control ? __atomic_load_n(&db_timestamp, MM_CST) : __atomic_load_n(&g_timestamp, MM_CST);
        trans->l_commit_ts = ts_tmp + 1;

        // Fill missing row_id's
        if (trans->l_inc_rows) {
            int n_inc_rows = 0;
            for (int i = 0; i < trans->rubs.size(); i++) {
                auto [row_id_t, idx1, idx2, type] = decode_rub(trans->rubs[i]);
                if (row_id_t == INV_ROW_ID) {
#if defined(VERIFY_RESULTS)
                    assert(type == TYPE_INSERT);
#endif
                    trans->rubs[i] = encode_rub(g_number_of_rows + n_inc_rows, idx1, idx2, TYPE_INSERT);
                    trans->updated_rows.push_back(g_number_of_rows + n_inc_rows);
                    n_inc_rows ++;
                }
            }
            assert(n_inc_rows == trans->l_inc_rows);
        }

        // Link the new trans to trans_queue->tail
        // It is safe to update trans_queue->tail because we are holding the lock.
        __atomic_store_n(&trans_queue->tail->next, trans, MM_CST);
        __atomic_store_n(&trans_queue->tail, trans, MM_CST);

        // Update g_number_of_rows
        __atomic_add_fetch(&g_number_of_rows, trans->l_inc_rows, __ATOMIC_SEQ_CST);

        // Update local timestamp and g_timestamp
        __atomic_add_fetch(&g_timestamp, 1, __ATOMIC_SEQ_CST);

        // NOTE: Even if CUBIT is controlled by a DBMS (i.e., db_control == true)
        // We do not increment db_timestamp. It's the DBMS's responsibility to do this.

        __atomic_store_n(&th->active_trans, NULL, MM_RELAXED);
    }

    return 0;
}

int NbubLK::merge_bitmap(int tid, uint32_t val, struct TransDesc *trans,
        struct Bitmap *bitmap_old, struct Bitmap *bitmap_new, 
        map<uint32_t, uint64_t> &rubs)
{
    struct ibis::bitvector *my_btv = __atomic_load_n(&bitmap_new->btv, MM_RELAXED);

    /* Prepare trans_merge */
    struct TransDesc *trans_merge = allocate_trans();
    trans_merge->l_start_ts = trans->l_start_ts;
    trans_merge->l_end_trans = trans->l_end_trans;

    for (auto const &[row_id_t, rub_t] : rubs) {
	auto [row_id_t_2, idx1, idx2, type] = decode_rub(rub_t);
	assert((idx1 == val) || (idx2 == val));
	// NOTE: the situation in which (idx1 == idx2 == INV_IDX)
        //       is valid and required.
	trans_merge->rubs.push_back(encode_rub(row_id_t_2, 
                    ((idx1 == val) ? idx2 : idx1), INV_IDX, TYPE_MERGE));
	trans_merge->updated_rows.push_back(row_id_t_2);
    }
    assert(trans_merge->rubs.size() == rubs.size());

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

    auto ts_tmp = db_control ? __atomic_load_n(&db_timestamp, MM_CST) : __atomic_load_n(&g_timestamp, MM_CST);
	trans_merge->l_commit_ts = bitmap_new->l_commit_ts = ts_tmp + 1;

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

