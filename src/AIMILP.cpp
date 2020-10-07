#include <iostream>
#include <fstream>
#include <string>
#include "AIMILP.h"
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


int main(int argc, char *argv[])
{
    std::string fileName = "/media/zijun/Windows10_OS/Users/Zijun/Documents/2020Summer/AIM/AIMILP/graph.json";
    std::string pDFileName = "/media/zijun/Windows10_OS/Users/Zijun/Documents/2020Summer/AIM/AIMILP/pairDistance.json";
    searchGraph_t searchGraph;
    std::unordered_map<std::string, vertex_t> vNameToV;
    std::unordered_map<std::string, edge_t> eNameToE;
    std::vector<std::vector<vertex_t>> vehicles;
    rapidjson::Document pairDistances;
    //e = time i reach the intersection
    loadSearchGraph(searchGraph, vNameToV, eNameToE, fileName, pairDistances, pDFileName);

    //pairDistances[vehicles[i].front()][vehicles[i].back()];



    //initialize vehicles
    std::vector<vertex_t> v0, v1, v2;
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
    vehicles.push_back(v0);
    vehicles.push_back(v1);
    vehicles.push_back(v2);

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
    double maxSpeed = 10;
    double e[3] = {1,2,3};
    double Mij[vehicles.size()][vehicles.size()];

// Mij is a larger positive constant
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
    int maxNode = 1000000;
    int offset = 0;
    for(int i = 0; i < vehicles.size(); ++i){//for each vehicle
        for(int j = 0; j < vehicles[i].size(); ++j){
            var.add(IloNumVar(env, 0, maxNode, ILOFLOAT));
            var.add(IloNumVar(env, 0, maxNode, ILOFLOAT));
            offset += 2;
        }
    }

    for(int i = 0; i < vehicles.size(); ++i)
        for(int j = 0; j < vehicles.size(); ++j)
            var.add(IloNumVar(env, 0, 1, ILOINT));

    //15

    std::cout << "15\n";
    for(int i = 0, c = 0; i < vehicles.size(); c += vehicles[i].size(), ++i){
        int last = vehicles[i].size() - 1;
        sum_obj += var[c*2 + last*2];
        sum_obj += var[c*2 + last*2 + 1];
    }
    model.add(IloMinimize(env, sum_obj));

    //16
    std::cout << "16\n";
    for(int i = 0, c = 0; i < vehicles.size(); c += vehicles[i].size(), ++i){
        con.add(var[c*2] >= e[i]);
    }

    //17
    std::cout << "17\n";
    for(int i = 0, ci = 0; i < vehicles.size(); ci += vehicles[i].size(), ++i){
        for(int j = 0, cj = 0; j < vehicles.size(); cj += vehicles[j].size(), ++j){
            if(i != j && vehicles[i].front() == vehicles[j].front() && e[i] < e[j]){
                con.add(var[ci*2] 
                      + var[ci*2+1] 
                      - var[cj*2] <= 0);
            }
        }
    }

    //18 without w and Li
    std::cout << "18\n";
    for(int i = 0, c = 0; i < vehicles.size(); c += vehicles[i].size(), ++i){
        int last = vehicles[i].size() - 1;
        for(int j = 0; j < vehicles[i].size(); ++j){
            con.add(
                (var[c*2 + last*2 ] - var[c*2])
                /(pairDistances[searchGraph[vehicles[i].front()].name.c_str()][searchGraph[vehicles[i].back()].name.c_str()].GetDouble())
                - var[c*2 + j*2 + 1] == 0);
        }
    }

    //19
    std::cout << "19\n";
    for(int i = 0, c = 0; i < vehicles.size(); c += vehicles[i].size(), ++i){
        int last = vehicles[i].size() - 1;
        con.add(pairDistances[searchGraph[vehicles[i].front()].name.c_str()][searchGraph[vehicles[i].back()].name.c_str()].GetDouble()/maxSpeed <= var[c*2 + last*2 ] - var[c*2]);
        con.add(pairDistances[searchGraph[vehicles[i].front()].name.c_str()][searchGraph[vehicles[i].back()].name.c_str()].GetDouble()/minSpeed >= var[c*2 + last*2 ] - var[c*2]);
    }

    //20
    std::cout << "20\n";
    for(int i = 0, c = 0; i < vehicles.size(); c += vehicles[i].size(), ++i){
        int last = vehicles[i].size() - 1;
        for(int j = 0; j < vehicles[i].size(); ++j){
            con.add(
                (var[c*2 + j*2] - var[c*2])/
                pairDistances[searchGraph[vehicles[i].front()].name.c_str()][searchGraph[vehicles[i][j]].name.c_str()].GetDouble()
              - (var[c*2 + last*2 ] - var[c*2])/pairDistances[searchGraph[vehicles[i].front()].name.c_str()][searchGraph[vehicles[i].back()].name.c_str()].GetDouble() == 0
                );
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
                                con.add(var[ci*2 + vi*2] 
                                  + var[ci*2+ vi*2 + 1] 
                                  - var[cj*2 + vj*2] <= 0);
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
                                con.add(var[ci*2 + vi*2] 
                                  + var[ci*2+ vi*2 + 1] 
                                  - var[cj*2 + vj*2]
                                  - (1 - var[offset + i*vehicles.size() + j])/Mij[i][j]<= 0);
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

    model.add(con);
    //IloCplex cplex(env);
    double result = 0;
    
    IloCplex cplex(env);
    if (cplex.solve()){
        
        result = cplex.getObjValue();
        IloNumArray v(env);



        cplex.getValues(v, var);
        
        
        for(int i = 0; i < offset; ++i)
            std::cout << v[i] << "\n";
        cplex.exportModel("model.lp");
    }
    else
    {
        std::cout << cplex.getStatus();
    }

    env.end();

    std::cout << result << "\n";


    

    return 0;
}
