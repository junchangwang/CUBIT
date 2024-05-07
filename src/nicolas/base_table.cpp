#include <urcu.h>
#include <chrono>
#include <shared_mutex>

#include "fastbit/bitvector.h"
#include "nicolas/base_table.h"
#include "cubit/table.h"

using namespace std;

bool run_merge_func = false;

void merge_func(BaseTable *table, int begin, int range, Table_config *config, shared_mutex *bitmap_mutex)
{
    assert((config->approach == "cubit-lf") || (config->approach == "cubit-lk"));

    rcu_register_thread();

    cubit::Cubit* table2 = dynamic_cast<cubit::Cubit*>(table);

    while (READ_ONCE(run_merge_func)) {
        int n_merges = 0;
        {
            if(bitmap_mutex)
                lock_guard<shared_mutex> guard(*bitmap_mutex);
            for (int q = begin; q < begin + range; q++) 
            {
                lock_guard<mutex> lock(table2->lk_merge_req_queues[q]);
                // More sophisticated strategies can be used
                // if there are more reqs in a single queue.
                while (table2->merge_req_queues[q].size() > 0) 
                {
                    struct cubit::Cubit::merge_req *req = table2->merge_req_queues[q].front();

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
                        // call_rcu(&req->btm_old->head, cubit::free_bitmap_cb);
                        // cout << "[CUBIT:] Successfully merge one bitvector on btv " << req->val 
                        //     << " from " << req->btm_old << " to " << req->btm_new << endl;
                    }
                    else {
                        cubit::free_bitmap_cb(&req->btm_new->head);
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
