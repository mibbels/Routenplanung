#include "Utility.hpp"

namespace Core
{
    #define PBSTR   "############################################################"
    #define PBWIDTH 60

    void Utility::Display_ui8vec(const ui8vec& vector, const uint8_t numberOfBytes)
    {
        //Sanity check
        if(numberOfBytes <= vector.size())
        {
            for(uint32_t i = 0; i < numberOfBytes; i++)
            {
                printf("0x%02x ", vector.at(i));
            }
            printf("\n");
        }
    }

    void Utility::Display_Node(const Node& node)
    {
        printf("Node %d | data: %d byte | id: %d | version: %d | lat: %2.4f | lon: %2.4f\n",
               node.nodeCount, node.dataLength, node.id, node.version, node.lat, node.lon);
    }

    std::vector<uint8_t> Utility::DeltaEncode_sInt32(int32_t integer)
    {
        uint8_t byte0 = integer >> 7;
        uint8_t byte1 = integer & 0x00ff;

        std::vector<uint8_t> res;

        //Disregard bit 7 (most significant bit) of every byte
        byte0 &= 0x7f;
        byte1 &= 0x7f;

        //Set indicator bit (next byte belongs to the same number)
        if(byte0 != 0x00)
        {
            byte1 |= 0x80;
            res.push_back(byte1);
            res.push_back(byte0);

            return res;
        }

        res.push_back(byte1);

        return res;
    }

    int32_t Utility::DeltaDecode_sInt32(uint8_t* rawData, uint8_t dataLength)
    {
        if(dataLength == 1)
        {
            return rawData[0];
        }
        else
        {
            //Set second byte as MSB in the result
            int32_t res = rawData[1] << 7;

            //Delete indicator bit of first byte
            rawData[0] &= 0x7f;

            //Set first byte as LSB in the result
            res |= rawData[0];

            return res;
        }
    }

    void Utility::PrintProgressBar(double percentage)
    {
        int32_t val  = (int) (percentage * 100);
        int32_t lpad = (int) (percentage * PBWIDTH);
        int32_t rpad = PBWIDTH - lpad;
        printf("\r%3d%%  [ %.*s%*s]", val, lpad, PBSTR, rpad, "");
        fflush(stdout);
    }
}