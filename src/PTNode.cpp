#include "PTNode.h"
#include <fstream>

PTNode::PTNode(vector<Path> pl, std::map<int, std::set<int> > pr){
	plan = pl;
	priority = pr;
}

void PTNode::writeToFile(const string& file_name)
{
	std::ofstream file;
	file.open(file_name);
	if(file.is_open()){
		for(auto agent = plan.begin(); agent != plan.end(); ++agent){
			file <<  (*agent)[0].conflict_point << ";" << (*agent)[0].arrival_time << '\n';
		}
	}
	else std::cout << "unable to open file";
	file.close();
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
    for (int i = 0; i < plan.size(); ++i)
        visited[i] = false;
 
    // Call the recursive helper function
    // to store Topological
    // Sort starting from all
    // vertices one by one
    for (int i = 0; i < plan.size(); ++i)
        if (visited[i] == false)
            topologicalSortUtil(i, visited, List);
 
	return List;
}

void PTNode::calculateCost(){
	std::cout << "calculate cost running\n";
	double c = 0;
	for(auto it = plan.begin(); it != plan.end(); ++it){
		c += it->back().leaving_time_tail;
	}
	cost = c;
}


std::tuple<int, int, int> PTNode::getFirstCollision(Instance& instance){
	//todo
	std::cout << "running getFirstCollision555\n";
	std::tuple<int, int, int> result = std::make_tuple(-1, -1, -1);
	ReservationTable table(instance.getNumOfVertices());
	//vector<list<TimeInterval> >
	std::set<int> s;
	for(int i = 0; i < plan.size();++i) s.insert(i);
	getRTFromP(instance, table, s);
/*
	std::cout<<"agents size: " << plan.size()<< "\n";
	std::cout<<"rt\n";
	for(int i = 0; i < table.size(); ++i){
		std::cout<<"conflictpoint: " << i << "\n";
		for(auto it = table[i].begin(); it != table[i].end(); ++it){
			std::cout<< it->t_min << " " <<it->t_max << " " << it->agent_id << "\n";
		}
	}
	std::cout<<"rt\n";
	*/
	
	//iterating path in plan vector<vector<PathEntry>>
	int aid = 0;
	for(auto it = plan.begin(); it != plan.end(); ++it, ++aid){
		//iterating points in path
		for(auto it2 = it->begin(); it2 != it->end(); ++it2){
			//it2 is path entry
			std::list<TimeInterval> vertexTimeTable = table[it2->conflict_point];
			for(auto it3 = vertexTimeTable.begin(); it3 != vertexTimeTable.end(); ++it3){
				if(aid == it3->agent_id) {
					std::cout << "checkingids "<<aid << " " << it3->agent_id <<"\n";
					continue;
				}
				//if  collision
				if((it2->arrival_time > it3->t_min && it2->arrival_time < it3->t_max)
					|| (it2->leaving_time_tail > it3->t_min && it2->leaving_time_tail < it3->t_max) 
					|| (it2->arrival_time < it3->t_min && it2->leaving_time_tail > it3->t_max)){
					/*
					std::cout << "check 1: " <<(it2->arrival_time > it3->t_min && it2->arrival_time < it3->t_max) << "\n";
					std::cout << "check 2: " <<(it2->leaving_time_tail > it3->t_min && it2->leaving_time_tail < it3->t_max) << "\n";
					std::cout << "check 3: " <<(it2->arrival_time < it3->t_min && it2->leaving_time_tail > it3->t_max) << "\n";
					std::cout <<"$\n$\n$\n" << it2->arrival_time << " " << it2->leaving_time_tail << " " << it3->t_min << " " << it3->t_max << "\n$\n$\n$\n";
					std::cout<<"FOUND CONFLICT!: " << aid << " " << it3->agent_id << " " << it2->conflict_point << "\n";
					*/
					result = std::make_tuple(aid, it3->agent_id, it2->conflict_point);
					
					return result;
				}
			}
		}
	}

	//std::cout << std::get<0>(result) << " " << std::get<1>(result) << " " << std::get<2>(result) << "\n";
	return result;
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
	for (int i = 0; i < plan.size(); ++i){
		if(p.find(i) == p.end()){ //not already in the list
			if(priority[i].find(index) != priority[i].end()){
				p.insert(i);
				getRTP(p, i);
			}
		}
	}
}

void PTNode::getRTFromP(Instance& instance , ReservationTable& rt, std::set<int> p){
	std::cout<<"running getRTFromP\n";
	for(auto it = p.begin(); it != p.end(); ++it){
		for(auto it2 = plan[*it].begin(); it2 != plan[*it].end(); ++it2){
			//plan: vector<vector<pathentry>>
				TimeInterval newTI;
				
				std::vector<int> CPs = instance.getConflictPoints(it2->conflict_point);
				newTI.t_max = it2->leaving_time_tail;
				newTI.t_min = it2->arrival_time;
				newTI.agent_id = *it;
				rt[it2->conflict_point].push_back(newTI);
				std::cout << it2->conflict_point << " conflict with ";
				for(auto it3 = CPs.begin(); it3 != CPs.end(); ++it3){
					std::cout <<*it3 << " ";
					rt[*it3].push_back(newTI);
				}
				std::cout << "\n";
		}
	}
	//std::cout<<"rtsize: " << rt.size() << "\n";
}