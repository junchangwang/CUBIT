
# Brief history of CUBIT

The CUBIT project originated from an effort to parallelize UpBit [1], the state-of-the-art updatable bitmap index that runs queries and UDIs sequentially on multicore architectures. We refer to this preliminary version as **CUBIT v1**, with the major contribution **being the first concurrent updatable bitmap index**. Based on CUBIT v1, we submitted a research paper to a top database conference in 2023.

Although the paper was not accepted, we used the constructive feedback from the reviewers, along with additional observations, to **substantially** enhance the algorithm from various perspectives. The resulting algorithm, **CUBIT v2**, is an efficient, scalable bitmap index that supports real-time updates, significantly broadening the applicability of bitmap indexing by, for the first time, enabling DBMSs to maintain bitmap indexes on frequently updated, large-cardinality attributes. We experimented CUBIT v2 with both the OLAP and HTAP workloads. Our thorough evaluation shows that **CUBIT v2 is a promising indexing candidate for analytical queries**. For example, experiments on 12 out of 22 TPC-H queries show that our CUBIT-powered query engine is 1.2x - 2.7x faster than DuckDB's native approaches (please refer to the DuckDB submodule).

[1] Manos Athanassoulis, Zheng Yan, and Stratos Idreos. UpBit: Scalable In-Memory Updatable Bitmap Indexing. In SIGMOD'16.

# Readme

How is this project organized?
-------------------------------------------

The core code resides in the src/ folder, which contains the parallelized updatable bitmap indexes, including In-place+ (src/naive), UCB+ (src/ucb), UpBit+ (src/ub), and our algorithm CUBIT v2 (src/cubit). These indexes are parallelized for modern multicore architectures by using different strategies including fine-grained read-write latching and snapshotting.

The files src/cubit/table_lk.cpp and src/cubit/table_lf.cpp contain the code for our algorithms CUBIT-lk and CUBIT-lf, respectively. Both files inherit from src/cubit/table.cpp, which includes several important common functions such as Evaluate and Query.


How to run CUBIT and replicate key experiments?
--------------------------------

CUBIT v2 relies on c++17, python3, liburcu(-dev), and the Boost library. 

First, use the following command to compile the entire project. 

```sh
./build.sh 
```

Then, reproduce the key results (Figures 2, 8, 9, and 10) by using the following command. 

```sh
./run.sh core
./plot.sh core
```

The above command writes the experimental results to a group of folders named graphs_x, where x is the ID of each trial. It then analyzes the results in graphs_x and draws figures.

We can replicate other experiments by specifying different experiment types in {core, 1B, earth, MT, Para, Seg, WPMT, zipf}. The following command performs all experiments.

```sh
./run.sh all
./plot.sh all
```

Note that it may take dozens of hours to run the whole experimentation for the first time because it involves generating datasets, which is time-consuming. Therefore, it is better to execute only the experiments that are of most interest each time.


How to replicate the DBx1000 and DuckDB experiments?
----------------------------------------------

Our changes to DBx1000 and DuckDB are maintained as submodules of this repository (The DuckDB submodule is currently under code reorganization and will be publicly available soon.) You can download the contents of CUBIT-powered DBx1000 explicitly by using the command:

```
git submodule update --init --recursive
```

Then, please follow the instructions in the subdirectories to compile the code and run the experiments.
