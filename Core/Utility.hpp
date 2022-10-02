#pragma once

#include "Datatypes.hpp"
#include "Logger.hpp"

namespace Core
{
    class Utility
    {
        public:
            static void Display_ui8vec(const ui8vec& vector, uint8_t numberOfBytes);
            static void PrintProgressBar(double percentage);
    };
}