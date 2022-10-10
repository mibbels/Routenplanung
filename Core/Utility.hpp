#pragma once

#include "Datatypes.hpp"
#include "Logger.hpp"

namespace Core
{
    class Utility
    {
        private:
            static bool    BitIsSet(uint32_t value, uint32_t index);
            static bool    ArrayGotValue(const uint8_t* arr, uint8_t length, uint8_t index);

        public:
            static void    Display_ui8vec(const ui8vec& vector, uint8_t numberOfBytes);
            static void    Display_Node(const Node& node);
            static void    Display_ProgressBar(double percentage);

            static std::vector<uint8_t> DeltaEncode_uInt32(uint32_t value);
            static std::vector<uint8_t> DeltaEncode_Int32 (int32_t  value);
            static uint32_t             DeltaDecode_uInt32(const uint8_t* rawData, uint8_t dataLength);
            static int32_t              DeltaDecode_Int32 (uint8_t* rawData, uint8_t dataLength);
    };
}