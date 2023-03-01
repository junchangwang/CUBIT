#include "fastbit/bitvector.h"

#include <iostream>
#include <sstream>
#include <boost/filesystem.hpp>
#include <sys/mman.h>
#include <queue>	// priority queue
#include <fstream>

#include "tablex.h"
#include "util.h"

TableX::TableX(int32_t cardinality) : cardinality(cardinality), number_of_rows(0) {
    for (int i = 0; i < cardinality; ++i) {
        bitmaps.push_back(new ibis::bitvector());
    }
}

void TableX::appendRow(int val, Table_config *config) {
    for (int i = 0; i < cardinality; ++i) {
        bitmaps[i]->setBit(number_of_rows, i == val, config);
    }
    number_of_rows += 1;
}

void TableX::write(std::string dir) {
    boost::filesystem::path dir_path(dir);
    boost::filesystem::create_directory(dir_path);  
    for (int i = 0; i < cardinality; ++i) {
        std::stringstream ss;
        //ss << dir << i << ".bm";
        ss << dir <<"/" << i << ".bm";
        bitmaps[i]->write(ss.str().c_str());
    }    
}

void TableX::check() {
    cout << "Build index done with " << number_of_rows << " rows" << endl;
    for (int i = 0; i < cardinality; i++) {
        cout << "cnt of bitmap " << i << " is " << bitmaps[i]->do_cnt() << endl;
    }
}
