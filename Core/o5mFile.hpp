#pragma once

#include <thread>
#include <atomic>
#include <chrono>
#include <map>
#include <algorithm>

#include "Datatypes.hpp"
#include "Logger.hpp"
#include "Utility.hpp"

namespace Core
{
    class o5mFile
    {
        private:
            uint8_t           _header[7]           = {0};   //File header
            nodeVec_t         _nodeVector;                  //Node storage
            stringPairTable_t _stringPairTable;             //The string reference table
            uint32_t          _currentTableIndex   = 1;     //Index into the string reference table
            nodeMap_t         _nodeMap;                     //Node mapping (osmID, index)
            wayVec_t          _wayVector;                   //Way storage
            edgeVec_t         _edgeVector;                  //Edge storage
            uint64_t          _nodeDeltaCounter    = 0;     //Delta encoding number for the node id
            uint64_t          _wayDeltaCounter     = 0;     //Delta encoding number for the way id
            uint64_t          _refNodeDeltaCounter = 0;     //Delta encoding number for the referenced nodes in the ways

            //--- Internal data processing
            void ResetDeltaCounters();
            void ProcessNode(const std::vector<uint8_t>& nodeData, uint64_t dataLength);
            void ProcessWay (const std::vector<uint8_t>& wayData,  uint64_t dataLength);

            //--- Internal displaying
            void DisplayNode(const Node_t& node);
            void DisplayWay (const Way_t& way);
            void DisplayEdge(const Edge_t& edge);

            //--- Internal Thread stuff
            inline static std::atomic<bool>   _runThread    = std::atomic<bool>();
            inline static std::atomic<double> _fileProgress = std::atomic<double>();
            static void ProgressThread();

        public:
            o5mFile();

            //--- Public API
            void               ReadIn(const std::string& filepath);
            uint64_t           GetNodeIndex(uint64_t osmID);
            nodeVec_t*         GetNodeVector();
            const nodeVec_t*   GetNodeVectorConst();
            stringPairTable_t* GetStringPairTable();
            wayVec_t*          GetWayVector();
            edgeVec_t*         GetEdgeVector();
            const edgeVec_t*   GetEdgeVectorConst();
            void               SortEdgesStartAscending();
            void               SortEdgesEndAscending();

            //--- Displaying
            void DisplayStatistics();

            void DisplayAllNodes();
            void DisplayFirstThreeNodes();
            void DisplayLastThreeNodes();

            void DisplayAllWays();
            void DisplayFirstThreeWays();
            void DisplayLastThreeWays();

            void DisplayAllEdges();
            void DisplayFirstThreeEdges();
            void DisplayLastThreeEdges();

            void DisplayLastThreeStringTableEntries();
    };
}