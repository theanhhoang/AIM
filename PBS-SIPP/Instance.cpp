#include "Instance.h"
#include <iostream>
#include <fstream>
#include <string>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <cstdio>
#include <math.h>

Instance::Instance(const string& map_name)
{
}


int Instance::getNumOfVertices() const
{
    return 0;
}

// NEED ATTENTION HERE: CHANGE THE SEARCH_GRAPH TO INT KEY RATHER THAN THE NAME
void Instance::loadSearchGraph(
    searchGraph_t& searchGraph,

    std::unordered_map<std::string, int>& vNameToID,

    std::vector<vertex_t>& vNameToV,

    std::vector<int>& vNameToDirection,

    std::unordered_map<std::string, edge_t>& eNameToE,
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

            auto fromIter = vNameToID.find(fromName);
            auto toIter = vNameToID.find(toName);

            // auto fromIter = vNameToV.find(fromName);
            // auto toIter = vNameToV.find(toName);


            if (fromIter == vNameToID.end()
                || toIter == vNameToID.end() 
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


position_t Instance::nodeAsPos(const float x, const float y)
{  
    return position_t(x,y);
}