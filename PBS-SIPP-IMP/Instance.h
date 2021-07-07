#pragma once
#include <set>
#include <unordered_map>
#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include "Common.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::undirectedS > searchGraphTraits_t;
typedef searchGraphTraits_t::vertex_descriptor vertex_t;
typedef searchGraphTraits_t::edge_descriptor edge_t;

struct position_t
{
    float x;
    float y;
    position_t(){};
    position_t(float a, float b) {
        x = a;
        y = b;
    };
};

struct Vertex
{
    std::string name;
    position_t pos;
    std::set<int> generalizedVertexConflicts;
    int id;
};

struct Edge
{
    std::string name;
    float length;
};

typedef boost::adjacency_list<
    boost::vecS, boost::vecS, boost::undirectedS,
    Vertex, Edge>
    searchGraph_t;


struct Agent
{
    int id;
    string name;
    int start_location;
    int goal_location;
    int importance;
    std::vector<int> trajectory;

    // pairDistances[point1][point2]

    double earliest_start_time;

    double v_min;
    double v_max;
    double length; // length of the vehicle

    double velocity;
};


class Instance
{
public:
    vector<Agent> agents;
    int getNumOfVertices() const;
    void loadVehicles(const std::string& arrivalFile, int step);
    //void loadVehicles(const std::string& arrivalFile, int step, vector<Agent> &agents, std::unordered_map<std::string, int>& vNameToID);
    Instance(const string& map_name);
    void loadSearchGraph(
        searchGraph_t& searchGraph,
        std::unordered_map<std::string, int>& vNameToID,
        std::unordered_map<int, std::string>& vIDToName,
        std::vector<vertex_t>& vNameToV,
        std::vector<int>& vNameToDirection,
        std::unordered_map<int, std::vector<int> >& vIDToConflictPoints,
        const std::string& fileName, 
        rapidjson::Document& pairDistances,
        std::map<int, std::map<int, double> >& pairDistancesMap,
        const std::string& pDFileName
        );
    position_t nodeAsPos(const float x, const float y);

    std::map<int, std::map<int, double> > getPairDistancesMap();
    std::vector<int> getVNameToDirection();
    std::unordered_map<std::string, int> getVNameToID();
    std::unordered_map<int, std::string> getVIDToName();
    std::unordered_map<int, std::vector<int> > getVIDToConflictPoints();
    std::vector<Agent> getAgents();
    std::vector<int> getConflictPoints(int);
    double getEarliestStartTime(int);
    int getStartCP(int index);
    void loadSamePoint();
    bool isSamePoint(int, int);

private:
    std::string fileName;
    std::string pDFileName = "pairDistance2.json";
    searchGraph_t searchGraph;

    std::unordered_map<std::string, int> vNameToID;
    std::unordered_map<int, std::string> vIDToName;
    std::vector<vertex_t> vNameToV;

    std::vector<int> vNameToDirection;

    rapidjson::Document pairDistances;
    std::map<int, std::map<int, double> > pairDistancesMap;
    std::unordered_map<int, std::vector<int> > vIDToConflictPoints;
    bool samePoint[104][104] = {{false}};
};