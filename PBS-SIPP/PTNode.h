#pragma once
#include <iostream>
#include <fstream>
#include <stack>
#include <set>
#include <tuple>
#include <vector>
#include "Common.h"
#include "Instance.h"


class PTNode{
public:
	vector<Path> plan; //plan is vector of paths	size of vector = size of vehicles
	std::map<int, std::set<int> > priority; //map of index with set of lower prio
	double cost;

	PTNode();
	PTNode(vector<Path>, std::map<int, std::set<int> >);
	void writeToFile(const string&);	
	void calculateCost();
	void topologicalSortUtil(int, bool*, std::list<int>&);
	std::list<int> topologicalSort();
	std::tuple<int, int, int> getFirstCollision(Instance&);
	//void getRT(ReservationTable&, int);
	void getRTP(std::set<int>&, int);
	void getRTFromP(ReservationTable, std::set<int>);
};

