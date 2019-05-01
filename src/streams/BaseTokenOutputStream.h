#pragma once

#include <core/Token.h>
#include <streams/BaseTokenInputStream.h>

namespace tokenize
{
class BaseTokenOutputStream
{
protected:
    size_t buffer_size;
protected:
    virtual bool next(TokenPtr token) = 0;
public:
    BaseTokenOutputStream(size_t buffer_size_ = 4096)
        : buffer_size(buffer_size_)
    {}
    virtual void write(TokenPtr token) = 0;
    virtual void start() {};
    virtual void finish() {};
    virtual void flush() {};
};
}
