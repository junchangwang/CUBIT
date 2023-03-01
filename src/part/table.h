#ifndef PART_TABLE_H_
#define PART_TABLE_H_

#include "fastbit/bitvector.h"

#include <string>
#include <vector>


namespace part {

    class Table {
    public:
        Table(std::string dir, uint32_t cardinality, uint32_t number_of_rows);
        void update(uint32_t rowid, int to_val);
        void remove(uint32_t rowid);
        void append(int val);

        int evaluate(uint32_t val);
        int getMem();

    protected:
        const std::string dir;
        const uint32_t cardinality;
        uint32_t number_of_rows;

        std::vector<std::vector<ibis::bitvector>> bitmaps;
        std::vector<uint32_t> sizes;

        std::string getBMName(int val);

        int getValue(uint32_t rowid);
    };
};

#endif
