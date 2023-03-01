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
#include "nbub/table_lf.h"
#include "nbub/table_lk.h"
#include "config.h"

#ifdef LINUX
#include "nicolas/perf.h"
#endif

using namespace std;

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
    << timer.realTime() << " elapsed seconds" << endl;

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
    if (config->approach == "ub") {
        ub::Table* table2 = dynamic_cast<ub::Table*>(table);
        assert(table2->get_value(rid) == expected_val);
    }
    else if ((config->approach == "nbub-lf") || (config->approach == "nbub-lk")) {
        uint64_t rub_t = 0UL;
        nbub::Nbub* table2 = dynamic_cast<nbub::Nbub*>(table);
        assert(table2->get_value_rcu(rid, table2->get_g_timestamp(), &rub_t) == expected_val);
    }
    return;
}

// NOTE: Junchang removed the code snippets for the Linux perf
//       which was originally in nicolas, because Linux perf 
//       has contained an 'annotate' command that can already perform
//       fine-grained performance analysis.
void worker_func(int tid, const string& mode, Table_config *config)
{
    // FIXME: need to check if the following random number generator
    //          is correct and efficient when used in parallelized programs.
    auto seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);
    uniform_int_distribution<int> val_distribution(0, config->g_cardinality - 1);
#if defined(VERIFY_RESULTS)
    int rid_min = (config->n_rows/config->n_workers)*tid;
    int rid_max = (config->n_rows/config->n_workers)*(tid+1) - 1;
    uniform_int_distribution<uint32_t> rid_distribution(rid_min, rid_max);
    cout << "Thread " << tid << ". Rid range: [" << rid_min << ", " << rid_max << "]" << endl;
#else
    uniform_int_distribution<uint32_t> rid_distribution(0, config->n_rows - 1);
#endif

    // struct timeval time_out_begin, time_out_end;
    // struct timeval before, after;
    // vector<double> times;
    unsigned long timeout_begin, timeout_end;
    unsigned long t_before, t_after; // HZ numbers
    vector<unsigned long> times_Q; // Query
    vector<unsigned long> times_I; // Insert
    vector<unsigned long> times_D; // Delete
    vector<unsigned long> times_U; // Update
    long l_n_query=0, l_n_insert=0, l_n_delete=0, l_n_update=0; // Simple, yet efficient counters.

    double ratio = (double) config->n_deletes / (double) (config->n_deletes + config->n_queries);
    uniform_real_distribution<double> ratio_distribution(0.0, 1.0);
    string _mode = mode;

    LOGGER(ibis::gVerbose > 5)
        << "Creating worker thread: " << tid 
        << ". Thread id: " << this_thread::get_id() << endl;

    rcu_register_thread();

    // gettimeofday(&time_out_begin, NULL);
    timeout_begin = read_timestamp();
    for (int i = 0; i < config->n_queries + config->n_deletes; ++i) {
        int val = val_distribution(generator);
        if (ratio_distribution(generator) < ratio) {
            uint32_t rid = rid_distribution(generator);
            if (mode == "mix") {
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
                // gettimeofday(&before, NULL);
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
                // gettimeofday(&after, NULL);
                t_after = read_timestamp();
                // times.push_back(time_diff(before, after));
                if (config->verbose)
                    times_U.push_back(rdtsc_diff(t_before, t_after));
                //if (verbose)
                //    cout << "U " << time_diff(before, after) << endl;
            }
            else if (_mode == "delete") {
                // gettimeofday(&before, NULL);
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
                //      which, actually, should be called g_n_deletes.
                l_n_delete ++;
                // gettimeofday(&after, NULL);
                t_after = read_timestamp();
                // times.push_back(time_diff(before, after));
                if (config->verbose)
                    times_D.push_back(rdtsc_diff(t_before, t_after));
                // if (verbose)
                //    cout << "D " << time_diff(before, after) << endl;
            }
            else if (_mode == "insert") {
                // gettimeofday(&before, NULL);
                t_before = read_timestamp();
                table->append(tid, val);
                l_n_insert ++;
                // gettimeofday(&after, NULL);
                t_after = read_timestamp();
                // times.push_back(time_diff(before, after));
                if (config->verbose)
                    times_I.push_back(rdtsc_diff(t_before, t_after));
                // if (verbose)
                //    cout << "I " << time_diff(before, after) << endl;
            }
        } else {
            // gettimeofday(&before, NULL);
            t_before = read_timestamp();
            table->evaluate(tid, val);
            l_n_query ++;
            // gettimeofday(&after, NULL);
            t_after = read_timestamp();
            // times.push_back(time_diff(before, after));
            if (config->verbose)
                times_Q.push_back(rdtsc_diff(t_before, t_after));
            // if (verbose)
            //    cout << "Q " << time_diff(before, after)  << endl;
        }

        if (config->verbose && config->show_memory && i % 1000 == 0) {
            table->printMemory();
            table->printUncompMemory();
        }

        /*
        if (times.size() > 1000 && !verbose) {
            double sum = std::accumulate(times.begin(), times.end(), 0.0);
            double mean = sum / times.size();
            double sq_sum = std::inner_product(times.begin(), times.end(), times.begin(), 0.0);
            double stddev = std::sqrt(sq_sum / times.size() - mean * mean);
            printf("time:\t %.0lf us \tstddev: %.0lf us\n", mean, stddev);
            times.clear();
        }
        */

        if (config->time_out > 0 && i % 100 == 0) {
            // gettimeofday(&time_out_end, NULL);
            // if (time_diff(time_out_begin, time_out_end) > time_out * 1000000)
            timeout_end = read_timestamp();
            if (rdtsc_diff(timeout_begin, timeout_end) > config->time_out * 1000000000) {
                break;
            }
        }
    }
    timeout_end = read_timestamp();

    rcu_unregister_thread();

    {
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
    if (config->approach == "ub") {
        table = new ub::Table(config);
    } else if (config->approach == "nbub-lk") {
        table = new nbub_lk::NbubLK(config);
    } else if (config->approach == "nbub-lf" || config->approach =="nbub") {
        table = new nbub_lf::NbubLF(config);
    } else if (config->approach == "ucb") {
        table = new ucb::Table(config);
    } else if (config->approach == "naive") {
        table = new naive::Table(config);
    }
    else {
        cerr << "Unknown approach." << endl;
        exit(-1);
    }

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

#define WORKERS_PER_MERGE_TH (config->n_workers_per_merge_th)
    int n_merge_ths;
    std::thread *merge_ths;

    if ((config->approach == "nbub-lf") || (config->approach == "nbub-lk")) 
    {
        nbub::Nbub *bitmap = dynamic_cast<nbub::Nbub*>(table);
        merge_ths = new thread[config->n_workers / WORKERS_PER_MERGE_TH + 1];

        __atomic_store_n(&run_merge_func, true, MM_CST);

        n_merge_ths = config->n_workers / WORKERS_PER_MERGE_TH;
        for (int i = 0; i < n_merge_ths; i++) {
            int begin = i * WORKERS_PER_MERGE_TH;
            int range = WORKERS_PER_MERGE_TH;
            cout << "Range for merge thread " << i << " : " << begin << " : " << range << endl;
            merge_ths[i] = std::thread(merge_func, table, begin, range, config);
        }
        if ( config->n_workers % WORKERS_PER_MERGE_TH != 0) {
            int begin = n_merge_ths * WORKERS_PER_MERGE_TH;
            int range = (config->n_workers % WORKERS_PER_MERGE_TH);
            cout << "Range for merge thread " << n_merge_ths << " : " << begin << " : " << range << endl;
            merge_ths[n_merge_ths] = std::thread(merge_func, table, begin, range, config);
            n_merge_ths ++;
        }
        cout << "Creating " << n_merge_ths << " merging threads" << endl;
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

    if ((config->approach == "nbub-lf") || (config->approach == "nbub-lk")) 
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

// FIXME: Single-threaded version
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
        table->range(r, config->n_range);
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

int main(const int argc, const char *argv[]) {
    po::variables_map options = get_options(argc, argv);
    Table_config *config = new Table_config{};
    init_config(config, options);

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
        } else if (mode == "range") {
            evaluateRange(config);
        } else {
            evaluate(config, mode);
        }
    }

    return 0;
}
