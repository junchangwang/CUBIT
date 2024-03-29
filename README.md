
**What is CUBIT?**

CUBIT is a scalable bitmap index that provides efficient reads and real-time updates, such that it has the potential to serve as a general secondary index for both OLTP and OLAP DBMSs.

**How is this project organized?**

The code artifact of our paper is in the src/ folder, which contains several parallelized updatable bitmap indexes including In-place (src/naive), UCB (src/ucb), UpBit (src/ub), and our algorithm CUBIT (src/nbub). They are parallelized for modern multicore architectures by using different strategies (e.g., fine-grained reader-writer locks and MVCC). Please check the paper for more details.

The files src/nbub/table_lk.cpp and src/nbub/table_lf.cpp correspond to the algorithms CUBIT-lk and CUBIT-lf, respectively. Both files inherit src/nbub/table.cpp that contains several important common functions like Evaluate/Query.

**How to compile and run the code?**

CUBIT relies on c++17, python3, the package liburcu(-dev), and Boost libraries. 

We use the following command to compile the entire project. 

```
./build.sh 
```

Now, we can reproduce the main experiments (Figures 2, 8, 9, and 10) of the paper by using the following command, which performs the experiments and writes the results in a group of folders named graphs_x, where x is the ID of each trial.

```
python3 run.py 
```

We can analyze the results in graphs_x and automatically draw all of the figures by using the following command:

```
./gen_graphs_normal.sh graphs_x
```


You can reproduce other experiments of the paper by using the corresponding scripts (with different suffixes) in the directory figures-scripts/ . For example, the evaluation of 1B entries can be performed by using the two scripts run_sz_1B.sh and gen_graphs_1B.sh.

Note that it may take dozens of hours to run the whole experimentation for the first time because it involves generating datasets, which is time-consuming. Therefore, it is better to execute only the experiments that are of most interest each time.
