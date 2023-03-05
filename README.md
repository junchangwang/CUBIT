
**What is CUBIT?**

CUBIT is a scalable bitmap index that provides efficient reads and real-time updates.

**How this project is organized?**

The code artifact of our paper is in the src/ folder, which contains several parallelized updatable bitmap indexes including In-place (src/naive), UCB (src/ucb), UpBit (src/ub), and our algorithm CUBIT (src/nbub). They are parallelized for modern multicore architectures by using different strategies (e.g., fine-grained reader-writer locks and MVCC). Please check the paper.

The files src/nbub/table_lk.cpp and src/nbub/table_lf.cpp correspond to the algorithms CUBIT-lk and CUBIT-lf, respectively.


**How to compile and run the code?**

First, please make sure that your environment supports c++17. Second, python3, the package liburcu(-dev), and Boost libraries are required.

Then, You can use 
```
./build.sh 
```
compile entire project. 

You can reproduce the main experiments (Figures 1, 6, 7, and 8) of the paper as follows:
* run run_sz.py which performs the experiments and writes the results in a group of folders named graphs_x, where x is the ID of each trial.
* use gen_graphs_normal.sh to analyze the results in graphs_x and automatically draw all of the figures.

You can reproduce other experiments of the paper by using the corresponding scripts (with different suffixes) in the directory figures-scripts/ . For example, the evaluation on 1B entries can be performed by using the two scripts run_sz_1B.sh and gen_graphs_1B.sh.

Note that it may take a dozen of hours to a few days to run the code for the first time because it involves generating datasets, which is time-consuming. Therefore, it is better to execute only the experiments that are of most interest each time.

