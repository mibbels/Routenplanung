#pragma once

#include <string>

#include "Datatypes.hpp"
#include "Logger.hpp"
#include "Utility.hpp"

namespace Core
{
    class Parser
    {
        private:
            static void ProcessNode(uint32_t nodeCount, uint8_t dataLength, const ui8vec& nodeData);

        public:
            static void ReadIn_o5m(o5mFile_t& fileStatistics, const std::string& filepath);
            static void ShowStatistics(const o5mFile_t& fileStatistics);
    };
}