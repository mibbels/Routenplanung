#include "Utility.hpp"

namespace Core
{
    #define PBSTR   "############################################################"
    #define PBWIDTH 60

    uint64_t Utility::GetLengthOfValue(const std::vector<uint8_t>& data, uint64_t currentIndex)
    {
        uint64_t startIndex = currentIndex;

        while(Utility::BitIsSet(data.at(currentIndex), 7))
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

    uint64_t Utility::CalculateWeight(const Node_t& n1, const Node_t& n2)
    {
        double dist1 = fabs(n1.lat - n2.lat);
        double dist2 = fabs(n1.lon - n2.lon);

        return (uint64_t)((dist1 + dist2) * WEIGHT_FACTOR);
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
        auto val  = (int32_t)(percentage * 100);
        auto lpad = (int32_t)(percentage * PBWIDTH);
        auto rpad = PBWIDTH - lpad;
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
            if(ArrayGotValue(byteArray, 4, i + 1))
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

    uint32_t Utility::DeltaDecode_uInt32(const uint8_t* rawData)
    {
        uint8_t  currentByte;
        uint8_t  count  = 0;
        uint32_t value  = 0;

        //Do while first bit is set (next byte belongs to the same number)
        do
        {
            currentByte = rawData[count];
            value |= (uint32_t)(currentByte & 0x7f) << (count * 7);
            count++;
        }
        while(currentByte & 0x80);

        return value;
    }

    uint64_t Utility::DeltaDecode_uInt64(const uint8_t* rawData)
    {
        uint8_t  currentByte;
        uint8_t  count = 0;
        uint64_t value = 0LL;

        //Do while first bit is set (next byte belongs to the same number)
        do
        {
            currentByte = rawData[count];
            value |= (long long)(currentByte & 0x7f) << (count * 7);
            count++;
        }
        while(currentByte & 0x80);

        return value;
    }

    int32_t Utility::DeltaDecode_Int32(const uint8_t* rawData)
    {
        uint32_t value = DeltaDecode_uInt32(rawData);

        //Check if lsb is set (indicates negative value)
        if(value & 0x01)
        {
            return -(int32_t)(value >> 1) - 1;
        }
        else
        {
            return (int32_t)(value >> 1);
        }
    }

    int64_t Utility::DeltaDecode_Int64(const uint8_t* rawData)
    {
        uint64_t value = DeltaDecode_uInt64(rawData);

        //Check if lsb is set (indicates negative value)
        if(value & 0x01)
        {
            return -(int64_t)(value >> 1) - 1;
        }
        else
        {
            return (int64_t)(value >> 1);
        }
    }

    double Utility::DeltaDecode_Float(const uint8_t* rawData)
    {
        return (double)DeltaDecode_Int32(rawData) / pow(10.0, 7.0);
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

    uint64_t Utility::Geohash(double _lat, double _long, int precision)
    {
        // --- largely taken from -> https://www.factual.com/blog/how-geohashes-work/

        static char BASE32[32] =
                { '0', '1', '2', '3', '4', '5', '6', '7',
                  '8', '9', 'b', 'c', 'd', 'e', 'f', 'g',
                  'h', 'j', 'k', 'm', 'n', 'p', 'q', 'r',
                  's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };

        bool even = true;
        uint64_t result = 0ULL;

        double latMin = -90.0,  latMax = 90.0;
        double longMin = -180.0, longMax = 180.0;
        double mid;

        for(int i = 0; i < precision* 5; i++)
        {
            if(even)
            {
                mid = (longMin + longMax) * 0.5;
                if (_long < mid)
                {
                    result <<= 1;
                    longMax = mid;
                }
                else
                {
                    longMin = mid;
                    result <<= 1;
                    result |= 1;
                }
            }
            if (!even)
            {
                mid = (latMin + latMax) * 0.5;
                if (_lat < mid)
                {
                    result <<= 1;
                    latMax = mid;
                }
                else
                {
                    latMin = mid;
                    result <<= 1;
                    result |= 1;
                }
            }
            even = !even;
        }

        return result;
    }

    std::pair<double, double> Utility::Geohash_Decode(uint64_t val, int precision)
    {
        bool even = true;
        double _lat = 0.0, _long = 0.0;
        double latMin = -90.0,  latMax = 90.0;
        double longMin = -180.0, longMax = 180.0;
        double mid;
        uint64_t curr_bit = 1ULL;
        uint64_t bitop = 1ULL << (precision * 5 - 1);

        for(int i = 0; i < precision* 5; i++)
        {
            curr_bit = val & bitop;
            curr_bit >>= precision * 5 - 1 - i;
            if(even)
            {
                mid = (longMin + longMax) * 0.5;
                if (curr_bit == 0)
                {
                    longMax = mid;
                }
                else
                {
                    longMin = mid;
                }
                _long = (longMin + longMax) * 0.5;
            }
            if (!even)
            {
                mid = (latMin + latMax) * 0.5;
                if (curr_bit == 0)
                {
                    latMax = mid;
                }
                else
                {
                    latMin = mid;
                }
                _lat = (latMin + latMax) * 0.5;
            }
            even = !even;
            bitop >>= 1;
        }

        return std::make_pair(_lat, _long);
    }

    std::string Utility::Geohash_ToString(uint64_t val, int precision)
    {
        static char BASE32[32] =
                { '0', '1', '2', '3', '4', '5', '6', '7',
                  '8', '9', 'b', 'c', 'd', 'e', 'f', 'g',
                  'h', 'j', 'k', 'm', 'n', 'p', 'q', 'r',
                  's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };

        std::string hashval = "";
        const char bitop = 0b00011111;
        int idx;
        for (int i = 0; i < precision; i++)
        {
            idx = val & bitop;
            hashval = BASE32[idx] + hashval;
            for (int j = 0; j < 5; j++)
                val >>= 1;
            //result >>= bitop;

        }
        return hashval;
    }

    uint64_t Utility::Geohash_FromString(std::string hashval, int precision)
    {

    }
}