#ifndef NBUB_LF_TABLE_H_
#define NBUB_LF_TABLE_H_

#include "table.h"
#include "nicolas/util.h"

using namespace std;
using namespace nbub;

namespace nbub_lf {

class NbubLF: public Nbub{
private:
    /* *********************** */

    int help_insert_trans(struct TransDesc *tail, struct TransDesc *trans);

public:
    NbubLF(Table_config *config);

    int trans_commit(int tid) override;

    int merge_bitmap(int tid, uint32_t val, struct TransDesc *trans, struct Bitmap *bitmap_old, struct Bitmap *bitmap_new, map<uint32_t, uint64_t> &rubs) override;
};

};

#endif
