#include "Parser.hpp"

namespace Core
{
    Node_t Parser::ProcessNode(uint32_t nodeCount, const std::vector<uint8_t>& nodeData, uint8_t dataLength)
    {
        static uint32_t currentID = 0;
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
            //TODO: Fehlerbehandlung
            //Hier dürften wir nämlich nie landen, ansonsten ist der raw byte stream korrupt
        }

        //Find starting index of second float while remembering starting index of first float
        uint8_t firstFloatIndex = index;
        while(Utility::BitIsSet(nodeData.at(index), 7))
        {
            index++;
        }
        index++;

        //Calculate length of first float
        uint8_t firstFloatLength = index - firstFloatIndex;

        //Get latitude and longitude delta
        currentLon += Utility::DeltaDecode_Float(&nodeData.at(firstFloatIndex), firstFloatLength);
        currentLat += Utility::DeltaDecode_Float(&nodeData.at(index), dataLength - index);

        //Return node
        return
        {
            nodeCount,
            currentID,
            currentLat,
            currentLon,
        };
    }
}