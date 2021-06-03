#include "PBS.h"

PBS::PBS(Instance& instance): sipp(instance), instance(instance)
{
	std::cout << "pbs initialized\n";
	arrivingVehicles = instance.getAgents();
	std::cout << "vehicle size: " << arrivingVehicles.size() << std::endl;

	for(int i = 0; i < arrivingVehicles.size(); ++i){
        std::cout << arrivingVehicles[i].id << " " << arrivingVehicles[i].name << " " << arrivingVehicles[i].earliest_start_time << "\n";
    }

    std::cout <<"\n\n\n\n";
}

bool PBS::UpdatePlan(PTNode& node, int index)
{
	std::cout << "running updateplan on agent " << index << "\n";
	std::list<int> list = node.topologicalSort();
	list.reverse();

	std::cout << "list size: " << list.size() << std::endl;
	for(auto it = list.begin(); it != list.end(); ++ it){
		std::cout << *it <<" ";
	}
	std::cout << "\n";

	bool replanned = false;
	for(auto it = list.begin(); it != list.end(); ++it){
		//it is the index of agents that will be in the rt
		if(replanned){
			for(auto it2 = node.plan[*it].begin(); it2 != node.plan[*it].end(); ++it2){
				//it2 is the path entries of the agent it
				for(size_t i = index; i < *it; ++i){
					//iterating from index to one before it
					for(auto it3 = node.plan[i].begin(); it3 != node.plan[i].end(); ++it3){
						//it3 is path entries of the agent index
						//if collision
						if(it2->conflict_point == it3->conflict_point){
							if((it2->arrival_time > it3->arrival_time && it2->arrival_time < it3->leaving_time_tail) || (it2->leaving_time_tail > it3->arrival_time && it2->leaving_time_tail < it3->leaving_time_tail) || (it2->arrival_time < it3->arrival_time && it2->leaving_time_tail > it3->leaving_time_tail)){
								std::cout << "$$$$$$$$$replanning\n";
								ReservationTable rt(instance.getNumOfVertices());
								//node.getRT(rt, *it);
								std::set<int> rtp;
								node.getRTP(rtp, *it);;
								node.getRTFromP(instance, rt, rtp);
								Path path = sipp.run(*it, rt);
								if(path.empty()) return false;
								node.plan[*it] = path;
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
			std::cout << "printing rtp set\n";
			for(auto ittemp = rtp.begin(); ittemp != rtp.end(); ++ittemp){
				std::cout<< *ittemp << " ";
			}
			std::cout <<"\n";
			node.getRTFromP(instance, rt, rtp);


			std::cout<<"conflict point size: " << rt.size()<< "\n";
			std::cout<<"rt\n";
			for(int i = 0; i < rt.size(); ++i){
				std::cout<<"conflictpoint: " << i << "\n";
				for(auto ittemp = rt[i].begin(); ittemp != rt[i].end(); ++ittemp){
					std::cout<< ittemp->t_min << " " <<ittemp->t_max << " " << ittemp->agent_id << "\n";
				}
			}
			std::cout<<"rt\n";


			Path path = sipp.run(*it, rt);
			std::cout << "printing path for agent " << index << "\n" ;
			std::cout << "path size: " << path.size() <<"\n";
			for(auto itt = path.begin(); itt != path.end(); ++itt){
				std::cout << itt->conflict_point << " "  << itt->arrival_time << " " << itt->leaving_time_tail << "\n";
			}
			/*
			std::cout << "vertex: " << path[0].vertex << "\n";
			std::cout << "cp: " << path[0].conflict_point << "\n";
			std::cout << "arrival_time: " << path[0].arrival_time << "\n";
			std::cout << "leaving_time_tail: " << path[0].leaving_time_tail << "\n";
			std::cout << "leaving_time_head: " << path[0].leaving_time_head << "\n";
			std::cout << "cost: " << path[0].cost << "\n";
			*/
			if(path.empty()) return false;
			node.plan[index] = path;
			replanned = true;
		}
	}
	return true;
}

void PBS::run(const string& outputFileName)
{
	std::cout << "\n\npbs running" << std::endl;
    std::stack<PTNode> POStack;

    //1 2
    vector<Path> plan;
    for(int i = 0; i < arrivingVehicles.size(); ++i){
    	Path newPath;
    	plan.push_back(newPath);
    }

    std::map<int, std::set<int> > priority;
	PTNode Root = PTNode(plan, priority);
	//3
	for(size_t i = 0; i < plan.size(); ++i)
		if(!UpdatePlan(Root, i)) std::cout << "no solution1\n" ;

	//7
	Root.calculateCost();
	//8
	POStack.push(Root);
	
	//9
	int test = 0;
	while (POStack.size() != 0){
		std::cout << "\n\n\n////////////////////////////\nloop starts\n\n\n";
		//10 11
		PTNode N = POStack.top();
		POStack.pop();
		std::cout << "node cost: "  << N.cost << "\n";
		//use list 12 13 14 agent1 agent2 point
		std::tuple<int, int, int> C = N.getFirstCollision(instance);
		if(std::get<0> (C) == -1){
			N.writeToFile(outputFileName);
			std::cout << "$$FOUND SOLUTION$$\n";
			return;
		}
		//std::cout<<"FOUND CONFLICT!: " << get<0> (C) << " " << get<1> (C) << " " << get<2> (C) << "\n";
		//16 17 19
		std::cout<<"\n$NewNode\n\n";
		std::map<int, std::set<int> > newPriority = N.priority;
		newPriority[std::get<0>(C)].insert(std::get<1>(C));
		PTNode newNode = PTNode(N.plan, newPriority);


		for(auto it = newNode.priority.begin(); it != newNode.priority.end(); ++it){
			std::cout << it->first << ": ";
			for(auto it2 = it->second.begin(); it2 != it->second.end(); ++it2){
				std::cout << *it2 << " ";
			}
			std::cout << "\n";
		}


		//20 21 22
		if(UpdatePlan(newNode, std::get<1>(C))) newNode.calculateCost();

		std::cout<<"\n$NewNode2\n\n";
		//16 17 19
		std::map<int, std::set<int> > newPriority2 = N.priority;
		newPriority2[std::get<1>(C)].insert(std::get<0>(C));
		PTNode newNode2 = PTNode(N.plan, newPriority2);


		for(auto it = newNode2.priority.begin(); it != newNode2.priority.end(); ++it){
			std::cout << it->first << ": ";
			for(auto it2 = it->second.begin(); it2 != it->second.end(); ++it2){
				std::cout << *it2 << " ";
			}
			std::cout << "\n";
		}


		//20 21 22
		if(UpdatePlan(newNode2, std::get<0>(C))) newNode2.calculateCost();



		//*******************************************DEBUG
		std::cout << "$$$$$$$$$$$$$newNode.cost: " << newNode.cost << "\n";
		
		
		std::cout << "$$$$$$$$$$$$$newNode2.cost: " << newNode2.cost << "\n";
		
		//*******************************************DEBUG




		//23 non increasing order
		if(newNode.cost >= newNode2.cost){
			POStack.push(newNode);
			POStack.push(newNode2);
		}
		else{
			POStack.push(newNode2);
			POStack.push(newNode);
		}

		//*******************************************DEBUG
//		test++;
//		if(test == 2) return;
		//*******************************************DEBUG
	}

	std::cout << "no solution2\n";
}

