#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <limits>
#include <climits>          //Needed for compilation with linux
#include <unordered_map>
#include <unordered_set>
#include <tuple>
#include <algorithm>
#include <queue>

#include "Datatypes.hpp"
#include "Logger.hpp"

namespace Core
{
    //--------------------------------------------------------------------------------------------------------------------//
    class node
    {
    public:
        node(const int a_iCode);

        int         m_iNameCode;
        uint32_t    nodeCount;
        uint8_t     dataLength;
        uint32_t    id;
        uint8_t     version;
        float       lat;
        float       lon;
    };

    //--------------------------------------------------------------------------------------------------------------------//
    class edge
    {
    public:
        edge(int a_uiWeight, int a_iCodeStart, int a_iCodeEnd);

        int     m_uiWeight;
        int     m_iNameCodeStart;
        int     m_iNameCodeEnd;
    };

    class graph
    {
    public:
        graph(std::vector<node*>& a_vecNodes, std::vector<EdgeComponents>& a_vecEdges);

        void SetStringHashes(std::unordered_map<std::string, int>& a_mapStringHashes);
        void PrintNodes(const std::vector<node*>& a_vecNodes) const;

        node* GetNode(int a_iNameCode) const;
        std::vector<edge*> GetEdge(int a_iNameCode) const;

        std::vector<node*> DijkstraShortestPath(std::string a_strStartNodeName,
                                                std::string a_strEndNodeName) const;

    private:
        // map code to original name
        std::unordered_map<std::string, int>        m_mapStringHashes;
        // map code to node-object
        std::unordered_map<int, node*>              m_mapNodes;
        // map code of starting node to vector of outgoing edges
        std::unordered_map<int, std::vector<edge*>> m_mapEdges;
    };
}

