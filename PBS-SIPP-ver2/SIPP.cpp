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
}

Path SIPP::run(int agentID, const ReservationTable& rt)
{

    // // from agent's ID -> get agent's start_location, goal location, earliest_start_time, maximum velocity
    
    // agent details
    Agent agent = agents[agentID];
    int id = agent.id;
    vector<int> trajectory = agent.trajectory;
    double earliest_start_time = agent.earliest_start_time;
    double v_min = agent.v_min;
    double v_max = agent.v_max;
    double length = agent.length; // length of the vehicle




    vector<Node> open;
    vector<Node> result_nodes;
    vector<Node> p[trajectory.size()];
    vector<Node> node_storage[trajectory.size()];


    list<TimeInterval> safe_intervals = getSafeIntervals( rt[trajectory[0]] );
    int interval_idx = 0;
    for (list<TimeInterval>::const_iterator it = safe_intervals.begin(); it!= safe_intervals.end(); ++it){
        if (earliest_start_time <= it->t_max)
        {
            Node a_new_node;
            a_new_node.current_point = trajectory[0];


            a_new_node.cost_min = 0;

            a_new_node.g = 0;
            a_new_node.h = estimate_cost(trajectory[0], trajectory.back(), v_max);
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
            a_new_node.h = estimate_cost(trajectory[i], trajectory.back(), v_max);
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



    for (int i =0; i < p[0].size(); i++){
        open.push_back(p[0][i]);
    }
    
    int first_conflict_point_counter = 0;
    Path result_path;
    while (open.size() != 0 ){



        //    remove s with the smallest f-value from OPEN

        int k = find_min(open);


        Node s = open.at(k);


        open.erase(open.begin() + k);
        if (result_path.size() > 0){
            if (s.f >= result_path.back().arrival_time){
                return result_path;
            }
        }
        if (s.index == trajectory.size() - 1 ){
            // // NOW WE HAVE A VECTOR OF POSSIBLE_SUCCESSOR (SUCCESSORS), WE NEED TO CONVERT IT INTO THE VECTOR OF PATH_ENTRY (PATH)  
            result_nodes.clear();
            Node res = s;
            while (true){

                result_nodes.insert(result_nodes.begin(), res);
                if (res.previous_point  == -1){
                    break;
                }

                res = node_storage[res.parent_index_1][res.parent_index_2];
                
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

            int final_conflict_point_direction = vNameToDirection[result_nodes.back().current_point];
            sum_obj = var[1] + pairDistancesMap[result_nodes[0].current_point][result_nodes.back().current_point]*var[0] + Li(final_conflict_point_direction, length)/w + Li(final_conflict_point_direction, length)*var[0];

            model.add(IloMinimize(env, sum_obj));

            


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
            cplex.setWarning(env.getNullStream());
            cplex.setError(env.getNullStream());
            cplex.extract(model);
            cplex.exportModel("model.lp");
            
            
            if (cplex.solve()){
                Path potential_path;
                
                cplex.writeSolution("solution");

                for (int i =0; i < trajectory.size(); i++){
                    PathEntry a_path;

                    a_path.conflict_point = result_nodes[i].current_point;

                    a_path.arrival_time = cplex.getValue(var[1]) + pairDistancesMap[result_nodes[0].current_point][result_nodes[i].current_point]*cplex.getValue(var[0]);

                    int direction = vNameToDirection[result_nodes[i].current_point]; 
                    a_path.leaving_time_tail = a_path.arrival_time + Li(direction, length)/w + Li(direction, length)*cplex.getValue(var[0]);

                    potential_path.push_back(a_path);

                }                
                env.end();
                sum_obj.end();

                
                if (result_path.size() == 0){
                    copy(potential_path.begin(), potential_path.end(), back_inserter(result_path));
                }
                else{
                    if (potential_path.back().leaving_time_tail < result_path.back().leaving_time_tail){
                        result_path.clear();
                        copy(potential_path.begin(), potential_path.end(), back_inserter(result_path));
                    }
                }

            }

           
           

        }
        else{
            node_storage[s.index].push_back(s);

            // int current_position = find_point(trajectory.size(), p, open[k].current_point);
            Successors successors = get_successors(p, s, trajectory.size(), v_min, v_max, length, rt, first_conflict_point_counter);

            for (int j = 0; j < successors.size(); ++j){

                int next_position = successors[j].index;
                successors[j].parent = &(node_storage[s.index].back());

                successors[j].parent_index_1 = s.index;
                successors[j].parent_index_2 = node_storage[s.index].size() -1;

                // cout << "Compare:  " << successors[j].arrival_time_min << "    " << s.arrival_time_min + successors[j].cost_min<< endl;
                successors[j].g = successors[j].arrival_time_min;
                successors[j].f = successors[j].g + successors[j].h;



                open.push_back(successors[j]);

            }


        }
    }
    if (result_path.size() == 0){
        cout << "finish" << endl;
        // return a Path, which is a vector of PathEntry. a PathEntry contains vertex (int), arrivalTime (double) and leavingTime (double)
        Path result_empty_path;
        return result_empty_path;

    }else{
        return result_path;
    }
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

    for (int it = 0; it!= p[s.index + 1].size(); ++it){
        Node possible_successor = p[s.index + 1][it];

        possible_successor.color = 1;
        if ((possible_successor.interval_t_min > end_t) || (possible_successor.interval_t_max < start_t)){
            continue;
        }



        double t_min = max(possible_successor.interval_t_min, start_t);
        double t_max = min(possible_successor.interval_t_max, end_t);
        // cout << possible_successor.interval_t_min << " " << end_t << " "<< possible_successor.interval_t_max << " "<< start_t << endl;
        //    t = earliest arrival time at cfg during interval i with no collisions

        //    if t does not exist:

        //        continue

        //    s_successor = start of configuration cfg with interval i and time t

        //    insert s_successor into successors

        possible_successor.arrival_time_min = t_min;
        possible_successor.arrival_time_max = t_max;


        possible_successor.cost_min = t_min - s.arrival_time_min;

        successors.push_back(possible_successor);
    }

   
    return successors;
}


float SIPP::Li(int direction, double agent_length){
    // straight 
    if (direction == 2){
        return agent_length;
    }
    //right, turnRadiusRight
    else if (direction == 1)
    {
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
    vector<TimeInterval> safe_intervals;
    if (rt.size() == 0){
        TimeInterval aInterval;
        aInterval.t_min = 0;
        aInterval.t_max = 100000;

        safe_intervals.push_back(aInterval);

        list<TimeInterval> output;

        copy(safe_intervals.begin(), safe_intervals.end(), back_inserter(output));
        return output;
    }

    list<TimeInterval> localRT = rt;
    localRT.sort([](const TimeInterval &f, const TimeInterval &s) { return f.t_min < s.t_min; });



    if (localRT.begin()->t_min > 0){
        TimeInterval aInterval_1;
        aInterval_1.t_min = 0;
        aInterval_1.t_max = localRT.begin()->t_min;
        safe_intervals.push_back(aInterval_1);
    }
    if (localRT.begin()->t_max < 100000){
        TimeInterval aInterval_2;
        aInterval_2.t_min = localRT.begin()->t_max;
        aInterval_2.t_max = 100000;
        safe_intervals.push_back(aInterval_2);
    }
    for (list<TimeInterval>::iterator it = std::next(localRT.begin()); it!= localRT.end(); ++it){
        vector<int> toBeDeleted;
        for (int sfIter = 0; sfIter < safe_intervals.size(); sfIter++){
            if (safe_intervals[sfIter].t_min >= it->t_min && safe_intervals[sfIter].t_min < it->t_max && safe_intervals[sfIter].t_max > it->t_max){
                safe_intervals[sfIter].t_min = it->t_max;
            }
            if (safe_intervals[sfIter].t_min < it->t_min && safe_intervals[sfIter].t_max > it->t_min && safe_intervals[sfIter].t_max <= it->t_max){
                safe_intervals[sfIter].t_max = it->t_min;
            }

            if (safe_intervals[sfIter].t_min < it->t_min && safe_intervals[sfIter].t_max > it->t_max){
                TimeInterval aInterval_3;
                aInterval_3.t_min = it->t_max;
                aInterval_3.t_max = safe_intervals[sfIter].t_max;


                safe_intervals[sfIter].t_max = it->t_min;
                safe_intervals.insert(safe_intervals.begin() + sfIter+1, aInterval_3);
                break;

            }

            if (safe_intervals[sfIter].t_min > it->t_min && safe_intervals[sfIter].t_max < it->t_max){
                toBeDeleted.push_back(sfIter);
            }

        }
        
        if (toBeDeleted.size()>0){
            for (int toBeDeletedIter = toBeDeleted.size()-1; toBeDeletedIter >=0; toBeDeletedIter--){
                safe_intervals.erase(safe_intervals.begin() + toBeDeleted[toBeDeletedIter]);
            }
        }
  
    }


    list<TimeInterval> output;
    copy(safe_intervals.begin(), safe_intervals.end(), back_inserter(output));
    return output;
}
