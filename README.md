## Overall Usage

The Controller folder contains the file ```controller.py```. Call the the script: 
```shell script 
python controller.py
```

This script will run 100 simulations in total (but you can change the number of simulations in line 227). For each simulation, the script will generate a new vehicle arrival profile (```python generate_arrivals_new.py```). With that new vehicle arrival profile, we will run the PSL algorithm, MILP algorithm, and First Come First Serve algorithm, then records all the performance indexes (runtime, average vehicle delay and throughput).

The current code is set for the demand of 800vphpl; then, all results will be stored in ```Controller/result/800vph``` folder. If you wish to change the demand to a different value, please update accordingly to in the ```controller.py``` and ```generate_arrivals_new.py``` files.

The  ```Controller/result``` is currently storing our results of our simulations in the cases of 500vphpl and 800vphpl.

We include an Excel file, which is used to analyse our results. The file is located in ```Controller/result``` folder.

## PSL Algorithm
Our algorithm is already compiled in ```PBS-SIPP-ver2/AIMPBS```. However, if you want to re-complile the source code, you can follow the below information.

The code requires the external library [boost](https://www.boost.org/).
If you are using Ubantu, you can install it simply by
```shell script
sudo apt install libboost-all-dev
``` 

If the above method does not work, you can also follow the instructions
on the [boost](https://www.boost.org/) website and install it manually.

The code also requires the external library Cplex from IBM. You can follow this Youtube [link](https://www.youtube.com/watch?v=c9uVSaQkd8o) to install the package.


After you installed boost and cplex and downloaded the source code, go into the directory of the source code (```PBS-SIPP-ver2/```) and compile it with CMake:
```shell script
cmake .
make
```

## Other baseline Algorithms
If you wish to modify or recompile our baseline Algorithms (MILP in ```src/``` and MILP-FCFS in ```MILP-FCFS/```), please follow the same instructions in section PSL Algorithm above. Again you Boost and Cplex are required library.
