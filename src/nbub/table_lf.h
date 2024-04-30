#ifndef NBUB_LF_TABLE_H_
#define NBUB_LF_TABLE_H_

#include "table.h"
#include "nicolas/util.h"
#include "nicolas/base_table.h"

// using namespace std;
// using namespace nbub;

namespace nbub_lf {

class NbubLF: public nbub::Nbub{
private:
    /* *********************** */

    int help_insert_trans(TransDesc *tail, TransDesc *trans);

public:
    NbubLF(Table_config *config);

    int trans_commit(int tid, uint64_t db_timestamp_t, uint64_t db_row_nums) override;

    int merge_bitmap(int tid, uint32_t val, TransDesc *trans, nbub::Bitmap *bitmap_old, nbub::Bitmap *bitmap_new, std::map<uint64_t, RUB> *rubs) override;
};

};

#endif
