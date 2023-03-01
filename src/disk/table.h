#ifndef DISK_TABLE_H_
#define DISK_TABLE_H_

#include <string>
#include <vector>

#include "fastbit/bitvector.h"

namespace disk {

    class Table {
    public:
        Table(uint32_t cardinality, uint32_t n_rows);
        void update(unsigned int rowid, int to_val);
        void update(unsigned int rowid, int to_val, int from_val);
        void remove(unsigned int rowid);
        void remove(unsigned int rowid, unsigned int val);
        void append(int val);

        int evaluate(uint32_t val);
        int evaluate(uint32_t val, ibis::bitvector &res);

        int getMem();

    protected:
        int *src;

        const uint32_t cardinality;
        uint32_t number_of_rows;

        std::vector<ibis::bitvector *> update_bitmaps;
        std::vector<uint32_t> last_rowid;

        std::vector<std::vector<uint32_t> *> new_part;

        std::string getBMName(int val);

        int getValue(uint32_t rowid);
        void readAll();

        std::vector<std::vector<std::pair<int, int>>> indices;
    };

};

#endif
