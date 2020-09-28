#include <iostream>
#include <fstream>
#include <string>
#include "MAPF.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <cstdio>
#include <ilcplex/ilocplex.h>
position_t nodeAsPos(const float x, const float y)
{  
    return position_t(x,y);
}

void loadSearchGraph(
    searchGraph_t& searchGraph,
    std::unordered_map<std::string, vertex_t>& vNameToV,
    std::unordered_map<std::string, edge_t>& eNameToE,
    std::vector<vertex_t>& starts, std::vector<vertex_t>& goals,
    const std::string& fileName)
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

}


int main(int argc, char *argv[])
{
    std::string fileName = "/media/zijun/Windows10_OS/Users/Zijun/Documents/2020Summer/AIM/MAPF/graph.json";
    searchGraph_t searchGraph;
    std::unordered_map<std::string, vertex_t> vNameToV;
    std::unordered_map<std::string, edge_t> eNameToE;
    std::vector<vertex_t> starts;
    //e = time i reach the intersection
    std::vector<vertex_t> goals;
    loadSearchGraph(searchGraph, vNameToV, eNameToE, starts, goals, fileName);
    starts.push_back(vNameToV.find("WSR_0")->second);
    starts.push_back(vNameToV.find("NSR_0")->second);
    goals.push_back(vNameToV.find("WSR_1")->second);
    goals.push_back(vNameToV.find("NSR_7")->second);
    //std::cout<< searchGraph[vNameToV.find("WSR_1")->second].name;

/*
    auto neighbours = boost::adjacent_vertices(starts[0], searchGraph);
    std::cout << starts[0] << "\n";
    for (auto vd : make_iterator_range(neighbours))
    std::cout <<  vd << "\n";
*/

    
    double minSpeed = 1;
    double maxSpeed = 10;
    int vSize = vNameToV.size();
    double e[2] = {1,2};
    int pairDistances[vSize][vSize];
    double Mij[starts.size()][starts.size()];

    IloEnv env = IloEnv();
    IloModel model = IloModel(env);
    IloExpr sum_obj = IloExpr(env);
    IloNumVarArray var(env);
    IloRangeArray con(env);

/*variables: t(r-) = time arrive at point r-; T(r-)= time path through at r-;
                        di(c1,c2) distance between two points c1 and c2;
                        U1 min speed, U2 max speed; sigmaij(c):whether ij conflict at c
*/
    int maxNode = 1000000;
    for(int i = 0; i < starts.size(); ++i){//for each vehicle
        for(auto it = vNameToV.begin(); it != vNameToV.end(); ++it){
            var.add(IloNumVar(env, 0, maxNode, ILOFLOAT));
            var.add(IloNumVar(env, 0, maxNode, ILOFLOAT));
        }
    }

    for(int i = 0; i < starts.size(); ++i){
        for(int j = 0; j < starts.size(); ++j){
            for(int k = 0; k < vSize; ++k){
                var.add(IloNumVar(env, 0, 1, ILOINT));
            }
        }
    }
    
    //15
    for(int i = 0; i < starts.size(); ++i){
        sum_obj += var[vSize*i*2 + goals[i]];
        sum_obj += var[vSize*i*2 + vSize + goals[i]];
    }
    model.add(IloMinimize(env, sum_obj));

    //16
    for(int i = 0; i < starts.size(); ++i){
        con.add(var[vSize*i*2 + starts[i]] >= e[i]);
    }

    //17
    for(int i = 0; i < starts.size(); ++i){
        for(int j = 0; j < starts.size(); ++j){
            if(starts[i] == starts[j] && e[i] < e[j]){
                con.add(var[vSize*i*2 + starts[i]] 
                      + var[vSize*i*2 + vSize + starts[i]] 
                      - var[vSize*j*2 + starts[j]] <= 0);
            }
        }
    }

    //18 without w and Li
    for(int i = 0; i < starts.size(); ++i){
        for(int j = 0; j < vSize; ++j){
            con.add(
                (var[vSize*i*2 + goals[i]] - var[vSize*i*2 + starts[i]])
                /(pairDistances[starts[i]][goals[i]])
                - var[vSize*i*2 + vSize + j] == 0);
        }
    }

    //19
    for(int i = 0; i < starts.size(); ++i){
        con.add(pairDistances[starts[i]][goals[i]]/maxSpeed <= var[vSize*i*2 + goals[i]] - var[vSize*i*2 + starts[i]]);
    }
    for(int i = 0; i < starts.size(); ++i){
        con.add(pairDistances[starts[i]][goals[i]]/minSpeed >= var[vSize*i*2 + goals[i]] - var[vSize*i*2 + starts[i]]);
    }

    //20
    for(int i = 0; i < starts.size(); ++i){
        for(int j = 0; j < vSize; ++j){
            con.add(
                (var[vSize*i*2 + j] - var[vSize*i*2 + starts[i]])/pairDistances[starts[i]][j] 
            - (var[vSize*i*2 + goals[i]] - var[vSize*i*2 + starts[i]])/pairDistances[starts[i]][goals[i]] == 0
                );
        }
    }

    //21 intersection not yet implemented
    for(int i = 0; i < starts.size(); ++i){
        for(int j = 0; j < starts.size(); ++j){
            if(starts[i] == starts[j] && e[i] < e[j]){
                for(int k = 0; k < vSize; ++k){
                    con.add(var[vSize*i*2 + k] 
                      + var[vSize*i*2 + vSize + k] 
                      - var[vSize*j*2 + k] <= 0);
                }
            }
        }
    }

    //22
    for(int i = 0; i < starts.size(); ++i){
        for(int j = 0; j < starts.size(); ++j){
            if(starts[i] != starts[j]){
                for(int k = 0; k < vSize; ++k){
                    if(i != j){
                        con.add(var[vSize*i*2 + k] 
                          + var[vSize*i*2 + vSize + k] 
                          - var[vSize*j*2 + k] 
                          - (1 - var[starts.size()*vSize + i*starts.size()*vSize + j*vSize + k])/Mij[i][j]<= 0);
                    }
                }
            }
        }
    }

    //23
    for(int i = 0; i < starts.size(); ++i){
        for(int j = 0; j < starts.size(); ++j){
            if(starts[i] != starts[j] && i < j){
                for(int k = 0; k < vSize; ++k){
                    con.add(
                        var[starts.size()*vSize + i*starts.size()*vSize + j*vSize + k]
                    + var[starts.size()*vSize + j*starts.size()*vSize + i*vSize + k]
                    -1 == 0
                        );
                }
            }
        }
    }

    IloCplex cplex(model);

    if (cplex.solve()){

    }
    else
    {
        std::cout << cplex.getStatus();
    }


    env.end();
    return 0;
}
