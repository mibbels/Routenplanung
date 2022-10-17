#include "Parser.hpp"

namespace Core
{
    void Parser::ProcessNode(uint32_t nodeCount, uint8_t dataLength, const ui8vec& nodeData)
    {
        static uint32_t currentID = 0;
        static double   currentLat = 0.0;
        static double   currentLon = 0.0;

        uint8_t index = 0;

        //Display raw data stream
        Utility::Display_ui8vec(nodeData, nodeData.size());

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
        uint8_t version = nodeData.at(index - 1);

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

        //Get latitude and longitude
        currentLon += Utility::DeltaDecode_Float(&nodeData.at(firstFloatIndex), firstFloatLength);
        currentLat += Utility::DeltaDecode_Float(&nodeData.at(index), dataLength - index);

        //Create node
        Node_t node
        {
            nodeCount,
            currentID,
            currentLat,
            currentLon,
        };

        Utility::Display_Node(node);
    }

    void Parser::ReadIn_o5m(o5mFile_t& fileStatistics, const std::string& filepath)
    {
        //Open the file
        std::streampos fileSize;
        std::ifstream file(filepath, std::ios::binary);

        ui8vec nodeData0;

        if(file)
        {
            //Get size
            file.seekg(0, std::ios::end);
            fileSize = file.tellg();
            LOG(INFO) << "Opened file: " << filepath << " | Size: " << fileSize << " Byte";
            file.seekg(0, std::ios::beg);

            //Temporary storage
            uint8_t currentByte[1] = {0};

            //Read data byte by byte
            for(uint32_t i = 0; i < fileSize; i++)
            {
                //Read next byte
                file.read((char*)&currentByte[0], sizeof(uint8_t));

                //Process header
                if(i < 7)
                {
                    fileStatistics.header[i] = currentByte[0];
                }

                //Process node
                else if(currentByte[0] == 0x10)
                {
                    //Increase node count
                    fileStatistics.nodeCount++;

                    //Read next byte (should be the length of the payload)
                    file.read((char*)&currentByte[0], sizeof(uint8_t));
                    fileSize -= 1;
                    uint8_t lengthOfData = currentByte[0];

                    //For every byte of node data
                    for(uint32_t j = 0; j < lengthOfData; j++)
                    {
                        //Read next byte
                        file.read((char*)&currentByte[0], sizeof(uint8_t));
                        fileSize -= 1;

                        if(fileStatistics.nodeCount <= 50)
                        {
                            nodeData0.push_back(currentByte[0]);
                        }
                    }

                    if(fileStatistics.nodeCount <= 50)
                    {
                        ProcessNode(fileStatistics.nodeCount, lengthOfData, nodeData0);
                        nodeData0.clear();
                    }
                }

                //Bloß nicht benutzen... ist scheiße langsam
                //Deswegen =>  TODO: File-Management multithreaden
                //Utility::PrintProgressBar((double)i/(double)fileSize);
            }

            LOG(INFO) << "File has been read in!";
        }
        else
        {
            LOG(ERROR) << "File at: " << filepath << " cannot be opened!";
        }
    }

    void Parser::ShowStatistics(const o5mFile_t& fileStatistics)
    {
        LOG(INFO) << "##### File statistics #####";

        uint8_t headerData0[7] = {0xff, 0xe0, 0x04, 0x6f, 0x35, 0x6d, 0x32};
        uint8_t headerData1[7] = {0xff, 0xe0, 0x04, 0x6f, 0x35, 0x63, 0x32};

        if(Utility::ArrayCompareContent(&fileStatistics.header[0], &headerData0[0], sizeof(headerData0)))
        {
            LOG(INFO) << "Type: 'o5m2'";
        }
        else if(Utility::ArrayCompareContent(&fileStatistics.header[0], &headerData0[1], sizeof(headerData1)))
        {
            LOG(INFO) << "Type: 'o5c2'";
        }
        else
        {
            LOG(ERROR) << "Type: INVALID";
        }

        LOG(INFO) << "NodeCount: " << fileStatistics.nodeCount;
    }
}