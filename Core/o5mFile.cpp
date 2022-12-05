#include "o5mFile.hpp"

namespace Core
{
    void o5mFile::ResetDeltaCounters()
    {
        _nodeDeltaCounter    = 0;
        _wayDeltaCounter     = 0;
        _refNodeDeltaCounter = 0;
    }

    void o5mFile::ProcessNode(const std::vector<uint8_t>& nodeData, uint64_t dataLength)
    {
        static double currentLat = 0.0;
        static double currentLon = 0.0;

        uint64_t index     = 0;                  //Index to move through the byte stream
        uint64_t nodeCount = _nodeVector.size(); //Current node index

        //Add id delta to ongoing id value
        _nodeDeltaCounter += Utility::DeltaDecode_Int64(&nodeData.at(index));

        //Increment index while it's not a zero to skip all version bytes
        while(nodeData.at(index) != 0x00)
        {
            index++;
        }

        //Increment index (next byte is beginning of first float)
        index++;

        //Get length of first float and increment index
        uint8_t firstFloatIndex  = index;
        uint8_t firstFloatLength = Utility::GetLengthOfValue(nodeData, index);
        index += firstFloatLength;

        //Get length of second float and increment index
        uint8_t secondFloatIndex  = index;
        uint8_t secondFloatLength = Utility::GetLengthOfValue(nodeData, index);
        index += secondFloatLength;

        //Get latitude and longitude delta
        currentLon += Utility::DeltaDecode_Float(&nodeData.at(firstFloatIndex));
        currentLat += Utility::DeltaDecode_Float(&nodeData.at(secondFloatIndex));

        //Set index reference to 0 (placeholder string)
        uint32_t oldIndex = 0;

        //If there is still data left to be processed (then there are <tags> with string pairs)
        if(index < dataLength)
        {
            //Get current table index
            oldIndex = _currentTableIndex;

            //Get current byte
            uint8_t currentByte = nodeData.at(index);

            //Check if current byte is a 0x00 (buffer) byte
            if(currentByte == 0x00)
            {
                //Increment index
                index++;

                //Decode string pair and save it in table
                _stringPairTable.at(oldIndex) = Utility::Decode_StringPair(&nodeData.at(index));

                //Increment index
                _currentTableIndex++;
            }

                //Else it's an index reference to an already saved string pair
            else
            {
                //Subtract delta (byte) from current table index
                oldIndex -= currentByte;
            }
        }

        //Save node mapping (osmID, index)
        _nodeMap.insert(std::pair<uint64_t, uint64_t>(_nodeDeltaCounter, nodeCount));

        //Save node
        Node_t node{nodeCount, _nodeDeltaCounter, currentLat, currentLon, oldIndex};
        _nodeVector.push_back(node);
    }

    void o5mFile::ProcessWay(const std::vector<uint8_t>& wayData, uint64_t dataLength)
    {
        uint64_t index           = 0;                 //Index to move through the byte stream
        uint64_t refNodeIDLength;                     //Length of every referenced node id
        uint64_t wayCount        = _wayVector.size(); //Current way index
        uint64_t refNodeCount    = 0;                 //Amount of referenced nodes in this way
        uint64_t startEdge       = 0;                 //Starting edge

        //Add id delta to ongoing id value
        _wayDeltaCounter += Utility::DeltaDecode_Int64(&wayData.at(index));

        //Increment index while it's not a zero to skip all version bytes
        while(wayData.at(index) != 0x00)
        {
            index++;
        }

        //Increment index (next byte is the beginning of the length of the reference section)
        index++;

        //Get length of the reference section variable (to know how much to move the index)
        uint64_t lengthOfRefSectionVariable = Utility::GetLengthOfValue(wayData, index);

        //Get length of the "real" reference section (all the data in it)
        uint64_t lengthOfRefSection         = Utility::DeltaDecode_uInt64(&wayData.at(index));

        //Sanity check
        if(index + lengthOfRefSection < dataLength)
        {
            //Increment index accordingly
            index += lengthOfRefSectionVariable;

            //Decode and save the id of every referenced node
            while(lengthOfRefSection > 0)
            {
                //Increase node reference count
                refNodeCount++;

                //Get length of current referenced node
                refNodeIDLength = Utility::GetLengthOfValue(wayData, index);

                //Decode and save the id of the current referenced node
                _refNodeDeltaCounter += Utility::DeltaDecode_Int64(&wayData.at(index));

                //Save starting edge
                if(startEdge == 0)
                {
                    startEdge = _refNodeDeltaCounter;
                }

                //Save edge in vector (lengthOfRefSection as weight is just temporary)
                else
                {
                    Edge_t edge{startEdge, _refNodeDeltaCounter, lengthOfRefSection};
                    _edgeVector.push_back(edge);

                    //Set new starting edge
                    startEdge = _refNodeDeltaCounter;
                }


                //Increment the amount of bytes which already got processed
                index += refNodeIDLength;

                //Decrease the amount of reference section that is left to be processed
                lengthOfRefSection -= refNodeIDLength;
            }
        }

        //Additional <tags> will be discarded

        //Save way
        Way_t way{wayCount, _wayDeltaCounter, refNodeCount};
        _wayVector.push_back(way);
    }

    void o5mFile::DisplayNode(const Node_t& node)
    {
        printf("osmID: %11lu | lat: %2.7f | lon: %2.7f | NodeIndex: %8lu \n", node.osmID, node.lat, node.lon, node.index);

        if(node.stringTableIndex != 0)
        {
            printf("\t\t\t k=\"%s\", v=\"%s\"\n",
                   _stringPairTable.at(node.stringTableIndex).first.c_str(),
                   _stringPairTable.at(node.stringTableIndex).second.c_str());
        }
    }

    void o5mFile::DisplayWay(const Way_t& way)
    {
        printf("osmID: %11lu | Referenced nodes: %4lu | WayIndex: %7lu \n", way.osmID, way.refNodeCount, way.index);
    }

    void o5mFile::DisplayEdge(const Edge_t& edge)
    {
        printf("StartNode: %11lu | EndNode: %11lu | Weight: %3lu \n", edge.startNode, edge.endNode, edge.weight);
    }

    void o5mFile::ProgressThread()
    {
        while(_runThread)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            Utility::Display_ProgressBar(_fileProgress);
        }

        std::cout << "\n";
        fflush(stdout);
    }

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
                else if(currentByte[0] == 16 && !processWays && _nodeVector.size() < 17567130)
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
                else if(currentByte[0] == 17 && _wayVector.size() < 3137873)
                //else if(currentByte[0] == 17 && _wayVector.size() < 90000)
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

    void o5mFile::SortEdgesStartAscending()
    {
        LOG(INFO) << "Sort edges ascending ... (this can take up to 40 seconds in debug)";
        std::cout << "\n";

        std::sort(_edgeVector.begin(), _edgeVector.end(),
                  [](const Edge_t& a, const Edge_t& b)
                  {return a.startNode < b.startNode;});
    }

    void o5mFile::SortEdgesEndAscending()
    {
        std::sort(_edgeVector.begin(), _edgeVector.end(),
                  [](const Edge_t& a, const Edge_t& b)
                  {return a.endNode < b.endNode;});
    }

    void o5mFile::DisplayStatistics()
    {
        LOG(INFO) << "#############\t File statistics \t#############";

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
        std::cout << "EdgeCount:\t " << _edgeVector.size() << "\n\n";
    }

    void o5mFile::DisplayAllNodes()
    {
        LOG(INFO) << "#############\t All nodes \t\t#############";

        for(const Node_t& node : _nodeVector)
        {
            DisplayNode(node);
        }

        std::cout << "\n";
    }

    void o5mFile::DisplayFirstThreeNodes()
    {
        LOG(INFO) << "#############\t First 3 nodes \t\t#############";

        for(uint8_t i = 0; i < 3; i++)
        {
            DisplayNode(_nodeVector.at(i));
        }

        std::cout << "\n";
    }

    void o5mFile::DisplayLastThreeNodes()
    {
        LOG(INFO) << "#############\t Last 3 nodes \t\t#############";

        if(_nodeVector.size() > 2)
        {
            for(uint64_t i = _nodeVector.size() - 3; i < _nodeVector.size(); i++)
            {
                DisplayNode(_nodeVector.at(i));
            }
        }

        std::cout << "\n";
    }

    void o5mFile::DisplayAllWays()
    {
        LOG(INFO) << "#############\t All ways \t\t#############";

        for(const Way_t& way : _wayVector)
        {
            DisplayWay(way);
        }

        std::cout << "\n";
    }

    void o5mFile::DisplayFirstThreeWays()
    {
        LOG(INFO) << "#############\t First 3 ways \t\t#############";

        for(uint8_t i = 0; i < 3; i++)
        {
            DisplayWay(_wayVector.at(i));
        }

        std::cout << "\n";
    }

    void o5mFile::DisplayLastThreeWays()
    {
        LOG(INFO) << "#############\t Last 3 ways \t\t#############";

        if(_wayVector.size() > 2)
        {
            for(uint64_t i = _wayVector.size() - 3; i < _wayVector.size(); i++)
            {
                DisplayWay(_wayVector.at(i));
            }
        }

        std::cout << "\n";
    }

    void o5mFile::DisplayAllEdges()
    {
        LOG(INFO) << "#############\t All edges \t\t#############";

        for(const Edge_t& edge : _edgeVector)
        {
            DisplayEdge(edge);
        }

        std::cout << "\n";
    }

    void o5mFile::DisplayFirstThreeEdges()
    {
        LOG(INFO) << "#############\t First 3 edges \t\t#############";

        for(uint8_t i = 0; i < 3; i++)
        {
            DisplayEdge(_edgeVector.at(i));
        }

        std::cout << "\n";
    }

    void o5mFile::DisplayLastThreeEdges()
    {
        LOG(INFO) << "#############\t Last 3 edges \t\t#############";

        if(_edgeVector.size() > 2)
        {
            for(uint64_t i = _edgeVector.size() - 3; i < _edgeVector.size(); i++)
            {
                DisplayEdge(_edgeVector.at(i));
            }
        }

        std::cout << "\n";;
    }

    void o5mFile::DisplayLastThreeStringTableEntries()
    {
        LOG(INFO) << "#############\t Last 3 strings \t#############";

        if(_stringPairTable.size() > 2)
        {
            for(uint8_t i = 3; i > 0; i--)
            {
                printf("\t\t\t k=\"%s\", v=\"%s\"\n",
                       _stringPairTable.at(_currentTableIndex - i).first.c_str(),
                       _stringPairTable.at(_currentTableIndex - i).second.c_str());
            }
        }

        std::cout << "\n";
    }
}