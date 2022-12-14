#include "o5mFile.hpp"

namespace Core
{
    // ====================================== PRIVATE ======================================

    void o5mFile::DisplayProgressThread()
    {
        while(_runDisplayThread)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            Utility::Display_ProgressBar(_globalProgress);
        }

        std::cout << "\n";
        fflush(stdout);
    }

    void o5mFile::DisplayNode(const Node_t& node)
    {
        printf("osmID: %11lu | lat: %2.7f | lon: %2.7f | NodeIndex: %8lu \n", node.osmID, node.lat, node.lon, node.index);

        printf("1. InEdge: %11lu | 1. OutEdge: %11lu | Weight: %3lu\n",
               node.inEdges.at(0), node.outEdges.at(0), node.outEdgesWeight.at(0));

        if(node.stringTableIndex != 0)
        {
            printf("\t\t\t k=\"%s\", v=\"%s\"\n",
                   _stringPairTable.at(node.stringTableIndex).first.c_str(),
                   _stringPairTable.at(node.stringTableIndex).second.c_str());
        }
    }

    void o5mFile::DisplayWay(const Way_t& way)
    {
        printf("osmID: %11lu | Referenced nodes: %4lu | WayIndex: %7lu \n", way.osmID, way.refNodeCount, way.index);
    }

    void o5mFile::DisplayEdge(const Edge_t& edge)
    {
        printf("StartNode: %11lu | EndNode: %11lu | Weight: %3lu \n", edge.startNode, edge.endNode, edge.weight);
    }

    // ====================================== PUBLIC ======================================

    void o5mFile::DisplayStatistics()
    {
        std::cout << "\n";
        LOG(INFO) << "#############\t File statistics \t#############";

        uint8_t headerData0[7] = {0xff, 0xe0, 0x04, 0x6f, 0x35, 0x6d, 0x32};
        uint8_t headerData1[7] = {0xff, 0xe0, 0x04, 0x6f, 0x35, 0x63, 0x32};

        if(Utility::ArrayCompareContent(&_header[0], &headerData0[0], sizeof(headerData0)))
        {
            std::cout << "Type:\t\t o5m2\n";
        }
        else if(Utility::ArrayCompareContent(&_header[0], &headerData0[1], sizeof(headerData1)))
        {
            std::cout << "Type:\t\t o5c2\n";
        }
        else
        {
            std::cout << "Type:\t\t INVALID\n";
        }

        std::cout << "NodeCount:\t " << _nodeVector.size() << "\n";
        std::cout << "StringCount: " << _currentTableIndex - 1 << "\n";
        std::cout << "WayCount:\t "  << _wayVector.size() << "\n";
        std::cout << "EdgeCount:\t " << _edgeVector.size() << "\n\n";
    }

    void o5mFile::DisplayAllNodes()
    {
        LOG(INFO) << "#############\t All nodes \t\t#############";

        for(const Node_t& node : _nodeVector)
        {
            DisplayNode(node);
        }

        std::cout << "\n";
    }

    void o5mFile::DisplayFirstThreeNodes()
    {
        LOG(INFO) << "#############\t First 3 nodes \t\t#############";

        for(uint8_t i = 0; i < 3; i++)
        {
            DisplayNode(_nodeVector.at(i));
        }

        std::cout << "\n";
    }

    void o5mFile::DisplayLastThreeNodes()
    {
        LOG(INFO) << "#############\t Last 3 nodes \t\t#############";

        if(_nodeVector.size() > 2)
        {
            for(uint64_t i = _nodeVector.size() - 3; i < _nodeVector.size(); i++)
            {
                DisplayNode(_nodeVector.at(i));
            }
        }

        std::cout << "\n";
    }

    void o5mFile::DisplayAllWays()
    {
        LOG(INFO) << "#############\t All ways \t\t#############";

        for(const Way_t& way : _wayVector)
        {
            DisplayWay(way);
        }

        std::cout << "\n";
    }

    void o5mFile::DisplayFirstThreeWays()
    {
        LOG(INFO) << "#############\t First 3 ways \t\t#############";

        for(uint8_t i = 0; i < 3; i++)
        {
            DisplayWay(_wayVector.at(i));
        }

        std::cout << "\n";
    }

    void o5mFile::DisplayLastThreeWays()
    {
        LOG(INFO) << "#############\t Last 3 ways \t\t#############";

        if(_wayVector.size() > 2)
        {
            for(uint64_t i = _wayVector.size() - 3; i < _wayVector.size(); i++)
            {
                DisplayWay(_wayVector.at(i));
            }
        }

        std::cout << "\n";
    }

    void o5mFile::DisplayAllEdges()
    {
        LOG(INFO) << "#############\t All edges \t\t#############";

        for(const Edge_t& edge : _edgeVector)
        {
            DisplayEdge(edge);
        }

        std::cout << "\n";
    }

    void o5mFile::DisplayFirstThreeEdges()
    {
        LOG(INFO) << "#############\t First 3 edges \t\t#############";

        for(uint8_t i = 0; i < 3; i++)
        {
            DisplayEdge(_edgeVector.at(i));
        }

        std::cout << "\n";
    }

    void o5mFile::DisplayLastThreeEdges()
    {
        LOG(INFO) << "#############\t Last 3 edges \t\t#############";

        if(_edgeVector.size() > 2)
        {
            for(uint64_t i = _edgeVector.size() - 3; i < _edgeVector.size(); i++)
            {
                DisplayEdge(_edgeVector.at(i));
            }
        }

        std::cout << "\n";
    }

    void o5mFile::DisplayLastThreeStringTableEntries()
    {
        LOG(INFO) << "#############\t Last 3 strings \t#############";

        if(_stringPairTable.size() > 2)
        {
            for(uint8_t i = 3; i > 0; i--)
            {
                printf("\t\t\t k=\"%s\", v=\"%s\"\n",
                       _stringPairTable.at(_currentTableIndex - i).first.c_str(),
                       _stringPairTable.at(_currentTableIndex - i).second.c_str());
            }
        }

        std::cout << "\n";
    }
}