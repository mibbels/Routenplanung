#include "Utility.hpp"

namespace Core
{
    #define PBSTR   "############################################################"
    #define PBWIDTH 60

    uint8_t Utility::GetLengthOfFloat(const std::vector<uint8_t>& nodeData, uint8_t currentIndex)
    {
        uint8_t startIndex = currentIndex;

        while(Utility::BitIsSet(nodeData.at(currentIndex), 7))
        {
            currentIndex++;
        }
        currentIndex++;

        return currentIndex - startIndex;
    }

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

    Node_t Utility::ProcessNode
    (
        uint32_t                    nodeCount,
        const std::vector<uint8_t>& nodeData,
        uint8_t                     dataLength,
        stringPairTable_t*          strPairTable,
        uint32_t*                   currTableIndex
    )
    {

        static uint32_t currentID  = 0;
        static double   currentLat = 0.0;
        static double   currentLon = 0.0;

        uint8_t index = 0;

        //Display raw data stream
        //Utility::Display_ui8Vec(nodeData, nodeData.size());

        //Find index of the first 0x00 byte (it's the byte after the version)
        while(nodeData.at(index) != 0x00)
        {
            index++;
        }

        //Get id delta out of raw data stream
        uint32_t id = Utility::DeltaDecode_Int32(&nodeData.at(0), index - 1);

        //Add id delta
        currentID += id;

        //Get version out of raw data stream
        //uint8_t version = nodeData.at(index - 1);

        //Increment index if current byte is a 0x00 (buffer) byte
        if(nodeData.at(index) == 0x00)
        {
            index++;
        }
        else
        {
            //TODO: Catch error. We should never get here, otherwise the raw data stream got bytes we didn't expect
        }

        //Get length of first float and increment index
        uint8_t firstFloatIndex  = index;
        uint8_t firstFloatLength = Utility::GetLengthOfFloat(nodeData, index);
        index += firstFloatLength;

        //Get length of second float and increment index
        uint8_t secondFloatIndex  = index;
        uint8_t secondFloatLength = Utility::GetLengthOfFloat(nodeData, index);
        index += secondFloatLength;

        //Get latitude and longitude delta
        currentLon += Utility::DeltaDecode_Float(&nodeData.at(firstFloatIndex), firstFloatLength);
        currentLat += Utility::DeltaDecode_Float(&nodeData.at(secondFloatIndex), secondFloatLength);

        //Set index reference to 0 (placeholder string)
        uint32_t oldIndex = 0;

        //If there is still data left to be processed (then there are <tags> with string pairs)
        if(index < dataLength)
        {
            //Get current table index
            oldIndex = *currTableIndex;

            //Get current byte
            uint8_t currentByte = nodeData.at(index);

            //Check if current byte is a 0x00 (buffer) byte
            if(currentByte == 0x00)
            {
                //Increment index
                index++;

                //Decode string pair and save it in table
                strPairTable->at(oldIndex) = Utility::Decode_StringPair(&nodeData.at(index));

                //Increment index
                (*currTableIndex)++;
            }

            //Else it's an index reference to an already saved string pair
            else
            {
                //Subtract delta (byte) from current table index
                oldIndex -= currentByte;
            }
        }

        return
        {
            nodeCount,
            currentID,
            currentLat,
            currentLon,
            oldIndex
        };
    }

    void Utility::Display_ui8Vec(const std::vector<uint8_t>& vector, uint8_t numberOfBytes)
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
        uint8_t byte3 = (value >> 22) & 0x7f; //Bug? Shouldn't it be value >> 21 ??

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

    int32_t Utility::DeltaDecode_Int32(const uint8_t* rawData, uint8_t dataLength)
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

    double Utility::DeltaDecode_Float(const uint8_t* rawData, uint8_t dataLength)
    {
        return (double)DeltaDecode_Int32(rawData, dataLength) / pow(10.0, 7.0);
    }

    //This function returns the first string pair (k, v) from a <tag>
    //If there are additional <tags> they will be discarded (for now)
    stringPair_t Utility::Decode_StringPair(const uint8_t* nodeData)
    {
        uint8_t index = 0;

        //Get ending byte of first string
        uint8_t firstStringIndex = index;

        while(nodeData[index] != 0x00)
        {
            index++;
        }

        //Calculate length of first string
        uint8_t firstStringLength = index - firstStringIndex;

        //Parse first string
        std::string firstString;
        for(uint32_t i = firstStringIndex; i < firstStringLength; i++)
        {
            firstString += (char)nodeData[i];
        }

        //Increment index if current byte is a 0x00 (buffer) byte
        if(nodeData[index] == 0x00)
        {
            index++;
        }
        else
        {
            return {"Error in string decoding", "First string wasn't null terminated"};
        }

        //Get ending byte of second string
        uint8_t secondStringIndex = index;

        while(nodeData[index] != 0x00)
        {
            index++;
        }

        //Calculate length of second string
        uint8_t secondStringLength = index - secondStringIndex;

        //Parse second string
        std::string secondString;
        for(uint32_t i = secondStringIndex; i < secondStringIndex + secondStringLength; i++)
        {
            secondString += (char)nodeData[i];
        }

        //Sanity check
        if(nodeData[index] != 0x00)
        {
            return {"Error in string decoding", "Second string wasn't null terminated"};
        }

        return
        {
            firstString, secondString
        };
    }
}