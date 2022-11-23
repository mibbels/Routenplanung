#include "o5mFile.hpp"

namespace Core
{
    void o5mFile::DisplayNode(const Node_t& node)
    {
        printf("id: %11lu | lat: %2.7f | lon: %2.7f | Node: %8lu \n", node.id, node.lat, node.lon, node.nodeCount);

        if(node.stringTableIndex != 0)
        {
            printf("\t\t\t k=\"%s\", v=\"%s\"\n",
                   _stringPairTable.at(node.stringTableIndex).first.c_str(),
                   _stringPairTable.at(node.stringTableIndex).second.c_str());
        }
    }

    void o5mFile::DisplayWay(const Way_t& way)
    {
        printf("id: %11lu | Way: %8lu \n", way.id, way.wayCount);

        for(auto refs : way.nodeRefs)
        {
            printf("\t\t\t ref=\"%11lu\"\n", refs);
        }
    }

    void o5mFile::ProgressThread()
    {
        while(_runThread)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            Utility::Display_ProgressBar(_fileProgress);
        }

        std::cout << "\n";
        fflush(stdout);
    }

    o5mFile::o5mFile()
    {
        //Reserve space for 20M nodes and 1M ways
        _nodeVector.reserve(20000000);
        _wayVector.reserve(1000000);

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
        std::vector<uint8_t> rawWays;

        //Init
        Utility::ResetDeltaCounters();

        if(file)
        {
            //Get file size
            file.seekg(0, std::ios::end);
            fileSize = (uint64_t)file.tellg();
            LOG(INFO) << "Opened file: " << filepath << " | Size: " << fileSize << " Byte";
            file.seekg(0, std::ios::beg);

            //Temporary storage
            uint8_t currentByte[1] = {0};

            //States
            bool processWays = false;
            bool processSomethingElse = false;

            //Start second thread to display progress
            LOG(INFO) << "Start processing!";
            _runThread = true;
            std::thread t1(ProgressThread);

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
                else if(currentByte[0] == 16 && !processWays)
                {
                    //Read next byte (should be the length of the payload)
                    file.read((char*)&currentByte[0], sizeof(uint8_t));
                    fileIndex++;
                    uint8_t lengthOfData = currentByte[0];

                    if(lengthOfData > 0)
                    {
                        //For every byte of node data
                        for(uint32_t j = 0; j < lengthOfData; j++)
                        {
                            //Read next byte, increment index and save it in a vector
                            file.read((char*)&currentByte[0], sizeof(uint8_t));
                            fileIndex++;
                            rawNodes.push_back(currentByte[0]);
                        }

                        //Create and push back the created node
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

                //Process way
                else if(currentByte[0] == 17 && _wayVector.size() < 1000000) //To read the first 1M ways
                {
                    processWays = true;

                    //Read next byte (should be the length of the payload)
                    file.read((char*)&currentByte[0], sizeof(uint8_t));
                    fileIndex++;
                    uint64_t lengthOfData = currentByte[0];

                    //If the payload length is longer than 1 byte, decode it
                    if(Utility::BitIsSet(lengthOfData, 7))
                    {
                        std::vector<uint8_t> payloadLengthData = {currentByte[0]};

                        while(Utility::BitIsSet(currentByte[0], 7))
                        {
                            file.read((char*)&currentByte[0], sizeof(uint8_t));
                            payloadLengthData.push_back(currentByte[0]);
                            fileIndex++;
                        }

                        lengthOfData = Utility::DeltaDecode_uInt64(&payloadLengthData.at(0));
                    }

                    if(lengthOfData > 0)
                    {
                        //For every byte of way data
                        for(uint32_t j = 0; j < lengthOfData; j++)
                        {
                            //Read next byte, increment index and save it in a vector
                            file.read((char*)&currentByte[0], sizeof(uint8_t));
                            fileIndex++;
                            rawWays.push_back(currentByte[0]);
                        }

                        //std::cout << "\n\n";
                        //Utility::Display_ui8Vec(rawWays, rawWays.size());

                        //Create and push back the created way
                        _wayVector.push_back
                        (
                            Utility::ProcessWay
                            (
                                _wayVector.size(),
                                rawWays,
                                lengthOfData
                            )
                        );

                        rawWays.clear();
                    }
                }

                //Reset byte
                else if(currentByte[0] == 255)
                {
                    Utility::ResetDeltaCounters();
                }

                //Update file progress
                _fileProgress = (double)fileIndex/(double)fileSize;
            }

            //Join thread
            _runThread = false;
            t1.join();

            LOG(INFO) << "File has been read in!";
        }
        else
        {
            LOG(ERROR) << "File at: " << filepath << " cannot be opened!";
        }
    }

    void o5mFile::DisplayStatistics()
    {
        LOG(INFO) << "#############\t DisplayStatistics() \t\t#############";

        uint8_t headerData0[7] = {0xff, 0xe0, 0x04, 0x6f, 0x35, 0x6d, 0x32};
        uint8_t headerData1[7] = {0xff, 0xe0, 0x04, 0x6f, 0x35, 0x63, 0x32};

        if(Utility::ArrayCompareContent(&_header[0], &headerData0[0], sizeof(headerData0)))
        {
            std::cout << "Type:\t\t o5m2\n";
        }
        else if(Utility::ArrayCompareContent(&_header[0], &headerData0[1], sizeof(headerData1)))
        {
            std::cout << "Type:\t\t o5c2\n";
        }
        else
        {
            std::cout << "Type:\t\t INVALID\n";
        }

        std::cout << "NodeCount:\t " << _nodeVector.size() << "\n";
        std::cout << "StringCount: " << _currentTableIndex - 1 << "\n";
        std::cout << "WayCount:\t "  << _wayVector.size() << "\n";
    }

    void o5mFile::DisplayAllNodes()
    {
        LOG(INFO) << "#############\t DisplayAllNodes() \t#############";

        for(const Node_t& node : _nodeVector)
        {
            DisplayNode(node);
        }
    }

    void o5mFile::DisplayFirstThreeNodes()
    {
        LOG(INFO) << "#############\t DisplayFirstThreeNodes() \t#############";

        for(uint8_t i = 0; i < 3; i++)
        {
            DisplayNode(_nodeVector.at(i));
        }
    }

    void o5mFile::DisplayLastThreeNodes()
    {
        LOG(INFO) << "#############\t DisplayLastThreeNodes() \t#############";

        for(uint64_t i = _nodeVector.size() - 3; i < _nodeVector.size(); i++)
        {
            DisplayNode(_nodeVector.at(i));
        }
    }

    void o5mFile::DisplayAllWays()
    {
        LOG(INFO) << "#############\t DisplayAllWays() \t#############";

        for(const Way_t& way : _wayVector)
        {
            DisplayWay(way);
        }
    }

    void o5mFile::DisplayFirstThreeWays()
    {
        LOG(INFO) << "#############\t DisplayFirstThreeWays() \t#############";

        for(uint8_t i = 0; i < 3; i++)
        {
            DisplayWay(_wayVector.at(i));
        }
    }

    void o5mFile::DisplayLastThreeWays()
    {
        LOG(INFO) << "#############\t DisplayLastThreeWays() \t#############";

        for(uint64_t i = _wayVector.size() - 3; i < _wayVector.size(); i++)
        {
            DisplayWay(_wayVector.at(i));
        }
    }
}