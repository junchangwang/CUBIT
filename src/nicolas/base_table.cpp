#include <urcu.h>
#include <chrono>
#include <shared_mutex>

#include "fastbit/bitvector.h"
#include "nicolas/base_table.h"
#include "nbub/table.h"

using namespace std;

extern shared_mutex bitmap_mutex;

bool run_merge_func = false;

void merge_func(BaseTable *table, int begin, int range, Table_config *config)
{
    assert((config->approach == "nbub-lf") || (config->approach == "nbub-lk"));

    rcu_register_thread();

    nbub::Nbub* table2 = dynamic_cast<nbub::Nbub*>(table);

    while (READ_ONCE(run_merge_func)) {
        int n_merges = 0;
        {
            lock_guard<shared_mutex> guard(bitmap_mutex);
            for (int q = begin; q < begin + range; q++) 
            {
                lock_guard<mutex> lock(table2->lk_merge_req_queues[q]);
                // More sophisticated strategies can be used
                // if there are more reqs in a single queue.
                while (table2->merge_req_queues[q].size() > 0) 
                {
                    struct nbub::Nbub::merge_req *req = table2->merge_req_queues[q].front();

                    if (config->enable_fence_pointer) {
                        if (config->segmented_btv) {
                            req->btm_new->seg_btv->buildAllIndex();
                        }
                        else {
                            req->btm_new->btv->index.clear();
                            req->btm_new->btv->buildIndex();
                        }
                    }
                    
                    int ret = table2->merge_bitmap(req->tid, req->val, req->trans, 
                            req->btm_old, req->btm_new, req->rubs);

                    if (ret == 0) {
                        // call_rcu(&req->btm_old->head, nbub::free_bitmap_cb);
                        // cout << "[CUBIT:] Successfully merge one bitvector on btv " << req->val 
                        //     << " from " << req->btm_old << " to " << req->btm_new << endl;
                    }
                    else {
                        nbub::free_bitmap_cb(&req->btm_new->head);
                        // cout << "[CUBIT:] Failed to merge one bitvector on btv " << req->val 
                        //     << " from " << req->btm_old << " to " << req->btm_new << endl;
                    }

                    delete req->rubs;
                    delete req;

                    table2->merge_req_queues[q].pop();

                    n_merges ++;
                }
            } // end of for loop
        }
#define SLEEP_WHEN_IDEL (100)
        if (n_merges == 0) {
#if defined(VERIFY_RESULTS)
            //cout << "MERGE: Nothing to be done. Going to sleep for 1ms " << endl;
#endif
            this_thread::sleep_for(chrono::microseconds(SLEEP_WHEN_IDEL));
        }
    }

    rcu_unregister_thread();
}
