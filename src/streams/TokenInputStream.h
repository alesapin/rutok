#pragma once
#include <core/Token.h>
#include <string>
#include <deque>
#include <iostream>
#include <nonstd/span.hpp>
#include <memory>

namespace tokenize
{
template<typename T>
using Span = nonstd::span<T>;

class TokenInputStream
{
private:
    std::unique_ptr<char[]> buffer;
    std::string appendix;
    std::istream & is;
    std::deque<std::string> segments;
    size_t buffer_size;

protected:
    virtual bool next();

public:
    TokenInputStream(std::istream & is_, size_t buffer_size_ = 4096);
    virtual TokenPtr read();
    virtual bool eof() const;

};
}
