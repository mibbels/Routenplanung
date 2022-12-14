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
            nodeEdgeStorageVec_t  _nodeEdgeStorageVector;       //Edge storage for all the nodes
            wayVec_t              _wayVector;                   //Way storage
            edgeVec_t             _edgeVector;                  //Edge storage
            uint64_t              _nodeDeltaCounter    = 0;     //Delta encoding number for the node id
            uint64_t              _wayDeltaCounter     = 0;     //Delta encoding number for the way id
            uint64_t              _refNodeDeltaCounter = 0;     //Delta encoding number for the referenced nodes in the ways
            std::atomic<bool>     _runDisplayThread    = false; //Starts and stops the display progress thread
            std::atomic<double>   _globalProgress      = 0.0;   //Displayed progress

            //--- Internal data processing
            void ProcessEdge(const Edge_t& edge);
            void ProcessNode(const std::vector<uint8_t>& nodeData, uint64_t dataLength);
            void ProcessWay (const std::vector<uint8_t>& wayData,  uint64_t dataLength);
            void ResetDeltaCounters();

            //--- Internal displaying
            void DisplayProgressThread();
            void DisplayNode(const Node_t& node);
            void DisplayWay (const Way_t&  way);
            void DisplayEdge(const Edge_t& edge);

        public:

            //--- Public API
            o5mFile();
            void                        ReadIn(const std::string& filepath);
            void                        SortEdgesStartAscending();
            void                        SortEdgesEndAscending();
            uint64_t                    GetNodeIndex(uint64_t osmID);
            nodeVec_t*                  GetNodeVector();
            const nodeVec_t*            GetNodeVectorConst();
            nodeEdgeStorageVec_t*       GetNodeEdgeStorageVector();
            const nodeEdgeStorageVec_t* GetNodeEdgeStorageVectorConst();
            stringPairTable_t*          GetStringPairTable();
            wayVec_t*                   GetWayVector();
            edgeVec_t*                  GetEdgeVector();
            const edgeVec_t*            GetEdgeVectorConst();

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
            void DisplayAllNodeEdges(uint64_t osmID);
    };
}