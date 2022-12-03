#pragma once

#include "stxxl.h"

namespace Core
{
    #define STRING_TABLE_SIZE 20000

    struct __attribute__((__packed__)) Node_t
    {
        uint64_t index;
        uint64_t osmID;
        double   lat;
        double   lon;
        uint32_t stringTableIndex;
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