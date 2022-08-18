## Note
Our algorithm is developed in Ubuntu OS. However, it should work properly in other OS systems.

## Compiling PSL Algorithm
To complile the PSL Algorithm, please go to folder ```PBS-SIPP-ver2/```. You can then follow the below information.

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
