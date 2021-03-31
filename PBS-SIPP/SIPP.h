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




struct Node
{
    int current_point;
    int next_point;
    int previous_point;
    
    double arrival_time_min; // arrival time of the head of the vehicle
    double arrival_time_max; // arrival time of the head of the vehicle

    double speed_for_arrival_time_min;
    double speed_for_arrival_time_max;
    
    double cost_min;
    double cost_max;


	int g;      // khoang cach tu dinh ban dau den dinh hien ta
	int f;      // f = h + g;
	int h;      // duong di ngan nhat
	int color;  // danh dau dinh di qua
};




typedef vector<Node> Successors;


class SIPP {

public:
    SIPP(Instance& instance);
    Path run(int agent, const ReservationTable& rt);
    float Li(int direction, double agent_length);
    double estimate_cost(int start_point, int end_point, double speed);

    int find_min(std::vector<Node>& open);
    int find(std::vector<Node>& open);
    int find_point(int n ,Node* p, int current_point);

    // Successors get_successors( 
    //     int current_point,
    //     int next_point, 
    //     double v_min, 
    //     double v_max, 
    //     double length,
    //     double arrival_time_min, 
    //     double arrival_time_max, 
    //     const ReservationTable& rt);

    Successors get_successors( 
        Node p[],
        int current_position,
        int trajectory_size,
        double v_min, 
        double v_max, 
        double length,
        const ReservationTable& rt);


    void loadSearchGraph(
        searchGraph_t& searchGraph,
        std::unordered_map<std::string, int>& vNameToID,
        std::vector<vertex_t>& vNameToV,
        std::vector<int>& vNameToDirection,
        const std::string& fileName, 
        rapidjson::Document& pairDistances,
        std::map<int, std::map<int, double> >& pairDistancesMap,
        const std::string& pDFileName);
private:
    Instance& instance;
    vector<Agent> agents = instance.agents;
    float turn_radius_right = 16*0.6;
    float turn_radius_left = 32*0.6;
    float w = 5;


    // std::string lanes[8] = {"WR", "WL", "ER", "EL", "NR", "NL", "SR", "SL"};
    // std::string fileName = "intro_graph.json";
    // std::string pDFileName = "pairDistance2.json";
    // searchGraph_t searchGraph;

    std::unordered_map<std::string, int> vNameToID;

    // std::vector<vertex_t> vNameToV;

    std::vector<int> vNameToDirection;

    // std::unordered_map<std::string, edge_t> eNameToE;
    // rapidjson::Document pairDistances;
    std::map<int, std::map<int, double> > pairDistancesMap;
    //e = time i reach the intersection





};