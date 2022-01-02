#include "PBS.h"
#include "Instance.h"
#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>
#include <boost/filesystem.hpp>
#include <cstdlib>

#include <chrono>
using namespace std::chrono;


int main(int argc, char** argv)
{  
   string sourcePath = boost::filesystem::current_path().parent_path().string();
   string arrFile = sourcePath + "/vehicleArrival.json";
   string introGraphFile = sourcePath + "/intro_graph_3.json";
   string pairDistanceFile = sourcePath + "/pairDistance_3.json";

   // default to using step 35
   int step = 35;
   if(argc > 2){
    step = atoi(argv[1]);
   }

   Instance instance(introGraphFile, pairDistanceFile);
   instance.loadVehicles(arrFile, step);
   auto startTime = high_resolution_clock::now();


   SIPP sipp(instance);
   PBS pbs(instance);
   pbs.run("output.txt");

   auto stopTime = high_resolution_clock::now();
   auto duration = duration_cast<microseconds>(stopTime - startTime); 
   std::cout<<"finished planning in:  "<< duration.count()/1000000.0 <<" seconds\nresult saved to output.txt\n";

   return 0;
}