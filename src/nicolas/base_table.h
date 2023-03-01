#ifndef BASE_TABLE_H
#define BASE_TABLE_H

#include "fastbit/bitvector.h"
#include "nicolas/util.h"
#include <thread>
#include <future>

struct TransDesc {
    struct TransDesc *next;

    uint32_t l_number_of_rows;
    uint32_t l_inc_rows;

    uint64_t l_start_ts;
    uint64_t l_commit_ts;

    vector<uint64_t> rubs;
    vector<uint32_t> updated_rows;

    struct TransDesc *l_end_trans;

    // For helping update the new bitmap.
    // For nbub_lf only.
    uint32_t val_h;
    void *bitmap_old_h;
    void *bitmap_new_h;

} DOUBLE_CACHE_ALIGNED;

struct ThreadInfo {
    int tid;
    TransDesc * active_trans;
} DOUBLE_CACHE_ALIGNED;

class BaseTable {
public:
    BaseTable(int32_t cardinality) : cardinality(cardinality) {}

    Table_config *config;

    virtual int update(int tid, unsigned int rowid, int to_val) { return -1; }

    virtual int remove(int tid, unsigned int rowid) { return -1; }

    virtual int append(int tid, int val) { return -1; }

    virtual int evaluate(int tid, uint32_t val) { return -1; }

    virtual void printMemory() { return; }

    virtual void printUncompMemory() { return; }

protected:
    const int32_t cardinality;

    std::string getBitmapName(int val) {
        std::stringstream ss;
        //ss << INDEX_PATH << val << ".bm";
        ss << config->INDEX_PATH << "/" << val << ".bm";
        return ss.str();
    }
};

void merge_func(BaseTable *table, int begin, int range, Table_config *config);

extern bool run_merge_func;

#endif
