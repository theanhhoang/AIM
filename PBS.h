#pragma once
#include "SIPP.h"
#include "PTNode.h"
class PBS
{
public:
    PBS(Instance& instance);
    void run();
    void writeToFile(const string& file_name);
    //ReservationTable getReservationTable(PTNode, std::list<int>);
    bool UpdatePlan(PTNode, int);
    int* topologicalSort();
    SIPP sipp;
    Instance& instance;
};