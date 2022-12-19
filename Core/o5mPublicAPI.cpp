#include "o5mFile.hpp"

namespace Core
{
    // ====================================== PUBLIC ======================================

    o5mFile::o5mFile(const std::string& filepath)
    {
        //Reserve space
        _nodeVector.reserve(1500000);
        _nodeEdgeStorageVector.reserve(1500000);
        _nodeMap.reserve(1500000);
        _wayVector.reserve(300000);
        _edgeVector.reserve(1500000);

        //Init string pair table
        for(uint32_t i = 0; i < STRING_TABLE_SIZE; i++)
        {
            _stringPairTable.at(i) = {"Placeholder (1)", "Placeholder (2)"};
        }

        ResetDeltaCounters();
        ReadIn(filepath);
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

    nodeEdgeStorageVec_t* o5mFile::GetNodeEdgeStorageVector()
    {
        return &_nodeEdgeStorageVector;
    }

    const nodeEdgeStorageVec_t* o5mFile::GetNodeEdgeStorageVectorConst()
    {
        return &_nodeEdgeStorageVector;
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