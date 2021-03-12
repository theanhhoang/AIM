#pragma once
#include "Instance.h"
#include <iostream>
#include <fstream>
#include <string>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <cstdio>
#include <math.h>


using std::vector;


class SIPP {
private:
    Instance& instance;
    vector<Agent> agents = instance.agents;
    float turn_radius_right = 16*0.6;
    float turn_radius_left = 32*0.6;
    float w = 5;


    std::string lanes[8] = {"WR", "WL", "ER", "EL", "NR", "NL", "SR", "SL"};
    std::string fileName = "intro_graph.json";
    std::string pDFileName = "pairDistance2.json";
    searchGraph_t searchGraph;
    std::unordered_map<std::string, vertex_t> vNameToV;

    std::unordered_map<std::string, vertex_t> vNameToDirection;

    std::unordered_map<std::string, edge_t> eNameToE;
    // std::vector<std::vector<vertex_t>> vehicles;
    rapidjson::Document pairDistances;
    //e = time i reach the intersection
    struct PossibleSuccessor
    {
        int vertex;
        int conflict_point;
        double arrival_time_min; // arrival time of the head of the vehicle
        double arrival_time_max; // arrival time of the head of the vehicle

        double cost_min;
        double cost_max;
    };
    typedef vector<PossibleSuccessor> Successors;


public:
    SIPP(Instance& instance);
    Path run(int agent, const ReservationTable& rt);
    float Li(int direction, double agent_length);
    double estimate_cost(int start_point, int end_point, double speed);




    Successors get_successors( 
        int current_point,
        int next_point, 
        double v_min, 
        double v_max, 
        double length,
        double arrival_time_min, 
        double arrival_time_max, 
        const ReservationTable& rt);


    void loadSearchGraph(
        searchGraph_t& searchGraph,
        std::unordered_map<std::string, vertex_t>& vNameToV,
        std::unordered_map<std::string, vertex_t>& vNameToDirection,
        std::unordered_map<std::string, edge_t>& eNameToE,
        const std::string& fileName, 
        rapidjson::Document& pairDistances,
        const std::string& pDFileName);


};