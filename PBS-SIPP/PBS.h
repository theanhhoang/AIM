#pragma once
#include "SIPP.h"
#include "PTNode.h"
class PBS
{
public:
    PBS(Instance& instance);
    void run(const string&);
    //void writeToFile(const string& file_name);
    //ReservationTable getReservationTable(PTNode, std::list<int>);
    bool UpdatePlan(PTNode&, int);
    SIPP sipp;
    Instance& instance;
    std::vector<Agent> arrivingVehicles;
};