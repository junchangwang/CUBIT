#include "fastbit/bitvector.h"
#include <boost/filesystem.hpp>
#include <iostream>
#include <bitvector.h>

#include "part/table.h"

#ifdef LINUX
#include <omp.h>
#endif

using namespace part;
using namespace std;

const int part_size = 31 * 300000;

Table::Table(std::string dir, uint32_t cardinality, uint32_t number_of_rows) : dir(dir), cardinality(cardinality), number_of_rows(number_of_rows) {
    for (int i = 0; i < cardinality; ++i) {
        std::vector<ibis::bitvector> vec;
        ibis::bitvector bitvector;
        bitvector.read(getBMName(i).c_str());
        ibis::bitvector mask;
        for (int j = 0; j <= bitvector.size() / part_size; ++j) {
            mask.clear();
            if (j > 0)
                mask.appendFill(0, part_size * j);
            if (j == bitvector.size() / part_size)
                mask.appendFill(1, bitvector.size() - part_size * j);
            else {
                mask.appendFill(1, part_size);
                if (mask.size() < bitvector.size())
                    mask.appendFill(0, bitvector.size() - part_size * (j + 1));
            }
            ibis::bitvector bv;
            bitvector.subset(mask, bv);
            vec.push_back(bv);
        }
        bitmaps.push_back(vec);
    }
}

void Table::append(int val) {

}

void Table::update(uint32_t rowid, int to_val) {
    int from_val = getValue(rowid);
    if (from_val == to_val)
        return;
    bitmaps[from_val][rowid / part_size].setBit(rowid % part_size, 0);
    bitmaps[to_val][rowid / part_size].setBit(rowid % part_size, 1);
}

void Table::remove(uint32_t rowid) {

}

int Table::evaluate(uint32_t val) {
    ibis::bitvector res;
    int ret, cur = 0, size = 0;
    for (int i = 0; i < bitmaps[val].size(); ++i) {
        // ret += bitmaps[val][i].cnt();
        size += bitmaps[val][i].m_vec.size();
    }
    res.m_vec.resize(size);
    for (int i = 0; i < bitmaps[val].size(); ++i) {
        for (int j = 0; j < bitmaps[val][i].m_vec.size(); ++j) {
            res.m_vec[cur + j] = bitmaps[val][i].m_vec[j];
        }
        cur += bitmaps[val][i].m_vec.size();
    }
    return ret;
}

std::string Table::getBMName(int val) {
    std::stringstream ss;
    ss << dir << val << ".bm";
    return ss.str();
}

int Table::getMem() {
    int mem = 0;
    return mem;
}

int Table::getValue(uint32_t rowid) {
    volatile bool flag = false;
    int ret = -1;
#ifdef LINUX
    omp_set_dynamic(0);
    omp_set_num_threads(8);
#endif
#pragma omp parallel for shared(flag)
    for (int i = 0; i < cardinality; ++i) {
        if (flag)
            continue;
        // cout << i << " " << rowid / part_size << " " << rowid % part_size << " " << bitmaps[i][rowid / part_size].getBit(rowid % part_size) << endl;
        if (bitmaps[i][rowid / part_size].getBit(rowid % part_size) == 1) {
            flag = true;
            ret = i;
        }
    }
    return ret;
}