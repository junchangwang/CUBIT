#ifndef UB_TABLE_H_
#define UB_TABLE_H_

#include <string>
#include <vector>
#include <mutex>
#include <shared_mutex>

#include "nicolas/base_table.h"

// using namespace std;

namespace ub {

    class Table : public BaseTable {
    public:
        Table(Table_config *config);

        int update(int tid, uint64_t rowid, int to_val);
        int remove(int tid, uint64_t rowid);
        int append(int tid, int val);
        int evaluate(int tid, uint32_t val);

        int get_value(uint64_t rowid);
        int range(int tid, uint32_t start, uint32_t range);
        void _read_btv(int begin, int range);

        void printMemory();
        void printUncompMemory();

    protected:
        // Per-<vb, ub>-pair reader-writer lock.
        std::vector<std::shared_mutex *>  lk_bitmaps;
        std::vector<ibis::bitvector *> bitmaps;
        std::vector<ibis::bitvector *> update_bitmaps;
        int num_bitmaps;

        // Mutex lock to protect the shared variable number_of_rows.
        // Append operations must first grab this lock.
        std::shared_mutex lk_append;
        uint64_t number_of_rows;

        uint64_t total_rows;

        void _get_value(uint64_t rowid, int begin, int range, bool *flag, int *result);
        int __get_point_val(unsigned int column, unsigned int row);
    };
};

#endif
