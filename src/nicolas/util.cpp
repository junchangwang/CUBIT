#include <urcu.h>

#include "fastbit/bitvector.h"
#include "nicolas/base_table.h"
#include "nicolas/tablex.h"
#include "nicolas/util.h"
#include "ub/table.h"
#include "ucb/table.h"
#include "naive/table.h"
#include "nbub/table_lf.h"
#include "nbub/table_lk.h"
#include "config.h"

#define N_MAX_WORKERS (256)
#define HZ (2.0) //FIXME: only for the SkyLake server with Intel(R) Xeon(R) Gold 5117 CPU @ 2.00GHz.

struct ThreadInfo g_ths_info[N_MAX_WORKERS];

// Export this variable to bitvector. This is a shortcut fix. 
// Bitvector should use the version in Table_config in future.
bool enable_fence_pointer = false;

double time_diff(struct timeval x, struct timeval y) {
    double x_ms , y_ms , diff;
    x_ms = (double) x.tv_sec * 1000000 + (double) x.tv_usec;
    y_ms = (double) y.tv_sec * 1000000 + (double) y.tv_usec;
    diff = y_ms - x_ms;
    return diff;
}

// Receive two HZ numbers, and return the difference in nanoseconds.
long rdtsc_diff(long before, long after)
{
    if (after <= before)
        return 0;
    return (after-before) / (HZ);
}

int _CAS2(volatile long * ptr, long * cmp1, long * cmp2, long val1, long val2)
{
  char success;
  long tmp1 = *cmp1;
  long tmp2 = *cmp2;

  __asm__ __volatile__(
      "lock cmpxchg16b %1\n"
      "setz %0"
      : "=q" (success), "+m" (*ptr), "+a" (tmp1), "+d" (tmp2)
      : "b" (val1), "c" (val2)
      : "cc" );

  *cmp1 = tmp1;
  *cmp2 = tmp2;
  return success;
}

po::variables_map get_options(const int argc, const char *argv[]) {
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
            ("workers,w",po::value<uint32_t>(), "number of concurrent workers")
            ("mode,m", po::value<string>(), "build / query / test")
            ("data-path,d", po::value<string>(), "data file path")
            ("index-path,i", po::value<string>(), "index file path")
            ("number-of-rows,n", po::value<uint32_t>(), "number of rows")
            ("number-of-queries", po::value<uint32_t>(), "number of queries")
            ("merge", po::value<uint32_t>()->default_value(128), "merge UB")
            ("verbose,v", po::value<bool>()->default_value(true), "verbose")
            ("removed,r", po::value<uint32_t>(), "number of deleted rows")
            ("fence-pointer", po::value<bool>()->default_value(false), "enable fence pointers")
            ("show-memory", po::value<bool>()->default_value(false), "show memory")
            ("help", "produce help message")
            ("on-disk", po::value<bool>()->default_value(false), "on disk")
            ("approach,a", po::value<string>()->default_value(std::string("ub")), "naive, ucb, ub, nbub-lk, nbub-lf")
            ("fence-length", po::value<unsigned int>()->default_value(1000), "lengh of fence pointers")
            ("num-threads", po::value<unsigned int>()->default_value(16), "num of threads for parallelized get_value()")
            ("cache-misses", po::value<bool>()->default_value(false), "measure cache misses")
            ("perf", po::value<bool>()->default_value(false), "enable perf")
            ("time-out,t", po::value<unsigned int>()->default_value(0), "time out (s)")
            ("range", po::value<uint32_t>()->default_value(0), "range length")
            ("range-algorithm", po::value<string>()->default_value(std::string("pq")), "pq, naive, uncompress")
            ("cardinality,c", boost::program_options::value<int32_t >()->default_value(1), "cardinality")
            ("workers_per_merge_th,q",po::value<uint32_t>(), "n_workers_per_merge_th ")
            ("merge-thresh,p",po::value<int>(), "merge threshold");

    po::positional_options_description p;
    p.add("mode", -1);

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << endl;
        exit(1);
    }

    return vm;
}

void init_config(Table_config *config, po::variables_map options) {
    config->n_workers = options.count("workers") ?
                    options["workers"].as<uint32_t>() : 1;
    if (options.count("data-path"))
        config->DATA_PATH = options["data-path"].as<string>();
    if (options.count("cardinality"))
        config->g_cardinality = options["cardinality"].as<int32_t>();
    if (options.count("index-path"))
        config->INDEX_PATH = options["index-path"].as<string>();
    if (options.count("number-of-rows"))
        config->n_rows = options["number-of-rows"].as<uint32_t>();
    if (options.count("removed"))
        config->n_deletes = options["removed"].as<uint32_t>();
    if (options.count("number-of-queries"))
        config->n_queries = options["number-of-queries"].as<uint32_t>();
    config->n_merge = options["merge"].as<uint32_t>();
    config->verbose = options["verbose"].as<bool>();
    config->enable_fence_pointer = options["fence-pointer"].as<bool>();
    enable_fence_pointer = config->enable_fence_pointer;
    config->show_memory = options["show-memory"].as<bool>();
    config->on_disk = options["on-disk"].as<bool>();
    config->approach = options["approach"].as<string>();
    // This variable is defind in fastbit/bitvector.cpp
    // Ideally, each bitmap must have its own INDEX_WORDS.
    // But for simplicity, we assume they use the same configuration here.
    INDEX_WORDS = options["fence-length"].as<unsigned int>();
    config->nThreads_for_getval = options["num-threads"].as<unsigned int>();
    config->time_out = options["time-out"].as<unsigned int>();
    config->n_range = options["range"].as<uint32_t>();
    config->range_algo = options["range-algorithm"].as<string>();
    config->showEB = false;
    config->decode = false;
    config->autoCommit = true;
    config->n_merge_threash = options.count("merge-thresh") ?
                    options["merge-thresh"].as<int>() : 1;    
    config->db_control = false;
    config->n_workers_per_merge_th = options.count("workers_per_merge_th") ?
                    options["workers_per_merge_th"].as<uint32_t>() : 4; 
}
