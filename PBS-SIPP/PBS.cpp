#include "PBS.h"

PBS::PBS(Instance& instance): sipp(instance), instance(instance)
{

}

bool PBS::UpdatePlan(PTNode node, int index)
{
	std::list<int> list = node.topologicalSort();
	bool replanned = false;
	ReservationTable rt(instance.getNumOfVertices());
	for(auto it = list.begin(); it != list.end(); ++it){
		//it is the index of agents that will be in the rt
		if(replanned){
			for(auto it2 = node.plan[*it].begin(); it2 != node.plan[*it].end(); ++it2){
				//it2 is the path entries of the agent it
				for(auto it3 = node.plan[index].begin(); it3 != node.plan[index].end(); ++it3){
					//it3 is path entries of the agent index
					//if collision
					if(it2->vertex == it3->vertex){
						if((it2->arrival_time >= it3->arrival_time && it2->arrival_time <= it3->leaving_time) || (it2->leaving_time >= it3->arrival_time && it2->leaving_time <= it3->leaving_time) || (it2->arrival_time <= it3->arrival_time && it2->leaving_time >=it3->leaving_time)){
							Path path = sipp.run(*it, rt);
							node.plan[*it] = path;
						}
					}
				}
			}
		}
		if(index == *it){
			//get reservation table
			Path path = sipp.run(*it, rt);
			node.plan[index] = path;
			replanned = true;
		}
		for(auto it2 = node.plan[*it].begin(); it2 != node.plan[*it].end(); ++it2){
			//it2 is the path entries
			TimeInterval newTI;
			newTI.t_max = it2->leaving_time;
			newTI.t_min = it2->arrival_time;
			newTI.agent_id = *it;
			rt[it2->vertex].push_back(newTI);
		}		
	}
	return true;
}

void PBS::run()
{
    std::stack<PTNode> POStack;

    //1 2
    vector<Path> plan;
    std::map<int, std::set<int> > priority;
	PTNode Root = PTNode(plan, priority);

	//3
	for(size_t i = 0; i < plan.size(); ++i)
		if(!UpdatePlan(Root, i)) std::cout << "no solution\n" ;
	
	//7
	Root.calculateCost();
	//8
	POStack.push(Root);
	//9
	while (POStack.size() != 0){
		//10 11
		PTNode N = POStack.top();
		POStack.pop();

		//use list 12 13 14 agent1 agent2 point
		std::tuple<int, int, int> C = N.getFirstCollision(instance);
		if(std::get<0>(N.getFirstCollision(instance)) == -1) std::cout << "found solution\n";

		//16 17 19
		std::map<int, std::set<int> > newPriority = N.priority;
		newPriority[std::get<0>(C)].insert(std::get<1>(C));
		PTNode newNode = PTNode(N.plan, newPriority);
		//20 21 22
		if(UpdatePlan(newNode, std::get<1>(C))) newNode.calculateCost();

		//16 17 19
		std::map<int, std::set<int> > newPriority2 = N.priority;
		newPriority2[std::get<1>(C)].insert(std::get<0>(C));
		PTNode newNode2 = PTNode(N.plan, newPriority2);
		//20 21 22
		if(UpdatePlan(newNode2, std::get<0>(C))) newNode2.calculateCost();

		//23 non increasing order
		if(newNode.cost <= newNode2.cost){
			POStack.push(newNode);
			POStack.push(newNode2);
		}
		else{
			POStack.push(newNode2);
			POStack.push(newNode);
		}
	}

	std::cout << "no solution\n";
}


void PBS::writeToFile(const string& file_name)
{

}