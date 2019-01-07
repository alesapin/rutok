#include <streams/SmallGroupsTokenConcatInputStream.h>
namespace tokenize
{

SmallGroupsTokenConcatInputStream::SmallGroupsTokenConcatInputStream(BaseTokenInputStream & in_)
    : BaseTokenConcatInputStream(in_, 3)
{
}

namespace
{
TokenPtr concatExclamatingQuestion(TokenPtr first, TokenPtr second)
{
    if (first->getData() == "?" && second->getData() == "!")
        return Token::concat({first, second});
    else if (first->getData() == "!" && second->getData() == "?")
        return Token::concat({first, second});
    return nullptr;
}
TokenPtr concatThreeDots(TokenPtr first, TokenPtr second, TokenPtr third)
{
    if (first->getData() == "." && *first == *second && *first == *third)
        return Token::concat({first, second, third});
    return nullptr;
}

TokenPtr concatThreeHyphenWords(TokenPtr first, TokenPtr second, TokenPtr third)
{
    if (second->getData() == "-")
    {
        if (first->getTokenType() == ETokenType::WORD
            && third->getTokenType() == ETokenType::WORD)
        {
            return Token::concat({first, second, third});
        }
        else if (first->getTokenType() == ETokenType::NUMBER
            && third->getTokenType() == ETokenType::WORD)
            return Token::concat({first, second, third});
        else if (first->getTokenType() == ETokenType::WORD
            && third->getTokenType() == ETokenType::NUMBER)
            return Token::concat({first, second, third});
    }
    return nullptr;
}

std::tuple<TokenPtr, size_t, bool> concatNHyphenWords(const std::deque<TokenPtr> & window)
{
    long concat_size = 0;
    bool possible_more = true;
    for (size_t i = 0; i < window.size(); i += 2)
    {
        if (i + 1 == window.size())
        {
            concat_size = static_cast<long>(window.size());
            break;
        }
        else if (i + 2 == window.size())
        {
            possible_more = static_cast<bool>(window.back()->getData() == "-");
            concat_size = static_cast<long>(window.size()) - 1;
            break;
        }

        if (concatThreeHyphenWords(window[i], window[i + 1], window[i + 2]) != nullptr)
        {
            concat_size += i == 0 ? 3 : 2;
        }
        else
        {
            possible_more = false;
            break;
        }
    }
    if (concat_size != 0)
    {
        std::vector<TokenPtr> to_concat(window.begin(), window.begin() + concat_size);
        return {Token::concat(to_concat), concat_size, possible_more};
    }
    return {nullptr, 0, false};
}

}

ConcatResult SmallGroupsTokenConcatInputStream::concat(const std::deque<TokenPtr> & window, bool last)
{
    if (window.size() < 2)
    {
        if (last)
            return {EConcatStatus::NOT_CONCATED, nullptr, 0};
        else
            return {EConcatStatus::NEED_MORE, nullptr, 0};
    }

    if (auto token = concatExclamatingQuestion(window[0], window[1]); token)
        return {EConcatStatus::CONCATED, token, 2};

    if (window.size() < 3)
    {
        if (last)
            return {EConcatStatus::NOT_CONCATED, nullptr, 0};
        else
            return {EConcatStatus::NEED_MORE, nullptr, 0};
    }

    auto [token, size, possible_next] = concatNHyphenWords(window);
    if (possible_next && !last)
        return {EConcatStatus::NEED_MORE, nullptr, 0};
    else if (token != nullptr)
        return {EConcatStatus::CONCATED, token, size};

    if (auto token = concatThreeDots(window[0], window[1], window[2]); token)
        return {EConcatStatus::CONCATED, token, 3};

    return {EConcatStatus::NOT_CONCATED, nullptr, 0};
}
}
