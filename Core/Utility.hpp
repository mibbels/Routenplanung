#pragma once

#include "Datatypes.hpp"
#include "Logger.hpp"

namespace Core
{
    class Utility
    {
        private:
            static uint8_t GetLengthOfValue(const std::vector<uint8_t>& nodeData, uint8_t currentIndex);

        public:
            static bool    BitIsSet(uint32_t value, uint32_t index);
            static bool    ArrayGotValue(const uint8_t* arr, uint8_t length, uint8_t index);
            static bool    ArrayCompareContent(const uint8_t* fileData, const uint8_t* compareData, uint8_t dataSize);
            static Node_t  ProcessNode
                           (
                                uint32_t                    nodeCount,
                                const std::vector<uint8_t>& nodeData,
                                uint8_t                     dataLength,
                                stringPairTable_t*          strPairTable,
                                uint32_t*                   currTableIndex
                           );

            static void    Display_ui8Vec(const std::vector<uint8_t>& vector, uint8_t numberOfBytes);
            static void    Display_ProgressBar(double percentage);

            static std::vector<uint8_t> DeltaEncode_uInt32(uint32_t value);
            static std::vector<uint8_t> DeltaEncode_Int32 (int32_t  value);
            static uint32_t             DeltaDecode_uInt32(const uint8_t* rawData, uint8_t dataLength);
            static int32_t              DeltaDecode_Int32 (const uint8_t* rawData, uint8_t dataLength);
            static double               DeltaDecode_Float(const uint8_t* rawData, uint8_t dataLength);
            static stringPair_t         Decode_StringPair(const uint8_t* rawData);
    };
}