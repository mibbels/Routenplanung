#include "Parser.hpp"

namespace Core
{
    bool Parser::CheckForContentAt(const uint8_t* fileData, const uint8_t* compareData, uint32_t dataSize)
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

    void Parser::ProcessNode(uint32_t nodeCount, uint8_t dataLength, const ui8vec& nodeData)
    {
        //Display raw data stream
        Utility::Display_ui8vec(nodeData, nodeData.size());

        //Storage
        uint8_t  version = 0;
        uint8_t  lengthOfId = 0;
        uint32_t id = 0;

        //Find version byte and length of id in raw data
        for(uint32_t i = 0; i < nodeData.size(); i++)
        {
            //By finding 0x00
            if(nodeData.at(i) == 0x00)
            {
                version = nodeData.at(i - 1);
                lengthOfId = i - 1;
                break;
            }
        }

        //Calculate id
        //TODO: Decode delta encoding
        for(uint32_t i = 0; i < lengthOfId; i++)
        {
            id += nodeData.at(i);
        }

        //Create node
        Node node
        {
            nodeCount,
            dataLength,
            id,
            version,
            51.1758304f, //Placeholder for now
            7.238217f,   //Placeholder for now
        };

        Utility::Display_Node(node);
    }

    void Parser::ReadIn_o5m(o5mFile& fileStatistics, const std::string& filepath)
    {
        //Open the file
        std::streampos fileSize;
        std::ifstream file(filepath, std::ios::binary);

        ui8vec nodeData0;
        bool processedNode0 = false;

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

                        if(!processedNode0)
                        {
                            nodeData0.push_back(currentByte[0]);
                        }
                    }

                    if(!processedNode0)
                    {
                        ProcessNode(fileStatistics.nodeCount, lengthOfData, nodeData0);
                    }

                    processedNode0 = true;

                    //Increase node count
                    fileStatistics.nodeCount++;
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

    void Parser::ShowStatistics(const o5mFile& fileStatistics)
    {
        LOG(INFO) << "##### File statistics #####";

        uint8_t headerData0[7] = {0xff, 0xe0, 0x04, 0x6f, 0x35, 0x6d, 0x32};
        uint8_t headerData1[7] = {0xff, 0xe0, 0x04, 0x6f, 0x35, 0x63, 0x32};

        if(CheckForContentAt(&fileStatistics.header[0], &headerData0[0], sizeof(headerData0)))
        {
            LOG(INFO) << "Type: 'o5m2'";
        }
        else if(CheckForContentAt(&fileStatistics.header[0], &headerData0[1], sizeof(headerData1)))
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