#include <iostream>
#include <sys/mman.h>
#include <mutex>
#include <vector>
#include <random>
#include <chrono>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <thread>
#include <fstream>
#include <urcu.h>

#include "fastbit/bitvector.h"
#include "nicolas/base_table.h"
#include "nicolas/tablex.h"
#include "nicolas/util.h"
#include "ub/table.h"
#include "ucb/table.h"
#include "naive/table.h"
#include "cubit/table.h"
#include "cubit/table_lf.h"
#include "cubit/table_lk.h"

#ifdef LINUX
#include "nicolas/perf.h"
#endif

using namespace std;

bool perf_enabled = false;
#define WAIT_FOR_PERF_U (1000 * 50)

static BaseTable *table;

// To guarantee worker threads can print out results sequentially 
// (i.e., without mixing).
mutex g_dump_results_lock;

uint64_t db_timestamp __attribute__((aligned(128)));
uint64_t db_number_of_rows;

void build_index(Table_config *config) {
    // Check whether the index has been built before.
    ifstream doneFlag;
    doneFlag.open(config->INDEX_PATH + "/" + "done");
    if (doneFlag.good()) { 
        cout << "WARNING: The index for " + config->INDEX_PATH + " has been built before. Skip building." << endl;
        doneFlag.close();
        return;
    }
    
    ibis::horometer timer;
    timer.start();

    int *src;
    int fdes = open(config->DATA_PATH.c_str(), OPEN_READONLY);
    struct stat statbuf;
    fstat(fdes, &statbuf);
    src = (int *) mmap(0, statbuf.st_size, PROT_READ, MAP_PRIVATE, fdes, 0);

    TableX table(config->g_cardinality);
    for (int i = 0; i < config->n_rows; ++i) {
        table.appendRow(src[i] - 1, config);
    }
    table.write(config->INDEX_PATH);

    timer.stop();
    std::cout << "nicolas:: build_index() took "
    << timer.CPUTime() << " CPU seconds, "
    << timer.realTime() << " elapsed seconds. Encoding scheme: " << config->encoding << endl;

    table.check();

    //create file "done" 
    std::fstream done;   
    done.open(config->INDEX_PATH + "/" + "done", ios::out);
    if (done.is_open()) {
        done << "Succeeded in building bitmap files";
        cout << "Succeeded in building bitmap files in " << config->INDEX_PATH << endl;
        done.close(); 
    }
    else {
        cout << "Failed in building bitmap files in " << config->INDEX_PATH << endl;
    } 
}

void verify_op_rcu(int rid, int expected_val, Table_config *config)
{
    /* WARNING: To enable this feature, we need a way to protect
     *          get_value by using locks.
     */
    /*
    if (approach =="naive") {
        naive::Table* table2 = dynamic_cast<naive::Table*>(table);
        assert(table2->get_value(rid) == expected_val);
    }
    */ 
    // if (config->approach == "ub") {
    //     ub::Table* table2 = dynamic_cast<ub::Table*>(table);
    //     assert(table2->get_value(rid) == expected_val);
    // }
    if ((config->approach == "cubit-lf") || (config->approach == "cubit-lk")) {
        RUB rub_t{0, TYPE_INV, {}};
        cubit::Cubit* table2 = dynamic_cast<cubit::Cubit*>(table);
        assert(table2->get_value_rcu(rid, table2->get_g_timestamp(), rub_t) == expected_val);
    }
    return;
}

void worker_func(int tid, const string& mode, Table_config *config)
{
    auto seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);
    int btv_range;
    if (config->encoding == EE) {
        btv_range = config->g_cardinality - 1;
    }
    else if (config->encoding == RE || config->encoding == AE) {
        btv_range = config->g_cardinality - 2;
    }
    else {
        assert(0);
    }
    uniform_int_distribution<int> val_distribution(0, btv_range);

#if defined(VERIFY_RESULTS)
    int rid_min = (config->n_rows/config->n_workers)*tid;
    int rid_max = (config->n_rows/config->n_workers)*(tid+1) - 1;
    uniform_int_distribution<uint32_t> rid_distribution(rid_min, rid_max);
    cout << "Thread " << tid << ". Rid range: [" << rid_min << ", " << rid_max << "]" << endl;
#else
    uniform_int_distribution<uint32_t> rid_distribution(0, config->n_rows - 1);
#endif

    unsigned long timeout_begin, timeout_end;
    unsigned long t_before, t_after; // HZ numbers
    vector<unsigned long> times_Q; // Query
    vector<unsigned long> times_I; // Insert
    vector<unsigned long> times_D; // Delete
    vector<unsigned long> times_U; // Update
    long l_n_query=0, l_n_insert=0, l_n_delete=0, l_n_update=0; // Numbers of different ops.

    double ratio = (double) config->n_udis / (double) (config->n_udis + config->n_queries);
    uniform_real_distribution<double> ratio_distribution(0.0, 1.0);
    string _mode = mode;

    LOGGER(ibis::gVerbose > 5)
        << "Creating worker thread: " << tid 
        << ". Thread id: " << this_thread::get_id() << endl;

    rcu_register_thread();

    int perf_pid;
    if (perf_enabled == true && config->segmented_btv && tid == 1) {
        string size = to_string(config->n_rows / config->rows_per_seg);
        cout << "Generating perf process for segmented btv " << size << endl;
        perf_pid = gen_perf_process((char *)size.c_str());
    }

    timeout_begin = read_timestamp();
    for (int i = 0; i < config->n_queries + config->n_udis; ++i) {
        int val = val_distribution(generator);
        if (ratio_distribution(generator) < ratio) {
            uint32_t rid = rid_distribution(generator);
            if (mode == "mix" || mode == "range") {
                int test = rid_distribution(generator);
                switch (test % 3) {
                    case 0:
                        _mode = "update";
                        break;
                    case 1:
                        _mode = "delete";
                        break;
                    case 2:
                        _mode = "insert";
                        break;
                }
            }
            if (_mode == "update") {
                t_before = read_timestamp();
                #if defined(VERIFY_RESULTS)
                rcu_read_lock();
                #endif
                int ret = table->update(tid, rid, val);
                #if defined(VERIFY_RESULTS)
                if (!ret) {
                    verify_op_rcu(rid, val, config);
                }
                rcu_read_unlock();
                #endif
                l_n_update ++;
                t_after = read_timestamp();
                if (config->verbose)
                    times_U.push_back(rdtsc_diff(t_before, t_after));
            }
            else if (_mode == "delete") {
                t_before = read_timestamp();
                #if defined(VERIFY_RESULTS)
                rcu_read_lock();
                #endif
                int ret = table->remove(tid, rid);
                #if defined(VERIFY_RESULTS)
                if (!ret) {
                    verify_op_rcu(rid, -1, config);
                }
                rcu_read_unlock();
                #endif
                //FIXME: In util.cpp, a counter is called n_delete(s),
                //      which, actually, should be called g_n_UDIs.
                l_n_delete ++;
                t_after = read_timestamp();
                if (config->verbose)
                    times_D.push_back(rdtsc_diff(t_before, t_after));
            }
            else if (_mode == "insert") {
                t_before = read_timestamp();
                table->append(tid, val);
                l_n_insert ++;
                t_after = read_timestamp();
                if (config->verbose)
                    times_I.push_back(rdtsc_diff(t_before, t_after));
            }
        } else {
            t_before = read_timestamp();
            if (mode == "range" && config->approach == "cubit-lk") 
            {
                cubit::Cubit *table2 = dynamic_cast<cubit::Cubit*>(table);
                table2->range(tid, 0, config->n_range);
            } else {
                table->evaluate(tid, val);
            }
            l_n_query ++;
            t_after = read_timestamp();
            if (config->verbose)
                times_Q.push_back(rdtsc_diff(t_before, t_after));
        }

        if (config->verbose && config->show_memory && i % 1000 == 0) {
            table->printMemory();
            table->printUncompMemory();
        }

        if (config->time_out > 0 && i % 100 == 0) {
            timeout_end = read_timestamp();
            if (rdtsc_diff(timeout_begin, timeout_end) > config->time_out * 1000000000) {
                break;
            }
        }
    }
    timeout_end = read_timestamp();

    if (perf_enabled == true && config->segmented_btv && tid == 1) {
        kill_perf_process(perf_pid);
        usleep(WAIT_FOR_PERF_U);
    }

    rcu_unregister_thread();

    {
        // To dump experimental results sequentially.
        lock_guard<mutex> guard(g_dump_results_lock);

        auto l_n_total = l_n_query + l_n_insert + l_n_delete + l_n_update;
        cout << endl << "Thread " << dec << tid 
            << " Queries " << l_n_query
            << " Inserts " << l_n_insert
            << " Deletes " << l_n_delete
            << " Updates " << l_n_update
            << " Total " << l_n_total 
            << endl;

        if (config->verbose) {
            cout << "Details are shown below." << endl;
            for (long t : times_Q)
                cout << "Q " << t << endl;
            for (long t : times_I)
                cout << "I " << t << endl;                
            for (long t : times_D)
                cout << "D " << t << endl;
            for (long t : times_U)
                cout << "U " << t << endl;
            cout << "Thread " << tid << " has dumped all results." << endl << endl;
        }
        else {
            cout << "Throughput " << l_n_total / (rdtsc_diff(timeout_begin, timeout_end) / 1000000000.0) 
                << "   op/s" << endl;
        }
    }
}

void evaluate(Table_config *config, string mode) 
{
    auto t1 = std::chrono::high_resolution_clock::now();
    if (config->approach == "ub") {
        table = new ub::Table(config);
    } else if (config->approach == "cubit-lk") {
        table = new cubit_lk::CubitLK(config);
    } else if (config->approach == "cubit-lf" || config->approach =="cubit") {
        table = new cubit_lf::CubitLF(config);
    } else if (config->approach == "ucb") {
        table = new ucb::Table(config);
    } else if (config->approach == "naive") {
        table = new naive::Table(config);
    }
    else {
        cerr << "Unknown approach." << endl;
        exit(-1);
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    long long time1 = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
    cout << "=== Bitmap init time (ms): " << time1/1000 << endl;

    if (config->show_memory) {
        table->printMemory();
        table->printUncompMemory();
    }

    #if defined(VERIFY_RESULTS)
    cout << "**************************************************" << endl <<
            "WARNING: Debug option VERIFY_RESULTS is set." << endl <<
            "         The result should no be used for " << endl <<
            "         performance evaluation." << endl <<
            "**************************************************" << endl;
    #endif

    // To initiate background maintaince threads for CUBIT.
    uint32_t WORKERS_PER_MERGE_TH = config->n_workers_per_merge_th;
    int n_merge_ths;
    std::thread *merge_ths;

    if ((config->approach == "cubit-lf") || (config->approach == "cubit-lk")) 
    {
        cubit::Cubit *bitmap = dynamic_cast<cubit::Cubit*>(table);
        merge_ths = new thread[config->n_workers / WORKERS_PER_MERGE_TH + 1];

        __atomic_store_n(&run_merge_func, true, MM_CST);

        n_merge_ths = config->n_workers / WORKERS_PER_MERGE_TH;
        for (int i = 0; i < n_merge_ths; i++) {
            int begin = i * WORKERS_PER_MERGE_TH;
            int range = WORKERS_PER_MERGE_TH;
            cout << "Range for merge thread " << i << " : " << begin << " : " << range << endl;
            merge_ths[i] = std::thread(merge_func, table, begin, range, config, nullptr);
        }
        if ( config->n_workers % WORKERS_PER_MERGE_TH != 0) {
            int begin = n_merge_ths * WORKERS_PER_MERGE_TH;
            int range = (config->n_workers % WORKERS_PER_MERGE_TH);
            cout << "Range for merge thread " << n_merge_ths << " : " << begin << " : " << range << endl;
            merge_ths[n_merge_ths] = std::thread(merge_func, table, begin, range, config, nullptr);
            n_merge_ths ++;
        }
        cout << "Creating " << n_merge_ths << " merging threads" << endl;

        #if defined(VERIFY_RESULTS)
        assert(bitmap->init_sequential_test());
        #endif
    }

    std::thread *ths = new thread[config->n_workers];
    for (int i = 0; i < config->n_workers; i++) {
        struct ThreadInfo *info = &g_ths_info[i];
        info->tid = i;
        info->active_trans = NULL;
        
        ths[i] = std::thread(worker_func, i, mode, config);
    }

    for (int i = 0; i < config->n_workers; i++) {
        ths[i].join();
    }
    delete[] ths;

    if ((config->approach == "cubit-lf") || (config->approach == "cubit-lk")) 
    {
        __atomic_store_n(&run_merge_func, false, MM_CST);

        for (int i = 0; i < n_merge_ths; i++) {
            merge_ths[i].join();
        }
        delete[] merge_ths;
    }
    
    if (config->show_memory) {
        table->printMemory();
        table->printUncompMemory();
    }

    return;
}

int index_is_valid(Table_config *config)
{
    ifstream doneFlag;
    doneFlag.open(config->INDEX_PATH + "/" + "done");
    if (!doneFlag.good()) {
        cout << "WARNING: The index for " + config->INDEX_PATH + " hasn't been set correctly. Please check." << endl;
        return 0;
    }
    doneFlag.close();
    return 1;
}

////////////////////////////////////////////////////////////////////
// FIXME: The following functions are single-threaded versions
//          that haven't been parallelized yet.
////////////////////////////////////////////////////////////////////
void evaluateRange(Table_config *config) {
    auto seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);
    uniform_int_distribution<uint32_t> distribution(0, config->g_cardinality - 1 - config->n_range);

    ub::Table *table = new ub::Table(config);

    struct timeval before, after;
    struct timeval time_out_begin, time_out_end;

    cout << "range = " << config->n_range << endl;

    gettimeofday(&time_out_begin, NULL);
    for (int i = 0; i < config->n_queries; ++i) {
        auto r = distribution(generator);
        gettimeofday(&before, NULL);
        table->range(0, r, config->n_range);
        gettimeofday(&after, NULL);

        if (config->verbose)
            cout << "Q " << time_diff(before, after) << endl;

        if (config->time_out > 0 && i % 10 == 0) {
            gettimeofday(&time_out_end, NULL);
            if (time_diff(time_out_begin, time_out_end) > config->time_out * 1000000000) {
                break;
            }
        }
    }
}

// FIXME: Single-threaded version
void evaluateGetValue(Table_config *config);
void evaluateImpact(Table_config *config, int tid) {
    auto seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);
    uniform_int_distribution<int> distribution(0, config->g_cardinality - 1);
    uniform_int_distribution<uint32_t> rid_distribution(0, config->n_rows - 1);

    struct timeval before, after;
    struct timeval time_out_begin, time_out_end;

    uniform_real_distribution<double> ratio_distribution(0.0, 1.0);

    int comp[] = {5,10,15,20,50,100,200,300,500,1000,2000,5000,10000};

    gettimeofday(&time_out_begin, NULL);
    for (int i = 0; i < 13; ++i) {
        BaseTable *table = new ub::Table(config);
        evaluateGetValue(config);

        int thres = comp[i];

        cout << "Current comp: " << thres << endl;

        for (int j = 0; j < thres; ++j) {
            uint32_t rid = rid_distribution(generator);
            int val = 0;
            gettimeofday(&before, NULL);
            table->update(tid, rid, val);
            gettimeofday(&after, NULL);
            cout << "U " << time_diff(before, after) << endl;
        }

        gettimeofday(&before, NULL);
        table->evaluate(tid, 0);
        gettimeofday(&after, NULL);
        cout << "Q " << time_diff(before, after) << endl;
    }
}

// FIXME: Single-threaded version
void evaluateGetValue(Table_config *config) {
    ub::Table table(config);

    auto seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);
    uniform_int_distribution<uint32_t> distribution(0, config->n_rows - 1);

    struct timeval before, after;
    gettimeofday(&before, NULL);
    for (int i = 0; i < 10000; ++i) {
        int test = distribution(generator);
        table.get_value(test);
    }
    gettimeofday(&after, NULL);
    cout << time_diff(before, after) << endl;
}

int main(const int argc, const char *argv[]) 
{
    po::variables_map options = get_options(argc, argv);
    Table_config *config = new Table_config{};
    init_config(config, options);

    if (options.count("encoding-scheme")) {
        string encoding = options["encoding-scheme"].as<string>();
        if (encoding == "EE") config->encoding = EE;
        else if (encoding == "RE") config->encoding = RE;
        else if (encoding == "AE") config->encoding = AE;
        else if (encoding == "IE") config->encoding = IE;
        else {
            cout << "Invalid encoding scheme: " << encoding << endl;
            assert(0);
        }

        if (config->encoding == RE) {       // Range encoding
            config->INDEX_PATH += "_RE";
            cout << "Index path: " << config->INDEX_PATH << endl;
        }

        if (config->encoding == AE) {        // Anchor encoding
            config->AE_left_margin = 2;
            config->AE_right_margin = 2;
            config->AE_interval = 3;

            // To set anchors
            int n_btvs = config->g_cardinality - 1;  // Each RE or AE has cardinality-1 bitvectors.
            int AE_btvs = n_btvs - config->AE_left_margin - config->AE_right_margin;
            if (AE_btvs >= config->AE_interval) {
                config->encoding = AE;
                config->INDEX_PATH += "_AE";
                cout << "Index path: " << config->INDEX_PATH << endl;
                for (int i = config->AE_right_margin; i < n_btvs - config->AE_left_margin; i += config->AE_interval) {
                    config->AE_anchors[i] = i;
                    for (int j = 1; j < config->AE_interval; j++) {
                        if ((i+j) >= n_btvs - config->AE_left_margin) break;
                        config->AE_curves[i+j] = i;  // The value field indicates the corresponding anchor.
                    }
                }

                for (int i = 0; i < config->AE_right_margin; i++)
                    config->AE_margins[i] = i;

                for (int i = 1; i <= config->AE_left_margin; i++)
                    config->AE_margins[n_btvs-i] = n_btvs-i;

                #if defined(VERIFY_RESULTS)
                map<int, int> tmp = {};
                tmp.insert(config->AE_margins.begin(), config->AE_margins.end());
                tmp.insert(config->AE_anchors.begin(), config->AE_anchors.end());
                tmp.insert(config->AE_curves.begin(), config->AE_curves.end());
                assert(tmp.size() == n_btvs);
                #endif
            } else {
                cout << "Cardinality is too small to leverage Anchor Encoding. Use Range Encoding instead." << endl;
                config->encoding = RE;
                config->INDEX_PATH += "_RE";
                cout << "Index path: " << config->INDEX_PATH << endl;
            }
        }
    }
    cout << "=== Encoding scheme: " << config->encoding << endl;

    if (options.count("mode")) {
        string mode = options["mode"].as<string>();

        if (mode == "build") {
            build_index(config);
            return 0;
        }

        if (!index_is_valid(config))
            return 0;

        if (mode == "getvalue") {
            evaluateGetValue(config);
        } else if (mode == "impact") {
            evaluateImpact(config, 0);
        } 
        // else if (mode == "range") {
        //     evaluateRange(config);
        // } 
        else {
            evaluate(config, mode);
        }
    }

    return 0;
}

