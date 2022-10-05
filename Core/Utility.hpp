#pragma once

#include "Datatypes.hpp"
#include "Logger.hpp"

namespace Core
{
    class Utility
    {
        private:
            static bool    Bit_Is_Set(uint32_t value, uint32_t index);
            static bool    Array_Has_Value(const uint8_t* arr, uint8_t length, uint8_t index);

        public:
            static void    Display_ui8vec(const ui8vec& vector, uint8_t numberOfBytes);
            static void    Display_Node(const Node& node);

            static std::vector<uint8_t> DeltaEncode_uInt32(uint32_t value);
            //static int32_t              DeltaDecode_sInt32(uint8_t* rawData, uint8_t dataLength);

            static void    PrintProgressBar(double percentage);
    };
}