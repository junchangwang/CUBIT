#ifndef NBUB_TABLE_H_
#define NBUB_TABLE_H_

#include <vector>
#include <mutex>
#include <cstdint>
#include <atomic>
#include <tuple>
#include <urcu.h>
#include <queue> // For queue passing merge requests

#include "nicolas/base_table.h"
#include "nicolas/util.h"

using namespace std;

extern uint64_t db_timestamp __attribute__((aligned(128)));
extern uint64_t db_number_of_rows;

namespace nbub {

// const int NBUB_PAD_BITS = (1024 * 1024 / 310) * 31 ;  // Allow one million insertions in maximum.
const int NBUB_PAD_BITS = 0 ; 
 
#define TYPE_INSERT (1)                                            
#define TYPE_UPDATE (2)                                            
#define TYPE_DELETE (3)
#define TYPE_MERGE (4)
#define TYPE_INV (numeric_limits<uint8_t>::max())                  
  
#define N_BITS_PER_IDX (12)
#define N_BITS_TYPE (8)
#define N_BITS_POS (64 - N_BITS_PER_IDX*2 - N_BITS_TYPE)           
  
#define OFFSET_POS (N_BITS_PER_IDX*2 + N_BITS_TYPE)                
#define OFFSET_IDX1 (N_BITS_PER_IDX + N_BITS_TYPE)                 
#define OFFSET_IDX2 (N_BITS_TYPE) 
                                                                   
#define MAX_CARDINALITY (1UL << N_BITS_PER_IDX)                    
      
#define INV_IDX (((uint16_t)1<<N_BITS_PER_IDX) - 1)
#define INV_TIMESTAMP (numeric_limits<uint64_t>::max())            
#define INV_ROW_ID (numeric_limits<uint32_t>::max())   

struct Bitmap {
    uint64_t l_commit_ts;
    TransDesc * l_start_trans;
    ibis::bitvector * btv;
    struct Bitmap *next;
    struct rcu_head head;
} DOUBLE_CACHE_ALIGNED;

static void free_bitmap_cb(struct rcu_head *head)
{
    struct Bitmap *bitmap = caa_container_of(head, struct Bitmap, head);
    delete bitmap->btv;
    delete bitmap;
}

typedef struct _queue_t {
  struct TransDesc * head DOUBLE_CACHE_ALIGNED;
  struct TransDesc * tail DOUBLE_CACHE_ALIGNED;
} queue_t DOUBLE_CACHE_ALIGNED;

class Nbub : public BaseTable {

private:
    /* *********************** */
    /* ** pool for TransDesc** */
    /* *********************** */
    struct TransDesc *trans_pool;
    uint64_t n_trans_pool;
    uint64_t cnt_trans_used;

    uint32_t total_rows;

protected:

    /* *********************** */
    queue_t * trans_queue;



    /* Configure parameters */
    int MERGE_THRESH;
    bool autoCommit;
    bool db_control;

    uint64_t encode_rub(uint32_t pos, uint16_t idx1, uint16_t idx2, uint8_t type);
    tuple<uint32_t, uint16_t, uint16_t, uint8_t> decode_rub(uint64_t rub);
    void _get_value(uint32_t rowid, int begin, int range, uint64_t l_timestamp, bool *flag, int *result, uint64_t *last_rub, uint64_t *last_rub_tsp);
    TransDesc * get_rub_on_row(uint64_t tsp_begin, uint64_t tsp_end, TransDesc *trans_begin, uint32_t row_id, uint64_t &rub, uint64_t &rub_tsp);
    TransDesc * get_rubs_on_btv(uint64_t tsp_begin, uint64_t tsp_end, TransDesc *trans_begin, uint32_t val, map<uint32_t, uint64_t> &rubs);

    struct TransDesc * allocate_trans();
    int delete_trans(int tid, struct TransDesc *trans);

    int check_conflicts(struct TransDesc *trans, struct TransDesc *tail);

    bool init_sequential_test();

public:

    /* *********************** */
    struct Bitmap **bitmaps;

    // FIXME: can be moved to private domain in the future.
    uint64_t g_timestamp __attribute__((aligned(128)));
    uint64_t g_number_of_rows;

    Nbub(Table_config *config);

    int append(int tid, int val);
    int remove(int tid, unsigned int rowid);
    int update(int tid, unsigned int rowid, int to_val);
    int evaluate(int tid, uint32_t val);

    TransDesc * trans_begin(int tid);
    virtual int trans_commit(int tid) { assert(0); return -1; };
    virtual int merge_bitmap(int tid, uint32_t val, struct TransDesc *trans, struct Bitmap *bitmap_old, struct Bitmap *bitmap_new, map<uint32_t, uint64_t> &rubs) { assert(0); return -1; };

    void printMemory();
    void printUncompMemory();

    int get_value_rcu(uint32_t rowid, uint64_t tsp_end, uint64_t *last_rub); 
    int range(uint32_t start, uint32_t range);
    uint64_t get_g_timestamp() { return g_timestamp; }
    uint64_t get_g_number_of_rows() { return g_number_of_rows; }

    // For merge thread
    struct merge_req {
        int tid;
        uint32_t val; 
        struct TransDesc *trans; 
        struct Bitmap *btm_old; 
        struct Bitmap *btm_new; 
        map<uint32_t, uint64_t> *rubs;
    };

    int __init_append(int tid, int rowID, int val);

    // An array of message queues, each of which connect a worker thread and the merge daemon thread.
    // The message is a pointer to struct merge_req.
    queue<struct merge_req *> * merge_req_queues;
    mutex * lk_merge_req_queues;
};

};

#endif
