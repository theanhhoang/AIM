#pragma once
#include "SIPP.h"
#include "PTNode.h"

extern const double EPSILON;

class PBS
{
public:
    PBS(Instance& instance);
    void run(const string&);
    //void writeToFile(const string& file_name);
    //ReservationTable getReservationTable(PTNode, std::list<int>);
    bool hasCollision(PathEntry, PathEntry);
    bool hasCollision(PathEntry, TimeInterval);
    bool replanVehicle(PTNode&, int);
    bool UpdatePlan(PTNode&, int);
    SIPP sipp;
    Instance& instance;
    std::vector<Agent> arrivingVehicles;
    std::map<int, std::map<int,std::vector<int> > > trajectoryToAgent;
    void initializeAgents(std::map<int, std::map<int, std::vector<int> > >&);
    void initializePriority(std::map<int, std::set<int> >&, std::map<int, std::map<int, std::vector<int> > >&);
    void printPath(Path);
    void printRT(ReservationTable);
    void printPriority(std::map<int, std::set<int> >);
    bool checkValid(ReservationTable& rt, Path& path, int agent);
};