#pragma once

#include <string>

#include "Datatypes.hpp"
#include "Logger.hpp"

namespace Core
{
    class Parser
    {
        private:
            static bool CheckForContentAt(const ui8vec& vector, const uint8_t pos, const uint8_t* data, const uint32_t dataSize);

        public:
            static void ReadIn_o5m(ui8vec& vector, const std::string& filepath);
            static void Display_ui8vec(const ui8vec& vector, const uint8_t numberOfBytes);
            static void ShowStatistics(const ui8vec& vector);
    };
}