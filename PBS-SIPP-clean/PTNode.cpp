#include "PTNode.h"
#include <fstream>

PTNode::PTNode(vector<Path> pl, std::map<int, std::set<int> > pr){
	plan = pl;
	priority = pr;
}

void PTNode::writeToFile(Instance& instance, const string& file_name)
{
	std::ofstream file;
	// file.open(file_name, std::ios::app);
	file.open(file_name);
	if(file.is_open()){
		file << "In Point;Out Point;Length;Earliest Arrival;In Time;Out Time Head; Out Time Tail;Time spend;Speed" << '\n';

		for(int i = 0; i < (signed)plan.size(); ++i){
			double pairDistanceValue = (instance.getPairDistancesMap())[plan[i][0].conflict_point][plan[i][plan[i].size()-1].conflict_point];
			file <<  instance.getVIDToName()[plan[i][0].conflict_point]  \
				<< ";" <<  instance.getVIDToName()[plan[i][plan[i].size()-1].conflict_point]  \
				<< ";" <<  pairDistanceValue \
				<< ";" << instance.getEarliestStartTime(i) \
				<< ";" << plan[i][0].arrival_time \
				<< ";" << plan[i][plan[i].size()-1].arrival_time \
				<< ";" << plan[i][plan[i].size()-1].leaving_time_tail \
				<< ";" << plan[i][plan[i].size()-1].arrival_time - plan[i][0].arrival_time\
				<< ";" << pairDistanceValue/(plan[i][plan[i].size()-1].arrival_time - plan[i][0].arrival_time)\
				<< '\n';
		}
	}
	else std::cout << "unable to open file";
	file.close();
}

bool PTNode::hasCollision(PathEntry a, TimeInterval b){
	if((a.leaving_time_tail - b.t_min < EPSILON) || (b.t_max - a.arrival_time < EPSILON)) {
		return false;
	}
	return true;
}

void PTNode::topologicalSortUtil(int v, bool visited[], std::list<int>& List)
{
    // Mark the current node as visited.
    visited[v] = true;
 
    // Recur for all the vertices
    // adjacent to this vertex

    for (auto it = priority[v].begin(); it != priority[v].end(); ++it)
        if (!visited[*it])
            topologicalSortUtil(*it, visited, List);
 
    // Push current vertex to stack
    // which stores resultpriority
    List.push_back(v);
}

std::list<int> PTNode::topologicalSort(){
	std::list<int> List;
 
    // Mark all the vertices as not visited
    bool* visited = new bool[plan.size()];
    for (int i = 0; i < (signed) plan.size(); ++i)
        visited[i] = false;
 
    // Call the recursive helper function
    // to store Topological
    // Sort starting from all
    // vertices one by one
    for (int i = 0; i < (signed)plan.size(); ++i){
        if (visited[i] == false)
            topologicalSortUtil(i, visited, List);
    }
 	delete[] visited;
	return List;
}

std::list<int> PTNode::topologicalSort(int agent){
	std::list<int> List;
 
    // Mark all the vertices as not visited
    bool* visited = new bool[plan.size()];
    for (int i = 0; i < (signed) plan.size(); ++i)
        visited[i] = false;
 
    // Call the recursive helper function
    // to store Topological
    // Sort starting from all
    // vertices one by one

    topologicalSortUtil(agent, visited, List);

 	delete[] visited;
	return List;
}

void PTNode::calculateCost(){
	//std::cout << "calculate cost running\n";
	double c = 0;
	for(auto it : plan){
		c += it.back().leaving_time_tail;
	}
	cost = c;
}


void printRT(ReservationTable rt){
	std::cout<<"\n\n_________________________________printing ReservationTable_________________________\n";
	for(int i = 0; i < (signed) rt.size(); ++i){
		std::cout<<"cp" << i << "\t";
		for(auto ittemp : rt[i]){
			std::cout<< ittemp.t_min << "\t" <<ittemp.t_max << "\t" << ittemp.agent_id << "\t";
		}
		std::cout<<"\n";
	}
	std::cout<<"_____________________________________________________________________________________\n\n";
}

std::tuple<int, int, int> PTNode::getFirstCollision(Instance& instance){
	std::tuple<int, int, int> result = std::make_tuple(-1, -1, -1);
	ReservationTable table(instance.getNumOfVertices());
	std::set<int> s;
	for(int i = 0; i < (signed) plan.size();++i) s.insert(i);
	getRTFromP(instance, table, s);

	//iterating path in plan vector<vector<PathEntry>>
	int aid = 0;
	for(auto it : plan){
		//iterating points in path
		for(auto it2 : it){
			//it2 is path entry
			std::list<TimeInterval> vertexTimeTable = table[it2.conflict_point];
			for(auto it3 : vertexTimeTable){
				if(aid == it3.agent_id)
					continue;
				//if  collision
				if(hasCollision(it2, it3)){
					result = std::make_tuple(aid, it3.agent_id, it2.conflict_point);
					return result;
				}
			}
		}
		++aid;
	}
	return result;
}

std::tuple<int, int, int> PTNode::getEarliestCollision(Instance& instance){
	//std::cout << "running getFirstCollision\n";
	std::vector<std::tuple<double, int, int, int> > allCollisions;
	ReservationTable table(instance.getNumOfVertices());
	//vector<list<TimeInterval> >
	std::set<int> s;
	for(int i = 0; i < (signed) plan.size();++i) s.insert(i);
	getRTFromP(instance, table, s);

	int vehicle1ID = 0;
	for(Path vehicle1Path : plan){
		//iterating points in path
		for(PathEntry vehicle1PathEntry : vehicle1Path){
			//it2 is path entry
			std::list<TimeInterval> conflictPointTimeTable = table[vehicle1PathEntry.conflict_point];
			for(TimeInterval vehicle2AtConflictPoint : conflictPointTimeTable){
				if(vehicle1ID == vehicle2AtConflictPoint.agent_id)
					continue;
				//if  collision
				if(hasCollision(vehicle1PathEntry, vehicle2AtConflictPoint)){
					allCollisions.push_back(std::make_tuple(std::max(vehicle1PathEntry.arrival_time, vehicle2AtConflictPoint.t_min), vehicle1ID, vehicle2AtConflictPoint.agent_id, vehicle1PathEntry.conflict_point));
				}
			}
		}
		++vehicle1ID;
	}

	if(allCollisions.empty()){
		return std::make_tuple(-1, -1, -1);
	}

	std::tuple<double, int, int, int> * earliestCollision = &allCollisions[0];
	for(std::tuple<double, int, int, int> collision : allCollisions){
		if(std::get<0>(collision) < std::get<0>(*earliestCollision)){
			earliestCollision = &collision;
		}
	}
	return std::make_tuple(get<1>(*earliestCollision), get<2>(*earliestCollision), get<3>(*earliestCollision));
}


/*
void PTNode::getRT(ReservationTable &rt, int index){
	for (int i = 0; i < plan.size(); ++i){
		if(priority[i].find(index) != priority[i].end()){
			for(auto it = plan[i].begin(); it != plan[i].end(); ++it){
				TimeInterval newTI;
				newTI.t_max = it->leaving_time_tail;
				newTI.t_min = it->arrival_time;
				newTI.agent_id = i;
				rt[it->vertex].push_back(newTI);
			}
			getRT(rt, i);
		}
	}
}*/

void PTNode::getRTP(std::set<int> &p, int index){
	for (int i = 0; i < (signed) plan.size(); ++i){
		if(p.find(i) != p.end()){ 
			// already in the list
			continue;
		}
		if(priority[i].find(index) == priority[i].end()){
			continue;
		}
		p.insert(i);
		getRTP(p, i);
	}
}

void PTNode::getRTFromP(Instance& instance , ReservationTable& rt, std::set<int> p){
	for(auto it : p){
		for(auto it2 : plan[it]){
			//plan: vector<vector<pathentry>>
			TimeInterval newTI;
			std::vector<int> CPs = instance.getConflictPoints(it2.conflict_point);
			newTI.t_max = it2.leaving_time_tail;
			newTI.t_min = it2.arrival_time;
			newTI.agent_id = it;
			rt[it2.conflict_point].push_back(newTI);
			for(auto it3 : CPs){
				rt[it3].push_back(newTI);
			}
		}
	}
}

void PTNode::getRTFromP(Instance& instance , ReservationTable& rt, std::set<int> p, int index, std::map<int, std::map<int,std::vector<int> > >& ta){
	vector<int> same = ta[instance.agents[index].trajectory[0]][instance.agents[index].trajectory[instance.agents[index].trajectory.size()-1]];

    for(int i = 0; i < same.size(); ++i){
        if(same[i] == index){
        	continue;
        }
        if (instance.getEarliestStartTime(same[i]) >= instance.getEarliestStartTime(index)){
        	continue;
        }
        for(int i2 = 0; i2 < plan[same[i]].size(); ++i2){
            TimeInterval newTI;
            std::vector<int> CPs = instance.getConflictPoints(plan[same[i]][i2].conflict_point);
            newTI.t_max = plan[same[i]][i2].leaving_time_tail;
            newTI.t_min = 0;
            newTI.agent_id = same[i];
            rt[plan[same[i]][i2].conflict_point].push_back(newTI);
            for(auto it3 : CPs){
                rt[it3].push_back(newTI);  
            }
        }                
    }


    // get vehicles having same entry point only
    int conflictPointID = instance.getConflictPoints(instance.agents[index].trajectory[0])[0];
    for(auto it5 : ta){
        if (conflictPointID != it5.first){
        	continue;
        }

        for(auto it6 : it5.second){
            for(auto it7 : it6.second){
                if (instance.getEarliestStartTime(it7) >= instance.getEarliestStartTime(index)){
                	continue;
                }

                // if vehicle is already having a plan
                if (plan[it7].size() > 0){
                    TimeInterval newTI;
                    std::vector<int> CPs = instance.getConflictPoints(plan[it7][0].conflict_point);
                    newTI.t_max = plan[it7][0].leaving_time_tail;
                    newTI.t_min = 0;
                    newTI.agent_id = it7;
                    rt[plan[it7][0].conflict_point].push_back(newTI);
                    for(auto it8 : CPs){
                        rt[it8].push_back(newTI);
                    }
                }
                else{
                    TimeInterval newTI;
                    std::vector<int> CPs = instance.getConflictPoints(instance.getAgents()[it7].start_location);
                    newTI.t_max = instance.getAgents()[it7].earliest_start_time;
                    newTI.t_min = 0;
                    newTI.agent_id = it7;
                    for(auto it8 : CPs){
                        rt[it8].push_back(newTI);
                    }
                }
            }
        }
    }

    // Vehicle could not arrive before the Earliest arrival time
	TimeInterval beginTI;
	beginTI.t_min = 0;
	beginTI.t_max = instance.getEarliestStartTime(index);
	beginTI.agent_id = index;
	rt[instance.getStartCP(index)].push_back(beginTI);
	std::vector<int> startCPs = instance.getConflictPoints(instance.getStartCP(index));
	for(auto it : startCPs){
		rt[it].push_back(beginTI);
	}


	for(auto it : p){
		for(auto it2 : plan[it]){
			//plan: vector<vector<pathentry>>
			TimeInterval newTI;
			std::vector<int> CPs = instance.getConflictPoints(it2.conflict_point);
			newTI.t_max = it2.leaving_time_tail;
			newTI.t_min = it2.arrival_time;
			newTI.agent_id = it;
			rt[it2.conflict_point].push_back(newTI);
			for(auto it3 : CPs){
				rt[it3].push_back(newTI);
			}
		}
	}
}
