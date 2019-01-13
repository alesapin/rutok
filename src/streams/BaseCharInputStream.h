#pragma once
#include <string>

namespace tokenize
{
class BaseCharInputStream
{
protected:
    size_t buffer_size;
protected:
    virtual bool next() = 0;

public:
    BaseCharInputStream(size_t buffer_size_ = 4096)
        : buffer_size(buffer_size_)
    {}

    virtual bool read(std::string & data, size_t read_size) = 0;
    virtual bool eof() const = 0;
    virtual ~BaseCharInputStream() {}
};
}
