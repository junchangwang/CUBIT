#ifndef NIC_UTIL_H
#define NIC_UTIL_H

#include <string>
#include <iostream>
#include <numeric> // std::accumulate
#include <cmath>   // std::minus
#include <boost/program_options.hpp>
#include <vector>

using namespace std;

#define T_WORD uint64_t

namespace po = boost::program_options;

typedef __int128 int128_t;
typedef unsigned __int128 uint128_t;

/***********************************/

#define PAGE_SIZE 4096
#define CACHE_LINE_SIZE 64
#define CACHE_ALIGNED __attribute__((aligned(CACHE_LINE_SIZE)))
#define DOUBLE_CACHE_ALIGNED __attribute__((aligned(2 * CACHE_LINE_SIZE)))

// Generate 64-bits long timestamp
// by fetching the rdtsc hardware register.
static __inline__ long read_timestamp(void)
{
    unsigned int __a, __d;
    __asm__ __volatile__("rdtsc" : "=a" (__a), "=d"(__d));
    return ((long)__a) | (((long)__d)<<32);
}

/************************************/

#define MM_CST          __ATOMIC_SEQ_CST
#define MM_RELAXED      __ATOMIC_RELAXED
#define MM_ACQUIRE      __ATOMIC_ACQUIRE
#define MM_RELEASE      __ATOMIC_RELEASE

int _CAS2(volatile long * ptr, long * cmp1, long * cmp2, long val1, long val2);

#define CAS2(p, o1, o2, n1, n2) \
  _CAS2((volatile long *) p, (long *) o1, (long *) o2, (long) n1, (long) n2)

/* Borrowed from perfbook */
#define ACCESS_ONCE(x) (*(volatile typeof(x) *)&(x))
#define READ_ONCE(x) \
                ({ typeof(x) ___x = ACCESS_ONCE(x); ___x; })
#define WRITE_ONCE(x, val) \
                do { ACCESS_ONCE(x) = (val); } while (0)
#define barrier() __asm__ __volatile__("": : :"memory")

#define cmpxchg(ptr, o, n) \
({ \
    typeof(*ptr) _____actual = (o); \
    \
    __atomic_compare_exchange_n((ptr), (void *)&_____actual, (n), 0, \
            __ATOMIC_SEQ_CST, __ATOMIC_RELAXED); \
    _____actual; \
})


/***********************************/

class Table_config {
    public:
        int32_t g_cardinality;
        string DATA_PATH;
        string INDEX_PATH;
        uint32_t n_workers;
        uint32_t n_rows;
        uint32_t n_deletes;
        uint32_t n_queries;
        uint32_t n_merge;
        bool verbose;
        bool enable_fence_pointer;
        bool show_memory;
        bool on_disk;
        string approach;
        unsigned int nThreads_for_getval;
        unsigned int time_out;
        uint32_t n_range;
        string range_algo;
        bool showEB;
        bool decode;
        bool autoCommit;
        int n_merge_threash;
        bool db_control;
        uint32_t n_workers_per_merge_th;
};

/***********************************/

double time_diff(struct timeval x , struct timeval y);
po::variables_map get_options(const int argc, const char *argv[]);
void init_config(Table_config *config, po::variables_map options);
long rdtsc_diff(long before, long after);

extern struct ThreadInfo g_ths_info[];
extern bool enable_fence_pointer;

#endif
