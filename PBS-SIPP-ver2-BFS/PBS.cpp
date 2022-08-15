#include <queue>
#include "PBS.h"

PBS::PBS(Instance& instance): sipp(instance), instance(instance)
{
	// std::cout << "pbs initialized\n";
	arrivingVehicles = instance.getAgents();
	initializeAgents(trajectoryToAgent);
	// for(auto it = trajectoryToAgent.begin(); it != trajectoryToAgent.end(); ++it){
	// 	for(auto it2 = it->second.begin(); it2 != it->second.end(); ++it2){
	// 		std::cout << it->first << " " << it2->first <<": \n";
	// 		for(auto it3 = it2->second.begin(); it3 != it2->second.end(); ++it3){
	// 			std::cout << *it3 <<" ";
	// 		}
	// 		std::cout << "\n";
	// 	}
	// }
	// std::cout << "vehicle size: " << arrivingVehicles.size() << std::endl;

	// for(int i = 0; i < (signed)arrivingVehicles.size(); ++i){
    //     std::cout << arrivingVehicles[i].id << " " << arrivingVehicles[i].name << " " << arrivingVehicles[i].earliest_start_time << "\n";
    // }

    // std::cout <<"\n\n";
}

bool PBS::UpdatePlan(PTNode& node, int index)
{
	// std::cout << "running updateplan on agent " << index << "\n";
	std::list<int> list = node.topologicalSort(index);
	// std::list<int> list = node.topologicalSort();
	list.reverse();

	
	// std::cout << "List: ";
	// for(auto it = list.begin(); it != list.end(); ++ it){
	// 	std::cout << *it <<" ";
	// }
	// std::cout << "\n";
	


	bool replanned = false;
	for(auto it = list.begin(); it != list.end(); ++it){
		//it is the index of agents in list
		if(replanned){
			for(auto it2 = list.begin(); it2 != list.end(); ++it2){
				if (*it2 == *it){
					break;
				}
				// std::cout << *it<< "--" << *it2 << " ";
				for(auto it3 = node.plan[*it].begin(); it3 != node.plan[*it].end(); ++it3){
					//it2 is the path entries of the agent it

					for(auto it4 = node.plan[*it2].begin(); it4 != node.plan[*it2].end(); ++it4){
						//it3 is the path entries of the agent index
						//std::cout << it2->conflict_point << " " << it3->conflict_point << "\n";

						// std::cout << "it2: " << it2->conflict_point <<" "<<it2->arrival_time<<" "<<it2->leaving_time_tail<<"\n";
						// std::cout << "it3: " << it3->conflict_point << " "<< it3->arrival_time<<" "<<it3->leaving_time_tail<<"\n";
						if( instance.isSamePoint(it3->conflict_point, it4->conflict_point)){
							// std::cout << "it2: " << it2->arrival_time <<" "<< it2->leaving_time_tail <<"\n";
							// std::cout << "it3: " << it3->arrival_time << " "<<it3->leaving_time_tail <<"\n";
							// std::cout << "check1: "<< (it2->arrival_time - it3->arrival_time > EPSILON &&  it3->leaving_time_tail - it2->arrival_time > EPSILON) << "\n";
							// std::cout << "check2: "<< (it2->leaving_time_tail - it3->arrival_time > EPSILON &&  it3->leaving_time_tail - it2->leaving_time_tail > EPSILON) << "\n";
							// std::cout << "check3: "<< (it3->arrival_time - it2->arrival_time > EPSILON && it2->leaving_time_tail - it3->leaving_time_tail > EPSILON) << "\n";
							if(!(it3->leaving_time_tail - it4->arrival_time < EPSILON) && !(it4->leaving_time_tail - it3->arrival_time < EPSILON)){
								/*std::cout << "check 1: " <<(it2->arrival_time - it3->arrival_time > EPSILON &&  it3->leaving_time_tail - it2->arrival_time > EPSILON) << "\n";
								std::cout << "check 2: " <<(it2->leaving_time_tail - it3->arrival_time > EPSILON &&  it3->leaving_time_tail - it2->leaving_time_tail > EPSILON) << "\n";
								std::cout << "check 3: " <<(it3->arrival_time - it2->arrival_time > EPSILON && it2->leaving_time_tail - it3->leaving_time_tail > EPSILON) << "\n";
								std::cout <<"$\n$\n" << it2->arrival_time << " " << it2->leaving_time_tail << " " << it3->arrival_time << " " << it3->leaving_time_tail << "\n$\n$\n";*/
								//std::cout<<"FOUND CONFLICT!: " << *it2 << " " << *it << " " << it3->conflict_point << "\n";
								// std::cout << "\n$$$$$$$$$replanning\n";
								ReservationTable rt(instance.getNumOfVertices());
								//node.getRT(rt, *it);
								std::set<int> rtp;
								node.getRTP(rtp, *it);;
								node.getRTFromP(instance, rt, rtp, *it, trajectoryToAgent);
								// if (*it == 25 ){
								// 	printRT(rt);
								// }

								Path path = sipp.run(*it, rt);
								if(path.empty()) return false;
								node.plan[*it] = path;



								ReservationTable rtdebug(instance.getNumOfVertices());
								node.getRTFromP(instance, rtdebug, rtp, *it, trajectoryToAgent);
								if(!checkValid(rtdebug, path, *it)) {
									std::fstream outputRT("ReservationTable.txt", std::fstream::in |
					                            std::fstream::out |
					                            std::fstream::trunc |
					                            std::fstream::binary );
									for(auto itt = rt.begin(); itt != rt.end(); ++itt){
										for(auto itt2 = itt->begin(); itt2 != itt->end(); ++itt2){
											// std::cout<<"writting!\n";
											outputRT.write((char*)&itt2->t_min, sizeof(itt2->t_min));
											outputRT << ',';
											outputRT.write((char*)&itt2->t_max, sizeof(itt2->t_max));
											outputRT << ',' << itt2->agent_id <<';';
										}
										outputRT<<'\n';
									}
									outputRT.close();
									std::cout << "replanned agent " << *it << '\n';
									std::cout << "rtp: ";
									for(auto itt = rtp.begin(); itt != rtp.end(); ++ itt){
										std::cout << *itt <<" ";
									}
									printRT(rt);
									std::cout <<"agent " <<*it <<'\n';
									printPath(path);
									return false;
								}









							}
						}
					}
				}
			}
		}
		if(index == *it){
			ReservationTable rt(instance.getNumOfVertices());
			std::set<int> rtp;
			node.getRTP(rtp, *it);;
			//node.getRT(rt, *it);
			
			// std::cout << "printing rtp set\n";
			// for(auto ittemp = rtp.begin(); ittemp != rtp.end(); ++ittemp){
			// 	std::cout<< *ittemp << " ";
			// }
			// std::cout <<"\n";
			// std::cout <<"agent" << *it<<"\n";
			node.getRTFromP(instance, rt, rtp, *it, trajectoryToAgent);
			// printRT(rt);
			Path path = sipp.run(*it, rt);
			// std::cout << "AFTER: printing path for agent " << index << "\n" ;

			//printRT(rt);
			// printPath(path);

			if(path.empty()) return false;
			node.plan[index] = path;
			replanned = true;


			ReservationTable rtdebug(instance.getNumOfVertices());
			node.getRTFromP(instance, rtdebug, rtp, *it, trajectoryToAgent);
			if(!checkValid(rtdebug, path, *it)) {
				std::fstream outputRT("ReservationTable.txt", std::ios::out | std::ios::binary);
				for(auto itt = rt.begin(); itt != rt.end(); ++itt){
					for(auto itt2 = itt->begin(); itt2 != itt->end(); ++itt2){
						outputRT.write((char*)&itt2->t_min, sizeof(itt2->t_min));
						outputRT.write((char*)&itt2->t_max, sizeof(itt2->t_max));
						outputRT.write((char*)&itt2->agent_id, sizeof(itt2->agent_id));
					}
					char space = ' ';
					outputRT.write((char*)& space, sizeof(space));
				}
				outputRT.close();
				std::cout << "List: ";
				for(auto it = list.begin(); it != list.end(); ++ it){
					std::cout << *it <<" ";
				}
				printRT(rt);
				std::cout <<"agent " <<*it <<'\n';
				printPath(path);
				return false;
			}
		}
	}
	return true;
}


struct PTNComparator{
	bool operator()(const PTNode ptn1, const PTNode ptn2) const
	{
		return ptn1.cost > ptn2.cost;
	}
};

void PBS::run(const string& outputFileName)
{
	// std::cout << "\n\npbs running" << std::endl;
    std::priority_queue<PTNode, std::vector<PTNode>, PTNComparator> POQueue;

    //1 2
    vector<Path> plan;
    for(int i = 0; i < (signed) arrivingVehicles.size(); ++i){
    	Path newPath;
    	plan.push_back(newPath);
    }

    std::map<int, std::set<int> > priority;
    initializePriority(priority, trajectoryToAgent);
    // printPriority(priority);
	PTNode Root = PTNode(plan, priority);
	//initialize priority

	//3
	for(size_t i = 0; i < plan.size(); ++i)
		if(!UpdatePlan(Root, i)) std::cout << "no solution1\n" ;

	//7
	Root.calculateCost();
	//8
	POQueue.push(Root);
	
	//9
	int test = 0;
	while (POQueue.size() != 0){
		// try
		// {
		// 	for (int printTest = 0; printTest < POQueue.top().plan.size(); ++printTest){
		// 		std::ofstream file;
		// 		file.open("output.txt", std::ios::app);
		// 		if(file.is_open()){
		// 			file << "agent: " << printTest <<";   ";
		// 			for(int i = 0; i < (signed)POQueue.top().plan[printTest].size(); ++i){
		// 				file << "cp" <<  POQueue.top().plan[printTest][i].conflict_point  << ";" << POQueue.top().plan[printTest][i].arrival_time << ";" << POQueue.top().plan[printTest][i].leaving_time_tail;
		// 				file << ";   ";
		// 			}
		// 			file << "\n";
		// 		}
		// 		else std::cout << "unable to open file";
		// 		file.close();

		// 	}
		// }
		// catch(const std::exception& e)
		// {
		// 	std::cerr << e.what() << '\n';
		// }

		// std::cout << "\n\n\n////////////////////////////\nloop starts\n\n\n";
		//10 11
		PTNode N = POQueue.top();
		POQueue.pop();

		
		// for(int i = 0; i < N.plan.size(); ++i){
        //     // if (i == 16 || i == 19){
        //         std::cout  <<  "agent: " << i << "\n";
        //         printPath(N.plan[i]);
        //     // }
		// }
		// printPriority(N.priority);
		// std::cout << "node cost: "  << N.cost << "\n";
		//use list 12 13 14 agent1 agent2 point
		std::tuple<int, int, int> C = N.getFirstCollision(instance);
		if(std::get<0> (C) == -1){
			N.writeToFile(instance, outputFileName);
			// std::cout << "$$FOUND SOLUTION$$\n";
			
			return;
		}
        // printPriority(N.priority);
        // std::cout<<"FOUND CONFLICT!: " << get<0> (C) << " " << get<1> (C) << " " << get<2> (C) << " " << instance.getConflictPoints(get<2> (C))[0]  <<  "\n";
		if( (N.priority[get<0> (C)].find(get<1> (C)) != N.priority[get<0> (C)].end()) || (N.priority[get<1> (C)].find(get<0> (C)) != N.priority[get<1> (C)].end())){
			std::cout <<"!ERROR on iteration " << test << "\n";
            // std::cout<<"FOUND CONFLICT!: " << get<0> (C) << " " << get<1> (C) << " " << get<2> (C) << " " << instance.getConflictPoints(get<2> (C))[0]  <<  "\n";
            // printPriority(N.priority);
			// for(int i = 0; i < N.plan.size(); ++i){
			// 	std::cout  <<  "agent: " << i << "\n";
			// 	// printPath(N.plan[i]);
			// }
			return;
		}
		//16 17 19
		// std::cout<<"\n$NewNode\n\n";
		std::map<int, std::set<int> > newPriority = N.priority;
		newPriority[std::get<0>(C)].insert(std::get<1>(C));
		PTNode newNode = PTNode(N.plan, newPriority);

		//rintPriority(newNode.priority);

		bool n1 = true;
		bool n2 = true;


		//20 21 22
		if(UpdatePlan(newNode, std::get<1>(C))) newNode.calculateCost();
		else n1 = false;

		


		// std::cout<<"\n$NewNode2\n\n";
		//16 17 19
		std::map<int, std::set<int> > newPriority2 = N.priority;
		newPriority2[std::get<1>(C)].insert(std::get<0>(C));
		PTNode newNode2 = PTNode(N.plan, newPriority2);


		//printPriority(newNode2.priority);


		//20 21 22
		if(UpdatePlan(newNode2, std::get<0>(C))) newNode2.calculateCost();
		else n2 = false;

		// for(int i = 0; i < newNode2.plan.size(); ++i){
		// 	std::cout  <<  "agent: " << i << "\n";
		// 	printPath(newNode2.plan[i]);
		// }


		//*******************************************DEBUG
		// std::cout << "$$$$$$$$$$$$$newNode.cost: " << newNode.cost << "\n";
		
		
		// std::cout << "$$$$$$$$$$$$$newNode2.cost: " << newNode2.cost << "\n";
		
		//*******************************************DEBUG
		// std::ofstream file;
		// file.open("output.txt", std::ios::app);
		// if(file.is_open()){
		// 	file << "newNode.cost: " << newNode.cost << "    newNode2.cost: " << newNode2.cost << "\n";
		// }
		// else file << "unable to open file";
		// file.close();


		if(n1 && n2){
			if(newNode.cost >= newNode2.cost){
				POQueue.push(newNode);
				POQueue.push(newNode2);
			}
			else{
				POQueue.push(newNode2);
				POQueue.push(newNode);
			}
		}
		else if(n1) POQueue.push(newNode);
		else if (n2) POQueue.push(newNode2);
        else return;
		//23 non increasing order0
		
		//*******************************************DEBUG
		test++;
		// if(test == 1) return;
		//*******************************************DEBUG
	}

	std::cout << "no solution2\n";
}

void PBS::initializePriority(std::map<int, std::set<int> >& p,std::map<int, std::map<int, std::vector<int> > >& ta){

	for(auto it = trajectoryToAgent.begin(); it != trajectoryToAgent.end(); ++it){
		for(auto it2 = it->second.begin(); it2 != it->second.end(); ++it2){
			for(auto it3 = it2->second.begin(); it3 != it2->second.end(); ++it3){
				for(auto it4 = next(it3); it4 != it2->second.end(); ++it4){
                    if (instance.getAgents()[*it3].earliest_start_time < instance.getAgents()[*it4].earliest_start_time){
                        p[*it3].insert(*it4);
                    }
				}


                int conflictPointID = instance.getConflictPoints(it->first)[0];
                for(auto it5 = trajectoryToAgent.begin(); it5 != trajectoryToAgent.end(); ++it5){
                    if (conflictPointID == it5->first){
                        // std::cout <<  conflictPointID << "--" << it5->first << std::endl;
                        for(auto it6 = it5->second.begin(); it6 != it5->second.end(); ++it6){
                            for(auto it7 = it6->second.begin(); it7 != it6->second.end(); ++it7){
                                // std::cout << "agent: " << *it3 << "-" << *it7 << std::endl;
                                if (instance.getAgents()[*it3].earliest_start_time < instance.getAgents()[*it7].earliest_start_time){
                                    p[*it3].insert(*it7);
                                }
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