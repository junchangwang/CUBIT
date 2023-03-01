#ifndef NAIVE_TABLE_H_
#define NAIVE_TABLE_H_

#include "fastbit/bitvector.h"
#include "nicolas/base_table.h"

#include <string>
#include <vector>
#include <mutex>
#include <shared_mutex>

namespace naive {
    class Table : public BaseTable {
    public:
        Table(Table_config *config);

        int update(int tid, uint32_t rowid, int to_val);
        int remove(int tid, uint32_t rowid);
        int append(int tid, int val);
        int evaluate(int tid, uint32_t val);

        void printMemory();
        void printUncompMemory();

        int get_value(uint32_t rowid);

        std::vector<ibis::bitvector *> bitmaps;

    protected:
	// Global read-write lock to protect the whole bitmap index.
        shared_mutex g_lock;

        void _get_value(uint32_t rowid, int begin, int range, bool *flag, int *result);

        uint32_t number_of_rows;
    };
};

#endif
