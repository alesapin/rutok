#pragma once
#include <sstream>
#include <streams/EncodingInputStreamFromStream.h>

namespace tokenize
{
class EncodingInputStreamFromString: public EncodingInputStreamFromStream
{
    std::istringstream iss;
public:
    EncodingInputStreamFromString(const std::string & str_, size_t size = 4096)
        : EncodingInputStreamFromStream(iss, size)
        , iss(str_)
    {
    }
};
}
