#pragma once

#include "stxxl.h"

namespace Core
{
    struct __attribute__((__packed__)) Node_t
    {
        uint32_t nodeCount;
        uint32_t id;
        double lat;
        double lon;
    };

    typedef stxxl::VECTOR_GENERATOR<uint8_t>::result ui8Vec;
    typedef stxxl::VECTOR_GENERATOR<int16_t>::result i16Vec;
    typedef stxxl::VECTOR_GENERATOR<int32_t>::result i32Vec;
    typedef stxxl::VECTOR_GENERATOR<int64_t>::result i64Vec;
    typedef stxxl::VECTOR_GENERATOR<float>::result f32Vec;
    typedef stxxl::VECTOR_GENERATOR<double>::result f64Vec;
    typedef stxxl::VECTOR_GENERATOR<Node_t>::result nodeVec;

    typedef std::tuple<int, int, int> EdgeComponents;
}