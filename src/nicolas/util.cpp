#include <urcu.h>

#include "fastbit/bitvector.h"
#include "nicolas/base_table.h"
#include "nicolas/tablex.h"
#include "nicolas/util.h"
#include "ub/table.h"
#include "ucb/table.h"
#include "naive/table.h"
#include "cubit/table_lf.h"
#include "cubit/table_lk.h"

#define N_MAX_WORKERS (256)
#define HZ (2.0) //FIXME: only for the SkyLake server with Intel(R) Xeon(R) Gold 5117 CPU @ 2.00GHz.

using namespace std;

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
            /* Algorithm Configuration */
            ("approach,a", po::value<string>()->default_value(std::string("ub")), "naive, ucb, ub, cubit-lk, cubit-lf")
            ("mode,m", po::value<string>(), "build / query / test")
            ("encoding-scheme,e", po::value<string>()->default_value("EE"), "EE / RE / AE / IE")
            ("data-path,d", po::value<string>(), "data file path")
            ("index-path,i", po::value<string>(), "index file path")
            ("number-of-rows,n", po::value<uint64_t>(), "number of rows")
            ("cardinality,c", boost::program_options::value<int32_t >()->default_value(1), "cardinality")
            ("number-of-queries", po::value<uint32_t>(), "number of queries")
            ("number-of-udis,r", po::value<uint32_t>(), "number of UDIs")
            /* Fence Pointer */
            ("fence-pointer", po::value<bool>()->default_value(false), "enable fence pointers")
            ("fence-length", po::value<unsigned int>()->default_value(1000), "lengh of fence pointers")
            /* Parallel */
            ("workers,w",po::value<uint32_t>(), "number of concurrent workers")
            ("workers_per_merge_th,q",po::value<uint32_t>()->default_value(4), "number of threads performing merge operation")
            ("merge-threshold,p",po::value<uint32_t>()->default_value(128), "merge threshold for both UpBit and CUBIT")
            ("num-embarr-parallel", po::value<unsigned int>()->default_value(8), "num of threads for parallelized get_value/bitmap_init/segbtv_cnt")
            ("parallel-cnt", po::value<bool>()->default_value(false), "enable segbtv_cnt")
            ("rows-per-seg,s", po::value<uint64_t>(), "Rows per segment")
            /* Helper Functions */
            ("show-memory", po::value<bool>()->default_value(false), "show memory")
            ("help", "produce help message")
            ("verbose,v", po::value<bool>()->default_value(true), "verbose")
            ("perf", po::value<bool>()->default_value(false), "enable perf")
            /* Retired */
            ("on-disk", po::value<bool>()->default_value(false), "on disk")
            ("cache-misses", po::value<bool>()->default_value(false), "measure cache misses")
            ("time-out,t", po::value<unsigned int>()->default_value(0), "time out (s)")
            ("range", po::value<uint32_t>()->default_value(1), "range length")
            ("range-algorithm", po::value<string>()->default_value(std::string("pq")), "pq, naive, uncompress");          

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
        config->n_rows = options["number-of-rows"].as<uint64_t>();
    if (options.count("number-of-udis"))
        config->n_udis = options["number-of-udis"].as<uint32_t>();
    if (options.count("number-of-queries"))
        config->n_queries = options["number-of-queries"].as<uint32_t>();
    config->verbose = options["verbose"].as<bool>();
    config->enable_fence_pointer = options["fence-pointer"].as<bool>();
    config->enable_parallel_cnt = options["parallel-cnt"].as<bool>();
    enable_fence_pointer = config->enable_fence_pointer;
    config->show_memory = options["show-memory"].as<bool>();
    config->on_disk = options["on-disk"].as<bool>();
    config->approach = options["approach"].as<string>();
    // This variable is defind in fastbit/bitvector.cpp
    // Ideally, each bitmap must have its own INDEX_WORDS.
    // But for simplicity, we assume they use the same configuration here.
    INDEX_WORDS = options["fence-length"].as<unsigned int>();
    config->nThreads_for_getval = options["num-embarr-parallel"].as<unsigned int>();
    config->time_out = options["time-out"].as<unsigned int>();
    // We didn't use the following two parameters.
    config->n_range = options["range"].as<uint32_t>();
    if (config->n_range <= 1) {
        config->n_range = 1;
        cout << "=== Range length must be greater than 1. Set to 1. ===" << endl;
    }
    config->range_algo = options["range-algorithm"].as<string>();

    config->showEB = false;
    config->decode = false;
    config->autoCommit = true;
    config->n_merge_threshold = options["merge-threshold"].as<uint32_t>();
    config->db_control = false;
    config->n_workers_per_merge_th = options["workers_per_merge_th"].as<uint32_t>();

    // For encoding schemes
    config->encoding = EE;
    config->AE_left_margin = 0;
    config->AE_right_margin = 0;
    config->AE_interval = 0;
    config->AE_anchors = {};

    // For segmented bitvectors
    config->encoded_word_len = 31;
    if (options.count("rows-per-seg") && (config->approach == "cubit-lk" || config->approach == "cubit-lf"))
        config->segmented_btv = true;
    else
        config->segmented_btv = false;
    if (config->segmented_btv) {
        config->rows_per_seg = options["rows-per-seg"].as<uint64_t>();
        cout << "=== Using Segmented Btv. Row per seg: " << config->rows_per_seg << endl;
    }
}
