#pragma once

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
            uint64_t          _wayCount = 0;
            stringPairTable_t _stringPairTable;
            uint32_t          _currentTableIndex = 1;

            void DisplayNode(const Node_t& node);

        public:
            o5mFile();
            void ReadIn(const std::string& filepath);
            void DisplayStatistics();
            void DisplayAllNodes();
            void DisplayFirstThreeNodes();
            void DisplayLastThreeNodes();
    };
}