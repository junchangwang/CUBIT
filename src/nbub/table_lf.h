#ifndef NBUB_LF_TABLE_H_
#define NBUB_LF_TABLE_H_

#include "table.h"
#include "nicolas/util.h"
#include "nicolas/base_table.h"

using namespace std;
using namespace nbub;

namespace nbub_lf {

class NbubLF: public Nbub{
private:
    /* *********************** */

    int help_insert_trans(TransDesc *tail, TransDesc *trans);

public:
    NbubLF(Table_config *config);

    int trans_commit(int tid) override;

    int merge_bitmap(int tid, uint32_t val, TransDesc *trans, Bitmap *bitmap_old, Bitmap *bitmap_new, map<uint64_t, RUB> *rubs) override;
};

};

#endif
