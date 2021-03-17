#include "PTNode.h"

PTNode::PTNode(vector<Path> pl, std::map<int, std::set<int> > pr){
	plan = pl;
	priority = pr;
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
	double c = 0;
	for(auto it = plan.begin(); it != plan.end(); ++it){
		c += it->back().leaving_time;
	}
	cost = c;
}

std::tuple<int, int, int> PTNode::getFirstCollision(Instance& instance){
	//todo
	std::tuple<int, int, int> result = std::make_tuple(-1, -1, -1);;
	ReservationTable table(instance.getNumOfVertices());
	//vector<list<TimeInterval> >
	
	//iterating path in plan
	int aid = 0;
	for(auto it = plan.begin(); it != plan.end(); ++it, ++aid){
		//iterating points in path
		for(auto it2 = it->begin(); it2 != it->end(); ++it2){
			std::list<TimeInterval> vertexTimeTable = table[it2->vertex];
			for(auto it3 = vertexTimeTable.begin(); it3 != vertexTimeTable.end(); ++it3){
				//if  collision
				if((it2->arrival_time >= it3->t_min && it2->arrival_time <= it3->t_max)
					|| (it2->leaving_time >= it3->t_min && it2->leaving_time <= it3->t_max) || (it2->arrival_time <= it3->t_min && it2->leaving_time >= it3->t_max)){
					result = std::make_tuple(aid, it3->agent_id, it2->vertex);
					break;
				}
			}
		}
	}
	
	return result;
}
