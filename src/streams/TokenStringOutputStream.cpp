#include <streams/TokenStringOutputStream.h>
#include <sstream>
namespace tokenize
{

TokenStringOutputStream::TokenStringOutputStream(
    std::ostream & os_,
    BaseTokenInputStream & input_,
    size_t buffer_size_,
    const std::string & separator_)
    : BaseTokenOutputStream(input_, buffer_size_)
    , os(os_)
    , separator(separator_)
{}


bool TokenStringOutputStream::next()
{
    if (input.eof() && pending.empty())
        return false;

    while(summary_pending_len < buffer_size && !input.eof())
    {
        auto token = input.read();
        if (token)
        {
            std::ostringstream oss;
            oss << "[" << token->getData()<< ", "
                << toString(token->getTokenType());
            if (token->getGraphemTag() != EGraphemTag::UNKNOWN)
                oss <<", " << toString(token->getGraphemTag());
            oss << "]" << separator;
            const auto & str = oss.str();
            summary_pending_len += str.length();
            pending.push_back(str);
        }
    }
    return true;
}

bool TokenStringOutputStream::write()
{
    if (pending.empty())
        if (!next())
            return false;

    /// For last character
    if (pending.size() == 1 && input.eof())
        pending.front().pop_back();

    os << pending.front();
    summary_pending_len -= pending.front().length();
    pending.pop_front();
    return true;
}

bool TokenStringOutputStream::eos() const
{
    return pending.empty() && input.eof();
}
}
