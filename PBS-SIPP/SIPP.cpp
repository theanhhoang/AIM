#include "SIPP.h"
#include <iostream>
#include <fstream>
#include <string>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <cstdio>
#include <math.h>



SIPP::SIPP(Instance& instance): instance(instance)
{
    instance.loadSearchGraph(searchGraph, vNameToID, vNameToV, vNameToDirection, eNameToE, fileName, pairDistances, pairDistancesMap, pDFileName);
}

Path SIPP::run(int agentID, const ReservationTable& rt)
{
    // from agent's ID -> get agent's start_location, goal location, earliest_start_time, maximum velocity
    
    // g_s_start = 0; empty OPEN
    std::vector<double> cost_g;
    cost_g.push_back(0);

    std::vector<int> open_list;

    //agent details
    Agent agent = agents[agentID];
    int id = agent.id;
    int start_location = agent.start_location;
    int goal_location = agent.goal_location;
    std::vector<int> trajectory = agent.trajectory;
    double earliest_start_time = agent.earliest_start_time;
    double v_min = agent.v_min;
    double v_max = agent.v_max;
    double length = agent.length; // length of the vehicle

    Successors result_intervals;
    PossibleSuccessor first_point;
    first_point.current_point = trajectory[0];
    first_point.next_point = trajectory[1];
    first_point.arrival_time_min = earliest_start_time;
    first_point.arrival_time_max = earliest_start_time + 120;  //120 seconds



    Path result_path;
    PathEntry first_path_entry;
    first_path_entry.arrival_time = earliest_start_time;
    first_path_entry.conflict_point = trajectory[0];
    result_path.push_back(first_path_entry);


    // first_point.leaving_time_tail = earliest_start_time; 
    
    result_intervals.push_back(first_point);

    // insert s_start into OPEN with f(s_start)=h_start
    open_list.push_back(trajectory[0]);
    std::vector<double> cost_f;
    cost_f.push_back(estimate_cost(trajectory.front(), trajectory.back(), v_max));


    std::vector<double> speeds;
    // speeds.push_back(0);

    int i = 0;
    // while s is not s_goal:
    while (i != trajectory.size()){

        //    remove s with the smallest f-value from OPEN

        //    successors = get_successors(s)

        int current_point = trajectory[i];
        int next_point = trajectory[i+1];
        Successors successors = get_successors(current_point, next_point, v_min, v_max, length, result_intervals[i].arrival_time_min, result_intervals[i].arrival_time_max, rt);
        //    for each s_successor in successors:
            for (int j = 0; j < successors.size(); ++j){
                if (i > cost_g.size()){
                    cost_g.push_back(100000);
                    cost_f.push_back(100000);

                    PossibleSuccessor new_interval;
                    new_interval.current_point = trajectory[i];
                    new_interval.next_point = trajectory[i+1];

                    result_intervals.push_back(new_interval);
                    speeds.push_back(0);


                    PathEntry new_path_entry;
                    new_path_entry.conflict_point = trajectory[i+1];
                    result_path.push_back(new_path_entry);
                }

                //        if g(s_successor) > g(s) + c(s,s_successor):
                if (cost_g[i+1] > cost_g[i] + successors[j].cost_min){
                    //            g(s_successor) = g(s) + c(s,s_successor)
                    cost_g[i+1] = cost_g[i] + successors[j].cost_min;
                    result_intervals[i+1] = successors[j];
                    speeds[i] = successors[j].speed_for_arrival_time_min;

                    // TESTING ONLY
                    result_path[i+1].arrival_time = successors[j].arrival_time_min;
                    result_path[i+1].leaving_time_tail = successors[j].arrival_time_min;


                //            update_time(s_successor)

                //            f(s_successor) = g(s_successor) + h(s_successor)
                    cost_f[i+1] = cost_g[i+1] + estimate_cost(trajectory[i+1], trajectory.back(), v_max);
                //            insert s_sussessor into OPEN with f(s_successor)
                    open_list.push_back(trajectory[i+1]);

                }

            }
            i++;
    }
    // NOW WE HAVE A VECTOR OF POSSIBLE_SUCCESSOR (SUCCESSORS), WE NEED TO CONVERT IT INTO THE VECTOR OF PATH_ENTRY (PATH)  
    // double speed_min = *std::min_element(speeds.begin(), speeds.end());



    // update ReservationTable
    // return a Path, which is a vector of PathEntry. a PathEntry contains vertex (int), arrivalTime (double) and leavingTime (double)
    return result_path;
}

Successors SIPP::get_successors(
        int current_point, 
        int next_point, 
        double v_min, 
        double v_max, 
        double length, 
        double arrival_time_min, 
        double arrival_time_max, 
        const ReservationTable& rt
    ){
    // empty successors, type vector
    Successors successors;

    // cfg = configuration of m applied to s

    // m = next possible motion -> get from Instance

    // m_time = time to excecute m- using formula t=d/v
    double m_time_min = estimate_cost(current_point, next_point, v_max);
    double m_time_max = estimate_cost(current_point, next_point, v_min);

    // start_t = time(s) + m_time
    double start_t = arrival_time_min + m_time_min;
    // ent_t = endTime(interval(s)) + m_time + time for vehicle to pass m
    int direction = vNameToDirection[next_point]; 
    double end_time = arrival_time_max;
    double end_t = end_time + m_time_max + Li(direction, length)/w + Li(direction, length)/v_min;
    // for each safe interval in cfg:
    for (std::list<TimeInterval>::const_iterator it = rt[0].begin(); it!= rt[0].end(); ++it){
        PossibleSuccessor possible_successor;
        //    if (start_time(i) > end_t) or (end_time(i) < start_t):
        if ((it->t_min > end_t) || (it->t_max < start_t)){
        //        continue
            continue;
        }

        double t_min = std::min(it->t_min, start_t);
        double t_max = std::max(it->t_max, end_t) - Li(direction, length)/w - Li(direction, length)/v_min;
        //    t = earliest arrival time at cfg during interval i with no collisions

        //    if t does not exist:

        //        continue

        //    s_successor = start of configuration cfg with interval i and time t

        //    insert s_successor into successors

        possible_successor.current_point = current_point;
        possible_successor.next_point = next_point;
        possible_successor.arrival_time_min = t_min;
        possible_successor.arrival_time_max = t_max;

        possible_successor.speed_for_arrival_time_min = t_min/pairDistances[current_point][next_point].GetDouble();
        possible_successor.speed_for_arrival_time_max = t_max/pairDistances[current_point][next_point].GetDouble();

        // NEED TO MODIFY HERE!!!
        possible_successor.cost_min = t_min - arrival_time_min;
        possible_successor.cost_max = t_max - arrival_time_min;

        successors.push_back(possible_successor);
    }


    return successors;
}


float SIPP::Li(int direction, double agent_length){
    // straight 
    if (direction == 2){
        // std::cout << "direction: " << direction << "-" << vehicleLength << std::endl; 
        return agent_length;
    }
    //right, turnRadiusRight
    else if (direction == 1)
    {
        // std::cout << "direction: " << direction << "-" << 4*turnRadius*asin(vehicleLength/(2*turnRadius)) << std::endl; 
        return 4*turn_radius_right*asin(agent_length/(2*turn_radius_right));
    }
    //left, turnRadiusLeft
    else if (direction == 0)
    {
        // std::cout << "direction: " << direction << "-" << 4*turnRadius*asin(vehicleLength/(2*turnRadius)) << std::endl; 
        return 4*turn_radius_left*asin(agent_length/(2*turn_radius_left));
    }
}

double SIPP::estimate_cost(int start_point, int end_point, double speed){
    return (pairDistances[start_point][end_point].GetDouble())/speed;
}
