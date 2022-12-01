#include "Graph.h"

namespace Core
{
    /*
    //--------------------------------------------------------------------------------------------------------------------//
    edge* ComponentsToEdge(const EdgeComponents& a_edgeComponents)
    {
        return new edge(std::get<0>(a_edgeComponents), std::get<1>(a_edgeComponents), std::get<2>(a_edgeComponents));
    }

    //--------------------------------------------------------------------------------------------------------------------//
    void graph::SetStringHashes(std::unordered_map<std::string, int>& a_mapStringHashes)
    {
        m_mapStringHashes = a_mapStringHashes;
    }
    */

    //--------------------------------------------------------------------------------------------------------------------//
    void graph::create(o5mFile &a_file)
    {
        m_nodeVec = &a_file._nodeVector;
        m_edgeVec  = &a_file._edgeVector;

        nodeVec_t::const_iterator nodeIter = a_file._nodeVector.begin();
        wayVec_t::const_iterator wayIter = a_file._wayVector_1.begin();
        uint64_t nodeIterCounter = 0, wayIterCounter = 0, iNodeID;

        /* from https://stxxl.org/tags/master/design_vector.html#design_vector_notes:
         * Warning
         * Do not store references to the elements of an external vector. Such references might be
         * invalidated during any following access to elements of the vector
         */

        uint64_t iNodeCode = a_file.GetNodeIndex(10065269349);
        Node_t nodeExample = m_nodeVec->at(iNodeCode);

        /*
         * ########## !!! IMPORTANT !!! ########## // Stand November
         *
         * original goal of this section: map a start-node s to all its outgoing edges
         *      m_mapEdges = (node : vec(edges))
         *      to achieve this I originally sorted a vector of edges by its starting-node, to then
         *      iteratively fill the map by taking a group of edges, until a new starting-node appears
         *
         *      temporal assumptions for the behaviour of "ways":
         *          - Way_t = Edge as a 3-tuple of (start, end, weight)
         *          - Way_t.nodeRefs is sorted -> element 0 and n can be seen as "start-" and "end-node"
         *          - weight is defined by Way_t.nodeRefs.size()
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
            uint64_t iCurrStartNode= currEdge.startNode;

            std::vector<uint64_t> vecNeighbourCodes;
            int iOffset = 1;

           vecNeighbourCodes.push_back(currEdge.endNode);

            while(it + iOffset != m_edgeVec->end())
            {
                Edge_t nextEdge = *(it + iOffset);

                if (iCurrStartNode != nextEdge.startNode)
                {
                    break;
                }
                vecNeighbourCodes.push_back(nextEdge.endNode);
                iOffset++;
            }
            it += iOffset;

            m_mapEdges.insert(std::make_pair(iCurrStartNode, vecNeighbourCodes));
        }
    }

    //--------------------------------------------------------------------------------------------------------------------//
    /*
    std::vector<edge*> graph::GetEdge(const int a_iNameCode) const
    {
        try
        {
            mapIntEdgeSet::const_iterator it = m_mapEdges.find(a_iNameCode);
            return it->second->edges;
        }
        catch (std::out_of_range &err)
        {
            return std::vector<edge*>{};
        }
    }

    //--------------------------------------------------------------------------------------------------------------------//
    node* graph::GetNode(const int a_iNameCode) const
    {
        try
        {
            //return m_mapNodes.at(a_iNameCode);
            mapIntNodePointer::const_iterator it = m_mapNodes.find(a_iNameCode);
            return &(*it->second);
        }
        catch (std::out_of_range &err)
        {
            //TODO
        }
    }
    */

    //--------------------------------------------------------------------------------------------------------------------//
    std::vector<uint64_t> graph::DijkstraShortestPath(uint64_t a_iStartNode, uint64_t a_iEndNode)
    {

        std::vector<uint64_t>                   vecShortestPath;
        std::unordered_set<uint64_t>            setVisited;

        std::unordered_map<uint64_t, uint64_t>  mapDistance;
        std::unordered_map<uint64_t, uint64_t>  mapPreviousNode;

        uint64_t                                iNodeCode;

        mapPreviousNode.reserve(m_nodeVec->size());
        mapDistance.reserve(m_nodeVec->size());

        auto compare  = [](std::pair<uint64_t, uint64_t> a, std::pair<uint64_t, uint64_t> b)
                {return a.second > b.second;};

        std::priority_queue<std::pair<uint64_t, uint64_t>, std::vector<std::pair<uint64_t, uint64_t>>,
        decltype(compare)> PQ(compare );

        nodeVec_t::const_iterator it;
        it = m_nodeVec->begin();

        while (it != m_nodeVec->end())
        {
            Node_t currNode = *it;
            iNodeCode= currNode.id; // or .nodeCount

            if (iNodeCode != a_iStartNode)
            {
                mapDistance[iNodeCode] = INT_MAX/2; //!!!!!!!!!!!!!!!!!!!
            }
            else
            {
                mapDistance[iNodeCode] = 0;
            }

            mapPreviousNode[iNodeCode] = NULL;
            PQ.push(std::make_pair(iNodeCode, mapDistance[iNodeCode]));

            it++;
        }

        std::vector<uint64_t> vecNeighbours;
        int iAltDistance = 0;

        while(PQ.size() != 0)
        {
            std::pair<int, int> PQNodePair = PQ.top();
            PQ.pop();


            // keep track of visited nodes, as to not unnecessarily visit old elements (see further down)

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
                vecNeighbours = m_mapEdges[PQNodePair.first];
            }
            catch(std::out_of_range)
            {
                continue;
            }

            for (uint64_t iEndNode : vecNeighbours)
            {
                uint64_t iWeight = 0;
                iAltDistance = iWeight + mapDistance.at(PQNodePair.first);
                if (iAltDistance < mapDistance.at(iEndNode))
                {
                    mapDistance.at(iEndNode) = iAltDistance;
                    mapPreviousNode[iEndNode] = PQNodePair.first;

                     //std::priority_queue does not implement a decrease-key type function.
                     // old element remains as dead weight.

                    PQ.push(std::make_pair(iEndNode, iAltDistance));
                }

            }
        }

        iNodeCode = a_iEndNode;
        while (iNodeCode != a_iStartNode)
        {
            vecShortestPath.push_back(iNodeCode);
            iNodeCode = mapPreviousNode[iNodeCode];

            if (iNodeCode == a_iStartNode)
            {
                vecShortestPath.push_back(iNodeCode);
            }
        }

        std::reverse(vecShortestPath.begin(), vecShortestPath.end());

        //LOG(INFO) << mapDistance[iEndNodeHash];

        return vecShortestPath;
    }

    /*
    //--------------------------------------------------------------------------------------------------------------------//
    int h(int a_iCurrentNode, int a_iEndNode)
    {
        //*
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
