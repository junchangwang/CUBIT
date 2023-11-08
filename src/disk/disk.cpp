#include <iostream>
#include <sys/mman.h>
#include <vector>
#include <random>
#include <chrono>

#include "fastbit/bitvector.h"
#include "disk/table.h"
#include "nicolas/util.h"

using namespace std;
using namespace disk;

void prepare(Table &table) {
    auto seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);
    uniform_int_distribution<int> distribution(0, cardinality - 1);
    for (int i = 0; i < 1000; ++i) {
        int r = distribution(generator);
        table.evaluate(r);
    }
}

void evaluate() {
    long long int seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);
    uniform_int_distribution<int> distribution(0, cardinality - 1);
    uniform_int_distribution<uint32_t> rid_distribution(0, n_rows - 1);

    Table table(cardinality, n_rows);
    prepare(table);

    struct timeval before, after;
    vector<double> times;

    for (int i = 0; i < cardinality; ++i) {
        for (int j = 0; j < n_deletes; ++j) {
            uint32_t rid = rid_distribution(generator);
            table.update(rid, j);
        }
    }

    for (int i = 0; i < n_queries + n_deletes; ++i) {
        int r = distribution(generator);
        gettimeofday(&before, NULL);
        table.evaluate(r);
        gettimeofday(&after, NULL);
        times.push_back(time_diff(before, after));
        if (verbose)
            cout << "Q " << time_diff(before , after) << endl;
        if (times.size() > 1000) {
            double sum = std::accumulate(times.begin(), times.end(), 0.0);
            double mean = sum / times.size();
            double sq_sum = std::inner_product(times.begin(), times.end(), times.begin(), 0.0);
            double stddev = std::sqrt(sq_sum / times.size() - mean * mean);
            printf("time:\t %.0lf us \tstddev: %.0lf us\n", mean, stddev);
            times.clear();
        }
    }
}

int main(const int argc, const char *argv[]) {
    po::variables_map options = get_options(argc, argv);
    init(options);

    if (options.count("mode")) {
        string mode = options["mode"].as<string>();
        if (mode == "build") {
            evaluate();
        }
    }

    return 0;
}
