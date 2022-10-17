#pragma once

#include "Datatypes.hpp"
#include "Logger.hpp"
#include "Utility.hpp"
#include "Parser.hpp"

namespace Core
{
    class o5mFile
    {
        private:
            uint8_t _header[7]  = {0};
            nodeVec _nodeVector;

        public:
            void ReadIn(const std::string& filepath);
            void DisplayStatistics();
            void DisplayNodes();
    };
}