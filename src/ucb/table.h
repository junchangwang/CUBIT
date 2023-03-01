#ifndef UCB_TABLE_H_
#define UCB_TABLE_H_

#include "fastbit/bitvector.h"
#include "nicolas/base_table.h"

#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <shared_mutex>

namespace ucb {

    class Table : public BaseTable {
    public:
        Table(Table_config *config);

        int update(int tid, uint32_t rowid, int to_val);
        int remove(int tid, uint32_t rowid);
        int append(int tid, int val);
        int evaluate(int tid, uint32_t val);

        void printMemory();
        void printUncompMemory();
        void decode(ibis::bitvector res, std::vector<uint32_t> &rids);

        std::vector<ibis::bitvector *> bitmaps;

    protected:
        // Global read-write lock to protect the whole bitmap index (esp. EB).
        shared_mutex g_lock;

        uint32_t TBC;
        std::vector<uint32_t> append_rowids;
        ibis::bitvector existence_bitmap;
        uint32_t total_rows;
        
        // FIXME: The following two functions haven't been parallelized yet!!!
        void appendBitmap(int to_val);
        void fastAppend(int to_val);
 
        uint32_t number_of_rows;
    };

};

#endif
