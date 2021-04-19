#include "PBS.h"
#include "Instance.h"
#include <boost/program_options.hpp>
#include<boost/tokenizer.hpp>


int main(int argc, char** argv)
{
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
    std::string arrivalFile = "vehicleArrival.json";
    // read the map file
    Instance instance(vm["map"].as<string>());
    instance.loadVehicles(arrivalFile);
    PBS pbs(instance);
    pbs.run(vm["output"].as<string>());
    return 0;
}