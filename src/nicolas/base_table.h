#ifndef BASE_TABLE_H
#define BASE_TABLE_H

#include <thread>
#include <future>
#include <shared_mutex>

#include "nicolas/util.h"
#include "fastbit/bitvector.h"

struct ThreadInfo {
    int tid;
    void *active_trans;
} DOUBLE_CACHE_ALIGNED;

class BaseTable {
public:
    BaseTable(Table_config *config) : config(config), cardinality(config->g_cardinality) {}

    Table_config *const config;

    virtual int update(int tid, uint64_t rowid, int to_val) { return -1; }

    virtual int remove(int tid, uint64_t rowid) { return -1; }

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

void merge_func(BaseTable *table, int begin, int range, Table_config *config, std::shared_mutex *bitmap_mutex = nullptr);

extern bool run_merge_func;

#endif
