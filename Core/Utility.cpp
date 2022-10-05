#include "Utility.hpp"

namespace Core
{
    #define PBSTR   "############################################################"
    #define PBWIDTH 60

    bool Utility::Bit_Is_Set(uint32_t value, uint32_t index)
    {
        return (value & (1 << index)) != 0;
    }

    bool Utility::Array_Has_Value(const uint8_t* arr, uint8_t length, uint8_t index)
    {
        for(uint8_t i = index; i < length; i++)
        {
            if(arr[i] != 0)
            {
                return true;
            }
        }

        return false;
    }

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

    std::vector<uint8_t> Utility::DeltaEncode_uInt32(uint32_t value)
    {
        //msb := most significant bit
        //MSB := MOST SIGNIFICANT BYTE

        //Storage container
        std::vector<uint8_t> res;

        //Get bytes out of the given value
        uint8_t byte0 = value & 0x7f;
        uint8_t byte1 = value >> 7;
        uint8_t byte2 = value >> 15;
        uint8_t byte3 = value >> 23;
        uint8_t paddingByte = 0x00;

        //Create byte array
        uint8_t byteArray[5] = {byte0, byte1, byte2, byte3, paddingByte};

        //Iterate over all bytes except the last one
        for(uint8_t i = 0; i < 4; i++)
        {
            //Check if msb is set
            if(Bit_Is_Set(byteArray[i], 7))
            {
                //Add a +1 to the next byte and reset msb
                byteArray[i + 1] |= 0x01;
                byteArray[i] &= 0x7f;
            }
        }

        //Iterate over all bytes except the last one
        for(uint8_t i = 0; i < 4; i++)
        {
            //Check if there is a following byte which is not zero
            if(Array_Has_Value(byteArray, 5, i+1))
            {
                //Set indicator bit (next byte belongs to the same number)
                byteArray[i] |= 0x80;

                //Save byte in vector
                res.push_back(byteArray[i]);
            }

            //Else we got the last byte
            else
            {
                //Don't flip any bit, just save the byte (MSB)
                res.push_back(byteArray[i]);

                break;
            }
        }

        return res;
    }

    /*int32_t Utility::DeltaDecode_sInt32(uint8_t* rawData, uint8_t dataLength)
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
    }*/

    void Utility::PrintProgressBar(double percentage)
    {
        int32_t val  = (int) (percentage * 100);
        int32_t lpad = (int) (percentage * PBWIDTH);
        int32_t rpad = PBWIDTH - lpad;
        printf("\r%3d%%  [ %.*s%*s]", val, lpad, PBSTR, rpad, "");
        fflush(stdout);
    }
}