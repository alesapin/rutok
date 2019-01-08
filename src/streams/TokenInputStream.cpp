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
    : BaseTokenInputStream(buffer_size_)
    , is(is_)
    , buffer(new char[buffer_size])
{
    if (buffer_size == 0)
        throw std::runtime_error("Read buffer cannot be empty");

}

std::vector<std::string> fix_dot(const std::string & word)
{
    if (word.length() == 1)
        return {};
    bool dot_found = false;
    bool non_dot_found = false;
    for (const auto & sym : word)
    {
        if (sym != '.')
            non_dot_found = true;
        else if (sym == '.')
            dot_found = true;
    }
    if (dot_found && non_dot_found)
        return str_splitv_by(word, ".");
    return {};
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
    {
        const auto str_word = u_str(word);
        auto fixed = fix_dot(str_word);
        if (!fixed.empty())
        {
            for (size_t i = 0; i < fixed.size() - 1; ++i)
            {
                segments.push_back(fixed[i]);
                segments.push_back(".");
            }
            segments.push_back(fixed.back());
        }
        else
            segments.push_back(str_word);
    }

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
