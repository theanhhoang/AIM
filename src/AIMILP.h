#include <set>
#include <unordered_map>
#include <iostream>
#include <set>
#include <boost/graph/adjacency_list.hpp>

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
	std::set<vertex_t> generalizedVertexConflicts;
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

void loadSearchGraph(
    searchGraph_t& searchGraph,
    std::unordered_map<std::string, vertex_t>& vNameToV,
    std::unordered_map<std::string, vertex_t>& vNameToDirection,
    std::unordered_map<std::string, edge_t>& eNameToE,
    std::vector<vertex_t>& starts, std::vector<vertex_t>& goals,
    const std::string& fileName);










