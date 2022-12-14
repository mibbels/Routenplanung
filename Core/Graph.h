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
#include "o5mFile.hpp"

namespace Core
{

    struct compare_gt
    {
        bool operator () (const uint64_t & a, const uint64_t & b) const
        { return a > b; }
        static uint64_t max_value()
        { return std::numeric_limits<uint64_t>::min(); }
    };

    typedef stxxl::map<uint64_t, Node_t*, compare_gt, 4096, 4096> mapIntNodePointer;
    typedef stxxl::map<uint64_t, uint64_t, compare_gt, 4096, 4096> mapInt64Int64;

    class graph
    {
    public:
        graph(){};

        void create(o5mFile &a_file);

        std::vector<uint64_t> DijkstraShortestPath(uint64_t a_uiStartOsmID,
                                                   uint64_t a_uiEndOsmID,
                                                   o5mFile &a_file);

        std::vector<uint64_t> a_starShortestPath(uint64_t a_uiStartOsmID,
                                                 uint64_t a_uiEndOsmID,
                                                 o5mFile &a_file);


        //std::vector<node*> BellmanFordShortestPath(std::string a_strStartNodeName,
         //                                          std::string a_strEndNodeName);


    private:

        const nodeVec_t             *m_nodeVec;
        const wayVec_t              *m_wayVec;
        const edgeVec_t             *m_edgeVec;
        const nodeEdgeStorageVec_t  *m_nodeEdgeStorageVector;

        //test with heap-version first
        //std::unordered_map<uint64_t, std::vector<uint64_t>> m_mapEdges;
        std::unordered_map<uint64_t, std::vector<Edge_t>>   m_mapEdges;

        /*
        mapIntEdgeSet m_mapEdges{(mapIntEdgeSet::node_block_type::raw_size) * 3,
                                        (mapIntEdgeSet::leaf_block_type::raw_size) * 3};
        */
    };
}

