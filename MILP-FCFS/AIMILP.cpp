#define RAPIDJSON_HAS_STDSTRING 1

#include <iostream>
#include <fstream>
#include <string>
#include "AIMILP.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <cstdio>
#include <math.h>
#include <ilcplex/ilocplex.h>

#include <chrono>
using namespace std::chrono;


float turnRadiusRight = 10*0.3048;
float turnRadiusLeft = 40*0.3048;

float vehicleLength = 5;

float w = 7;

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
                // 2 mean straight direction
                vNameToDirection[std::to_string(v)] = 2;
            }else if ( name.find("EN")!= std::string::npos | 
                    name.find("WS")!= std::string::npos | 
                    name.find("NW")!= std::string::npos | 
                    name.find("SE")!= std::string::npos ) 
            {
                // 1 mean turning right direction
                vNameToDirection[std::to_string(v)] = 1;
            }else
            {
                // 0 mean turning left
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
    rapidjson::FileReadStream is2(pdfile, readBuffer2, sizeof(readBuffer2));
    pairDistances.ParseStream(is2);
    fclose(pdfile);
}


float Li(int direction){
    // straight 
    if (direction == 2){
        // std::cout << "direction: " << direction << "-" << vehicleLength << std::endl; 
        return vehicleLength;
    }
    //right, turnRadiusRight
    else if (direction == 1)
    {
        // std::cout << "direction: " << direction << "-" << 4*turnRadius*asin(vehicleLength/(2*turnRadius)) << std::endl; 
        return 4*turnRadiusRight*asin(vehicleLength/(2*turnRadiusRight));
    }
    //left, turnRadiusLeft
    else if (direction == 0)
    {
        // std::cout << "direction: " << direction << "-" << 4*turnRadius*asin(vehicleLength/(2*turnRadius)) << std::endl; 
        return 4*turnRadiusLeft*asin(vehicleLength/(2*turnRadiusLeft));
    }
}

void loadVehicleArriving(
        std::string lanes[], 
        std::unordered_map<std::string, vertex_t>& vNameToV,
        int& step, 
        std::string& arrivalFile, 
        std::vector<std::vector<vertex_t>>& vehicles,
        std::vector< double >& e
){
    FILE* fp = fopen(arrivalFile.c_str(), "r"); // non-Windows use "r"
    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    rapidjson::Document doc;
    doc.ParseStream(is);
    fclose(fp);

    auto vehicleList = doc.GetObject();
    for (int i = 0; i <= step; i++){
        
        std::string IDString = std::to_string(i);

        auto trajectoryArray = vehicleList[IDString]["trajectory"].GetArray();
        std::vector<vertex_t> aVehicle;
        for (rapidjson::Value::ConstValueIterator itr2 = trajectoryArray.Begin();
                            itr2 != trajectoryArray.End(); ++itr2){
            aVehicle.push_back(vNameToV.find((*itr2).GetString())->second);
            // std::cout << "get trajectory: " << (*itr2).GetString() << std::endl;
        }
        vehicles.push_back(aVehicle);
        e.push_back(vehicleList[IDString]["arrivalTime"].GetDouble());
    }

    // for (int laneNo=0; laneNo < 8; laneNo++){
    //     std::string laneName = lanes[laneNo];
    //     // std::cout << laneName << std::endl;
    //     auto lane = doc[  laneName  ].GetObject();
    //     // std::cout << "Has Member: " << lane.HasMember("0") << std::endl;
    //     // std::string id = lane["216"][0]["id"].GetString();
    //     // std::cout << "get type: " << doc["EL"]["216"][0].GetType() << std::endl;
    //     // std::cout << "id: " << id << std::endl;

        
    //     std::string timeString = std::to_string(step);
    //     if (lane.HasMember(timeString)) {
    //         auto arrivalsArray = lane[timeString].GetArray();

    //         // std::cout << "get type: " << arrivalsArray.Size() << std::endl;
    //         for (rapidjson::Value::ConstValueIterator itr = arrivalsArray.Begin();
    //                                         itr != arrivalsArray.End(); ++itr){
    //             // std::cout << "get name: " << (*itr)["id"].GetString() << std::endl;
    //             // std::cout << "get arrival time: " << (*itr)["arrivalTime"].GetInt() << std::endl;

    //             auto trajectoryArray = (*itr)["trajectory"].GetArray();

    //             std::vector<vertex_t> aVehicle;

    //             for (rapidjson::Value::ConstValueIterator itr2 = trajectoryArray.Begin();
    //                                 itr2 != trajectoryArray.End(); ++itr2){
    //                 aVehicle.push_back(vNameToV.find((*itr2).GetString())->second);
    //                 // std::cout << "get trajectory: " << (*itr2).GetString() << std::endl;
    //             }
    //             vehicles.push_back(aVehicle);
    //             e.push_back((*itr)["arrivalTime"].GetDouble());
    //         }
    //     }

    // }
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
    std::string lanes[8] = {"WR", "WL", "ER", "EL", "NR", "NL", "SR", "SL"};

    std::string fileName = "intro_graph_3.json";
    std::string pDFileName = "pairDistance_3.json";
    searchGraph_t searchGraph;
    std::unordered_map<std::string, vertex_t> vNameToV;

    std::unordered_map<std::string, vertex_t> vNameToDirection;

    std::unordered_map<std::string, edge_t> eNameToE;
    // std::vector<std::vector<vertex_t>> vehicles;
    rapidjson::Document pairDistances;
    //e = time i reach the intersection
    loadSearchGraph(searchGraph, vNameToV, vNameToDirection, eNameToE, fileName, pairDistances, pDFileName);



    std::string arrivalFile = "vehicleArrival.json";

    double minSpeed = 3;
    double maxSpeed = 15;

    int timeStep = 10;
    // implement ALAP policy
    int timeHorizon = 10;

    // // implement ASAP policy
    // int timeHorizon = 15;


    std::vector<std::vector<vertex_t>> vehicles;
    std::vector<std::vector<vertex_t>> nextTimeStepVehicles;

    std::vector< double > e;
    std::vector< double > nextTimeStepE;
    std::ofstream outputFile ("output.txt");
    if (outputFile.is_open())
    {
        outputFile << "In Point;Out Point;Length;Earliest Arrival;In Time;Out Time; Out Time Tail;Time spend;Speed\n";
    }
    // std::cout << "Measuring time execution from 1 to 40 vehicles cases" << std::endl;
    // for (int step = 0; step < 40; step++){

        int step = atoi(argv[1]);
        
        int currentTime = timeStep * step;
        int F = timeStep * step + timeHorizon;


        // double e[40] = {1,2,3,4};
        
        vehicles.clear();
        e.clear();
        // vehicles = nextTimeStepVehicles;
        // e = nextTimeStepE;
        loadVehicleArriving(lanes, vNameToV, step, arrivalFile, vehicles, e);


        auto startTime = high_resolution_clock::now();



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
        // std::cout << "15\n";
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
        // std::cout << "16\n";
        for(int i = 0, c = 0; i < vehicles.size(); c += vehicles[i].size(), ++i){
            con.add(var[c] >= e[i]);
        }
        // std::cout << con << "\n";


        // //17
        // std::cout << "17\n";
        for(int i = 0, ci = 0; i < vehicles.size(); ci += vehicles[i].size(), ++i){
            int last = vehicles[i].size() - 1;
            for(int j = 0, cj = 0; j < vehicles.size(); cj += vehicles[j].size(), ++j){
                std::set<vertex_t> iconflicts = searchGraph[vehicles[j].front()].generalizedVertexConflicts;
                for(auto it = iconflicts.begin(); it != iconflicts.end(); ++it){
                    bool samePoint =  (vehicles[i].front()==*it || vehicles[i].front() == vehicles[j].front());
                    // std::cout << samePoint << "==" << vehicles[i].front() << "=---" << vehicles[j].front() << "=---" << *it<< std::endl;
                    if(i != j && samePoint && e[i] < e[j]){

                        // std::cout << *it<< "=---" << searchGraph[*it].name << "=---" << searchGraph[vehicles[j][vj]].name << "\n\n"<< std::endl;

                        // Tau of entry conflict point
                        int direction = vNameToDirection.find(std::to_string(vehicles[i][0]))->second; 
                        IloExpr Tau = Li(direction)/w + (var[ci + last ] - var[ci])*Li(direction)/(pairDistances[searchGraph[vehicles[i].front()].name.c_str()][searchGraph[vehicles[i].back()].name.c_str()].GetDouble());
                        con.add(var[ci] 
                            + Tau
                            - var[cj] <= 0);
                    }
                }
                // std::cout << "\n\n" << std::endl;


            }
        }


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
        // std::cout << "19\n";
        for(int i = 0, c = 0; i < vehicles.size(); c += vehicles[i].size(), ++i){
            int last = vehicles[i].size() - 1;
            con.add(pairDistances[searchGraph[vehicles[i].front()].name.c_str()][searchGraph[vehicles[i].back()].name.c_str()].GetDouble()/maxSpeed <= var[c + last ] - var[c]);
            con.add(pairDistances[searchGraph[vehicles[i].front()].name.c_str()][searchGraph[vehicles[i].back()].name.c_str()].GetDouble()/minSpeed >= var[c + last ] - var[c]);
        }
        // std::cout << con << "\n";

        // //20
        // std::cout << "20\n";
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
        // std::cout << "21\n";
        for(int i = 0, ci = 0; i < vehicles.size(); ci += vehicles[i].size(), ++i){
            for(int j = 0, cj = 0; j < vehicles.size(); cj += vehicles[j].size(), ++j){

                std::set<vertex_t> vehicleJFirstConflicts = searchGraph[vehicles[j].front()].generalizedVertexConflicts;
                for(auto jcp = vehicleJFirstConflicts.begin(); jcp != vehicleJFirstConflicts.end(); ++jcp){
                    bool samePoint =  (vehicles[i].front()==*jcp || vehicles[i].front() == vehicles[j].front());
                
                    if(i != j && samePoint && e[i] < e[j]){
                        for(int vi = 0; vi < vehicles[i].size(); ++vi){
                            std::set<vertex_t> iconflicts = searchGraph[vehicles[i][vi]].generalizedVertexConflicts;
                            for(auto it = iconflicts.begin(); it != iconflicts.end(); ++it){
                                for(int vj = 0; vj < vehicles[j].size(); ++vj){

    

                                    if(searchGraph[*it].name == searchGraph[vehicles[j][vj]].name ||  searchGraph[vehicles[i][vi]].name == searchGraph[vehicles[j][vj]].name){
                                        // std::cout << searchGraph[*it].name<< "=---" << searchGraph[vehicles[i][vi]].name << "=---" << searchGraph[vehicles[j][vj]].name << std::endl;
                                        // std::cout << *it<< "=---" << vi << "=---" << vj << "\n\n"<< std::endl;

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
        }
        //22
        // std::cout << "22\n";
        for(int i = 0, ci = 0; i < vehicles.size(); ci += vehicles[i].size(), ++i){
            for(int j = 0, cj = 0; j < vehicles.size(); cj += vehicles[j].size(), ++j){
                std::set<vertex_t> vehicleJFirstConflicts = searchGraph[vehicles[j].front()].generalizedVertexConflicts;
                for(auto jcp = vehicleJFirstConflicts.begin(); jcp != vehicleJFirstConflicts.end(); ++jcp){
                    bool samePoint =  (vehicles[i].front()==*jcp || vehicles[i].front() == vehicles[j].front());
                    if(i != j && !samePoint){
                        for(int vi = 0; vi < vehicles[i].size(); ++vi){

                            std::set<vertex_t> iconflicts = searchGraph[vehicles[i][vi]].generalizedVertexConflicts;
                            for(auto it = iconflicts.begin(); it != iconflicts.end(); ++it){
                                for(int vj = 0; vj < vehicles[j].size(); ++vj){
                                    if(searchGraph[*it].name == searchGraph[vehicles[j][vj]].name){
                                        
                                        // Tau of conflict point vi
                                        int direction = vNameToDirection.find(std::to_string(vehicles[i][vi]))->second; 
                                        // std::cout << "        Conflict points: " << i << "-" << j  << "-" << searchGraph[vehicles[i][vi]].name << "-" << searchGraph[vehicles[j][vj]].name << "-" << ci + vi << "-" << cj + vj << std::endl;
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
        }



        //23
        // std::cout << "23\n";
        for(int i = 0, ci = 0; i < vehicles.size(); ci += vehicles[i].size(), ++i){
            for(int j = 0, cj = 0; j < vehicles.size(); cj += vehicles[j].size(), ++j){
                std::set<vertex_t> vehicleJFirstConflicts = searchGraph[vehicles[j].front()].generalizedVertexConflicts;
                for(auto jcp = vehicleJFirstConflicts.begin(); jcp != vehicleJFirstConflicts.end(); ++jcp){
                    bool samePoint =  (vehicles[i].front()==*jcp || vehicles[i].front() == vehicles[j].front());

                    if(i < j && !samePoint){
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
        }

        // FCFS
        for(int i = 0, ci = 0; i < vehicles.size(); ci += vehicles[i].size(), ++i){
            for(int j = 0, cj = 0; j < vehicles.size(); cj += vehicles[j].size(), ++j){
                if (i!=j && e[i]<e[j]){
                    int lastI = vehicles[i].size() - 1;
                    int lastJ = vehicles[j].size() - 1;
                    con.add(var[ci] - var[cj]  <= 0);
                    con.add(   var[ci+lastI] - var[cj+lastJ]  <= 0   );
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
        cplex.setOut(env.getNullStream());
        cplex.setWarning(env.getNullStream());
        cplex.setError(env.getNullStream());
        //std::cout << env.out() << "\n";
        cplex.extract(model);
        cplex.exportModel("model.lp");
        //std::cout<< "0:" << var[23] << "\n";
        if (cplex.solve()){
            
            // result = cplex.getObjValue();
            
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
                // outputFile << "Time;" << currentTime << "\n";
                for(int i = 0; i < vehicles.size(); ++i){//for each vehicle
                    // std::cout << "Vehicle no." << i << ":" << std::endl;
                    for(int j = 0; j < vehicles[i].size(); ++j){  // for each vetice
                        // std::cout << "    Vertex no." << j << " arrive at time: " << cplex.getValue(var[printOffset]) << std::endl;
                        // if (j== 0 && cplex.getValue(var[printOffset])>= F  )
                        // {
                        //     std::cout << "THERE IS VEHICLE THAT WILL BE ARRIVED LATE!" << std::endl;
                        //     nextTimeStepVehicles.push_back(vehicles[i]);
                        //     nextTimeStepE.push_back(e[i]);
                        // }
                        // if (j== 0 && cplex.getValue(var[printOffset]) < F  )
                        if (j== 0  )

                        {
                            // std::cout << searchGraph[vehicles[i].front()].name.c_str() 
                            //             << " " << searchGraph[vehicles[i].back()].name.c_str()
                            //             << " " << pairDistances[searchGraph[vehicles[i].front()].name.c_str()][searchGraph[vehicles[i].back()].name.c_str()].GetDouble()  
                            //             << " " << vehicles[i][0] 
                            //             << " " << vehicles[i].back() 
                            //             << " " << cplex.getValue(var[printOffset]) 
                            //             << " " << cplex.getValue(var[printOffset+vehicles[i].size()-1]) 
                            //             << std::endl;
                            double speed = (pairDistances[searchGraph[vehicles[i].front()].name.c_str()][searchGraph[vehicles[i].back()].name.c_str()].GetDouble())/(cplex.getValue(var[printOffset+vehicles[i].size()-1]) - cplex.getValue(var[printOffset]));
                            int VehDirection = vNameToDirection.find(std::to_string(vehicles[i][0]))->second; 
                            double VehTau = Li(VehDirection)/w + Li(VehDirection)/speed;

                            // for (int vertexIt = 0;vertexIt < vehicles[i].size(); vertexIt++){
                            //     std::cout << searchGraph[vehicles[i][vertexIt]].name.c_str()  << " " << printOffset + vertexIt<< " " << cplex.getValue(var[printOffset + vertexIt]) << " "<< cplex.getValue(var[printOffset + vertexIt])+VehTau << ";   ";
                            // }
                            // std::cout << "\n";
                        
                            outputFile << searchGraph[vehicles[i].front()].name.c_str()
                                        << ";" << searchGraph[vehicles[i].back()].name.c_str()
                                        << ";" << pairDistances[searchGraph[vehicles[i].front()].name.c_str()][searchGraph[vehicles[i].back()].name.c_str()].GetDouble()
                                        << ";" << e[i]
                                        << ";" << cplex.getValue(var[printOffset])
                                        << ";" << cplex.getValue(var[printOffset+vehicles[i].size()-1])
                                        << ";" << cplex.getValue(var[printOffset+vehicles[i].size()-1]) + VehTau
                                        << ";" << cplex.getValue(var[printOffset+vehicles[i].size()-1]) - cplex.getValue(var[printOffset])
                                        << ";" << (pairDistances[searchGraph[vehicles[i].front()].name.c_str()][searchGraph[vehicles[i].back()].name.c_str()].GetDouble())/(cplex.getValue(var[printOffset+vehicles[i].size()-1]) - cplex.getValue(var[printOffset]))
                                        << "\n";
                            // if (e[i] != cplex.getValue(var[printOffset])){
                            //     outputFile << "DIFF HERE: " << cplex.getValue(var[printOffset]) - e[i] << "\n";
                            // }
                        }
                        // outputFile << "    Vertex no." << j << " arrive at time: " << cplex.getValue(var[printOffset]) << "\n";



                        printOffset += 1;





                    }
                }


            }
            catch(IloCplex::Exception e){
                std::cout << "exception: " << e << "\n";
            }
            // cplex.extract(model);
            // cplex.exportModel("model.lp");

        
            /*
            for(int i = 0; i < offset; ++i)
                std::cout << v[i] << "\n";
                */

        }
        else
        {
            std::cout << cplex.getStatus();

        }

        sum_obj.end();

        env.end();



        auto stopTime = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stopTime - startTime); 
        std::cout << duration.count()/1000000.0 << std::endl;
        //std::cout << result << "\n";

    // }

    outputFile.close();



    return 0;
}
