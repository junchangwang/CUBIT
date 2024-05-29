
# Brief history of CUBIT

The CUBIT project stemmed from an effort to parallelize UpBit [1], the state-of-the-art updatable bitmap index that runs queries and UDIs sequentially on multicore architectures. We refer to this preliminary version as **CUBIT v1**, with the major contribution **being the first concurrent updatable bitmap index**. Based on CUBIT v1, we submitted a research paper to a top DB conference in 2023.

While the paper was not accepted, we used the constructive feedback we got from the reviewers and additional observations we made to **substantially** broaden the contributions of this project. More specifically, in **CUBIT v2**, we develop a very lightweight snapshotting mechanism for (wait-free) queries and a consolidation-aware helping mechanism for (latch-free) UDIs. The end result is that CUBIT v2 scales much better than any prior updatable bitmap index and can meet time requirements for indexing in practical DBMSs. More importantly, we adapt CUBIT v2 to various use cases with OLAP, HTAP, and OLTP workloads. Our thorough evaluation shows that **CUBIT v2 is a promising indexing candidate for selective queries on any workload with updates**.

[1] Manos Athanassoulis, Zheng Yan, and Stratos Idreos. UpBit: Scalable In-Memory Updatable Bitmap Indexing. In SIGMOD'16.

# Readme

How is this project organized?
-------------------------------------------

The core code resides in the src/ folder, which contains the parallelized updatable bitmap indexes, including In-place+ (src/naive), UCB+ (src/ucb), UpBit+ (src/ub), and our algorithm CUBIT v2 (src/cubit). They are parallelized for modern multicore architectures by using different strategies including fine-grained read-write latching and snapshotting.

The files src/cubit/table_lk.cpp and src/cubit/table_lf.cpp contain the code of our algorithms CUBIT-lk and CUBIT-lf, respectively. Both files inherit from src/cubit/table.cpp, which contains several important common functions like Evaluate/Query.


How to run CUBIT and replicate key experiments?
--------------------------------

CUBIT v2 relies on c++17, python3, liburcu(-dev), and the Boost library. 

First, use the following command to compile the entire project. 

```
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
./run.sh ALL
./plot.sh ALL
```

Note that it may take dozens of hours to run the whole experimentation for the first time because it involves generating datasets, which is time-consuming. Therefore, it is better to execute only the experiments that are of most interest each time.


How to replicate the DBx1000 and DuckDB experiments?
----------------------------------------------

Our changes to DBx1000 and DuckDB are maintained as submodules of this repository. One can download the contents of both CUBIT-powered DBx1000 and DuckDB explicitly by using the command:

```
git submodule update --init --recursive
```

Then, please follow the instructions in the subdirectories to compile the code and run the experiments.
