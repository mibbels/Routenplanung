#include "Graph.h"

namespace Core
{
    //--------------------------------------------------------------------------------------------------------------------//
    void graph::create(o5mFile &a_file)
    {
        m_nodeVec = a_file.GetNodeVector();
        m_edgeVec = a_file.GetEdgeVector();

        /* from https://stxxl.org/tags/master/design_vector.html#design_vector_notes:
         * Warning
         * Do not store references to the elements of an external vector. Such references might be
         * invalidated during any following access to elements of the vector
         */



        //sort ways by nodeRefs[0]
        /*
        struct cmp_way
        {
            bool operator() (Way_t a, Way_t b) const
            {
                return a.nodeRefs[0] < b.nodeRefs[0];
            }
            Way_t min_value() const { return Way_t{0,0,{0}}; };
            Way_t max_value() const { return Way_t{0,0,{std::numeric_limits<uint64_t>::max()}}; };
        };

        stxxl::sort(m_wayVec->begin(), m_wayVec->end(), cmp_way(), 256*1024*1024);
        */

        //IF SORTED: find all edges/ways belonging to a start-node by iterating until new start-node is found

        edgeVec_t::const_iterator it;
        it = m_edgeVec->begin();

        while (it != m_edgeVec->end())
        {
            Edge_t currEdge = *it;
            uint64_t iCurrStartNodeOsmID= currEdge.startNode;
            uint64_t iCurrStartNodeIndex= a_file.GetNodeIndex(currEdge.startNode);

            //std::vector<uint64_t> vecNeighbourCodes;
            std::vector<Edge_t> vecNeighbourCodes;
            int iOffset = 1;

           //vecNeighbourCodes.push_back(currEdge.endNode);
            vecNeighbourCodes.push_back(currEdge);

            while(it + iOffset != m_edgeVec->end())
            {
                Edge_t nextEdge = *(it + iOffset);

                if (iCurrStartNodeOsmID != nextEdge.startNode)
                {
                    break;
                }
                //vecNeighbourCodes.push_back(a_file.GetNodeIndex(nextEdge.endNode));
                vecNeighbourCodes.push_back(nextEdge);
                iOffset++;
            }
            it += iOffset;


            //m_mapEdges.insert(std::make_pair(iCurrStartNode, vecNeighbourCodes));
            m_mapEdges.insert(std::make_pair(iCurrStartNodeIndex, vecNeighbourCodes));
        }
        LOG(INFO) << "create graph mapping";
    }

    //--------------------------------------------------------------------------------------------------------------------//
    std::vector<uint64_t> graph::DijkstraShortestPath(uint64_t a_uiStartOsmID, uint64_t a_uiEndOsmID, o5mFile &a_file)
    {

        std::vector<uint64_t>   vecShortestPath;
        booleanVec_t            vecVisited;
        i64Vec_t                vecDistance;
        i64Vec_t                vecPreviousNode;

        uint64_t                uiNodeIndex;
        uint64_t                uiNodeOsmID;
        uint64_t                uiStartIndex = a_file.GetNodeIndex(a_uiStartOsmID);
        uint64_t                uiEndIndex   = a_file.GetNodeIndex(a_uiEndOsmID);

        //vecPreviousNode.reserve(m_nodeVec->size());
        //vecDistance.reserve(m_nodeVec->size());
        //vecVisited.reserve(m_nodeVec->size());

        vecPreviousNode.resize(m_nodeVec->size());
        vecDistance.resize(m_nodeVec->size());
        vecVisited.resize(m_nodeVec->size());
        /*
        auto compare  = [](std::pair<uint64_t, uint64_t> a, std::pair<uint64_t, uint64_t> b)
                {return a.second > b.second;};

        std::priority_queue<std::pair<uint64_t, uint64_t>, std::vector<std::pair<uint64_t, uint64_t>>,
        decltype(compare)> PQ(compare );
         */

        //typedef stxxl::tuple<uint64_t, uint64_t> uint64_pair_type;
        typedef stxxl::tuple<int64_t, int64_t> uint64_pair_type;

        /*
        struct dist_pair
        {
            uint64_pair_type data;
        };
         */

        struct cmp_lt
        {
            bool operator () (const uint64_pair_type& a, const uint64_pair_type& b) const
            {
                return (a.second < b.second);
            }

            uint64_pair_type min_value() const
            {
                return stxxl::tuple<int64_t, int64_t> {-1,-1};
            }
        };

        typedef stxxl::PRIORITY_QUEUE_GENERATOR<uint64_pair_type, cmp_lt, 128*1024*1024, 1024*1024>::result pqueue_type;

        typedef pqueue_type::block_type block_type;
        const unsigned int mem_for_pools = 16 * 1024 * 1024;
        stxxl::read_write_pool<block_type> pool((mem_for_pools / 2) / block_type::raw_size,
                                                (mem_for_pools / 2) / block_type::raw_size);
        pqueue_type PQ(pool);

        //uint64_t iNodeCode = a_file.GetNodeIndex(10065269349);
        //Node_t nodeExample = m_nodeVec->at(iNodeCode);

        nodeVec_t::const_iterator it;
        it = m_nodeVec->begin();

        while (it != m_nodeVec->end())
        {
            Node_t currNode = *it;
            uiNodeIndex= currNode.index;

            if (uiNodeIndex != uiStartIndex)
            {
                vecDistance[uiNodeIndex] = INT64_MAX/2; //!!!!!!!!!!!!!!!!!!!
            }
            else
            {
                vecDistance[uiNodeIndex] = 0;
            }

            vecPreviousNode[uiNodeIndex] = -1;

            PQ.push(uint64_pair_type(uiNodeIndex, vecDistance[uiNodeIndex]));

            it++;
        }
        LOG(INFO) << "filled PQ";

        //std::vector<uint64_t> vecNeighbours;
        std::vector<Edge_t> vecNeighbours;
        uint64_t iAltDistance = 0;

        while(PQ.size() != 0)
        {
            uint64_pair_type pairNodeDist = PQ.top();
            PQ.pop();

            // keep track of visited nodes, as to not unnecessarily visit old elements (see further down)

            if (vecVisited[pairNodeDist.first] == FALSE)
            {
                vecVisited[pairNodeDist.first] = TRUE;
            }
            else
            {
                continue;
            }

            try
            {
                vecNeighbours = m_mapEdges[pairNodeDist.first]; // lookup needed?
            }
            catch(std::out_of_range)
            {
                continue;
            }

            //for (uint64_t iEndNode : vecNeighbours)
            for (Edge_t e : vecNeighbours)
            {
                uint64_t uiWeight = e.weight;
                uint64_t uiEdgeEndNodeIndex = a_file.GetNodeIndex(e.endNode);

                iAltDistance = uiWeight + vecDistance[pairNodeDist.first];

                if (iAltDistance < vecDistance[uiEdgeEndNodeIndex])
                {
                    vecDistance[uiEdgeEndNodeIndex] = iAltDistance;
                    vecPreviousNode[uiEdgeEndNodeIndex] = pairNodeDist.first;

                     //std::priority_queue does not implement a decrease-key type function.
                     // old element remains as dead weight.

                    PQ.push(uint64_pair_type(uiEdgeEndNodeIndex, iAltDistance));
                    LOG(INFO) << "update PQ";
                }

            }
        }

        LOG(INFO) << "found shortest path!";

        uiNodeIndex = uiEndIndex;
        while (uiNodeIndex != uiStartIndex)
        {
            vecShortestPath.push_back(m_nodeVec->at(uiNodeIndex).osmID);
            uiNodeIndex = vecPreviousNode[uiNodeIndex];

            if (uiNodeIndex == uiStartIndex)
            {
                vecShortestPath.push_back(m_nodeVec->at(uiNodeIndex).osmID);
            }
        }

        std::reverse(vecShortestPath.begin(), vecShortestPath.end());

        LOG(INFO) << vecDistance[uiEndIndex];

        return vecShortestPath;
    }

    /*
    //--------------------------------------------------------------------------------------------------------------------//
    int h(int a_iCurrentNode, int a_iEndNode)
    {
        // * heuristic-function estimating the cost of navigating from current- to end-node.
        // * if h(x,y) = 0 for all Nodes x,y, a* algorithm equals dijkstra's algorithm.
        // *
        // * an example for a heuristic could b the absolute or quadratic difference between the x,y coordinates
        // * of the nodes.
        //

         return 0;
    }
    std::vector<node*> graph::a_starShortestPath(std::string a_strStartNodeName, std::string a_strEndNodeName)
    {
        std::vector<node*>      vecShortestPath;
        std::unordered_set<int> setVisited;

        int                     iNodeCode;
        int                     derivedDistance;
        int                     guessedDistance;
        int iStartNodeHash =    m_mapStringHashes.at(a_strStartNodeName);
        int iEndNodeHash =      m_mapStringHashes.at(a_strEndNodeName);

        auto compare  = [](std::pair<int, int> a, std::pair<int, int> b){return a.second > b.second;};
        std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, decltype(compare)> openlist(compare );

        std::vector<edge*> vecNeighbours;
        std::unordered_map<int, int> mapDistance;
        //std::unordered_map<int, int> mapGuessedDistance;
        std::unordered_map<int, int> mapPreviousNode;

        mapPreviousNode.reserve(m_mapNodes.size());
        mapDistance.reserve(m_mapNodes.size());
        //mapGuessedDistance.reserve(m_mapNodes.size());

        for (std::pair<int, node*> NodeEntry: m_mapNodes)
        {
            iNodeCode = NodeEntry.second->m_iNameCode;
            if (iNodeCode != iStartNodeHash)
            {
                mapDistance[iNodeCode] = INT_MAX/2;
                //mapGuessedDistance[iNodeCode] = INT_MAX;
            }
            else
            {
                mapDistance[iNodeCode] = 0;
                //mapGuessedDistance[iNodeCode] = 0;
            }

            mapPreviousNode[iNodeCode] = NULL;
            openlist.push(std::make_pair(iNodeCode, mapDistance[iNodeCode]));
        }

        while (openlist.size() != 0)
        {
            std::pair<int, int> PQNodePair = openlist.top();
            openlist.pop();

            if (setVisited.find(PQNodePair.first) == setVisited.end())
            {
                setVisited.insert(PQNodePair.first);
            }
            else
            {
                continue;
            }

            try
            {
                //vecNeighbours = m_mapEdges.at(PQNodePair.first);
                vecNeighbours = m_mapEdges[PQNodePair.first]->edges;
            }
            catch(std::out_of_range)
            {
                continue;
            }

            for (edge* e : vecNeighbours)
            {

                derivedDistance = mapDistance.at(PQNodePair.first) + e->m_uiWeight;
                if (derivedDistance < mapDistance.at(e->m_iNameCodeEnd))
                {
                    mapPreviousNode[e->m_iNameCodeEnd] = PQNodePair.first;
                    mapDistance.at(e->m_iNameCodeEnd) = derivedDistance;

                    // guess the presumed cost of navigating from the neighbour to the end-node
                    //
                    guessedDistance = derivedDistance + h(e->m_iNameCodeEnd,iEndNodeHash);
                    //mapGuessedDistance.at(e->m_iNameCodeEnd) = guessedDistance; //unnecessary?

                    openlist.push(std::make_pair(e->m_iNameCodeEnd, guessedDistance));
                }
            }
        }

        iNodeCode = iEndNodeHash;
        while (iNodeCode != iStartNodeHash)
        {
            //vecShortestPath.push_back(m_mapNodes.at(iNodeCode));
            vecShortestPath.push_back(m_mapNodes[iNodeCode]);
            iNodeCode = mapPreviousNode.at(iNodeCode);

            if (iNodeCode == iStartNodeHash)
            {
                //vecShortestPath.push_back(m_mapNodes.at(iNodeCode));
                vecShortestPath.push_back(m_mapNodes[iNodeCode]);
            }
        }

        std::reverse(vecShortestPath.begin(), vecShortestPath.end());
        //LOG(INFO) << mapDistance[iEndNodeHash];
        return vecShortestPath;
    }

    //--------------------------------------------------------------------------------------------------------------------//
    std::vector<node*> graph::BellmanFordShortestPath(std::string a_strStartNodeName, std::string a_strEndNodeName)
    {

        std::vector<node*>      vecShortestPath;
        int                     iNodeCode;
        int iStartNodeHash =    m_mapStringHashes.at(a_strStartNodeName);
        int iEndNodeHash =      m_mapStringHashes.at(a_strEndNodeName);

        std::unordered_map<int, int> mapDistance;
        std::unordered_map<int, int> mapPreviousNode;

        mapPreviousNode.reserve(m_mapNodes.size());
        mapDistance.reserve(m_mapNodes.size());

        for (std::pair<int, node*> NodeEntry: m_mapNodes)
        {
            iNodeCode = NodeEntry.second->m_iNameCode;
            if (iNodeCode != iStartNodeHash)
            {
                mapDistance[iNodeCode] = INT_MAX * 0.5;
            }
            else
            {
                mapDistance[iNodeCode] = 0;
            }

            mapPreviousNode[iNodeCode] = NULL;
        }

        for (int i = 1; i < m_mapNodes.size(); i++)
        {
            //for (std::pair<int, std::vector<edge*>> EdgePair : m_mapEdges)
            for (std::pair<int, edgeSet*> EdgePair : m_mapEdges)
            {
                //for (edge* e : EdgePair.second)
                for (edge* e : EdgePair.second->edges)
                {
                    if(e->m_iNameCodeEnd == iStartNodeHash) {
                        auto test1 = mapDistance[iStartNodeHash];
                        auto test2 = mapDistance[e->m_iNameCodeStart];
                        bool b = mapDistance[e->m_iNameCodeStart] + e->m_uiWeight < mapDistance[e->m_iNameCodeEnd];
                        int a = 1;
                    }
                    if (mapDistance[e->m_iNameCodeStart] + e->m_uiWeight < mapDistance[e->m_iNameCodeEnd])
                    {
                        mapDistance[e->m_iNameCodeEnd] = mapDistance[e->m_iNameCodeStart] + e->m_uiWeight;
                        mapPreviousNode[e->m_iNameCodeEnd] = e->m_iNameCodeStart;
                    }
                }
            }
        }


        iNodeCode = iEndNodeHash;
        while (iNodeCode != iStartNodeHash)
        {
            //vecShortestPath.push_back(m_mapNodes.at(iNodeCode));
            vecShortestPath.push_back(m_mapNodes[iNodeCode]);
            iNodeCode = mapPreviousNode[iNodeCode];

            if (iNodeCode == iStartNodeHash)
            {
                //vecShortestPath.push_back(m_mapNodes.at(iNodeCode));
                vecShortestPath.push_back(m_mapNodes[iNodeCode]);
            }
        }

        std::reverse(vecShortestPath.begin(), vecShortestPath.end());
        return vecShortestPath;
    }
    */

    /*
    //--------------------------------------------------------------------------------------------------------------------//
    void graph::PrintNodes(const std::vector<node*>& a_vecNodes) const
    {
        for (node* pNode : a_vecNodes)
        {
            for (auto it = m_mapStringHashes.begin(); it != m_mapStringHashes.end(); it++)
            {
                if (it->second == pNode->m_iNameCode) {
                    LOG(INFO) << it->first;
                    break;
                }
            }
        }
    }
     */
}
