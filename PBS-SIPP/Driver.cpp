#include "PBS.h"
#include "Instance.h"
#include <boost/program_options.hpp>
#include<boost/tokenizer.hpp>


int main(int argc, char** argv)
{



    /////////////////////TESTING SPACE//////////////////
    // Instance instance("intro_graph.json");
    // SIPP sipp(instance);
    // ReservationTable rt(instance.getNumOfVertices());

    // // ReservationTable
    // TimeInterval ti2;
    // ti2.t_min = 1;
    // ti2.t_max = 1000;
    // ti2.agent_id = 10;

    // TimeInterval ti1;
    // ti1.t_min = 2;
    // ti1.t_max = 1000;
    // ti1.agent_id = 10;

    // rt[2].push_back(ti2);
    // rt[3].push_back(ti2);
    // rt[4].push_back(ti1);
    // rt[5].push_back(ti2);
    // rt[6].push_back(ti2);
    // rt[7].push_back(ti2);
    // rt[8].push_back(ti2);
    // rt[9].push_back(ti2);
    // rt[10].push_back(ti2);

    // Agent agent;

    // Path test= sipp.run(0, rt);
    /////////////////////////////////////////////////



    namespace po = boost::program_options;
    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("map,m", po::value<std::string>()->required(), "input file for map")
            ("output,o", po::value<std::string>(), "output file for schedule")
            ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }

    po::notify(vm);
    srand((int)time(0));

    // read the map file
    Instance instance(vm["map"].as<string>());

    PBS pbs(instance);
    pbs.run();
    pbs.writeToFile(vm["output"].as<string>());
    return 0;
}