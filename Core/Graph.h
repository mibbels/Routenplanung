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

    struct edgeSet
    {
        edgeSet(std::vector<edge*> a_vecEdges){edges = a_vecEdges;};
        std::vector<edge*> edges;
    };

    struct compare_gt
    {
        bool operator () (const int & a, const int & b) const
        { return a > b; }
        static int max_value()
        { return std::numeric_limits<int>::min(); }
    };

    typedef stxxl::map<int, node*, compare_gt, 4096, 4096> mapIntNodePointer;
    typedef stxxl::map<int, edgeSet*, compare_gt, 4096, 4096> mapIntEdgeSet;

    class graph
    {
    public:
        graph(std::vector<node*>& a_vecNodes, std::vector<EdgeComponents>& a_vecEdges);

        void SetStringHashes(std::unordered_map<std::string, int>& a_mapStringHashes);
        void PrintNodes(const std::vector<node*>& a_vecNodes) const;

        node* GetNode(int a_iNameCode) const;
        std::vector<edge*> GetEdge(int a_iNameCode) const;

        std::vector<node*> DijkstraShortestPath(std::string a_strStartNodeName,
                                                std::string a_strEndNodeName);

        std::vector<node*> a_starShortestPath(std::string a_strStartNodeName,
                                              std::string a_strEndNodeName);

        std::vector<node*> BellmanFordShortestPath(std::string a_strStartNodeName,
                                                   std::string a_strEndNodeName);


    private:
        // map code to original name
        std::unordered_map<std::string, int>        m_mapStringHashes;

        // map code to node-object
        mapIntNodePointer  m_mapNodes{(mapIntNodePointer::node_block_type::raw_size) * 3,
        (mapIntNodePointer::leaf_block_type::raw_size) * 3};
        //std::unordered_map<int, node*>              m_mapNodes;

        // map code of starting node to vector of outgoing edges
        //std::unordered_map<int, std::vector<edge*>> m_mapEdges;

        mapIntEdgeSet m_mapEdges{(mapIntEdgeSet::node_block_type::raw_size) * 3,
                                        (mapIntEdgeSet::leaf_block_type::raw_size) * 3};
    };
}

