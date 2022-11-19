#pragma once

#include <thread>
#include <atomic>
#include <chrono>

#include "Datatypes.hpp"
#include "Logger.hpp"
#include "Utility.hpp"

namespace Core
{
    class o5mFile
    {
        private:
            uint8_t           _header[7] = {0};
            nodeVec_t         _nodeVector;
            wayVec_t          _wayVector;
            stringPairTable_t _stringPairTable;
            uint32_t          _currentTableIndex = 1;

            void DisplayNode(const Node_t& node);
            void DisplayWay(const Way_t& way);

            //Thread stuff
            inline static std::atomic<bool>   _runThread    = std::atomic<bool>();
            inline static std::atomic<double> _fileProgress = std::atomic<double>();
            static void ProgressThread();

        public:
            o5mFile();
            void ReadIn(const std::string& filepath);
            void DisplayStatistics();

            void DisplayAllNodes();
            void DisplayFirstThreeNodes();
            void DisplayLastThreeNodes();

            void DisplayAllWays();
            void DisplayFirstThreeWays();
            void DisplayLastThreeWays();
    };
}