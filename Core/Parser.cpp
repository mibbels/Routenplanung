#include "Parser.hpp"

namespace Core
{
    bool Parser::CheckForContentAt(const ui8vec& vector, const uint8_t pos, const uint8_t* data, const uint32_t dataSize)
    {
        //Sanity check
        if(dataSize < vector.size())
        {
            for(uint32_t i = 0; i < dataSize; i++)
            {
                if(vector.at(pos + i) != data[i])
                {
                    return false;
                }
            }

            return true;
        }
    }

    void Parser::ReadIn_o5m(ui8vec& vector, const std::string &filepath)
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

            //Reserve space
            for(uint32_t i = 0; i < fileSize; i++)
            {
                uint8_t byte = 0;
                vector.push_back(byte);
            }

            //Read data
            file.read((char*) &vector[0], fileSize);

            LOG(INFO) << "File has been read in!";
        }
        else
        {
            LOG(ERROR) << "File at: " << filepath << " cannot be opened!";
        }
    }

    void Parser::Display_ui8vec(const ui8vec& vector, const uint8_t numberOfBytes)
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

    void Parser::ShowStatistics(const ui8vec& vector)
    {
        LOG(INFO) << "##### File statistics #####";

        uint8_t headerData0[7] = {0xff, 0xe0, 0x04, 0x6f, 0x35, 0x6d, 0x32};
        uint8_t headerData1[7] = {0xff, 0xe0, 0x04, 0x6f, 0x35, 0x63, 0x32};

        if(CheckForContentAt(vector, 0, &headerData0[0], sizeof(headerData0)))
        {
            LOG(INFO) << "Type: 'o5m2'";
        }
        else if(CheckForContentAt(vector, 0, &headerData0[1], sizeof(headerData1)))
        {
            LOG(INFO) << "Type: 'o5c2'";
        }
        else
        {
            LOG(ERROR) << "Type: INVALID";
        }
    }
}