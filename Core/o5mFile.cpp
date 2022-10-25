#include "o5mFile.hpp"

namespace Core
{
    o5mFile::o5mFile()
    {
        //Init string reference table
        for(uint32_t i = 0; i < STRING_TABLE_SIZE; i++)
        {
            _stringPairTable.at(i) = {"Placeholder (1)", "Placeholder (2)"};
        }
    }

    void o5mFile::ReadIn(const std::string& filepath)
    {
        //Open the file
        std::streampos fileSize;
        std::ifstream file(filepath, std::ios::binary);

        //Temporary storage
        std::vector<uint8_t> rawNodes;

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
                    _header[i] = currentByte[0];
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

                        //TODO: Fix node id precision error. Happens between 195k and 198k
                        if(_nodeVector.size() <= 195000)
                        {
                            rawNodes.push_back(currentByte[0]);
                        }
                    }

                    if(_nodeVector.size() <= 195000)
                    {
                        _nodeVector.push_back
                        (
                            Utility::ProcessNode
                            (
                                _nodeVector.size(),
                                rawNodes,
                                lengthOfData,
                                &_stringPairTable,
                                &_currentTableIndex
                            )
                        );

                        rawNodes.clear();
                    }
                }

                //Bloß nicht benutzen... ist scheiße langsam
                //Utility::PrintProgressBar((double)i/(double)fileSize);
                //TODO: File-Parsing & Displaying multithreaden
            }

            LOG(INFO) << "File has been read in!";
        }
        else
        {
            LOG(ERROR) << "File at: " << filepath << " cannot be opened!";
        }
    }

    void o5mFile::DisplayStatistics()
    {
        LOG(INFO) << "########### DisplayStatistics() #######";

        uint8_t headerData0[7] = {0xff, 0xe0, 0x04, 0x6f, 0x35, 0x6d, 0x32};
        uint8_t headerData1[7] = {0xff, 0xe0, 0x04, 0x6f, 0x35, 0x63, 0x32};

        if(Utility::ArrayCompareContent(&_header[0], &headerData0[0], sizeof(headerData0)))
        {
            LOG(INFO) << "Type: 'o5m2'";
        }
        else if(Utility::ArrayCompareContent(&_header[0], &headerData0[1], sizeof(headerData1)))
        {
            LOG(INFO) << "Type: 'o5c2'";
        }
        else
        {
            LOG(ERROR) << "Type: INVALID";
        }

        LOG(INFO) << "NodeCount: " << _nodeVector.size();
        LOG(INFO) << "StringCount: " << _currentTableIndex - 1;
    }

    void o5mFile::DisplayNodes()
    {
        LOG(INFO) << "########### DisplayNodes() ############";

        for(uint32_t i = 0; i < _nodeVector.size(); i++)
        {
            if(_nodeVector.at(i).stringTableIndex == 0)
            {
                printf("id: %11lu | lat: %2.7f | lon: %2.7f | Node %8lu \n",
                       _nodeVector.at(i).id, _nodeVector.at(i).lat,
                       _nodeVector.at(i).lon, _nodeVector.at(i).nodeCount);
            }
            else
            {
                printf("id: %11lu | lat: %2.7f | lon: %2.7f | Node %8lu \n",
                       _nodeVector.at(i).id, _nodeVector.at(i).lat,
                       _nodeVector.at(i).lon, _nodeVector.at(i).nodeCount);

                std::cout << "\t\t\t k=\"" << _stringPairTable.at(_nodeVector.at(i).stringTableIndex).first
                             << "\", v=\"" << _stringPairTable.at(_nodeVector.at(i).stringTableIndex).second << "\"\n";
            }
        }
    }
}