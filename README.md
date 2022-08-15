# AIM

The Controller folder contains the file controller.py.
Simply call the the script: python controller.py.

This script will run 100 simulations in total (but you can change the number of simulation in line 227)
For each simulation, the script will generate a new vehicle arrival profile (python generate_arrivals_new.py). With that new vehilcle arrival profile, we will run the PSL algorithm, MILP algorithm and First Come First Serve algorithm, then records all the performance indexes (runtime, average vehicle delay and throughput).

The current code is set for the demand of 800vphpl, then all results will be stored in Controller/result/800vph folder. If you wish to change the demand to different value, please update accoringly to the in the controller.py and generate_arrivals_new.py files.

We include an Excel file, which is used to analyse our results. The file is located in Controller/result folder