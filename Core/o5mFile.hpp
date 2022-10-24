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
            stringPairTable_t _stringPairTable;
            uint32_t          _currentTableIndex = 1;

        public:
            o5mFile();
            void ReadIn(const std::string& filepath);
            void DisplayStatistics();
            void DisplayNodes();
    };
}