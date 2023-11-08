#include "fastbit/bitvector.h"

#include <iostream>
#include <sstream>
#include <boost/filesystem.hpp>
#include <sys/mman.h>

#include "disk/table.h"
#include "nicolas/util.h"

#ifdef LINUX
#include <omp.h>
#endif

using namespace disk;
using namespace std;

Table::Table(uint32_t cardinality, uint32_t n_rows) : cardinality(cardinality), number_of_rows(n_rows) {
    for (int i = 0; i < cardinality; ++i) {
        update_bitmaps.push_back(new ibis::bitvector());
        last_rowid.push_back(number_of_rows);
        new_part.push_back(new std::vector<uint32_t>());
    }
    readAll();

    int fdes = open(DATA_PATH.c_str(), OPEN_READONLY);
    struct stat statbuf;
    fstat(fdes, &statbuf);
    src = (int *) mmap(0, statbuf.st_size, PROT_READ, MAP_PRIVATE, fdes, 0);
}

void Table::append(int val) {
    new_part[val]->push_back(number_of_rows);
    number_of_rows += 1;
}

void Table::update(unsigned int rowid, int to_val) {
    int from_val = getValue(rowid);
    if (from_val == -1)
        return;
    update(rowid, to_val, from_val);
}

void Table::update(unsigned int rowid, int to_val, int from_val) {
    int a[] = {1, 0};
    update_bitmaps[to_val]->setBit(rowid, a[update_bitmaps[to_val]->getBit(rowid)]);
    update_bitmaps[from_val]->setBit(rowid, a[update_bitmaps[from_val]->getBit(rowid)]);
}

void Table::remove(unsigned int rowid) {
    // TODO: remove row in new part
    int val = getValue(rowid);
    if (val == -1)
        return;
    remove(rowid, val);
}

void Table::remove(unsigned int rowid, unsigned int val) {
    int a[] = {1, 0};
    update_bitmaps[val]->setBit(rowid, a[update_bitmaps[val]->getBit(rowid)]);
}

void Table::readAll() {
    for (uint32_t i = 0; i < cardinality; ++i) {
        ibis::bitvector bitvector;
        bitvector.read(getBMName(i).c_str());
        bitvector.buildIndex();
        indices.push_back(bitvector.index);
        update_bitmaps[i]->adjustSize(0, number_of_rows);
    }
}

int Table::evaluate(uint32_t val) {
    ibis::bitvector res;
    return evaluate(val, res);
}

int Table::evaluate(uint32_t val, ibis::bitvector &res) {
    ibis::bitvector bitvector;
    bitvector.read(getBMName(val).c_str());

    auto cnt = update_bitmaps[val]->cnt();

    if (cnt == 0) {
        res.copy(bitvector);
    }
    else {
        res.copy(*update_bitmaps[val]);
        res ^= bitvector;
    }

    // TODO: fix
    if (new_part[val]->size() > 0) {
        ibis::bitvector tmp;
        auto remain = last_rowid[val] % 31;
        for (auto it = new_part[val]->begin(); it != new_part[val]->end(); ++it) {
            auto r = *it - last_rowid[val];
            if (r < remain) {
                res.setBit(*it, 1);
            }
            tmp.setBit(1, r);
        }
        res.merge(tmp);
    }

    // TODO: threshold for part
    if (cnt > n_merge || new_part[val]->size() > n_merge) {
        res.write(getBMName(val).c_str());
        res.buildIndex();
        indices[val] = res.index;
        update_bitmaps[val]->clear();
        update_bitmaps[val]->appendFill(0, number_of_rows);
        new_part[val]->clear();
    }

    return res.cnt();
}

std::string Table::getBMName(int val) {
    std::stringstream ss;
    ss << INDEX_PATH << val << ".bm";
    return ss.str();
}

int Table::getValue(uint32_t rowid) {
    return src[rowid];
}

int Table::getMem() {
    int mem = 0;
//    for (int i = 0; i < cardinality; ++i) {
//        mem += bitmaps[i]->getSerialSize();
//        mem += bitmaps[i]->index.size() * sizeof(int) * 2;
//        mem += update_bitmaps[i]->getSerialSize();
//        mem += new_part[i]->size() * sizeof(uint32_t);
//    }
    return mem;
}
