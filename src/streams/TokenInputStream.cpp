#include <streams/TokenInputStream.h>
#include <unicorn/library.hpp>
#include <sstream>
#include <cstring>

namespace tokenize
{

using namespace RS;
using namespace RS::Unicorn;
using namespace RS::Unicorn::Literals;

TokenInputStream::TokenInputStream(std::istream & is_, size_t buffer_size_)
    : is(is_)
    , buffer_size (buffer_size_)
    , buffer(new char[buffer_size_])
{
    if (buffer_size_ == 0)
        throw std::runtime_error("Read buffer cannot be empty");

}

bool TokenInputStream::next()
{

    /// A lot of redundant copies
    if (is.eof() && appendix.empty())
        return false;

    size_t buffer_length = 0;

    if (!is.eof())
    {
        is.read(buffer.get(), buffer_size);
        buffer_length += is.gcount();
    }

    std::string tmp(buffer.get(), buffer_length);
    std::string total = appendix + tmp;
    appendix.clear();
    for (auto & word : word_range(total))
        segments.push_back(u_str(word));

    if (!is.eof())
    {
        appendix = segments.back();
        segments.pop_back();
    }
    return true;
}

TokenPtr TokenInputStream::read()
{
    while (segments.empty())
        if (!next())
            return nullptr;

    auto result = Token::createToken(segments.front());
    segments.pop_front();
    return result;
}

bool TokenInputStream::eof() const
{
    return is.eof() && appendix.empty() && segments.empty();
}
}
