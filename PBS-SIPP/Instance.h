#pragma once
#include "Common.h"

struct Agent
{
    int id;
    int start_location;
    int goal_location;
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
};

