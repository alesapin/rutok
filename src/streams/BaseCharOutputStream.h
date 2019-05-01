#pragma once
#include <string>

namespace tokenize
{
class BaseCharOutputStream
{
protected:
    size_t buffer_size;
public:
    BaseCharOutputStream(size_t buffer_size_ = 4096)
        : buffer_size(buffer_size_)
    {}

    virtual void write(const std::string & data) = 0;
    virtual ~BaseCharOutputStream() = default;
    virtual void flush() {};
};
}
