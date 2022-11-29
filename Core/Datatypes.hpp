#pragma once

#include "stxxl.h"

namespace Core
{
    #define STRING_TABLE_SIZE 20000

    struct __attribute__((__packed__)) Node_t
    {
        uint64_t nodeCount;
        uint64_t id;
        double   lat;
        double   lon;
        uint32_t stringTableIndex;
    };

    struct Way_t
    {
        uint64_t              wayCount;
        uint64_t              id;
        std::vector<uint64_t> nodeRefs;
    };

    struct Edge_t
    {
        uint64_t startNode;
        uint64_t endNode;
        uint64_t weight;
    };

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

    typedef std::tuple<int, int, int> EdgeComponents;
}