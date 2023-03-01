#ifndef TABLEX_H
#define TABLEX_H

#include "fastbit/bitvector.h"

class TableX {
public:
    TableX(int32_t cardinality);

    void appendRow(int val, Table_config *config);

    void write(std::string dir);

    void check();

protected:
    const int32_t cardinality;

    uint32_t number_of_rows;

    std::vector<ibis::bitvector *> bitmaps;
};

#endif