#include <iostream>
#include <sstream>
#include <ibis.h>
#include <numeric> // std::accumulate
#include <cmath>   // std::minus
#include <boost/program_options.hpp>
#include <random>
#include <chrono>

#include "nicolas/proto/queries.pb.h"
#include "nicolas/util.h"

using namespace std;

namespace po = boost::program_options;

string select_clause = "a";

void buildIndex() {
    ibis::horometer timer;
    timer.start();

    ibis::table* tbl(ibis::table::create(DATA_PATH.c_str()));
    tbl->buildIndex("a", "equality");

    timer.stop();
    std::cout << "nicolas:: build_index() took "
            << timer.CPUTime() << " CPU seconds, "
            << timer.realTime() << " elapsed seconds" << endl;
}

void evaluate(ibis::part &part) {
    int ierr;

//    if (n_udis > 0) {
//        long long int seed = chrono::system_clock::now().time_since_epoch().count();
//        default_random_engine generator(seed);
//        uniform_int_distribution<int> distribution(1, cardinality);
//
//        unique_ptr<ibis::tablex> tbl(ibis::tablex::create());
//        tbl->parseNamesAndTypes("a: int");
//
//        std::vector<uint32_t> rows;
//        uniform_int_distribution<int> rid_distribution(0, n_rows - 1);
//
//        for (int i = 0; i < n_udis; ++i) {
//            rows.push_back(rid_distribution(generator));
//            std::string s = std::to_string(distribution(generator));
//            tbl->appendRow(s.c_str());
//        }
//        part.deactivate(rows);
//        int ierr = tbl->write("update", NULL, NULL, "<binning none/><encoding equality/>", NULL);
//        ibis::table* ta(ibis::table::create("update"));
//        ta->buildIndex("a", "<binning none/><encoding equality/>");
//        delete ta;
//        part.append("update");
//    }

    long long int seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);
    uniform_int_distribution<int> distribution(0, cardinality - 1);
    uniform_int_distribution<uint32_t> rid_distribution(0, n_rows - 1);

    struct timeval before, after;
    vector<double> times;

    string where = "a = ";

    for (int i = 0; i < n_queries; ++i) {
        int r = distribution(generator);
        gettimeofday(&before, NULL);
        ibis::query aquery(ibis::util::userName(), &part);
        ierr = aquery.setWhereClause((where + std::to_string(r)).c_str());
        ierr = aquery.setSelectClause(select_clause.c_str());
        ierr = aquery.evaluate();
        gettimeofday(&after, NULL);
        times.push_back(time_diff(before , after));
        cout << "Q " << time_diff(before , after) << endl;
    }
}

int main(const int argc, const char *argv[]) {
    po::variables_map options = get_options(argc, argv);
    init(options);

    buildIndex();
    ibis::part part(DATA_PATH.c_str());
    part.loadIndexes();
    evaluate(part);

    return 0;
}
