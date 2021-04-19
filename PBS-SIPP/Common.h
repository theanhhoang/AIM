#pragma once
#include <vector>
#include <list>
#include <iostream>
#include <set>
#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>
#include <boost/graph/adjacency_list.hpp>

using std::vector;
using std::string;
using std::list;

using boost::unordered_map;
using boost::unordered_set;

struct PathEntry
{
    int vertex;
    int conflict_point;
    double arrival_time; // arrival time of the head of the vehicle

    double leaving_time_head;
    double leaving_time_tail; // leaving time of the tail of the vehicle;

    double cost;
};
typedef vector<PathEntry> Path;

struct TimeInterval
{
    double t_min;
    double t_max;
    int agent_id;
};
typedef vector<list<TimeInterval>> ReservationTable; // vector: vertex -> ordered list of occupied time intervals