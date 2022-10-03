#pragma once

#include "Datatypes.hpp"
#include "Logger.hpp"

namespace Core
{
    class Utility
    {
        public:
            static void    Display_ui8vec(const ui8vec& vector, uint8_t numberOfBytes);
            static void    Display_Node(const Node& node);

            static std::vector<uint8_t> DeltaEncode_sInt32(int32_t integer);
            static int32_t              DeltaDecode_sInt32(uint8_t* rawData, uint8_t dataLength);

            static void    PrintProgressBar(double percentage);
    };
}