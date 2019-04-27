#pragma once
#include <streams/BaseTokenInputStream.h>
#include <streams/BaseCharInputStream.h>
#include <core/Token.h>
#include <string>
#include <deque>
#include <iostream>
#include <memory>

namespace tokenize
{
class TokenInputStream : public BaseTokenInputStream
{
private:
    std::string appendix;
    BaseCharInputStream & input;
    std::deque<std::string> segments;

protected:
    bool next() override;

public:
    TokenInputStream(BaseCharInputStream & input_, size_t buffer_size_ = 4096);
    TokenPtr read() override;
    size_t alreadyReadBytes() const override { return input.alreadyReadBytes(); }
    bool eof() const override;

};
}
