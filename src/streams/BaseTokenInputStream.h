#pragma once
#include <core/Token.h>

namespace tokenize
{

class BaseTokenInputStream
{
protected:
    size_t buffer_size;
protected:
    virtual bool next() = 0;

public:
    BaseTokenInputStream(size_t buffer_size_ = 4096)
        : buffer_size(buffer_size_)
    {}
    virtual TokenPtr read() = 0;
    virtual bool eof() const = 0;
};

}
