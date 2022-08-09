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
    
    double cost_min;


	double g;      // 
	double f;      // f = h + g;
	double h;      // 
	int color;  // marking the point to see whether it was visited or not

    int interval_index;
    double interval_t_min;
    double interval_t_max;

    int index;
    Node *parent;

    int parent_index_1;
    int parent_index_2;
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
    std::list<TimeInterval> getSafeIntervals(std::list<TimeInterval> rt);

    Successors get_successors( 
        std::vector<Node> p[],
        Node s,
        int trajectory_size,
        double v_min, 
        double v_max, 
        double length,
        const ReservationTable& rt,
        int first_conflict_point_counter);

private:
    Instance& instance;
    vector<Agent> agents = instance.agents;
    float turn_radius_right = 10*0.3048;
    float turn_radius_left = 40*0.3048;
    float w = 7;


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