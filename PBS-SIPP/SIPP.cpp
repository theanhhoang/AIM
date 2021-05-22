#include "SIPP.h"
#include <iostream>
#include <fstream>
#include <string>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <cstdio>
#include <math.h>
#include <ilcplex/ilocplex.h>

using namespace std;

SIPP::SIPP(Instance& instance): instance(instance)
{
    pairDistancesMap = instance.getPairDistancesMap();
    vNameToDirection = instance.getVNameToDirection();
    vNameToID = instance.getVNameToID();


    // for (auto& it: vNameToID) {
    //     cout<< it.first << " :    " << it.second <<endl;
    // }
    
}

Path SIPP::run(int agentID, const ReservationTable& rt)
{

    // // from agent's ID -> get agent's start_location, goal location, earliest_start_time, maximum velocity
    
    // agent details
    Agent agent = agents[agentID];
    int id = agent.id;
    int start_location = agent.start_location;
    int goal_location = agent.goal_location;
    vector<int> trajectory = agent.trajectory;
    double earliest_start_time = agent.earliest_start_time;
    double v_min = agent.v_min;
    double v_max = agent.v_max;
    double length = agent.length; // length of the vehicle


    // //TEST
    // int id = 1;
    // int start_location = vNameToID.find("WER_0")->second;
    // int goal_location = vNameToID.find("WER_7")->second;

    // vector<int> trajectory;
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
    //////////////////////////////////////////////////////////


    vector<Node> open;
    vector<Node> result_nodes;
    vector<Node> p[trajectory.size()];


    list<TimeInterval> safe_intervals = getSafeIntervals( rt[trajectory[0]] );
    int interval_idx = 0;
    for (list<TimeInterval>::const_iterator it = safe_intervals.begin(); it!= safe_intervals.end(); ++it){
        if (earliest_start_time <= it->t_max)
        {
            Node a_new_node;
            a_new_node.current_point = trajectory[0];


            a_new_node.cost_min = 0;

            a_new_node.g = 0;
            a_new_node.h = estimate_cost(trajectory[0], trajectory.back(), v_min);
            a_new_node.f = 0;

            a_new_node.color = 1;


            a_new_node.previous_point = -1;
            a_new_node.arrival_time_min = max(earliest_start_time, it->t_min);
            a_new_node.arrival_time_max = it->t_max;
            a_new_node.next_point = trajectory[1];
            a_new_node.index = 0;
            
            a_new_node.interval_index = interval_idx;
            a_new_node.interval_t_min = it->t_min;
            a_new_node.interval_t_max = it->t_max;
            
            a_new_node.index = 0;
            p[0].push_back(a_new_node);                      

            interval_idx ++;

        }
  
    }





    for (int i = 1; i < trajectory.size(); i++){

        list<TimeInterval> safe_intervals = getSafeIntervals( rt[trajectory[i]] );
        int interval_idx = 0;
        for (list<TimeInterval>::const_iterator it = safe_intervals.begin(); it!= safe_intervals.end(); ++it){
        
        
            Node a_new_node;
            a_new_node.current_point = trajectory[i];

            a_new_node.arrival_time_min = 0;
            a_new_node.arrival_time_max = 0;

            a_new_node.cost_min = 0;

            a_new_node.g = 0;
            a_new_node.h = estimate_cost(trajectory[i], trajectory.back(), v_min);
            a_new_node.f = 0;

            a_new_node.color = 0;

            
            a_new_node.previous_point = i-1;
            
            
            if (i < trajectory.size()-1){
                a_new_node.next_point = trajectory[i+1];
            }else
            {
                a_new_node.next_point = -1;
            }

            a_new_node.interval_index = interval_idx;
            a_new_node.interval_t_min = it->t_min;
            a_new_node.interval_t_max = it->t_max;
            
            a_new_node.index = i;
            p[i].push_back(a_new_node);            

            interval_idx ++;
        }
    }


    open.push_back(p[0][0]);
    int first_conflict_point_counter = 0;
    // int i = 0;
    // while s is not s_goal:
    Path result_path;
    while (open.size() != 0 || first_conflict_point_counter!= p[0].size()-1  ){

        if (open.size() == 0){
            first_conflict_point_counter++;
            open.push_back(p[0][first_conflict_point_counter]);
        }
        //    remove s with the smallest f-value from OPEN

        int k = find_min(open);


        // cout << "Open size: " << open.size() << ",k: " << k << endl;
        Node s = open.at(k);

        // cout << "s.index: " << s.index << ", s.current_point: " << s.current_point << endl;

        open.erase(open.begin() + k);

        // cout << s.index << ". trajectory.size() - 1: " << trajectory.size() -1 << endl;

        if (s.index == trajectory.size() - 1 ){
            // cout << "hello" << endl;
            // // NOW WE HAVE A VECTOR OF POSSIBLE_SUCCESSOR (SUCCESSORS), WE NEED TO CONVERT IT INTO THE VECTOR OF PATH_ENTRY (PATH)  
            result_nodes.clear();
            Node res = s;
            // cout << res.current_point << " " << (res.parent)->current_point << endl;
            while (true){
                // cout << "test" << endl;
                // cout << res.current_point << "- " << res.interval_t_min << "--"<< res.arrival_time_min  << "- " << res.arrival_time_max << "--" << res.interval_t_max << endl;

                result_nodes.insert(result_nodes.begin(), res);
                if (res.previous_point  == -1){
                    break;
                }
                res = *(res.parent);
            }

            IloEnv env = IloEnv();
            IloModel model = IloModel(env);
            IloExpr sum_obj = IloExpr(env);
            IloNumVarArray var(env);
            IloRangeArray con(env);

            //speed
            var.add(IloNumVar(env, 1/v_max,1/v_min, ILOFLOAT));  //t
            //start time
            var.add(IloNumVar(env, result_nodes[0].interval_t_min, result_nodes[0].interval_t_max, ILOFLOAT));  //t

            // sum_obj = var[1];
            int final_conflict_point_direction = vNameToDirection[result_nodes.back().current_point];
            sum_obj = var[1] + pairDistancesMap[result_nodes[0].current_point][result_nodes.back().current_point]*var[0] + Li(final_conflict_point_direction, length)/w + Li(final_conflict_point_direction, length)*var[0];

            model.add(IloMinimize(env, sum_obj));

            
            // con.add(var[c] >= e[i]);


            for (int i =0; i < trajectory.size(); i++){
                IloExpr node_arrival_time_min = var[1] + pairDistancesMap[result_nodes[0].current_point][result_nodes[i].current_point]*var[0];

                int direction = vNameToDirection[result_nodes[i].current_point]; 
                IloExpr node_leaving_time_tail = node_arrival_time_min + Li(direction, length)/w + Li(direction, length)*var[0];

                if ( i > 0){
                    con.add(  node_arrival_time_min - result_nodes[i].arrival_time_min  >=0);

                }
                con.add(node_leaving_time_tail - result_nodes[i].arrival_time_max  <=0);                
            }

            model.add(con);
            IloCplex cplex(model);
            cplex.setOut(env.getNullStream());
            cplex.extract(model);
            cplex.exportModel("model.lp");

            // cout << cplex.solve() << endl;
            result_path.clear();
            if (cplex.solve()){
                // result = cplex.getObjValue();
                
                cplex.writeSolution("solution");

                // cout << cplex.getValue(var[0]) << "---" << cplex.getValue(var[1]) << endl;
                for (int i =0; i < trajectory.size(); i++){
                    PathEntry a_path;

                    a_path.conflict_point = result_nodes[i].current_point;

                    a_path.arrival_time = cplex.getValue(var[1]) + pairDistancesMap[result_nodes[0].current_point][result_nodes[i].current_point]*cplex.getValue(var[0]);

                    int direction = vNameToDirection[result_nodes[i].current_point]; 
                    a_path.leaving_time_tail = a_path.arrival_time + Li(direction, length)/w + Li(direction, length)*cplex.getValue(var[0]);

                    cout <<  a_path.arrival_time << " " << a_path.leaving_time_tail << endl;
                    result_path.push_back(a_path);
                }                
                cout << "result_path.size(): "<< result_path.size() << endl;
                return result_path;
            }
            else{
                for (int i = 0; i < trajectory.size(); i++){
                    for (int j = 0; j < p[i].size(); j++){
                        p[i][j].color = 0;
                    }
                }
            }

        }
        else{
            // int current_position = find_point(trajectory.size(), p, open[k].current_point);
            Successors successors = get_successors(p, s, trajectory.size(), v_min, v_max, length, rt, first_conflict_point_counter);
            // cout << "Successors size: " << successors.size() << endl;

            for (int j = 0; j < successors.size(); ++j){
                // cout << successors[j].current_point << "---" << successors[j].next_point << endl;

                int next_position = successors[j].index;
                int interval_idx = successors[j].interval_index;
                if (p[next_position][interval_idx].color == 0) // if p[next_position] has not been in Open yet //state, not position!!
                {
                    p[next_position][interval_idx].g = 100000;
                    p[next_position][interval_idx].f = 100000;
                    p[next_position][interval_idx].color = 1;

                }
                if (p[next_position][interval_idx].g > p[s.index][s.interval_index].g + successors[j].cost_min) // if p[next_position] has already been in Open
                {
                    successors[j].parent = &(p[s.index][s.interval_index]);





                    p[next_position][interval_idx] = successors[j];
                    p[next_position][interval_idx].g = p[s.index][s.interval_index].g + successors[j].cost_min;

                //            update_time(s_successor)

                    p[next_position][interval_idx].f = p[next_position][interval_idx].g + p[next_position][interval_idx].h;

                    // cout << p[next_position][interval_idx].current_point << "-----" << (p[next_position][interval_idx].parent)->current_point << endl;

                    open.push_back(p[next_position][interval_idx]);

                    // cout << open.front().index << "-----" << (open.front().parent)->index << endl;
                }
            }


        }
    }

    cout << "finish" << endl;
    // return a Path, which is a vector of PathEntry. a PathEntry contains vertex (int), arrivalTime (double) and leavingTime (double)
    Path result_empty_path;
    return result_empty_path;
}

Successors SIPP::get_successors(
        vector<Node> p[],
        Node s,
        int trajectory_size,
        double v_min, 
        double v_max, 
        double length, 
        const ReservationTable& rt,
        int first_conflict_point_counter
    ){
    // empty successors, type vector
    Successors successors;

    // cfg = configuration of m applied to s

    // m = next possible motion -> get from Instance

    // m_time = time to excecute m- using formula t=d/v

    // cout << "hello" << " " << p[current_position].current_point << " " << p[current_position].next_point << " " << v_max<< endl;
    // cout << "hello2 " << pairDistancesMap[p[current_position].current_point][p[current_position].next_point] << endl;


    double m_time_min = estimate_cost(s.current_point, s.next_point, v_max);
    double m_time_max = estimate_cost(s.current_point, s.next_point, v_min);

    // start_t = time(s) + m_time
    double start_t = s.arrival_time_min + m_time_min;
    // ent_t = endTime(interval(s)) + m_time + time for vehicle to pass m
    
    int direction = vNameToDirection[s.next_point]; 
    
    double end_time = s.arrival_time_max;
    double end_t = end_time + m_time_max + Li(direction, length)/w + Li(direction, length)/v_min;


    // for each safe interval in cfg:

    // transfer reservation table into safe intervals!!!
    // list<TimeInterval> safe_intervals = getSafeIntervals( rt[s.next_point]);

    for (int it = 0; it!= p[s.index + 1].size(); ++it){
        Node possible_successor = p[s.index + 1][it];

        possible_successor.color = 1;
        // cout << possible_successor.interval_t_min << " " << end_t << " "<< possible_successor.interval_t_max << " "<< start_t << endl;

       //    if (start_time(i) > end_t) or (end_time(i) < start_t):
        if ((possible_successor.interval_t_min > end_t) || (possible_successor.interval_t_max < start_t)){
        //        continue
            continue;
        }



        double t_min = max(possible_successor.interval_t_min, start_t);
        // double t_max = min(possible_successor.interval_t_max, end_t) - Li(direction, length)/w - Li(direction, length)/v_min;
        double t_max = min(possible_successor.interval_t_max, end_t);
        // cout << possible_successor.interval_t_min << " " << end_t << " "<< possible_successor.interval_t_max << " "<< start_t << endl;
        //    t = earliest arrival time at cfg during interval i with no collisions

        //    if t does not exist:

        //        continue

        //    s_successor = start of configuration cfg with interval i and time t

        //    insert s_successor into successors

        possible_successor.arrival_time_min = t_min;
        possible_successor.arrival_time_max = t_max;


        // NEED TO MODIFY HERE!!!
        possible_successor.cost_min = t_min - s.arrival_time_min;

        successors.push_back(possible_successor);
    }

   
    return successors;
}


float SIPP::Li(int direction, double agent_length){
    // straight 
    if (direction == 2){
        // cout << "direction: " << direction << "-" << vehicleLength << endl; 
        return agent_length;
    }
    //right, turnRadiusRight
    else if (direction == 1)
    {
        // cout << "direction: " << direction << "-" << 4*turnRadius*asin(vehicleLength/(2*turnRadius)) << endl; 
        return 4*turn_radius_right*asin(agent_length/(2*turn_radius_right));
    }
    //left, turnRadiusLeft
    else if (direction == 0)
    {
        // cout << "direction: " << direction << "-" << 4*turnRadius*asin(vehicleLength/(2*turnRadius)) << endl; 
        return 4*turn_radius_left*asin(agent_length/(2*turn_radius_left));
    }
}

double SIPP::estimate_cost(int start_point, int end_point, double speed){
    return ((pairDistancesMap)[start_point][end_point])/speed;
}


int SIPP::find(vector<Node>& open){
	for (int i = 0; i < open.size(); i++)
		if (open[i].color == 1)
			return i;
    return open.size() - 1;    
}

int SIPP::find_min(vector<Node>& open){
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

list<TimeInterval> SIPP::getSafeIntervals(list<TimeInterval> rt)
{
    list<TimeInterval> safe_intervals;
    // cout << "Size: " << rt.size() << endl;
    // cout << rt.begin()->t_min << "---" << rt.begin()->t_max << endl;
    if (rt.size() == 0){
        TimeInterval aInterval;
        aInterval.t_min = 0;
        aInterval.t_max = 100000;

        safe_intervals.push_back(aInterval);

        return safe_intervals;
    }



    TimeInterval aInterval_1;
    aInterval_1.t_min = 0;
    aInterval_1.t_max = rt.begin()->t_min;
    safe_intervals.push_back(aInterval_1);




    for (list<TimeInterval>::iterator it = rt.begin(); it!= prev(rt.end()); ++it){
        // cout << it->t_max << "_" << next(it)->t_min << "_" << next(it)->t_max << endl;
        TimeInterval aInterval_2;
        aInterval_2.t_min = it->t_max;
        aInterval_2.t_max = next(it)->t_min;
        safe_intervals.push_back(aInterval_2);
    }


    TimeInterval aInterval_3;
    // cout << prev(rt.end())->t_max << "__" << prev(rt.end())->t_max + 100000 << endl;
    aInterval_3.t_min = prev(rt.end())->t_max;
    aInterval_3.t_max = prev(rt.end())->t_max + 100000;
    safe_intervals.push_back(aInterval_3);  

    return safe_intervals;
}
