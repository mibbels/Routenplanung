#pragma once

#include <string>

#include "Datatypes.hpp"
#include "Logger.hpp"

namespace Core
{
    class Parser
    {
        public:
            static void ReadIn_o5m(ui8vec& vector, const std::string& filepath);
            static void Display(const ui8vec& vector, const uint8_t numberOfBytes);
    };
}