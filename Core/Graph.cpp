#include "Graph.h"

namespace Core
{
    //--------------------------------------------------------------------------------------------------------------------//
    node::node(const int a_iCode)
    {
        m_iNameCode =   a_iCode;
        nodeCount =     INT32_MAX;
        dataLength =    INT8_MAX;
        id =            INT32_MAX;
        version =       INT8_MAX;
        lat =           std::numeric_limits<float>::max();
        lon =           std::numeric_limits<float>::max();
    }

    //--------------------------------------------------------------------------------------------------------------------//
    edge::edge(const int a_uiWeight, const int a_iCodeStart, const int a_iCodeEnd)
    {
        m_uiWeight =        a_uiWeight;
        m_iNameCodeStart =  a_iCodeStart;
        m_iNameCodeEnd =    a_iCodeEnd;
    }

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

    //--------------------------------------------------------------------------------------------------------------------//
    graph::graph(std::vector<node*>& a_vecNodes, std::vector<EdgeComponents>& a_vecEdges)
    {
        int iNodeCode;

        for (node* pNode : a_vecNodes)
        {
            //TODO check already inserted or ignore
            iNodeCode = pNode->m_iNameCode;
            m_mapNodes[iNodeCode] = pNode;
        }

        /*
         * sort vector of edges by starting node
         */
        std::sort(a_vecEdges.begin(), a_vecEdges.end(),
                  [](EdgeComponents a, EdgeComponents b)
                  {
                      return std::get<1>(a) < std::get<1>(b);
                  });

        std::vector<EdgeComponents>::iterator it;
        it = a_vecEdges.begin();

        /*
         * find all edges belonging to starting node by iterating until new starting node is found
         */
        while (it != a_vecEdges.end())
        {
            EdgeComponents currComponents = *it;
            int iCurrStartNode= std::get<1>(currComponents);

            std::vector<EdgeComponents> vecNeighbours;
            int iOffset = 1;

            vecNeighbours.push_back(currComponents);

            while(it + iOffset != a_vecEdges.end())
            {
                EdgeComponents nextComponents = *(it + iOffset);

                if (iCurrStartNode != std::get<1>(nextComponents))
                {
                    break;
                }
                vecNeighbours.push_back(nextComponents);
                iOffset++;
            }
            it += iOffset;

            std::vector<edge*> vecNeighbourEdges ;
            for (EdgeComponents components : vecNeighbours)
            {
                vecNeighbourEdges.push_back(ComponentsToEdge(components));
            }
            m_mapEdges[iCurrStartNode] = vecNeighbourEdges;
        }
    }

    //--------------------------------------------------------------------------------------------------------------------//
    std::vector<edge*> graph::GetEdge(const int a_iNameCode) const
    {
        try
        {
            return m_mapEdges.at(a_iNameCode);
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
            return m_mapNodes.at(a_iNameCode);
        }
        catch (std::out_of_range &err)
        {
            //TODO
        }
    }

    //--------------------------------------------------------------------------------------------------------------------//
    std::vector<node*> graph::DijkstraShortestPath(std::string a_strStartNodeName, std::string a_strEndNodeName) const
    {

        std::vector<node*>      vecShortestPath;
        std::unordered_set<int> setVisited;

        int                     iNodeCode;
        int iStartNodeHash =    m_mapStringHashes.at(a_strStartNodeName);
        int iEndNodeHash =      m_mapStringHashes.at(a_strEndNodeName);

        auto compare  = [](std::pair<int, int> a, std::pair<int, int> b){return a.second > b.second;};
        std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, decltype(compare)> PQ(compare );

        std::unordered_map<int, int> mapDistance;
        std::unordered_map<int, int> mapPreviousNode;

        mapPreviousNode.reserve(m_mapNodes.size());
        mapDistance.reserve(m_mapNodes.size());

        for (std::pair<int, node*> NodeEntry: m_mapNodes)
        {
            iNodeCode = NodeEntry.second->m_iNameCode;
            if (iNodeCode != iStartNodeHash)
            {
                mapDistance[iNodeCode] = INT_MAX;
            }
            else
            {
                mapDistance[iNodeCode] = 0;
            }

            mapPreviousNode[iNodeCode] = 0;
            PQ.push(std::make_pair(iNodeCode, mapDistance[iNodeCode]));
        }

        std::vector<edge*> vecNeighbours;
        int iAltDistance = 0;

        while(PQ.size() != 0)
        {
            std::pair<int, int> PQNodePair = PQ.top();
            PQ.pop();

            /*
             * keep track of visited nodes, as to not unnecessarily visit old elements (see further down)
             */
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
                vecNeighbours = m_mapEdges.at(PQNodePair.first);
            }
            catch(std::out_of_range)
            {
                continue;
            }

            for (edge* e : vecNeighbours)
            {
                iAltDistance = e->m_uiWeight + mapDistance.at(PQNodePair.first);
                if (iAltDistance < mapDistance.at(e->m_iNameCodeEnd))
                {
                    mapDistance.at(e->m_iNameCodeEnd) = iAltDistance;
                    mapPreviousNode[e->m_iNameCodeEnd] = PQNodePair.first;
                    /*
                     * std::priority_queue does not implement a decrease-key type function.
                     * old element remains as dead weight.
                     */
                    PQ.push(std::make_pair(e->m_iNameCodeEnd, iAltDistance));
                }
            }
        }

        iNodeCode = iEndNodeHash;
        while (iNodeCode != iStartNodeHash)
        {
            vecShortestPath.push_back(m_mapNodes.at(iNodeCode));
            iNodeCode = mapPreviousNode[iNodeCode];

            if (iNodeCode == iStartNodeHash)
            {
                vecShortestPath.push_back(m_mapNodes.at(iNodeCode));
            }
        }

        std::reverse(vecShortestPath.begin(), vecShortestPath.end());
        return vecShortestPath;
    }

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
}