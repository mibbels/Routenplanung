#pragma once

#include "stxxl.h"

namespace Core
{
    typedef stxxl::VECTOR_GENERATOR<uint8_t>::result ui8vec;
    typedef stxxl::VECTOR_GENERATOR<int16_t>::result i16vec;
    typedef stxxl::VECTOR_GENERATOR<int32_t>::result i32vec;
    typedef stxxl::VECTOR_GENERATOR<int64_t>::result i64vec;
    typedef stxxl::VECTOR_GENERATOR<float>::result   f32vec;
    typedef stxxl::VECTOR_GENERATOR<double>::result  f64vec;

    typedef std::tuple<int, int, int>                EdgeComponents;

    struct o5mFile_t
    {
        uint8_t  header[7] = {0};
        uint32_t nodeCount = 0;
    };

    struct __attribute__((__packed__)) Node_t
    {
        uint32_t nodeCount;
        uint32_t id;
        float    lat;
        float    lon;
        uint8_t  version;
        uint8_t  dataLength;
    };

    struct GPS_t
    {
        float lat;
        float lon;
    };
}