#ifndef NBUB_TABLE_H_
#define NBUB_TABLE_H_

#include <vector>
#include <mutex>
#include <cstdint>
#include <atomic>
#include <tuple>
#include <urcu.h>
#include <queue> // For queue passing merge requests

#include "nicolas/util.h"
#include "nicolas/base_table.h"
#include "nicolas/segBtv.h"

// using namespace std;

extern uint64_t db_timestamp __attribute__((aligned(128)));
extern uint64_t db_number_of_rows;

typedef std::map<uint16_t, uint16_t> RLE_map;    // Run-Length Encoding

struct RUB {
    uint64_t row_id;
    int type;
    RLE_map pos;  // <position, length> pairs
};

struct TransDesc 
{
    TransDesc *next;

    uint64_t l_number_of_rows;
    uint32_t l_inc_rows;

    uint64_t l_start_ts;
    uint64_t l_commit_ts;

    std::map<uint64_t, RUB> rubs;

    TransDesc *l_end_trans;

    // For helping update the new bitmap.
    // For nbub_lf only.
    uint32_t val_h;
    void *bitmap_old_h;
    void *bitmap_new_h;
} DOUBLE_CACHE_ALIGNED;

namespace nbub {

// It is not neccessary for CUBIT to play this trick to get good insertion performance.
// const int NBUB_PAD_BITS = (1000000 / 310) * 31;
const int NBUB_PAD_BITS = 0;  

#define TYPE_INV (0)
#define TYPE_INSERT (1)                                            
#define TYPE_UPDATE (2)                                            
#define TYPE_DELETE (3)
#define TYPE_MERGE (4)

#define MAX_CARDINALITY (numeric_limits<uint16_t>::max())
#define INV_IDX (numeric_limits<uint32_t>::max())
#define INV_TIMESTAMP (numeric_limits<uint64_t>::max())            
#define INV_ROW_ID (numeric_limits<uint64_t>::max())   

struct Bitmap {
    struct rcu_head head;
    uint64_t l_commit_ts;
    TransDesc * l_start_trans;
    // In order to verify the correctness, we keep both *btv and *seg_btv.
    // In the future, we only maintain either of them to save memory space.
    ibis::bitvector * btv;
    SegBtv *seg_btv;
    Bitmap *next;
} DOUBLE_CACHE_ALIGNED;

static void free_bitmap_cb(struct rcu_head *head)
{
    struct Bitmap *bitmap = caa_container_of(head, struct Bitmap, head);
    if (bitmap->seg_btv)
        delete bitmap->seg_btv;
    if (bitmap->btv)
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
    TransDesc *trans_pool;
    uint64_t n_trans_pool;
    uint64_t cnt_trans_used;

    uint32_t total_rows;

protected:

    /* *********************** */
    queue_t * trans_queue;

    /* Configure parameters */
    bool autoCommit;
    bool db_control;
    int merge_threshold;

    void _get_value(uint64_t, int, int, uint64_t, bool *, int *, struct RUB *, uint64_t *);
    TransDesc * get_rub_on_row(uint64_t, uint64_t, TransDesc *, uint64_t, RUB &, uint64_t &);
    TransDesc * get_rubs_on_btv(uint64_t, uint64_t, TransDesc *, uint32_t, std::map<uint64_t, RUB> &);

    TransDesc * allocate_trans();
    int delete_trans(int, TransDesc *);

    int check_conflicts(TransDesc *, TransDesc *);

    int pos2RE(int start, int end, RLE_map &pos_map);

    int pos2AE(int start, int end, RLE_map &pos_map);

    int evaluate_common(int, uint32_t);

    int evaluate_Curve(int, uint32_t);

public:

    /* *********************** */
    struct Bitmap **bitmaps;
    int num_bitmaps;

    // FIXME: can be moved to private domain in the future.
    uint64_t g_timestamp __attribute__((aligned(128)));
    uint64_t g_number_of_rows;

    Nbub(Table_config *);

    int append(int, int, uint64_t row_id = UINT64_MAX);
    int remove(int, uint64_t);
    int update(int, uint64_t, int);
    int evaluate(int, uint32_t);
    void _read_btv(int, int, TransDesc*, Table_config*);

    TransDesc * trans_begin(int, uint64_t db_timestamp_t = UINT64_MAX);
    virtual int trans_commit(int, uint64_t db_timestamp_t = UINT64_MAX, uint64_t db_row_nums = UINT64_MAX) { assert(0); return -1; };
    virtual int merge_bitmap(int, uint32_t, TransDesc *, Bitmap *, Bitmap *, std::map<uint64_t, RUB> *) { assert(0); return -1; };

    void printMemory();
    void printUncompMemory();
    void printMemorySeg();
    void printUncompMemorySeg();

    int get_value_rcu(uint64_t, uint64_t, RUB &); 
    int range(uint32_t, uint32_t);
    uint64_t get_g_timestamp() { return g_timestamp; }
    uint64_t get_g_number_of_rows() { return g_number_of_rows; }

    bool init_sequential_test();

    // For merge thread
    struct merge_req {
        int tid;
        uint32_t val; 
        TransDesc *trans; 
        Bitmap *btm_old; 
        Bitmap *btm_new; 
        std::map<uint64_t, RUB> * rubs;
    };

    int __init_append(int tid, int rowID, int val);

    // An array of message queues, each of which connect a worker thread and the merge daemon thread.
    // The message is a pointer to struct merge_req.
    std::queue<struct merge_req *> * merge_req_queues;
    std::mutex * lk_merge_req_queues;
};

};

#endif
