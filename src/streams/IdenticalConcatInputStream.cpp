#include <streams/IdenticalConcatInputStream.h>
namespace tokenize
{
IdenticalConcatInputStream::IdenticalConcatInputStream(BaseTokenInputStream & in_)
    : BaseTokenConcatInputStream(in_, 3)
{
}

ConcatResult IdenticalConcatInputStream::concat(const std::deque<TokenPtr> & window, bool last)
{

    if (window.size() < 2)
    {
        if (last)
            return {EConcatStatus::NOT_CONCATED, nullptr, 0};
        else
            return {EConcatStatus::NEED_MORE, nullptr, 0};
    }

    if (window[0]->getTokenType() != ETokenType::PUNCT
        && window[0]->getTokenType() != ETokenType::SEPARATOR)
        return {EConcatStatus::NOT_CONCATED, nullptr, 0};

    const std::string & first_data = window[0]->getData();
    size_t i;
    for (i = 1; i < window.size(); ++i)
    {
        if (window[i]->getData() != first_data)
            break;
    }
    if (i == 1)
        return {EConcatStatus::NOT_CONCATED, nullptr, 0};

    if (i == window.size() && !last)
        return {EConcatStatus::NEED_MORE, nullptr, 0};

    std::vector<TokenPtr> to_concat{window.begin(), window.begin() + static_cast<long>(i)};
    return {EConcatStatus::CONCATED, Token::concat(to_concat), to_concat.size()};
}
}
