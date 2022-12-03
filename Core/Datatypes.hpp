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

    typedef stxxl::tuple<int64_t, int64_t> int64tuple_t;

    struct cmpLTint64tuple
    {
        //stxxl pqueue requires (min_value(), x) to be true for every x
        //minimum nodeMap-key is 0, min_value() must thus be < 0
        bool operator () (const int64tuple_t& a, const int64tuple_t& b) const
        {
            return (a.second < b.second);
        }

        int64tuple_t min_value() const
        {
            return stxxl::tuple<int64_t, int64_t> {-1,-1};
        }
    };

    typedef stxxl::PRIORITY_QUEUE_GENERATOR<int64tuple_t, cmpLTint64tuple, 128*1024*1024, 19000000/1024>::result pqueue_type;


}