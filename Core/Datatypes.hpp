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

    typedef stxxl::VECTOR_GENERATOR<uint8_t>::result ui8Vec_t;
    typedef stxxl::VECTOR_GENERATOR<int16_t>::result i16Vec_t;
    typedef stxxl::VECTOR_GENERATOR<int32_t>::result i32Vec_t;
    typedef stxxl::VECTOR_GENERATOR<int64_t>::result i64Vec_t;
    typedef stxxl::VECTOR_GENERATOR<float>::result   f32Vec_t;
    typedef stxxl::VECTOR_GENERATOR<double>::result  f64Vec_t;
    typedef stxxl::VECTOR_GENERATOR<Node_t>::result  nodeVec_t;

    typedef std::pair<std::string, std::string>         stringPair_t;
    typedef std::array<stringPair_t, STRING_TABLE_SIZE> stringPairTable_t;

    typedef std::tuple<int, int, int> EdgeComponents;
}