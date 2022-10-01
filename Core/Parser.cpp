#include "Parser.hpp"

namespace Core
{
    void Parser::ReadIn_o5m(ui8vec& vector, const std::string &filepath)
    {
        //Open the file
        std::streampos fileSize;
        std::ifstream file(filepath, std::ios::binary);

        if(file)
        {
            LOG(INFO) << "Opened file: " << filepath;

            //Get size
            file.seekg(0, std::ios::end);
            fileSize = file.tellg();
            LOG(INFO) << "Filesize: " << fileSize << " byte";
            file.seekg(0, std::ios::beg);

            //Reserve space
            for(uint32_t i = 0; i < fileSize; i++)
            {
                uint8_t byte = 0;
                vector.push_back(byte);
            }

            //Read data
            file.read((char*) &vector[0], fileSize);
        }
        else
        {
            LOG(ERROR) << "File at: " << filepath << " cannot be opened!";
        }
    }

    void Parser::Display(const ui8vec& vector, const uint8_t numberOfBytes)
    {
        if(numberOfBytes <= vector.size())
        {
            for(uint32_t i = 0; i < numberOfBytes; i++)
            {
                printf("0x%02x ", vector.at(i));
            }
        }
    }
}