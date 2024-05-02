
**Brief history of CUBIT**

The project CUBIT stemmed from an effort to parallelize UpBit [1], the state-of-the-art updatable bitmap index that runs queries and UDIs sequentially, on multicore architectures. We refer to this version as CUBIT v1, with the major contribution being the first concurrent updatable bitmap index. Based on CUBIT v1, we submitted a research paper to a top DB conference.

Unfortunately, our submission was rejected. Based on the constructive feedback from the reviewers, we pushed CUBIT forward and made heavy updates. The latest version is CUBIT v2, in which we develop a more lightweight snapshotting mechanism for (wait-free) queries and a consolidation-aware helping mechanism for (latch-free) UDIs. CUBIT v2 is more effective and can meet the time constraints for indexing in DBMSs. More importantly, we adapt CUBIT v2 to various use cases with OLAP, HTAP, and OLTP workloads. Our thorough evaluation shows that it is a promising indexing candidate for selective queries for any workload with updates.

[1] Manos Athanassoulis, Zheng Yan, and Stratos Idreos. UpBit: Scalable In-Memory Updatable Bitmap Indexing. In SIGMOD'16.

**How is this project organized?**

The core code resides in the src/ folder, which contains several parallelized updatable bitmap indexes, including In-place (src/naive), UCB (src/ucb), UpBit (src/ub), and our algorithm CUBIT v2 (src/nbub). They are parallelized for modern multicore architectures by using different strategies like fine-grained read-write latching or MVCC (Please find the details in our paper).

The files src/nbub/table_lk.cpp and src/nbub/table_lf.cpp contain the code of our algorithms CUBIT-lk and CUBIT-lf, respectively. Both files inherit from src/nbub/table.cpp, which contains several important common functions like Evaluate/Query.

**How to compile and run the code?**

CUBIT relies on c++17, python3, liburcu(-dev), and the Boost library. 

We use the following command to compile the entire project. 

```
./build.sh 
```

Now, we can reproduce the main experiments (Figures 2, 8, 9, and 10) of the paper by using the following command, which performs the experiments and writes the results to a group of folders named graphs_x, where x is the ID of each trial.

```
python3 run.py 
```

We can analyze the results in graphs_x and automatically draw the figures by using the following command:

```
./gen_graphs_normal.sh graphs_x
```


We can replicate other experiments by using the corresponding scripts (with different suffixes) in the directory figures-scripts/ . For example, the evaluation with 1B entries can be performed by using the two scripts run_sz_1B.sh and gen_graphs_1B.sh.

Note that it may take dozens of hours to run the whole experimentation for the first time because it involves generating datasets, which is time-consuming. Therefore, it is better to execute only the experiments that are of most interest each time.

**How to run the DBx1000 and DuckDB experiments?**

Our changes to DBx1000 and DuckDB are maintained as submodules of this repository. One can download the contents of both CUBIT-powered DBx1000 and DuckDB explicitly by using the command:

```
git submodule update --init --recursive
```

and then follow the instructions in the subdirectories to compile the code and replicate the experiments.

