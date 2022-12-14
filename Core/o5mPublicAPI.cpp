#include "o5mFile.hpp"

namespace Core
{
    // ====================================== PUBLIC ======================================

    o5mFile::o5mFile()
    {
        //Reserve space
        _nodeVector.reserve(20000000);
        _nodeMap.reserve(20000000);
        _wayVector.reserve(4000000);
        _edgeVector.reserve(25000000);

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
        ResetDeltaCounters();

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

            //Start second thread to display progress
            LOG(INFO) << "Start processing ...";
            _runDisplayThread = true;
            std::thread displayThread(&o5mFile::DisplayProgressThread, this);

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
                else if(currentByte[0] == 16 && !processWays && _nodeVector.size() < MAX_AMOUNT_OF_NODES)
                {
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
                        //For every byte of node data
                        for(uint32_t j = 0; j < lengthOfData; j++)
                        {
                            //Read next byte, increment index and save it in a vector
                            file.read((char*)&currentByte[0], sizeof(uint8_t));
                            rawNodes.push_back(currentByte[0]);
                            fileIndex++;
                        }

                        if(!rawNodes.empty())
                        {
                            //Process node (create and save it)
                            ProcessNode(rawNodes, lengthOfData);
                        }

                        //Clear raw data vector
                        rawNodes.clear();
                    }
                }

                //Process way
                else if(currentByte[0] == 17 && _wayVector.size() < TEST_AMOUNT_OF_WAYS)
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
                            rawWays.push_back(currentByte[0]);
                            fileIndex++;
                        }

                        if(!rawWays.empty())
                        {
                            //Process way (save some stats about the way and extract all the edges)
                            ProcessWay(rawWays, lengthOfData);
                        }

                        //Clear raw data vector
                        rawWays.clear();
                    }
                }

                //Reset byte
                else if(currentByte[0] == 255)
                {
                    ResetDeltaCounters();
                }

                //Update file progress
                _globalProgress = (double)fileIndex/(double)fileSize;
            }

            //Join thread
            _runDisplayThread = false;
            displayThread.join();

            LOG(INFO) << "File has been read in!";
        }
        else
        {
            LOG(ERROR) << "File at: " << filepath << " cannot be opened!";
        }
    }

    void o5mFile::SortEdgesStartAscending()
    {
        LOG(INFO) << "Sort start edges ascending ... (this can take up to 40 seconds in debug)";
        std::cout << "\n";

        std::sort(_edgeVector.begin(), _edgeVector.end(),
                  [](const Edge_t& a, const Edge_t& b)
                  {return a.startNode < b.startNode;});
    }

    void o5mFile::SortEdgesEndAscending()
    {
        LOG(INFO) << "Sort end edges ascending ... (this can take up to 40 seconds in debug)";
        std::cout << "\n";

        std::sort(_edgeVector.begin(), _edgeVector.end(),
                  [](const Edge_t& a, const Edge_t& b)
                  {return a.endNode < b.endNode;});
    }

    void o5mFile::PushEdgesInNodes()
    {
        LOG(INFO) << "Start data transforming ...";

        //Create chunks
        uint64_t chunkSize = _edgeVector.size() / NUMBER_OF_THREADS;

        //Init progress variables and create displaying thread
        _globalProgress   = 0;
        _localProgress    = 0;
        _runDisplayThread = true;
        std::thread displayThread(&o5mFile::DisplayProgressThread, this);

        //Storage
        std::thread threads[NUMBER_OF_THREADS];

        LOG(INFO) << "Create " << NUMBER_OF_THREADS << " threads ...";

        //Create threads
        for(uint64_t i = 0; i < NUMBER_OF_THREADS; i++)
        {
            threads[i] = std::thread(&o5mFile::PushEdgesInNodesThread, this, i, chunkSize);
        }

        //Wait for threads to finish
        for(auto& thread : threads)
        {
            thread.join();
        }

        _runDisplayThread = false;
        displayThread.join();

        LOG(INFO) << "Joined all threads!";
        std::cout << "\n";
    }

    uint64_t o5mFile::GetNodeIndex(uint64_t osmID)
    {
        return _nodeMap.at(osmID);
    }

    nodeVec_t* o5mFile::GetNodeVector()
    {
        return &_nodeVector;
    }

    const nodeVec_t* o5mFile::GetNodeVectorConst()
    {
        return &_nodeVector;
    }

    stringPairTable_t* o5mFile::GetStringPairTable()
    {
        return &_stringPairTable;
    }

    wayVec_t* o5mFile::GetWayVector()
    {
        return &_wayVector;
    }

    edgeVec_t* o5mFile::GetEdgeVector()
    {
        return &_edgeVector;
    }

    const edgeVec_t* o5mFile::GetEdgeVectorConst()
    {
        return &_edgeVector;
    }
}