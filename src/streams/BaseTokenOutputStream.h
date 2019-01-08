#pragma once

#include <core/Token.h>
#include <streams/BaseTokenInputStream.h>

namespace tokenize
{
class BaseTokenOutputStream
{
protected:
    size_t buffer_size;
    BaseTokenInputStream & input;
protected:
    virtual bool next() = 0;
public:
    BaseTokenOutputStream(BaseTokenInputStream & input_, size_t buffer_size_ = 4096)
        : buffer_size(buffer_size_)
        , input(input_)
    {}
    virtual bool write() = 0;
    virtual bool eos() const = 0;
    virtual void start() {};
    virtual void finish() {};
    virtual void flush() {};
};
}
