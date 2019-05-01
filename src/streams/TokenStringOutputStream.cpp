#include <streams/TokenStringOutputStream.h>
#include <sstream>
namespace tokenize
{

TokenStringOutputStream::TokenStringOutputStream(
    BaseCharOutputStream & os_,
    size_t buffer_size_,
    const std::string & separator_)
    : BaseTokenOutputStream(buffer_size_)
    , os(os_)
    , separator(separator_)
{}


bool TokenStringOutputStream::next(TokenPtr token)
{
    if (token)
    {
        std::ostringstream oss;
        oss << "[" << token->getEscapedData() << ", "
            << toString(token->getTokenType());
        if (token->getGraphemTag() != EGraphemTag::UNKNOWN)
            oss <<", " << toString(token->getGraphemTag());
        if (token->getSemanticTag() != ESemanticTag::UNKNOWN)
            oss <<", " << toString(token->getSemanticTag());
        oss << "]" << separator;
        const auto & str = oss.str();
        summary_pending_len += str.length();
        pending.push_back(str);
    }

    return summary_pending_len > buffer_size;
}

void TokenStringOutputStream::write(TokenPtr token)
{
    if (next(token))
    {
        for (auto & elem : pending)
            os.write(elem);
        pending.clear();
        summary_pending_len = 0;
    }

}

void TokenStringOutputStream::flush()
{
    for (auto & elem : pending)
        os.write(elem);
    pending.clear();
    summary_pending_len = 0;
    os.flush();
}
}
