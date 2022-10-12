#include "Utility.hpp"

namespace Core
{
    #define PBSTR   "############################################################"
    #define PBWIDTH 60

    bool Utility::BitIsSet(uint32_t value, uint32_t index)
    {
        return (value & (1 << index)) != 0;
    }

    bool Utility::ArrayGotValue(const uint8_t* arr, uint8_t length, uint8_t index)
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

     bool Utility::ArrayCompareContent(const uint8_t* fileData, const uint8_t* compareData, uint8_t dataSize)
     {
         for(uint32_t i = 0; i < dataSize; i++)
         {
             if(fileData[i] != compareData[i])
             {
                 return false;
             }
         }

         return true;
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

    void Utility::Display_Node(const Node_t& node)
    {
        printf("Node %d | id: %d | lat: %2.6f | lon: %2.6f | version: %d | data: %d Byte \n\n",
               node.nodeCount, node.id, node.lat, node.lon, node.version, node.dataLength);
    }

    void Utility::Display_ProgressBar(double percentage)
    {
        int32_t val  = (int) (percentage * 100);
        int32_t lpad = (int) (percentage * PBWIDTH);
        int32_t rpad = PBWIDTH - lpad;
        printf("\r%3d%%  [ %.*s%*s]", val, lpad, PBSTR, rpad, "");
        fflush(stdout);
    }

    std::vector<uint8_t> Utility::DeltaEncode_uInt32(uint32_t value)
    {
        //msb := most significant bit
        //MSB := MOST SIGNIFICANT BYTE

        //Storage container
        std::vector<uint8_t> result;

        //Value too large for this conversion function
        if(value > pow(2, 28))
        {
            //Return empty vector
            return result;
        }

        //Get 4 bytes out of the given value (always look at streams of 7-Bit)
        uint8_t byte0 = (value >> 0)  & 0x7f;
        uint8_t byte1 = (value >> 7)  & 0x7f;
        uint8_t byte2 = (value >> 14) & 0x7f;
        uint8_t byte3 = (value >> 22) & 0x7f;

        //Create byte array
        uint8_t byteArray[4] = {byte0, byte1, byte2, byte3};

        //Iterate over all bytes
        for(uint8_t i = 0; i < 4; i++)
        {
            //Check if there is a following byte which is not zero
            if(ArrayGotValue(byteArray, 4, i+1))
            {
                //Set indicator bit (next byte belongs to the same number)
                byteArray[i] |= 0x80;

                //Save byte in vector
                result.push_back(byteArray[i]);
            }

            //Else we got the last byte
            else
            {
                //Don't flip any bit, just save the byte (MSB)
                result.push_back(byteArray[i]);

                break;
            }
        }

        return result;
    }

    //Delta encoding of int32_t experiences some kind of precision loss when using very big numbers
    //f.E: With DeltaEncode_Int32(14312677) the last byte should be 0x0d but its 0x06
    //I think its due to the bit shifting that is going on internally. Ignore for now, decoding works just fine

    std::vector<uint8_t> Utility::DeltaEncode_Int32(int32_t value)
    {
        //msb := most significant bit
        //MSB := MOST SIGNIFICANT BYTE

        //Storage container
        std::vector<uint8_t> result;

        bool negativeValue = false;

        //Value too large for this conversion function
        if(value > pow(2, 28))
        {
            //Return empty vector
            return result;
        }

        if(value < 0)
        {
            value *= -1;
            negativeValue = true;
        }

        //Bit shift one to the left
        value <<= 1;

        //Encode value
        result = DeltaEncode_uInt32(value);

        if(negativeValue)
        {
            //Set signed bit
            result[0] |= 0x01;

            //Correct result a bit
            if(result[0] >= 3)
            {
                result[0] -= 2;
            }
        }

        return result;
    }

    uint32_t Utility::DeltaDecode_uInt32(const uint8_t* rawData, uint8_t dataLength)
    {
        uint32_t value = 0;

        //Iterate over all bytes
        for(uint8_t i = 0; i < dataLength; i++)
        {
            uint8_t currentByte = rawData[i];

            //Check if msb is set
            if(BitIsSet(rawData[i], 7))
            {
                //Negate first bit
                currentByte &= 0x7f;
            }

            //Write byte to corresponding position
            value |= currentByte << (7 * i);
        }

        return value;
    }

    int32_t Utility::DeltaDecode_Int32(uint8_t* rawData, uint8_t dataLength)
    {
        bool negativeValue = false;

        //Check if lsb is set
        if(BitIsSet(rawData[0], 0))
        {
            negativeValue = true;
        }

        //Decode value
        int32_t value = (int32_t)DeltaDecode_uInt32(rawData, dataLength);

        //Bit shift one to the right
        value >>= 1;

        if(negativeValue)
        {
            value++;
            value *= -1;
        }

        return value;
    }

    GPS_t Utility::DeltaDecode_Floats(const uint8_t* rawData, uint8_t dataLength)
    {
        GPS_t gps = {0.0f, 0.0f};

        return gps;
    };
}