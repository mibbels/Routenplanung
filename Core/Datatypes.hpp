#pragma once

#include "stxxl.h"

namespace Core
{
    #define STRING_TABLE_SIZE   20000
    #define MAX_AMOUNT_OF_EDGES 32

    struct __attribute__((__packed__)) Node_t
    {
        uint64_t index;
        uint64_t osmID;
        double   lat;
        double   lon;
        uint32_t stringTableIndex;
        uint8_t inEdgesIndex;
        uint8_t outEdgesIndex;
        std::array<uint64_t, MAX_AMOUNT_OF_EDGES> inEdges;
        std::array<uint64_t, MAX_AMOUNT_OF_EDGES> outEdges;
        std::array<uint64_t, MAX_AMOUNT_OF_EDGES> outEdgesWeight; //Cost to reach the node located at the corresponding index

        Node_t() = default;
        Node_t(uint64_t p_index, uint64_t p_osmID, double p_lat, double p_lon, uint32_t p_stringTableIndex)
            : index(p_index), osmID(p_osmID), lat(p_lat), lon(p_lon), stringTableIndex(p_stringTableIndex)
        {
            inEdgesIndex   = 0;
            outEdgesIndex  = 0;
            inEdges        = {0};
            outEdges       = {0};
            outEdgesWeight = {0};
        }
    };

    struct Way_t
    {
        uint64_t index;
        uint64_t osmID;
        uint64_t refNodeCount;
    };

    struct Edge_t
    {
        uint64_t startNode;
        uint64_t endNode;
        uint64_t weight;
    };

    typedef stxxl::VECTOR_GENERATOR<bool>::result    booleanVec_t;
    typedef stxxl::VECTOR_GENERATOR<uint8_t>::result ui8Vec_t;
    typedef stxxl::VECTOR_GENERATOR<int16_t>::result i16Vec_t;
    typedef stxxl::VECTOR_GENERATOR<int32_t>::result i32Vec_t;
    typedef stxxl::VECTOR_GENERATOR<int64_t>::result i64Vec_t;
    typedef stxxl::VECTOR_GENERATOR<float>::result   f32Vec_t;
    typedef stxxl::VECTOR_GENERATOR<double>::result  f64Vec_t;
    typedef stxxl::VECTOR_GENERATOR<Node_t>::result  nodeVec_t;
    typedef stxxl::VECTOR_GENERATOR<Way_t>::result   wayVec_t;
    typedef stxxl::VECTOR_GENERATOR<Edge_t>::result  edgeVec_t;

    typedef std::pair<std::string, std::string>         stringPair_t;
    typedef std::array<stringPair_t, STRING_TABLE_SIZE> stringPairTable_t;
    typedef std::unordered_map<uint64_t, uint64_t>      nodeMap_t;          //Node mapping (osmID, index)

    typedef stxxl::tuple<uint64_t, uint64_t> uint64tuple_t;

    struct cmpGreateruint64tuple
    {
        bool operator () (const uint64tuple_t& a, const uint64tuple_t& b) const
        {
            return (a.second > b.second);
        }

        uint64tuple_t min_value() const
        {
            return stxxl::tuple<uint64_t, uint64_t> {UINT64_MAX,UINT64_MAX};
        }
    };

    typedef stxxl::PRIORITY_QUEUE_GENERATOR<uint64tuple_t, cmpGreateruint64tuple, 128*1024*1024, 19000000/1024>::result pqueue_type;


}