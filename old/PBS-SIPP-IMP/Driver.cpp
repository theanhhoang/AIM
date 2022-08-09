#include "PBS.h"
#include "Instance.h"
#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>
#include <ctime>

#include <chrono>
using namespace std::chrono;

int main(int argc, char** argv)
{
       /////////////////////TESTING SPACE//////////////////

    string arrFile = "vehicleArrival.json";
    int step = atoi(argv[1]);

    Instance instance("intro_graph_3.json");
    instance.loadVehicles(arrFile, step);

    auto startTime = high_resolution_clock::now();


    SIPP sipp(instance);
    PBS pbs(instance, false);
    pbs.run("output.txt");

    auto stopTime = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stopTime - startTime); 
    std::cout << duration.count()/1000000.0 << std::endl;

    /*
    // ReservationTable
    ReservationTable rt(instance.getNumOfVertices());
    TimeInterval ti2;
    ti2.t_min = 6;
    ti2.t_max = 10;
    ti2.agent_id = 10;
    rt[2].push_back(ti2);


    TimeInterval ti3_2;
    ti3_2.t_min = 0;
    ti3_2.t_max = 5;
    ti3_2.agent_id = 10;
    rt[3].push_back(ti3_2);


    ti3_2.t_min = 8;
    ti3_2.t_max = 10;
    ti3_2.agent_id = 10;
    rt[3].push_back(ti3_2);


    TimeInterval ti3;
    ti3.t_min = 15;
    ti3.t_max = 20;
    ti3.agent_id = 10;
    rt[3].push_back(ti3);



    TimeInterval ti4;
    ti4.t_min = 20;
    ti4.t_max = 50;
    ti4.agent_id = 10;
    rt[4].push_back(ti4);

    TimeInterval ti5;
    ti5.t_min = 20;
    ti5.t_max = 50;
    ti5.agent_id = 10;
    rt[5].push_back(ti5);

    TimeInterval ti6;
    ti6.t_min = 20;
    ti6.t_max = 50;
    ti6.agent_id = 10;
    rt[6].push_back(ti6);

    TimeInterval ti7;
    ti7.t_min = 20;
    ti7.t_max = 50;
    ti7.agent_id = 10;
    rt[7].push_back(ti7);

    TimeInterval ti8;
    ti8.t_min = 20;
    ti8.t_max = 50;
    ti8.agent_id = 10;
    rt[8].push_back(ti8);

    TimeInterval ti9;
    ti9.t_min = 20;
    ti9.t_max = 50;
    ti9.agent_id = 10;
    rt[9].push_back(ti9);
    // rt[10].push_back(ti2);

    Agent agent;

    Path test= sipp.run(0, rt);

*/


    // namespace po = boost::program_options;
    // // Declare the supported options.
    // po::options_description desc("Allowed options");
    // desc.add_options()
    //         ("help", "produce help message")
    //         ("map,m", po::value<std::string>()->required(), "input file for map")
    //         ("output,o", po::value<std::string>(), "output file for schedule")
    //         ;

    // po::variables_map vm;
    // po::store(po::parse_command_line(argc, argv, desc), vm);

    // if (vm.count("help")) {
    //     std::cout << desc << std::endl;
    //     return 1;
    // }

    // po::notify(vm);
    // srand((int)time(0));
    // std::string arrivalFile = "vehicleArrival.json";
    // // read the map file
    // Instance instance(vm["map"].as<string>());
    // instance.loadVehicles(arrivalFile);
    // PBS pbs(instance);
    // pbs.run(vm["output"].as<string>());
    return 0;
}