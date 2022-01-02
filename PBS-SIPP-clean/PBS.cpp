#include "PBS.h"

PBS::PBS(Instance& instance): sipp(instance), instance(instance)
{
	arrivingVehicles = instance.getAgents();
	initializeAgents(trajectoryToAgent);
}
bool PBS::hasCollision(PathEntry a, PathEntry b){
	if((a.leaving_time_tail - b.arrival_time < EPSILON) || (b.leaving_time_tail - a.arrival_time < EPSILON)){
		return false;
	}
	return true;
}

bool PBS::hasCollision(PathEntry a, TimeInterval b){
	if((a.leaving_time_tail - b.t_min < EPSILON) || (b.t_max - a.arrival_time < EPSILON)) {
		return false;
	}
	return true;
}

bool PBS::replanVehicle(PTNode& node, int vehicle){
	ReservationTable rt(instance.getNumOfVertices());
	std::set<int> rtp;
	node.getRTP(rtp, vehicle);
	node.getRTFromP(instance, rt, rtp, vehicle, trajectoryToAgent);
	Path path = sipp.run(vehicle, rt);
	if(path.empty()) return false;
	node.plan[vehicle] = path;

	ReservationTable rtdebug(instance.getNumOfVertices());
	node.getRTFromP(instance, rtdebug, rtp, vehicle, trajectoryToAgent);
	if(!checkValid(rtdebug, path, vehicle)) {
		return false;
	}
	return true;
}

bool PBS::UpdatePlan(PTNode& node, int vehicleI)
{
	// replan for vehicle i
	if(!replanVehicle(node, vehicleI)){
		return false;
	}

	std::list<int> prioritySortedList = node.topologicalSort(vehicleI);
	prioritySortedList.reverse();
	auto indexLocation = std::find(prioritySortedList.begin(), prioritySortedList.end(), vehicleI);
	std::list<int> list(indexLocation, prioritySortedList.end());

	for(auto it : list){
		//it is the index of agents in list
		for(auto it2 : list){
			if (it2 == it){
				break;
			}
			for(auto it3 : node.plan[it]){
				//it2 is the path entries of the agent it
				for(auto it4 : node.plan[it2]){
					if(!instance.isSamePoint(it3.conflict_point, it4.conflict_point)){
						continue;
					}
					if(hasCollision(it3, it4)){
						if(!replanVehicle(node, it)){
							return false;
						}
					}
				}
			}
		}
		
	}
	return true;
}

void PBS::run(const string& outputFileName)
{
    std::stack<PTNode> POStack;

    //1 2
    vector<Path> plan;
    for(int i = 0; i < (signed) arrivingVehicles.size(); ++i){
    	Path newPath;
    	plan.push_back(newPath);
    }

    std::map<int, std::set<int> > priority;
    initializePriority(priority, trajectoryToAgent);
	PTNode Root = PTNode(plan, priority);
	//initialize priority

	//3
	for(size_t i = 0; i < plan.size(); ++i)
		if(!UpdatePlan(Root, i)) std::cout << "no solution1\n" ;

	//7
	Root.calculateCost();
	//8
	POStack.push(Root);
	
	//9
	int iteration = 0;
	while (POStack.size() != 0){
		PTNode N = POStack.top();
		POStack.pop();

		//default to using earliest collision
		//std::tuple<int, int, int> C = N.getFirstCollision(instance);
		std::tuple<int, int, int> C = N.getEarliestCollision(instance);
		if(std::get<0> (C) == -1){
			N.writeToFile(instance, outputFileName);
			return;
		}

		if( (N.priority[get<0> (C)].find(get<1> (C)) != N.priority[get<0> (C)].end()) || (N.priority[get<1> (C)].find(get<0> (C)) != N.priority[get<1> (C)].end())){
			std::cout <<"!ERROR on iteration " << iteration << "\n";
			return;
		}
		//16 17 19
		std::map<int, std::set<int> > newPriority = N.priority;
		newPriority[std::get<0>(C)].insert(std::get<1>(C));
		PTNode newNode = PTNode(N.plan, newPriority);

		bool n1 = true;
		bool n2 = true;

		//20 21 22
		if(UpdatePlan(newNode, std::get<1>(C))) newNode.calculateCost();
		else n1 = false;

		//16 17 19
		std::map<int, std::set<int> > newPriority2 = N.priority;
		newPriority2[std::get<1>(C)].insert(std::get<0>(C));
		PTNode newNode2 = PTNode(N.plan, newPriority2);

		//20 21 22
		if(UpdatePlan(newNode2, std::get<0>(C))) newNode2.calculateCost();
		else n2 = false;

		if(n1 && n2){
			if(newNode.cost >= newNode2.cost){
				POStack.push(newNode);
				POStack.push(newNode2);
			}
			else{
				POStack.push(newNode2);
				POStack.push(newNode);
			}
		}
		else if(n1) POStack.push(newNode);
		else if (n2) POStack.push(newNode2);
		//23 non increasing order0
		
		//*******************************************DEBUG
		iteration++;
		// if(test == 1) return;
		//*******************************************DEBUG
	}

	std::cout << "no solution2\n";
}

void PBS::initializePriority(std::map<int, std::set<int> >& p,std::map<int, std::map<int, std::vector<int> > >& ta){

	for(auto it : trajectoryToAgent){
		for(auto it2 : it.second){
			for(auto it3 = it2.second.begin(); it3 != it2.second.end(); ++it3){
				for(auto it4 = next(it3); it4 != it2.second.end(); ++it4){
                    if (instance.getAgents()[*it3].earliest_start_time < instance.getAgents()[*it4].earliest_start_time){
                        p[*it3].insert(*it4);
                    }
				}

                int conflictPointID = instance.getConflictPoints(it.first)[0];
                for(auto it5 : trajectoryToAgent){
                    if (conflictPointID != it5.first){
                    	continue;
                    }
                    for(auto it6 : it5.second){
                        for(auto it7 : it6.second){
                            if (instance.getAgents()[*it3].earliest_start_time < instance.getAgents()[it7].earliest_start_time){
                                p[*it3].insert(it7);
                            }
                        }
                    }
                }
			}
		}
	}
}

void PBS::initializeAgents(std::map<int, std::map<int, std::vector<int> > >& ta){
	bool* visited = new bool[arrivingVehicles.size()];
	for (int i = 0; i < (signed) arrivingVehicles.size(); ++i)
        visited[i] = false;

	for(int i = 0; i < (signed) arrivingVehicles.size(); ++i){
		if(visited[i]) continue;

		std::map<double, int>  samePath;
		samePath[arrivingVehicles[i].earliest_start_time] = i;
		visited[i] = true;
		for(int j = 0; j < (signed) arrivingVehicles.size(); ++j){
			if(visited[j]) continue;
			if((arrivingVehicles[i].trajectory[0] == arrivingVehicles[j].trajectory[0])
				&& ((arrivingVehicles[i].trajectory[arrivingVehicles[i].trajectory.size()-1] == arrivingVehicles[j].trajectory[arrivingVehicles[j].trajectory.size()-1]))){
				samePath[arrivingVehicles[j].earliest_start_time] = j;
				visited[j] = true;
			}
		}
		
		for(auto it = samePath.begin(); it != samePath.end(); ++it){
			ta[arrivingVehicles[i].trajectory[0]][arrivingVehicles[i].trajectory[arrivingVehicles[i].trajectory.size()-1]].push_back(it->second);
		} 
	}
	delete[] visited;
}

void PBS::printPath(Path path){
	std::cout<<"\n\n_________________________________printing Path____________________________________\n";
	std::cout << "path size: " << path.size() <<"\n";
		for(auto itt = path.begin(); itt != path.end(); ++itt){
			std::cout << "cp" << itt->conflict_point << "\tarrival: "  << itt->arrival_time << "\tleave: " << itt->leaving_time_tail << "\n";
	}
	std::cout<<"__________________________________________________________________________________\n\n";
}

void PBS::printRT(ReservationTable rt){
	std::cout<<"\n\n_________________________________printing ReservationTable_________________________\n";
	for(int i = 0; i < (signed) rt.size(); ++i){
		std::cout<<"cp" << i << "\t";
		for(auto ittemp = rt[i].begin(); ittemp != rt[i].end(); ++ittemp){
			std::cout<< ittemp->t_min << "\t" <<ittemp->t_max << "\t#" << ittemp->agent_id << "#\t";
		}
		std::cout<<"\n";
	}
	std::cout<<"_____________________________________________________________________________________\n\n";
}

void PBS::printPriority(std::map<int, std::set<int> > p){
	std::cout<<"\n\n_________________________________printing Priority_______________________________\n";
	for(auto it = p.begin(); it != p.end(); ++it){
		std::cout << it->first << ": ";
		for(auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
			std::cout << *it2 << " ";
		std::cout << "\n";
	}
	std::cout<<"_________________________________________________________________________________\n\n";
}


bool PBS::checkValid(ReservationTable& rt, Path& path, int agent){
	for(auto it = path.begin(); it != path.end(); ++it){
		int cp = 0;
		for(auto it2 = rt.begin(); it2 != rt.end(); ++it2,++cp){
			if(it->conflict_point == cp){
				for(auto it3 = it2->begin(); it3 != it2->end(); ++it3){
					if(it3->agent_id == agent) continue;

					if(!(it->leaving_time_tail - it3->t_min < EPSILON) && !(it3->t_max - it->arrival_time < EPSILON)) {
						std::cout << "agent " << agent << ": " << it->arrival_time << ' ' << it->leaving_time_tail <<'\n' << "agent " << it3->agent_id << ": " << it3->t_min << " " << it3->t_max << '\n';
						return false;
					}
				}
			}
			
		}
	}
	return true;
}
