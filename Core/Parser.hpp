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
            static bool CheckForContentAt(const uint8_t* fileData, const uint8_t* compareData, uint32_t dataSize);

        public:
            static void ReadIn_o5m(o5mFile& fileStatistics, const std::string& filepath);
            static void ShowStatistics(const o5mFile& fileStatistics);
    };
}