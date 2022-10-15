#include "Parser.hpp"

namespace Core
{
    void Parser::ProcessNode(uint32_t nodeCount, uint8_t dataLength, const ui8vec& nodeData)
    {
        static uint32_t currentID = 0;
        static double   currentLat = 0.0;
        static double   currentLon = 0.0;

        //Display raw data stream
        //Utility::Display_ui8vec(nodeData, nodeData.size());

        //Storage
        std::vector<uint8_t> rawID;

        uint8_t index = 0;

        //Find 0x00 byte (byte after version) to get the id in raw data
        while(nodeData.at(index + 1) != 0x00)
        {
            rawID.push_back(nodeData.at(index));
            index++;
        }

        //Get id out of raw data stream
        uint32_t id = Utility::DeltaDecode_Int32(&rawID.at(0), rawID.size());

        //Add last id's
        currentID += id;

        //Get version out of raw data stream and increment index
        uint8_t version = nodeData.at(index++);

        //Increment index if current byte is a zero (buffer) byte
        if(nodeData.at(index) == 0x00)
        {
            index++;
        }

        uint8_t secondFloatIndex = 0;

        //Get starting index of second float
        for(uint8_t i = index; i < dataLength - index; i++)
        {
            uint8_t byte = nodeData.at(index);

            if(Utility::BitIsSet(byte, 7))
            {
                secondFloatIndex++;
            }
            else
            {
                break;
            }
        }

        //Get latitude and longitude
        currentLon += Utility::DeltaDecode_Float(&nodeData.at(index), secondFloatIndex);
        index += secondFloatIndex;
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