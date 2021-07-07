#pragma once
#include "SIPP.h"
#include "PTNode.h"
#include <queue>

extern const double EPSILON;

class PBS
{
public:
    PBS(Instance& instance, bool);
    void run(const string&);
    //void writeToFile(const string& file_name);
    //ReservationTable getReservationTable(PTNode, std::list<int>);
    bool UpdatePlan(PTNode&, int);

    SIPP sipp;
    Instance& instance;
    std::vector<Agent> arrivingVehicles;
    std::map<int, std::map<int,std::vector<int> > > trajectoryToAgent;
    std::ofstream ofile;
    bool log;
    void initializeAgents(std::map<int, std::map<int, std::vector<int> > >&);
    void initializePriority(std::map<int, std::set<int> >&, std::map<int, std::map<int, std::vector<int> > >&);
    void printPath(Path);
    void printRT(ReservationTable);
    void printPriority(std::map<int, std::set<int> >);
};