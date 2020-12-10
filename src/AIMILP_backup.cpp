#include <iostream>
#include <fstream>
#include <string>
#include "AIMILP.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <cstdio>
#include <math.h>
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

float Li(){
    // std::cout << "debuggggG:\n";
    // std::cout << 4*turnRadius*asin(vehicleLength/(2*turnRadius)) << "\n";
    return 4*turnRadius*asin(vehicleLength/(2*turnRadius));
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
    std::string fileName = "intro_graph.json";
    std::string pDFileName = "pairDistance.json";
    searchGraph_t searchGraph;
    std::unordered_map<std::string, vertex_t> vNameToV;
    std::unordered_map<std::string, edge_t> eNameToE;
    std::vector<std::vector<vertex_t>> vehicles;
    rapidjson::Document pairDistances;
    //e = time i reach the intersection
    loadSearchGraph(searchGraph, vNameToV, eNameToE, fileName, pairDistances, pDFileName);

    //pairDistances[vehicles[i].front()][vehicles[i].back()];

    // print_map(eNameToE);

    //initialize vehicles
    std::vector<vertex_t> v0, v1, v2;
    v0.push_back(vNameToV.find("WSR_0")->second);
    v0.push_back(vNameToV.find("WSR_1")->second);
    vehicles.push_back(v0);
    /*
    v1.push_back(vNameToV.find("WER_0")->second);
    v1.push_back(vNameToV.find("WER_1")->second);
    v1.push_back(vNameToV.find("WER_2")->second);
    v1.push_back(vNameToV.find("WER_3")->second);
    v1.push_back(vNameToV.find("WER_4")->second);
    v1.push_back(vNameToV.find("WER_5")->second);
    v1.push_back(vNameToV.find("WER_6")->second);
    v1.push_back(vNameToV.find("WER_7")->second);
    vehicles.push_back(v1);
    */
    /*v2.push_back(vNameToV.find("NSR_0")->second);
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
    double e[3] = {1,2,3};
    double Mij[vehicles.size()][vehicles.size()];
    // std::cout << "\n\n vehicles.size(): " << vehicles.size() << "\n\n";

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

    // var.add(IloNumVar(env, 0, maxNode, ILOFLOAT));
    // var.add(IloNumVar(env, 0, maxNode, ILOFLOAT));
    // con.add(-2*var[0] + 2*var[1] >= 1);
    // con.add(-8*var[0] + 10*var[1] <= 13);
    // sum_obj  += (-var[0] - var[1]);
    // model.add(IloMinimize(env, sum_obj));

    // var.add ( IloNumVar ( env , 0, 40));
    var.add ( IloNumVar ( env )); // default : between 0 and +inf
    var.add ( IloNumVar ( env ));
    con.add ( - 2*var[0] + 2*var[1] >= 1);
    con.add ( -8*var[0] + 10 * var[1]  <= 13);
    sum_obj  += -var[0]-var[1];
    model.add ( IloMinimize ( env , sum_obj));


    std::cout << "\n\nsumobj:\n" << sum_obj << "\n\n";
    std::cout << "variables:\n" << var << "\n\n";
    std::cout << "constraints:\n" << con << "\n\n\n";
    model.add(con);
    std::cout << model << "\n";
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
        std::cout << "\nstatus: " << cplex.getStatus() << "\n";
        std::cout << "result: " << result << "\n";

        // IloNumArray v(env);
        // cplex.getValues(v, var);




        // IloNumArray x(env); cplex.getValues(x, var);


        try{
            IloNumArray v(env);
            cplex.getValues(v, var);
            for (IloInt i = 0; i < var.getSize(); i++){
                std::cout << "Food " << i << ":  Buy = " << cplex.getValue(var[i]) << std::endl;
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

