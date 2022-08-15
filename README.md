The Controller folder contains the file ```controller.py```. Call the the script: ```python controller.py```.

This script will run 100 simulations in total (but you can change the number of simulations in line 227). For each simulation, the script will generate a new vehicle arrival profile (```python generate_arrivals_new.py```). With that new vehicle arrival profile, we will run the PSL algorithm, MILP algorithm, and First Come First Serve algorithm, then records all the performance indexes (runtime, average vehicle delay and throughput).

The current code is set for the demand of 800vphpl; then, all results will be stored in ```Controller/result/800vph``` folder. If you wish to change the demand to a different value, please update accordingly to in the ```controller.py``` and ```generate_arrivals_new.py``` files.

The  ```Controller/result``` is currently storing our results of our simulations in the cases of 500vphpl and 800vphpl.

We include an Excel file, which is used to analyse our results. The file is located in ```Controller/result``` folder.
