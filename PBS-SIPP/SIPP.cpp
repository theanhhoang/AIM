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
    // for (auto& it: vNameToID) {
    //     std::cout<< it.first << "-" << it.second <<std::endl;
    // }

    pairDistancesMap = instance.getPairDistancesMap();
    vNameToDirection = instance.getVNameToDirection();
    vNameToID = instance.getVNameToID();
}

Path SIPP::run(int agentID, const ReservationTable& rt)
{
    // from agent's ID -> get agent's start_location, goal location, earliest_start_time, maximum velocity
    
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


    // //TEST
    // int id = 1;
    // int start_location = vNameToID.find("WER_0")->second;
    // int goal_location = vNameToID.find("WER_7")->second;

    // std::vector<int> trajectory;
    // trajectory.push_back(vNameToID.find("WER_0")->second);
    // trajectory.push_back(vNameToID.find("WER_1")->second);
    // trajectory.push_back(vNameToID.find("WER_2")->second);
    // trajectory.push_back(vNameToID.find("WER_3")->second);
    // trajectory.push_back(vNameToID.find("WER_4")->second);
    // trajectory.push_back(vNameToID.find("WER_5")->second);
    // trajectory.push_back(vNameToID.find("WER_6")->second);
    // trajectory.push_back(vNameToID.find("WER_7")->second);


    // double earliest_start_time = 0.0;
    // double v_min = 3.0;
    // double v_max = 10.0;
    // double length = 5.0; // length of the vehicle



    std::vector<Node> open;
    Node p[trajectory.size()];

    for (int i = 0; i < trajectory.size(); i++){
        p[i].current_point = trajectory[i];

        p[i].arrival_time_min = 0;
        p[i].arrival_time_max = 0;
        p[i].speed_for_arrival_time_min = 0;
        p[i].speed_for_arrival_time_max = 0;
        p[i].cost_min = 0;
        p[i].cost_max = 0;

        p[i].g = 0;


        p[i].h = estimate_cost(trajectory[i], trajectory[trajectory.size() -1 ], v_min);


        p[i].f = 0;
        p[i].color = 0;

        if (i > 0){
            p[i].previous_point = trajectory[i-1];
        }else
        {
            p[i].previous_point = -1;
            p[i].arrival_time_min = earliest_start_time;
            p[i].arrival_time_max = earliest_start_time + 0; // 2 seconds
        }
        
        if (i < trajectory.size()-1){
            p[i].next_point = trajectory[i+1];
        }else
        {
            p[i].next_point = -1;
        }

    
    }

    open.push_back(p[0]);


    // int i = 0;
    // while s is not s_goal:
    while (open.size() != 0){
        //    remove s with the smallest f-value from OPEN
        int k = find_min(open);
        open.erase(open.begin() + k);
        

        if (find_point(trajectory.size(), p, open[k].current_point) == trajectory.size() -1 ){
            break;
        }
        else
        {

            int current_position = find_point(trajectory.size(), p, open[k].current_point);
            int next_position = current_position + 1;
            Successors successors = get_successors(p, current_position, trajectory.size(), v_min, v_max, length, rt);

            for (int j = 0; j < successors.size(); ++j){
                if (p[next_position].color == 0) // neu dinh chua co trong Open
                {
                    p[next_position].g = 100000;
                    p[next_position].f = 100000;
                    p[next_position].color = 1;

                }
                if (p[next_position].g > p[current_position].g + successors[j].cost_min) // neu dinh da co trong Open
                {
                    p[next_position] = successors[j];
                    p[next_position].g = p[current_position].g + successors[j].cost_min;

                //            update_time(s_successor)

                    p[next_position].f = p[next_position].g + p[next_position].h;
                    open.push_back(p[next_position]);
                }            
            }


        }
    }
    // NOW WE HAVE A VECTOR OF POSSIBLE_SUCCESSOR (SUCCESSORS), WE NEED TO CONVERT IT INTO THE VECTOR OF PATH_ENTRY (PATH)  
    double speed_min = v_min; 
    double speed_max = v_max;
    for (int i =1; i < trajectory.size(); i++){
        if (p[i].speed_for_arrival_time_min < speed_max){
            speed_max = p[i].speed_for_arrival_time_min;
        }

        if (p[i].speed_for_arrival_time_max > speed_min){
            speed_min = p[i].speed_for_arrival_time_max;
        }
    }


    Path result_path;

    if (speed_min > speed_max){
        std::cout << "Could not find the optimal plan" << std::endl;
    }else{

        for (int i =0; i < trajectory.size(); i++){
            PathEntry a_path;

            a_path.conflict_point = p[i].current_point;
            if (i == 0){
                a_path.arrival_time = p[i].arrival_time_min;
            }
            else
            {
                a_path.arrival_time = p[0].arrival_time_min + pairDistancesMap[p[0].current_point][p[i].current_point]/speed_max;

                if (a_path.arrival_time >= p[i].arrival_time_min && a_path.arrival_time <= p[i].arrival_time_max){
                }
            }
            int direction = vNameToDirection[p[i].current_point]; 
            a_path.leaving_time_tail = a_path.arrival_time + Li(direction, length)/w + Li(direction, length)/speed_max;
            
            std::cout << p[i].arrival_time_min << " " << p[i].arrival_time_max << " " << a_path.arrival_time << std::endl;

            result_path.push_back(a_path);
        }
    }

    // std::cout << "finish" << std::endl;

    // return a Path, which is a vector of PathEntry. a PathEntry contains vertex (int), arrivalTime (double) and leavingTime (double)
    return result_path;
}

Successors SIPP::get_successors(
        Node p[],
        int current_position,
        int trajectory_size,
        double v_min, 
        double v_max, 
        double length, 
        const ReservationTable& rt
    ){
    // empty successors, type vector
    Successors successors;

    // cfg = configuration of m applied to s

    // m = next possible motion -> get from Instance

    // m_time = time to excecute m- using formula t=d/v

    // std::cout << "hello" << " " << p[current_position].current_point << " " << p[current_position].next_point << " " << v_max<< std::endl;
    // std::cout << "hello2 " << pairDistancesMap[p[current_position].current_point][p[current_position].next_point] << std::endl;



    double m_time_min = estimate_cost(p[current_position].current_point, p[current_position].next_point, v_max);
    double m_time_max = estimate_cost(p[current_position].current_point, p[current_position].next_point, v_min);

    // start_t = time(s) + m_time
    double start_t = p[current_position].arrival_time_min + m_time_min;
    // ent_t = endTime(interval(s)) + m_time + time for vehicle to pass m
    
    int direction = vNameToDirection[p[current_position].next_point]; 
    
    double end_time = p[current_position].arrival_time_max;
    double end_t = end_time + m_time_max + Li(direction, length)/w + Li(direction, length)/v_min;


    // for each safe interval in cfg:


    for (std::list<TimeInterval>::const_iterator it = rt[p[current_position].next_point].begin(); it!= rt[p[current_position].next_point].end(); ++it){
        Node possible_successor = p[current_position + 1];

       //    if (start_time(i) > end_t) or (end_time(i) < start_t):
        if ((it->t_min > end_t) || (it->t_max < start_t)){
        //        continue
            continue;
        }

        double t_min = std::max(it->t_min, start_t);
        double t_max = std::min(it->t_max, end_t) - Li(direction, length)/w - Li(direction, length)/v_min;
        //    t = earliest arrival time at cfg during interval i with no collisions

        //    if t does not exist:

        //        continue

        //    s_successor = start of configuration cfg with interval i and time t

        //    insert s_successor into successors

        possible_successor.arrival_time_min = t_min;
        possible_successor.arrival_time_max = t_max;

        possible_successor.speed_for_arrival_time_min = pairDistancesMap[p[0].current_point][possible_successor.current_point]/t_min;
        possible_successor.speed_for_arrival_time_max = pairDistancesMap[p[0].current_point][possible_successor.current_point]/t_max;

        // NEED TO MODIFY HERE!!!
        possible_successor.cost_min = t_min - p[current_position].arrival_time_min;
        possible_successor.cost_max = t_max - p[current_position].arrival_time_min;

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
    return ((pairDistancesMap)[start_point][end_point])/speed;
}


int SIPP::find(std::vector<Node>& open){
	for (int i = 0; i < open.size(); i++)
		if (open[i].color == 1)
			return i;
    return open.size() - 1;    
}

int SIPP::find_min(std::vector<Node>& open){
    int min_index = find(open);
	int min = open[min_index].f;
	for (int i = 0; i < open.size(); i++)
	{
		if (open[i].f < min && open[i].color == 1)
		{
			min_index = i;
			min = open[i].f;
		}
	}
	return min_index;

}


int SIPP::find_point(int n, Node* p, int current_point){
    for (int i = 0; i < n; i++)
		if (p[i].current_point == current_point)
			return  i;
    return -1;
}