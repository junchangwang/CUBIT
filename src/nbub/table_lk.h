#ifndef NBUB_LK_TABLE_H_
#define NBUB_LK_TABLE_H_

#include <mutex>
#include <shared_mutex>

#include "table.h"
#include "nicolas/util.h"
#include "nicolas/base_table.h"

// using namespace std;
// using namespace nbub;

namespace nbub_lk {

class NbubLK: public nbub::Nbub{
private:
    /* *********************** */

    // Protect lkqueue, g_timestamp, g_number_of_rows, and bitmaps, all together.
    std::shared_mutex g_trans_lk;

public:
    NbubLK(Table_config *config);

    int trans_commit(int tid, uint64_t db_timestamp_t, uint64_t db_row_nums) override;

    int merge_bitmap(int tid, uint32_t val, TransDesc *trans, nbub::Bitmap *bitmap_old, nbub::Bitmap *bitmap_new, std::map<uint64_t, RUB> *rubs) override;
};

};

#endif
