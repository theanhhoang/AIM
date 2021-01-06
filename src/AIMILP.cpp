#include <iostream>
#include <fstream>
#include <string>
#include "AIMILP.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <cstdio>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <random>
#include <ilcplex/ilocplex.h>

float turnRadius = 100;
float vehicleLength = 50;

float w = 40;

position_t nodeAsPos(const float x, const float y)
{  
    return position_t(x,y);
}

void loadSearchGraph(
    searchGraph_t& searchGraph,
    std::unordered_map<std::string, vertex_t>& vNameToV,
    std::unordered_map<std::string, vertex_t>& vNameToDirection,
    std::unordered_map<std::string, edge_t>& eNameToE,
    const std::string& fileName, 
    rapidjson::Document& pairDistances,
    const std::string& pDFileName)
{

    FILE* fp = fopen(fileName.c_str(), "r"); // non-Windows use "r"
    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    rapidjson::Document doc;
    doc.ParseStream(is);
    fclose(fp);

    
    // add vertices
    for (rapidjson::Value::ConstValueIterator itr = doc["vertices"].Begin();
                                             itr != doc["vertices"].End(); ++itr) {
        if (itr->HasMember("name")) {
            position_t pos = nodeAsPos((*itr)["pos"][0].GetDouble(),(*itr)["pos"][1].GetDouble());
            std::string name = (*itr)["name"].GetString();
            auto v = boost::add_vertex(searchGraph);
            searchGraph[v].name = name;
            searchGraph[v].pos = pos;
            vNameToV[name] = v;

            if ( name.find("EW")!= std::string::npos | 
                    name.find("WE")!= std::string::npos | 
                    name.find("NS")!= std::string::npos | 
                    name.find("SN")!= std::string::npos ) 
            {
                // 1 mean straight direction
                vNameToDirection[std::to_string(v)] = 1;
            }else
            {
                // 0 mean curve direction
                vNameToDirection[std::to_string(v)] = 0;
            }
            
            
        }
    }

    //add vertex conflicts
    for (rapidjson::Value::ConstValueIterator itr = doc["vertices"].Begin();
                                             itr != doc["vertices"].End(); ++itr) {
        if (itr->HasMember("name")) {
            std::string name = (*itr)["name"].GetString();
            auto iter = vNameToV.find(name);
            vertex_t v = iter->second;

            for (rapidjson::Value::ConstValueIterator node = (*itr)["vertex_conflicts"].Begin();
                                             node != (*itr)["vertex_conflicts"].End(); ++node) 
            {
                std::string cName = node->GetString();
                auto cIter = vNameToV.find(cName);
                if (cIter == vNameToV.end()) 
                {
                    std::cerr << "(2) ERROR: Could not find vertex " << cName << std::endl;
                    continue;
                }
                vertex_t u = cIter->second;
                searchGraph[v].generalizedVertexConflicts.insert(u);
            }
        }
    }

    //add edge
    for (rapidjson::Value::ConstValueIterator itr = doc["edges"].Begin();
                                             itr != doc["edges"].End(); ++itr) {
        if (itr->HasMember("name")) {
            std::string name = (*itr)["name"].GetString();
            std::string fromName = (*itr)["fro"].GetString();
            std::string toName = (*itr)["to"].GetString();

            auto fromIter = vNameToV.find(fromName);
            auto toIter = vNameToV.find(toName);
            if (fromIter == vNameToV.end()
                || toIter == vNameToV.end() 
                || fromIter->second == toIter->second) {
                std::cerr << "invalid edge! " << name << std::endl;
                continue;
            }
            auto e = boost::add_edge(fromIter->second, toIter->second, searchGraph);
            searchGraph[e.first].name = name;
            searchGraph[e.first].length = (*itr)["value"].GetDouble();

            eNameToE[name] = e.first;
        }
    }

    //read pair distance
    FILE* pdfile = fopen(pDFileName.c_str(), "r"); // non-Windows use "r"
    char readBuffer2[65536];
    rapidjson::FileReadStream is2(fp, readBuffer2, sizeof(readBuffer2));
    pairDistances.ParseStream(is2);
    fclose(pdfile);
}


float Li(int direction){
    if (direction == 1){
        // std::cout << "direction: " << direction << "-" << vehicleLength << std::endl; 
        return vehicleLength;
    }
    else if (direction == 0)
    {
        // std::cout << "direction: " << direction << "-" << 4*turnRadius*asin(vehicleLength/(2*turnRadius)) << std::endl; 
        return 4*turnRadius*asin(vehicleLength/(2*turnRadius));
    }
    return 0;
}



// template<typename K, typename V>
// void print_map(std::unordered_map<K,V> const &m)
// {
//     for (auto const& pair: m) {
//         std::cout << "{" << pair.first << ": " << pair.second << "}\n";
//     }
// }

int main(int argc, char *argv[])
{
	size_t numV = 40;

    std::string fileName = "/media/zijun/Data/Documents/2020Summer/AIM/AIMILP/graph.json";
    std::string pDFileName = "/media/zijun/Data/Documents/2020Summer/AIM/AIMILP/pairDistance.json";
    searchGraph_t searchGraph;
    std::unordered_map<std::string, vertex_t> vNameToV;

    std::unordered_map<std::string, vertex_t> vNameToDirection;

    std::unordered_map<std::string, edge_t> eNameToE;
    std::vector<std::vector<vertex_t>> vehicles;
    double e[numV];
    rapidjson::Document pairDistances;
    //e = time i reach the intersection
    loadSearchGraph(searchGraph, vNameToV, vNameToDirection, eNameToE, fileName, pairDistances, pDFileName);

    //pairDistances[vehicles[i].front()][vehicles[i].back()];

    // print_map(vNameToDirection);

    //random generator vehicles
    std::string straight[8][8] = {
    	{"WEL_0", "WEL_1", "WEL_2", "WEL_3", "WEL_4", "WEL_5", "WEL_6", "WEL_7"},
		{"WER_0", "WER_1", "WER_2", "WER_3", "WER_4", "WER_5", "WER_6", "WER_7"},
    	{"SNL_0", "SNL_1", "SNL_2", "SNL_3", "SNL_4", "SNL_5", "SNL_6", "SNL_7"},
    	{"SNR_0", "SNR_1", "SNR_2", "SNR_3", "SNR_4", "SNR_5", "SNR_6", "SNR_7"},
    	{"EWL_0", "EWL_1", "EWL_2", "EWL_3", "EWL_4", "EWL_5", "EWL_6", "EWL_7"},
    	{"EWR_0", "EWR_1", "EWR_2", "EWR_3", "EWR_4", "EWR_5", "EWR_6", "EWR_7"},
    	{"NSL_0", "NSL_1", "NSL_2", "NSL_3", "NSL_4", "NSL_5", "NSL_6", "NSL_7"},
    	{"NSR_0", "NSR_1", "NSR_2", "NSR_3", "NSR_4", "NSR_5", "NSR_6", "NSR_7"}
    };

    std::string left[4][8] = {
    	{"WNL_0", "WNL_1", "WNL_2", "WNL_3", "WNL_4", "WNL_5", "WNL_6", "WNL_7"},
   		{"SWL_0", "SWL_1", "SWL_2", "SWL_3", "SWL_4", "SWL_5", "SWL_6", "SWL_7"},
    	{"ESL_0", "ESL_1", "ESL_2", "ESL_3", "ESL_4", "ESL_5", "ESL_6", "ESL_7"},
    	{"NEL_0", "NEL_1", "NEL_2", "NEL_3", "NEL_4", "NEL_5", "NEL_6", "NEL_7"}
    };
    
    std::string right[4][2] = {
    	{"WSR_0", "WSR_1"},
	    {"SER_0", "SER_1"},
	    {"ENR_0", "ENR_1"},
	    {"NWR_0", "NWR_1"}
    };


    
    
    srand(time(0));
    std::default_random_engine generator(time(0));
    std::poisson_distribution<int> distribution(1.0);
    double time = 0;
    for(size_t i = 0; i < numV; ++i){
    	double arrival = distribution(generator);
    	
    	time += arrival;
    	e[i] = time;

    	std::vector<vertex_t> v;
    	int p = rand() % 40;
    	if(p < 32){ //straight
    		int index = p/4;
    		for(size_t pointIndex = 0; pointIndex < 8; ++pointIndex)
    			v.push_back(vNameToV.find(straight[index][pointIndex])->second);
    	}
    	else if(p < 36){ //left
    		int index = p - 32;
    		for(size_t pointIndex = 0; pointIndex < 8; ++pointIndex)
    			v.push_back(vNameToV.find(left[index][pointIndex])->second);
    	}
    	else if(p < 40){ //left
    		int index = p - 36;
    		for(size_t pointIndex = 0; pointIndex < 2; ++pointIndex)
    			v.push_back(vNameToV.find(left[index][pointIndex])->second);
    	}
    	vehicles.push_back(v);
    }


    //std::poisson_distribution<> d();
    //initialize vehicles
/*
    std::vector<vertex_t> v0, v1, v2, v3;
    v0.push_back(vNameToV.find("WSR_0")->second);
    v0.push_back(vNameToV.find("WSR_1")->second);
    
    v1.push_back(vNameToV.find("WER_0")->second);
    v1.push_back(vNameToV.find("WER_1")->second);
    v1.push_back(vNameToV.find("WER_2")->second);
    v1.push_back(vNameToV.find("WER_3")->second);
    v1.push_back(vNameToV.find("WER_4")->second);
    v1.push_back(vNameToV.find("WER_5")->second);
    v1.push_back(vNameToV.find("WER_6")->second);
    v1.push_back(vNameToV.find("WER_7")->second);
    

    v2.push_back(vNameToV.find("NSR_0")->second);
    v2.push_back(vNameToV.find("NSR_1")->second);
    v2.push_back(vNameToV.find("NSR_2")->second);
    v2.push_back(vNameToV.find("NSR_3")->second);
    v2.push_back(vNameToV.find("NSR_4")->second);
    v2.push_back(vNameToV.find("NSR_5")->second);
    v2.push_back(vNameToV.find("NSR_6")->second);
    v2.push_back(vNameToV.find("NSR_7")->second);

    v3.push_back(vNameToV.find("WSR_0")->second);
    v3.push_back(vNameToV.find("WSR_1")->second);

    vehicles.push_back(v0);
    vehicles.push_back(v1);
    vehicles.push_back(v2);
    vehicles.push_back(v3);
*/
/*
    std::set<vertex_t> conflicts = searchGraph[vehicles[0][0]].generalizedVertexConflicts;
    for(auto it = conflicts.begin(); it != conflicts.end(); ++it){
        std::cout << searchGraph[*it].name << "\n";
    }
    */
/*
    std::cout << "then" << "\n";
    std::cout << pairDistances[searchGraph[vehicles[0].front()].name.c_str()][searchGraph[vehicles[0].back()].name.c_str()].GetDouble() << "\n";
    std::cout << "now" << "\n";
*/
    //std::cout<< searchGraph[vNameToV.find("WSR_1")->second].name;

/*
    auto neighbours = boost::adjacent_vertices(starts[0], searchGraph);
    std::cout << starts[0] << "\n";
    for (auto vd : make_iterator_range(neighbours))
    std::cout <<  vd << "\n";
*/


    double minSpeed = 1;
    double maxSpeed = 500;
    //double e[4] = {1,2,3,4};
    double Mij[vehicles.size()][vehicles.size()];

// Mij is a large positive constant
    for(int i = 0; i < vehicles.size(); ++i)
        for(int j = 0; j < vehicles.size(); ++j)
            Mij[i][j] = 10000;

    IloEnv env = IloEnv();
    IloModel model = IloModel(env);
    IloExpr sum_obj = IloExpr(env);
    IloNumVarArray var(env);
    IloRangeArray con(env);

/*variables: t(r-) = time arrive at point r-; T(r-)= time path through at r-;
                        di(c1,c2) distance between two points c1 and c2;
                        U1 min speed, U2 max speed; sigmaij(c):whether ij conflict at c
*/
    //std::unordered_map<std::string, vertex_t> vNameToV;
    int maxNode = 1000000;
    int offset = 0;
    // std::cout << "\n\n var1: " << var << "\n\n";
    for(int i = 0; i < vehicles.size(); ++i){//for each vehicle
        for(int j = 0; j < vehicles[i].size(); ++j){
            var.add(IloNumVar(env, 0, maxNode, ILOFLOAT));  //t
            offset += 1;
        }
    }


    for(int i = 0; i < vehicles.size(); ++i)
        for(int j = 0; j < vehicles.size(); ++j)
            var.add(IloNumVar(env, 0, 1, ILOINT));



    //15: Objective function
    std::cout << "15\n";
    for(int i = 0, c = 0; i < vehicles.size(); c += vehicles[i].size(), ++i){
        int last = vehicles[i].size() - 1;
        sum_obj += var[c + last];


        int direction = vNameToDirection.find(std::to_string(vehicles[i][last]))->second; 

        // add \Tau
        IloExpr Tau = Li(direction)/w + (var[c + last ] - var[c])*Li(direction)/(pairDistances[searchGraph[vehicles[i].front()].name.c_str()][searchGraph[vehicles[i].back()].name.c_str()].GetDouble());
        sum_obj += Tau;
        
    }  
    //std::cout << sum_obj << "\n";
    model.add(IloMinimize(env, sum_obj));



    //16
    std::cout << "16\n";
    for(int i = 0, c = 0; i < vehicles.size(); c += vehicles[i].size(), ++i){
        con.add(var[c] >= e[i]);
    }
    // std::cout << con << "\n";


    // //17
    std::cout << "17\n";
    for(int i = 0, ci = 0; i < vehicles.size(); ci += vehicles[i].size(), ++i){
        int last = vehicles[i].size() - 1;
        for(int j = 0, cj = 0; j < vehicles.size(); cj += vehicles[j].size(), ++j){
            if(i != j && vehicles[i].front() == vehicles[j].front() && e[i] < e[j]){

                // Tau of entry conflict point
                int direction = vNameToDirection.find(std::to_string(vehicles[i][0]))->second; 
                IloExpr Tau = Li(direction)/w + (var[ci + last ] - var[ci])*Li(direction)/(pairDistances[searchGraph[vehicles[i].front()].name.c_str()][searchGraph[vehicles[i].back()].name.c_str()].GetDouble());
                con.add(var[ci] 
                      + Tau
                      - var[cj] <= 0);
            }
        }
    }
    // std::cout << con << "\n";


    // //18 without w and Li (buggggggggggggggg)
    // std::cout << "18\n";
    // for(int i = 0, c = 0; i < vehicles.size(); c += vehicles[i].size(), ++i){
    //     int last = vehicles[i].size() - 1;
    //     for(int j = 0; j < vehicles[i].size(); ++j){
    //         con.add(
    //             (var[c + last ] - var[c])*Li()
    //             - (pairDistances[searchGraph[vehicles[i].front()].name.c_str()][searchGraph[vehicles[i].back()].name.c_str()].GetDouble())
    //             * (var[c + j + 1] -Li()/w) == 0);
    //     }
    // }
    // std::cout << con << "\n";

    //19
    std::cout << "19\n";
    for(int i = 0, c = 0; i < vehicles.size(); c += vehicles[i].size(), ++i){
        int last = vehicles[i].size() - 1;
        con.add(pairDistances[searchGraph[vehicles[i].front()].name.c_str()][searchGraph[vehicles[i].back()].name.c_str()].GetDouble()/maxSpeed <= var[c + last ] - var[c]);
        con.add(pairDistances[searchGraph[vehicles[i].front()].name.c_str()][searchGraph[vehicles[i].back()].name.c_str()].GetDouble()/minSpeed >= var[c + last ] - var[c]);
    }
    // std::cout << con << "\n";

    // //20
    std::cout << "20\n";
    for(int i = 0, c = 0; i < vehicles.size(); c += vehicles[i].size(), ++i){
        int last = vehicles[i].size() - 1;
        for(int j = 1; j < vehicles[i].size() - 1; ++j){//not including r- and r+
            /*
            std::cout<< var[c*2 + j*2] <<"\n";
            std::cout<< var[c*2] <<"\n";
            std::cout<< pairDistances[searchGraph[vehicles[i].front()].name.c_str()][searchGraph[vehicles[i][j]].name.c_str()].GetDouble() <<"\n";


            std::cout << "\n\ndebug\n";
            std::cout << var[c*2 + last*2];
            std::cout << var[c*2] << "\n";
            std::cout << searchGraph[vehicles[i].front()].name.c_str() << "\n";
            std::cout << searchGraph[vehicles[i][j]].name.c_str() << "\n";
            std::cout << pairDistances[searchGraph[vehicles[i].front()].name.c_str()][searchGraph[vehicles[i][j]].name.c_str()].GetDouble() << "\n";
            std::cout << "\ndebug\n\n";
            */
            con.add(
                (var[c + j] - var[c])
                    *pairDistances[searchGraph[vehicles[i].front()].name.c_str()][searchGraph[vehicles[i].back()].name.c_str()].GetDouble()
                -(var[c + last ] - var[c])
                    *pairDistances[searchGraph[vehicles[i].front()].name.c_str()][searchGraph[vehicles[i][j]].name.c_str()].GetDouble() == 0
                );
            // std::cout << con << "\n";
        }
    }

    //21 
    std::cout << "21\n";
    for(int i = 0, ci = 0; i < vehicles.size(); ci += vehicles[i].size(), ++i){
        for(int j = 0, cj = 0; j < vehicles.size(); cj += vehicles[j].size(), ++j){
            if(i != j && vehicles[i].front() == vehicles[j].front() && e[i] < e[j]){
                for(int vi = 0; vi < vehicles[i].size(); ++vi){
                    std::set<vertex_t> iconflicts = searchGraph[vehicles[i][vi]].generalizedVertexConflicts;
                    for(auto it = iconflicts.begin(); it != iconflicts.end(); ++it){
                        for(int vj = 0; vj < vehicles[j].size(); ++vj){
                            if(searchGraph[*it].name == searchGraph[vehicles[j][vj]].name){

                                // Tau of conflict point vi
                                int direction = vNameToDirection.find(std::to_string(vehicles[i][vi]))->second; 
                                IloExpr Tau = Li(direction)/w + (var[ci + vehicles[i].size() -1 ] - var[ci])*Li(direction)/(pairDistances[searchGraph[vehicles[i].front()].name.c_str()][searchGraph[vehicles[i].back()].name.c_str()].GetDouble());

                                con.add(var[ci + vi] 
                                    // + var[ci*2+ vi*2 + 1] 
                                    + Tau
                                    - var[cj + vj] <= 0);
                            }
                        }
                    }
                }
            }
        }
    }
    //22
    std::cout << "22\n";
    for(int i = 0, ci = 0; i < vehicles.size(); ci += vehicles[i].size(), ++i){
        for(int j = 0, cj = 0; j < vehicles.size(); cj += vehicles[j].size(), ++j){
            if(i != j && vehicles[i].front() != vehicles[j].front() && e[i] < e[j]){
                for(int vi = 0; vi < vehicles[i].size(); ++vi){

                    std::set<vertex_t> iconflicts = searchGraph[vehicles[i][vi]].generalizedVertexConflicts;
                    for(auto it = iconflicts.begin(); it != iconflicts.end(); ++it){
                        for(int vj = 0; vj < vehicles[j].size(); ++vj){
                            if(searchGraph[*it].name == searchGraph[vehicles[j][vj]].name){

                                // Tau of conflict point vi
                                int direction = vNameToDirection.find(std::to_string(vehicles[i][vi]))->second; 
                                // std::cout << "        " << i << "-" << j  << "-" << vi << std::endl;
                                IloExpr Tau = Li(direction)/w + (var[ci + vehicles[i].size() -1 ] - var[ci])*Li(direction)/(pairDistances[searchGraph[vehicles[i].front()].name.c_str()][searchGraph[vehicles[i].back()].name.c_str()].GetDouble());

                                con.add(var[ci + vi] 
                                    + Tau
                                    - var[cj + vj]
                                    - (1 - var[offset + i*vehicles.size() + j])*Mij[i][j]<= 0);
                            }
                        }
                    }
                }
            }
        }
    }



    //23
    std::cout << "23\n";
    for(int i = 0, ci = 0; i < vehicles.size(); ci += vehicles[i].size(), ++i){
        for(int j = 0, cj = 0; j < vehicles.size(); cj += vehicles[j].size(), ++j){
            if(i < j && vehicles[i].front() != vehicles[j].front() && e[i] < e[j]){
                for(int vi = 0; vi < vehicles[i].size(); ++vi){
                    std::set<vertex_t> iconflicts = searchGraph[vehicles[i][vi]].generalizedVertexConflicts;
                    for(auto it = iconflicts.begin(); it != iconflicts.end(); ++it){
                        for(int vj = 0; vj < vehicles[j].size(); ++vj){
                            if(searchGraph[*it].name == searchGraph[vehicles[j][vj]].name){
                                con.add(var[offset + i*vehicles.size() + j]
                                    +   var[offset + j*vehicles.size() + i]
                                    - 1 == 0);
                            }
                        }
                    }
                }
            }
        }
    }
    
    // std::cout << "\n\nsumobj:\n" << sum_obj << "\n\n";
    // std::cout << "variables:\n" << var << "\n\n";
    // std::cout << "constraints:\n" << con << "\n\n\n";
    model.add(con);
    // std::cout << model << "\n";
    //IloCplex cplex(env);
    double result = 0;
    IloCplex cplex(model);
    //std::cout << env.out() << "\n";
    //cplex.extract(model);
    //cplex.exportModel("model.lp");
    //std::cout<< "0:" << var[23] << "\n";
    if (cplex.solve()){
        
        result = cplex.getObjValue();
        
        cplex.writeSolution("solution");
        // std::cout << "\nstatus: " << cplex.getStatus() << "\n";
        // std::cout << "result: " << result << "\n";
        


        try{
            // IloNumArray v(env);
            // cplex.getValues(v, var);
            // for (IloInt i = 0; i < var.getSize() ; i++){
            //     std::cout << "Var no " << i << ":  Value = " << cplex.getValue(var[i]) << std::endl;
            // }
            IloInt printOffset = 0;
            for(int i = 0; i < vehicles.size(); ++i){//for each vehicle
                std::cout << "Vehicle " << i << ":" << std::endl;
                for(int j = 0; j < vehicles[i].size(); ++j){  // for each vetice
                    std::cout << searchGraph[vehicles[i][j]].name << " arrive at time: " << cplex.getValue(var[printOffset]) << std::endl;
                    printOffset += 1;
                }
            }
            
        }
        catch(IloCplex::Exception e){
            std::cout << "exception: " << e << "\n";
        }
        cplex.extract(model);
        cplex.exportModel("model.lp");

        
        /*
        for(int i = 0; i < offset; ++i)
            std::cout << v[i] << "\n";
            */
    }
    else
    {
        std::cout << cplex.getStatus();
    }

    env.end();

    //std::cout << result << "\n";


    

    return 0;
}
