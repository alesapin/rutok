#pragma once
#include <streams/BaseTokenInputStream.h>
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
    std::unique_ptr<char[]> buffer;
    std::string appendix;
    std::istream & is;
    std::deque<std::string> segments;

protected:
    bool next() override;

public:
    TokenInputStream(std::istream & is_, size_t buffer_size_ = 4096);
    TokenPtr read() override;
    bool eof() const override;

};
}
