#include "fastbit/bitvector.h"

#include <iostream>
#include <sstream>
#include <boost/filesystem.hpp>
#include <sys/mman.h>
#include <queue>	// priority queue
#include <fstream>

#include "tablex.h"
#include "util.h"

using namespace std;

TableX::TableX(int32_t cardinality) : cardinality(cardinality), number_of_rows(0) {
    for (int i = 0; i < cardinality; ++i) {
        bitmaps.push_back(new ibis::bitvector());
    }
}

void TableX::appendRow(int val, Table_config *config)
{
    switch (config->encoding)
    {
        case EE:
             for (int i = 0; i < cardinality; i++) {
                 bitmaps[i]->setBit(number_of_rows, i == val, config);
             }
             break;
        case RE:
             for (int i = 0; i < cardinality - 1; i++) {
                 bitmaps[i]->setBit(number_of_rows, i >= val, config);
             }
             break;
        case AE:
             for (int i = 0; i < cardinality - 1; i++) 
             {
                 // Order from 0 -> (cardinality - 1) is critical to guarantee that
                 // the related anchor has been set before setting curves.

                 // Margin bitvectors
                 if (config->AE_margins.count(i)) { 
                     bitmaps[i]->setBit(number_of_rows, i >= val, config);
                 }
                 // Anchor bitvectors
                 else if (config->AE_anchors.count(i)) { 
                     /* Anchors use RE */
                     int anchor_val = (i >= val);
                     bitmaps[i]->setBit(number_of_rows, anchor_val, config); 

                     int advanced_btvs = 0;
                     for (int j = 1; j <= config->AE_interval; j++) {
                         if (config->AE_curves.count(i+j)) { 
                             /* Curve bitvectors */
                             int curve_val = ((i + j) >= val);
                             int AE_val = anchor_val ^ curve_val;
                             bitmaps[i+j]->setBit(number_of_rows, AE_val, config);
                             advanced_btvs ++;
                         }
                     }

                     i += advanced_btvs;
                 }
                 else {
                     // Curves have been processed above.
                     assert(0);
                 }
             }
             break;
        default:
             cout << "Unknown encoding scheme: " << config->encoding << endl;
             assert(0);
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
