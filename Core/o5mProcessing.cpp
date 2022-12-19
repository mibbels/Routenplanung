#include "o5mFile.hpp"

namespace Core
{
    // ====================================== PRIVATE ======================================

    void o5mFile::ReadIn(const std::string& filepath)
    {
        //Open the file
        std::ifstream file(filepath, std::ios::binary);
        uint64_t fileSize;

        //Temporary storage
        std::vector<uint8_t> rawNodes;
        std::vector<uint8_t> rawWays;

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

                // ============================== Process node ==============================
                else if(currentByte[0] == 16 && !processWays)
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

                // ============================== Process way ==============================
                else if(currentByte[0] == 17)
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

    void o5mFile::ProcessEdge(const Edge_t& edge)
    {
        //Get start and end node + weight
        uint64_t startNode = edge.startNode;
        uint64_t endNode   = edge.endNode;
        uint64_t weight    = edge.weight;

        //Get the indices of both nodes in the vector
        uint64_t startNodeIndex = GetNodeIndex(startNode);
        uint64_t endNodeIndex   = GetNodeIndex(endNode);

        // ==============================//
        // StartNode -> EndNode          //
        // StartNode := outEdge + weight //
        // EndNode   := inEdge           //
        // ==============================//

        //Get the indices of the corresponding in and out arrays
        const uint8_t outEdgeIndex = _nodeEdgeStorageVector.at(startNodeIndex).outEdgesIndex;
        const uint8_t inEdgeIndex  = _nodeEdgeStorageVector.at(endNodeIndex).inEdgesIndex;

        if(outEdgeIndex < MAX_AMOUNT_OF_EDGES && inEdgeIndex < MAX_AMOUNT_OF_EDGES)
        {
            //Set outEdge and weight in the startNode
            _nodeEdgeStorageVector.at(startNodeIndex).outEdges.at(outEdgeIndex)       = endNode;
            _nodeEdgeStorageVector.at(startNodeIndex).outEdgesWeight.at(outEdgeIndex) = weight;

            //Set inEdge in the endNode
            _nodeEdgeStorageVector.at(endNodeIndex).inEdges.at(inEdgeIndex) = startNode;

            //Increment both indices
            _nodeEdgeStorageVector.at(startNodeIndex).outEdgesIndex++;
            _nodeEdgeStorageVector.at(endNodeIndex).inEdgesIndex++;
        }
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
        _nodeEdgeStorageVector.emplace_back(nodeCount);
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

            #ifdef SAVE_ALL_EDGES
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

                    //Save edge in vector
                    else
                    {
                        //Get weight for this edge
                        uint64_t weight = Utility::CalculateWeight(
                                _nodeVector.at(GetNodeIndex(startEdge)),
                                _nodeVector.at(GetNodeIndex(_refNodeDeltaCounter)));

                        //Create edge
                        Edge_t edge{startEdge, _refNodeDeltaCounter, weight};
                        _edgeVector.push_back(edge);

                        //Add edge to nodes
                        ProcessEdge(edge);

                        //Set new starting edge
                        startEdge = _refNodeDeltaCounter;
                    }


                    //Increment the amount of bytes which already got processed
                    index += refNodeIDLength;

                    //Decrease the amount of reference section that is left to be processed
                    lengthOfRefSection -= refNodeIDLength;
                }
            #endif
        }

        //Additional <tags> will be discarded

        #ifdef SAVE_WAYS
            Way_t way{wayCount, _wayDeltaCounter, refNodeCount};
            _wayVector.push_back(way);
        #endif
    }

    void o5mFile::ResetDeltaCounters()
    {
        _nodeDeltaCounter    = 0;
        _wayDeltaCounter     = 0;
        _refNodeDeltaCounter = 0;
    }
}