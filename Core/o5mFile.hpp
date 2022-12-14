#pragma once

#include <thread>
#include <atomic>
#include <chrono>
#include <map>
#include <algorithm>
#include <mutex>

#include "Datatypes.hpp"
#include "Logger.hpp"
#include "Utility.hpp"

// ========================================================== //
// The implementation of the here defined functions are split //
// into three different source files for visibility reasons.  //
// ========================================================== //

namespace Core
{
    #define MAX_AMOUNT_OF_NODES 17567130
    #define MAX_AMOUNT_OF_WAYS  3137873
    #define TEST_AMOUNT_OF_WAYS 1000
    #define NUMBER_OF_THREADS   16
    #define SAVE_WAYS
    #define SAVE_ALL_EDGES

    class o5mFile
    {
        private:

            uint8_t               _header[7]           = {0};   //File header
            nodeVec_t             _nodeVector;                  //Node storage
            stringPairTable_t     _stringPairTable;             //The string reference table
            uint32_t              _currentTableIndex   = 1;     //Index into the string reference table
            nodeMap_t             _nodeMap;                     //Node mapping (osmID, index)
            wayVec_t              _wayVector;                   //Way storage
            edgeVec_t             _edgeVector;                  //Edge storage
            uint64_t              _nodeDeltaCounter    = 0;     //Delta encoding number for the node id
            uint64_t              _wayDeltaCounter     = 0;     //Delta encoding number for the way id
            uint64_t              _refNodeDeltaCounter = 0;     //Delta encoding number for the referenced nodes in the ways
            std::atomic<bool>     _runDisplayThread    = false; //Starts and stops the display progress thread
            std::atomic<double>   _globalProgress      = 0.0;   //Displayed progress
            std::atomic<uint64_t> _localProgress       = 0;     //Progress local to the chunk size of each thread
            std::mutex            _nodeVectorMutex;             //Mutex to lock the node vector completely

            //--- Internal data processing
            void ProcessEdge(uint64_t edgeIndex);
            void PushEdgesInNodesThread(uint64_t threadCount, uint64_t chunkSize);
            void ResetDeltaCounters();
            void ProcessNode(const std::vector<uint8_t>& nodeData, uint64_t dataLength);
            void ProcessWay (const std::vector<uint8_t>& wayData,  uint64_t dataLength);

            //--- Internal displaying
            void DisplayProgressThread();
            void DisplayNode(const Node_t& node);
            void DisplayWay (const Way_t&  way);
            void DisplayEdge(const Edge_t& edge);

        public:

            //--- Public API
            o5mFile();
            void               ReadIn(const std::string& filepath);
            void               SortEdgesStartAscending();
            void               SortEdgesEndAscending();
            void               PushEdgesInNodes();
            uint64_t           GetNodeIndex(uint64_t osmID);
            nodeVec_t*         GetNodeVector();
            const nodeVec_t*   GetNodeVectorConst();
            stringPairTable_t* GetStringPairTable();
            wayVec_t*          GetWayVector();
            edgeVec_t*         GetEdgeVector();
            const edgeVec_t*   GetEdgeVectorConst();

            //--- Public displaying
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