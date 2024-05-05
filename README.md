
Brief history of CUBIT
----------------------

The CUBIT project stemmed from an effort to parallelize UpBit [1], the state-of-the-art updatable bitmap index that runs queries and UDIs sequentially even on multicore architectures. We refer to this preliminary version as **CUBIT v1**, with the major contribution **being the first concurrent updatable bitmap index**. Based on CUBIT v1, we submitted a research paper to a top DB conference in 2023.

Unfortunately, our submission was rejected. Based on the constructive feedback from the reviewers, we pushed CUBIT forward and made heavy updates. The latest version is CUBIT v2, in which we develop a more lightweight snapshotting mechanism for (wait-free) queries and a consolidation-aware helping mechanism for (latch-free) UDIs. CUBIT v2 is more effective and can meet the time constraints for indexing in DBMSs. More importantly, we adapt it to various use cases with OLAP, HTAP, and OLTP workloads. Our thorough evaluation shows that **CUBIT v2 is a promising indexing candidate for selective queries on any workload with updates**.

[1] Manos Athanassoulis, Zheng Yan, and Stratos Idreos. UpBit: Scalable In-Memory Updatable Bitmap Indexing. In SIGMOD'16.

How is this project organized?
-------------------------------------------

The core code resides in the src/ folder, which contains the parallelized updatable bitmap indexes, including In-place+ (src/naive), UCB+ (src/ucb), UpBit+ (src/ub), and our algorithm CUBIT v2 (src/nbub). They are parallelized for modern multicore architectures by using different strategies including fine-grained read-write latching and snapshotting.

The files src/nbub/table_lk.cpp and src/nbub/table_lf.cpp contain the code of our algorithms CUBIT-lk and CUBIT-lf, respectively. Both files inherit from src/nbub/table.cpp, which contains several important common functions like Evaluate/Query.


How to run CUBIT and replicate key experiments?
--------------------------------

CUBIT v2 relies on c++17, python3, liburcu(-dev), and the Boost library. 

First, use the following command to compile the entire project. 

```
./build.sh 
```

Then, reproduce the key results (Figures 2, 8, 9, and 10) by using the following command. 

```
python3 run.py 
```

The above command writes the experimental results to a group of folders named graphs_x, where x is the ID of each trial. We can analyze the results in graphs_x and draw figures by using the following command:

```
./gen_graphs_normal.sh graphs_x
```

We can replicate other experiments by using the corresponding scripts (with different suffixes) in the directory figures-scripts/ . For example, the evaluation with 1B entries can be performed by using the two scripts run_sz_1B.sh and gen_graphs_1B.sh.

Note that it may take dozens of hours to run the whole experimentation for the first time because it involves generating datasets, which is time-consuming. Therefore, it is better to execute only the experiments that are of most interest each time.


How to replicate the DBx1000 and DuckDB experiments?
----------------------------------------------

Our changes to DBx1000 and DuckDB are maintained as submodules of this repository. One can download the contents of both CUBIT-powered DBx1000 and DuckDB explicitly by using the command:

```
git submodule update --init --recursive
```

Then, please follow the instructions in the subdirectories to compile the code and run the experiments.

