#pragma once
#include <streams/BaseCharInputStream.h>
#include <memory>
#include <iostream>

namespace tokenize
{
class EncodingInputStream : public BaseCharInputStream
{
private:
    std::istream & is;
    std::unique_ptr<char[]> buffer;
    size_t buffer_filled_length{0};
    std::string encoding;

protected:
    bool next() override;
public:
    EncodingInputStream(std::istream & is_, size_t buffer_size_ = 4096)
        : BaseCharInputStream(buffer_size_)
        , is(is_)
        , buffer(new char[buffer_size_])
    {
        if (buffer_size == 0)
            throw std::runtime_error("Read buffer cannot be empty");
    }
    bool read(std::string & data, size_t max_size = 4096) override;
    bool eof() const override;
    const std::string & getEncoding() const { return encoding; }
};
}
