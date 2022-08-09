#include "Instance.h"
#include <iostream>
#include <fstream>
#include <string>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <cstdio>
#include <math.h>
#include <unordered_map>

Instance::Instance(const string& map_name)
{
    // string arrFile = "vehicleArrival.json";
    string PDFile = "pairDistance_3.json";
    // int step = 39;
    fileName = map_name;
    loadSearchGraph(searchGraph, vNameToID, vIDToName, vNameToV, vNameToDirection, vIDToConflictPoints, fileName, pairDistances, pairDistancesMap, PDFile);
    // std::cout << "Instance: graph loaded\n";
    // loadVehicles(arrFile, step);
    loadSamePoint();
    // std::cout << "Instance: vehicle loaded\n";
    // std::cout << "getNumOfVertices(): " << getNumOfVertices() << std::endl;
}


int Instance::getNumOfVertices() const
{
    return vNameToID.size();
}


void Instance::loadVehicles(const std::string& arrivalFile, int step)
{
    FILE* fp = fopen(arrivalFile.c_str(), "r"); // non-Windows use "r"
    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    rapidjson::Document doc;
    doc.ParseStream(is);
    fclose(fp);

    // auto agentList = doc.GetObject();

    agents.clear();
    int agentId = 0;

    for (int i = 0; i <= step; i++){
        std::string IDString = std::to_string(i);


        Agent ag = {};
        ag.id = agentId;
        ag.name = std::to_string(doc[IDString.c_str()]["id"].GetInt());
        ag.importance = (doc[IDString.c_str()]["importance"].GetInt());

        ag.earliest_start_time = doc[IDString.c_str()]["arrivalTime"].GetDouble();
        auto trajectoryArray = doc[IDString.c_str()]["trajectory"].GetArray();

        // if (ag.importance == 2){
        //     ag.length = 15;
        // }
        // else{
            ag.length = 5;
        // }    
        
        ag.v_min = 3;
        ag.v_max = 15;
        for (rapidjson::Value::ConstValueIterator itr2 = trajectoryArray.Begin();
            itr2 != trajectoryArray.End(); ++itr2){
            ag.trajectory.push_back(vNameToID.find((*itr2).GetString())->second);
        }
        agents.push_back(ag);
        agentId++;
    }
}




// void Instance::loadVehicles(const std::string& arrivalFile, int step, vector<Agent> &agents, std::unordered_map<std::string, int>& vNameToID)
// {
//     std::string lanes[8] = {"WR", "WL", "ER", "EL", "NR", "NL", "SR", "SL"};
//     FILE* fp = fopen(arrivalFile.c_str(), "r"); // non-Windows use "r"
//     char readBuffer[65536];
//     rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
//     rapidjson::Document doc;
//     doc.ParseStream(is);
//     fclose(fp);
//     int agentId = 0;
//     for (int laneNo=0; laneNo < 8; laneNo++){
//         std::string laneName = lanes[laneNo];
//         auto lane = doc[laneName.c_str()].GetObject();
        
//         std::string timeString = std::to_string(step);
//         if (lane.HasMember(timeString.c_str())) {
//             auto arrivalsArray = lane[timeString.c_str()].GetArray();
//             for (rapidjson::Value::ConstValueIterator itr = arrivalsArray.Begin();
//                                             itr != arrivalsArray.End(); ++itr){
//                 Agent ag = {};
//                 ag.id = agentId;
//                 ag.name = (*itr)["id"].GetString();
//                 ag.earliest_start_time = (*itr)["arrivalTime"].GetDouble();
//                 auto trajectoryArray = (*itr)["trajectory"].GetArray();
                
//                 ag.earliest_start_time = (*itr)["arrivalTime"].GetDouble();
//                 ag.length = 5;
//                 ag.v_min = 3;
//                 ag.v_max = 10;

//                 for (rapidjson::Value::ConstValueIterator itr2 = trajectoryArray.Begin();
//                     itr2 != trajectoryArray.End(); ++itr2){
//                     ag.trajectory.push_back(vNameToID.find((*itr2).GetString())->second);
//                 }
//                 agents.push_back(ag);
//                 agentId++;
                
//             }
//         }

//     }
// }

void Instance::loadSearchGraph(
    searchGraph_t& searchGraph,

    std::unordered_map<std::string, int>& vNameToID,
    std::unordered_map<int, std::string>& vIDToName,

    std::vector<vertex_t>& vNameToV,

    std::vector<int>& vNameToDirection,
    std::unordered_map<int, std::vector<int> >& vIDToConflictPoints,

    const std::string& fileName, 
    rapidjson::Document& pairDistances,
    std::map<int, std::map<int, double> >& pairDistancesMap,
    const std::string& pDFileName)
{

    FILE* fp = fopen(fileName.c_str(), "r"); // non-Windows use "r"
    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    rapidjson::Document doc;
    doc.ParseStream(is);
    fclose(fp);

    int vertex_index = 0;
    // add vertices
    for (rapidjson::Value::ConstValueIterator itr = doc["vertices"].Begin();
                                             itr != doc["vertices"].End(); ++itr) {
        if (itr->HasMember("name")) {
            position_t pos = nodeAsPos((*itr)["pos"][0].GetDouble(),(*itr)["pos"][1].GetDouble());
            std::string name = (*itr)["name"].GetString();
            auto v = boost::add_vertex(searchGraph);
            searchGraph[v].name = name;
            searchGraph[v].pos = pos;
            searchGraph[v].id = vertex_index;

            vNameToID[name] = vertex_index;
            vIDToName[vertex_index] = name;
            vNameToV.push_back(v);


            if ( name.find("EW")!= std::string::npos | 
                    name.find("WE")!= std::string::npos | 
                    name.find("NS")!= std::string::npos | 
                    name.find("SN")!= std::string::npos ) 
            {
                // 2 mean straight direction
                vNameToDirection.push_back(2);
            }else if ( name.find("EN")!= std::string::npos | 
                    name.find("WS")!= std::string::npos | 
                    name.find("NW")!= std::string::npos | 
                    name.find("SE")!= std::string::npos ) 
            {
                // 1 mean turning right direction
                vNameToDirection.push_back(1);
            }else
            {
                // 0 mean turning left
                vNameToDirection.push_back(0);
            }
            
            vertex_index++;
        }
        
    }

    //add vertex conflicts
    for (rapidjson::Value::ConstValueIterator itr = doc["vertices"].Begin();
                                             itr != doc["vertices"].End(); ++itr) {
        if (itr->HasMember("name")) {
            std::string name = (*itr)["name"].GetString();

            auto iter = vNameToID.find(name);

            int v_idx = iter->second;

            vertex_t v = vNameToV[v_idx];

            for (rapidjson::Value::ConstValueIterator node = (*itr)["vertex_conflicts"].Begin();
                                             node != (*itr)["vertex_conflicts"].End(); ++node) 
            {
                std::string cName = node->GetString();
                // auto cIter = vNameToV.find(cName);
                auto cIter = vNameToID.find(cName);

                if (cIter == vNameToID.end()) 
                {
                    std::cerr << "(2) ERROR: Could not find vertex " << cName << std::endl;
                    continue;
                }
                int u = cIter->second;
                searchGraph[v].generalizedVertexConflicts.insert(u);
                vIDToConflictPoints[v_idx].push_back(u);
            }
        }
    }


    //read pair distance
    FILE* pdfile = fopen(pDFileName.c_str(), "r"); // non-Windows use "r"
    char readBuffer2[65536];
    rapidjson::FileReadStream is2(pdfile, readBuffer2, sizeof(readBuffer2));
    pairDistances.ParseStream(is2);

    // double pairDistancesArray[sizeof(vNameToID)][sizeof(vNameToID)];

    // std::map<int, std::map<int, double> > pairDistancesMap; 

    for (rapidjson::Value::ConstMemberIterator itr = pairDistances.MemberBegin();
    itr != pairDistances.MemberEnd(); ++itr)
    {
        std::string first_vertex_string = itr->name.GetString();

        auto first_vertex = vNameToID.find(first_vertex_string);

        int first_vertex_id = first_vertex->second;        


        auto second_vertex_list = itr->value.GetObject();

        for (rapidjson::Value::ConstMemberIterator itr2 = second_vertex_list.MemberBegin();
                    itr2 != second_vertex_list.MemberEnd(); ++itr2)
        {
            std::string second_vertex_string = itr2->name.GetString();
            auto second_vertex = vNameToID.find(second_vertex_string);

            int second_vertex_id = second_vertex->second;

            // pairDistancesArray[first_vertex_id][second_vertex_id] = itr2->value.GetDouble();
            pairDistancesMap[first_vertex_id][second_vertex_id] = itr2->value.GetDouble();      
        }
    }


    fclose(pdfile);
}

void Instance::loadSamePoint(){
    for(int i = 0; i < 104; i ++){
        samePoint[i][i] = true;
        std::vector<int> cps = getConflictPoints(i);
        for(auto it = cps.begin(); it != cps.end(); ++it){
            samePoint[i][*it] = true;
        }
    }
}

bool Instance::isSamePoint(int a, int b){
    return samePoint[a][b];
}

std::vector<int> Instance::getConflictPoints(int aid){
    return vIDToConflictPoints[aid];
}

position_t Instance::nodeAsPos(const float x, const float y)
{  
    return position_t(x,y);
}

std::map<int, std::map<int, double> > Instance::getPairDistancesMap(){
    return pairDistancesMap;
}

std::vector<int> Instance::getVNameToDirection(){
    return vNameToDirection;
}

std::unordered_map<std::string, int> Instance::getVNameToID(){
    return vNameToID;
}

std::vector<Agent> Instance::getAgents(){
    return agents;
}

std::unordered_map<int, std::vector<int> > Instance::getVIDToConflictPoints(){
    return vIDToConflictPoints;
}

std::unordered_map<int, std::string> Instance::getVIDToName(){
    return vIDToName;
}

double Instance::getEarliestStartTime(int index){
    return agents[index].earliest_start_time;
}

int Instance::getStartCP(int index){
    return agents[index].trajectory[0];
}