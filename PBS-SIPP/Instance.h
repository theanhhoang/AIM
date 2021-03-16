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
    int start_location;
    int goal_location;
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
    Instance(const string& map_name);
    void loadSearchGraph(
        searchGraph_t& searchGraph,
        std::unordered_map<std::string, int>& vNameToID,
        std::vector<vertex_t>& vNameToV,
        std::vector<int>& vNameToDirection,
        std::unordered_map<std::string, edge_t>& eNameToE,
        const std::string& fileName, 
        rapidjson::Document& pairDistances,
        std::map<int, std::map<int, double> >& pairDistancesMap,
        const std::string& pDFileName);
    position_t nodeAsPos(const float x, const float y);
// private:
//     std::string fileName = "intro_graph.json";
//     std::string pDFileName = "pairDistance2.json";
//     searchGraph_t searchGraph;

//     std::unordered_map<std::string, int> vNameToID;

//     std::vector<vertex_t> vNameToV;

//     std::vector<int> vNameToDirection;

//     std::unordered_map<std::string, edge_t> eNameToE;
//     // std::vector<std::vector<vertex_t>> vehicles;
//     rapidjson::Document pairDistances;
//     std::map<int, std::map<int, double> > pairDistancesMap;
};