#pragma once

#include "Datatypes.hpp"
#include "Logger.hpp"

namespace Core
{
    class Utility
    {
        private:
            inline static uint64_t _nodeDeltaCounter    = uint64_t();
            inline static uint64_t _wayDeltaCounter     = uint64_t();
            inline static uint64_t _refNodeDeltaCounter = uint64_t();

        public:

            //--- General utility
            static uint64_t GetLengthOfValue(const std::vector<uint8_t>& data, uint64_t currentIndex);
            static bool     BitIsSet(uint32_t value, uint32_t index);
            static bool     ArrayGotValue(const uint8_t* arr, uint8_t length, uint8_t index);
            static bool     ArrayCompareContent(const uint8_t* fileData, const uint8_t* compareData, uint8_t dataSize);

            //--- Data processing
            static void    ResetDeltaCounters();
            static Node_t  ProcessNode
                           (
                                uint64_t                    nodeCount,
                                const std::vector<uint8_t>& nodeData,
                                uint8_t                     dataLength,
                                stringPairTable_t*          strPairTable,
                                uint32_t*                   currTableIndex
                           );
            static Way_t   ProcessWay
                           (
                                uint64_t                    wayCount,
                                const std::vector<uint8_t>& wayData,
                                uint64_t                    dataLength
                           );

            //--- Data visualization
            static void    Display_ui8Vec(const std::vector<uint8_t>& vector, uint8_t numberOfBytes);
            static void    Display_ProgressBar(double percentage);

            //--- Data encoding and decoding
            static std::vector<uint8_t> DeltaEncode_uInt32(uint32_t value);
            static std::vector<uint8_t> DeltaEncode_Int32 (int32_t  value);
            static uint32_t             DeltaDecode_uInt32(const uint8_t* rawData);
            static uint64_t             DeltaDecode_uInt64(const uint8_t* rawData);
            static int32_t              DeltaDecode_Int32 (const uint8_t* rawData);
            static int64_t              DeltaDecode_Int64 (const uint8_t* rawData);
            static double               DeltaDecode_Float (const uint8_t* rawData);
            static stringPair_t         Decode_StringPair (const uint8_t* rawData);
    };
}