Effects of Varying Job Distribution Rate and Worker Node Selection in Distributed Cluster
=====================================================================
Distribute clusters are at times used to process a series of tasks or jobs. This sort of job processing can be structured by having one node as the master (or producer), while the remaining nodes serve as workers that process jobs given to them. This report studies the effects node selection method and job distribution rate have in this environment. These features are examined by creating a simulated environment and putting it through a series of test that exhibit varying configurations. The obtained results were used to provide a generalized argument as to which configurations should be utilized during certain workload scenarios.

Repository Structure
=====================
- simulation_code: contains all the source code necessary to run a single simulation
- plot_generation_code_and_data: contains data obtained on Rutgers Elf cluster and plots generated from that data

Tests Ran
===========
All of the 144 tests ran are listed in the PDF files located inside the 'plot_generation_code_and_data'

In order to rerun all of the 144 tests navigate to 'simulation_code/tests' directory and execute all of the bash scripts, they will generate all of the necessary configuration files. Then one by one run the bash scripts 'run_*_trial.sh' located in 'simulation_code' directory. These bash scripts will execute all tests for a particular job size and generate the necessary data.

Looking Through Data and Graphs
=================================
All of the original data obtained from Elf cluster can be located in 'plot\_generation\_code\_and\_data/results\_\*\_1000'. Inside those directories you will see directories representing each test case, in those directories you will find raw data outputted by the simulation code as well as the graphs generated from that data. In order to regenerate the graphs in a directory simply run the 'plot.py' file with the necessary arguments. The corresponding 'gen\_\*' bash scripts were used to create graphs found in 'combined\_\*' directories by taking a few graphs from 'results\_\*\_1000' and merging them into one png file in order to be used in the project report/writeup.