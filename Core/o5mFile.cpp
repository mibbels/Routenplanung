#include "o5mFile.hpp"

namespace Core
{
    void o5mFile::DisplayNode(const Node_t& node)
    {
        if(node.stringTableIndex == 0)
        {
            printf("id: %11lu | lat: %2.7f | lon: %2.7f | Node %8lu \n",
                   node.id, node.lat,
                   node.lon, node.nodeCount);
        }
        else
        {
            printf("id: %11lu | lat: %2.7f | lon: %2.7f | Node %8lu \n",
                   node.id, node.lat,
                   node.lon, node.nodeCount);

            std::cout << "\t\t\t k=\"" << _stringPairTable.at(node.stringTableIndex).first
                      << "\", v=\""    << _stringPairTable.at(node.stringTableIndex).second << "\"\n";
        }
    }

    o5mFile::o5mFile()
    {
        //Init string pair table
        for(uint32_t i = 0; i < STRING_TABLE_SIZE; i++)
        {
            _stringPairTable.at(i) = {"Placeholder (1)", "Placeholder (2)"};
        }
    }

    void o5mFile::ReadIn(const std::string& filepath)
    {
        //Open the file
        std::ifstream file(filepath, std::ios::binary);
        uint64_t fileSize;

        //Temporary storage
        std::vector<uint8_t> rawNodes;

        if(file)
        {
            //Get file size
            file.seekg(0, std::ios::end);
            fileSize = (uint64_t)file.tellg();
            LOG(INFO) << "Opened file: " << filepath << " | Size: " << fileSize << " Byte";
            file.seekg(0, std::ios::beg);

            //Temporary storage
            uint8_t currentByte[1] = {0};

            //Read data byte by byte
            for(uint64_t fileIndex = 0; fileIndex < fileSize; fileIndex++)
            {
                //Read next byte
                file.read((char*)&currentByte[0], sizeof(uint8_t));

                //Process header
                if(fileIndex < 7)
                {
                    _header[fileIndex] = currentByte[0];
                }

                //Process node
                else if(currentByte[0] == 0x10 && fileIndex <= 161095162) //Funktioniert nur bis ≤ 161095162
                {
                    //Read next byte (should be the length of the payload)
                    file.read((char*)&currentByte[0], sizeof(uint8_t));
                    fileIndex++;
                    uint8_t lengthOfData = currentByte[0];

                    //For every byte of node data
                    for(uint32_t j = 0; j < lengthOfData; j++)
                    {
                        //Read next byte
                        file.read((char*)&currentByte[0], sizeof(uint8_t));
                        fileIndex++;

                        rawNodes.push_back(currentByte[0]);
                    }

                    //Create and push_back node
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

                //Process way
                else if(currentByte[0] == 0x11)
                {
                    _wayCount++;
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
        LOG(INFO) << "StringPairCount: " << _currentTableIndex - 1;
        LOG(INFO) << "WayCount: " << _wayCount;
    }

    void o5mFile::DisplayAllNodes()
    {
        LOG(INFO) << "########### DisplayAllNodes() ############";

        for(uint64_t i = 0; i < _nodeVector.size(); i++)
        {
            DisplayNode(_nodeVector.at(i));
        }
    }

    void o5mFile::DisplayFirstThreeNodes()
    {
        LOG(INFO) << "########### DisplayFirstThreeNodes() ############";

        for(uint8_t i = 0; i < 3; i++)
        {
            DisplayNode(_nodeVector.at(i));
        }
    }

    void o5mFile::DisplayLastThreeNodes()
    {
        LOG(INFO) << "########### DisplayLastThreeNodes() ############";

        for(uint64_t i = _nodeVector.size() - 3; i < _nodeVector.size(); i++)
        {
            DisplayNode(_nodeVector.at(i));
        }
    }
}