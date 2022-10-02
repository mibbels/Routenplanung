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

    void Parser::ReadIn_o5m(o5mFile& fileStatistics, const std::string& filepath)
    {
        //Open the file
        std::streampos fileSize;
        std::ifstream file(filepath, std::ios::binary);

        if(file)
        {
            //Get size
            file.seekg(0, std::ios::end);
            fileSize = file.tellg();
            LOG(INFO) << "Opened file: " << filepath << " | Size: " << fileSize << " byte";
            file.seekg(0, std::ios::beg);

            //Temporary storage
            uint8_t currentByte[1];

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

                    //printf("Node: ");

                    //For every byte of node data
                    for(uint32_t j = 0; j < lengthOfData; j++)
                    {
                        //Read next byte
                        file.read((char*)&currentByte[0], sizeof(uint8_t));
                        fileSize -= 1;
                        //printf("0x%02x ", currentByte[0]);
                    }

                    //printf("\n");

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