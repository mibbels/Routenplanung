#pragma once

#include <string>

#include "Datatypes.hpp"
#include "Logger.hpp"
#include "Utility.hpp"

namespace Core
{
    class Parser
    {
        public:
            static Node_t ProcessNode(uint32_t nodeCount, const std::vector<uint8_t>& nodeData, uint8_t dataLength);
    };
}