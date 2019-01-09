#include <streams/SmallGroupsTokenConcatInputStream.h>
namespace tokenize
{

SmallGroupsTokenConcatInputStream::SmallGroupsTokenConcatInputStream(BaseTokenInputStream & in_)
    : BaseTokenConcatInputStream(in_, 3)
{
}

namespace
{
/// TODO clear copypaste
std::tuple<TokenPtr, size_t, bool> concatExclamatingQuestion(const std::deque<TokenPtr> & window)
{
    if (window.empty())
        return {nullptr, 0, true};

    if (window.front()->getData() != "!" && window.front()->getData() != "?")
        return {nullptr, 0, false};

    size_t to_concat;
    for (to_concat = 1; to_concat < window.size(); ++to_concat)
    {
        if (window[to_concat]->getData() != "?" && window[to_concat]->getData() != "!")
            break;
    }
    if (to_concat == 1)
        return {nullptr, 0, false};


    std::vector<TokenPtr> concated(window.begin(), window.begin() + static_cast<long>(to_concat));
    auto result = Token::concat(concated);
    if (to_concat == window.size())
        return {result, to_concat, true};

    return {result, to_concat, false};
}

std::tuple<TokenPtr, size_t, bool> concatDots(const std::deque<TokenPtr> & window)
{
    if (window.empty())
        return {nullptr, 0, true};

    if (window.front()->getData() != ".")
        return {nullptr, 0, false};

    size_t to_concat;
    for (to_concat = 1; to_concat < window.size(); ++to_concat)
    {
        if (window[to_concat]->getData() != ".")
            break;
    }
    if (to_concat == 1)
        return {nullptr, 0, false};


    std::vector<TokenPtr> concated(window.begin(), window.begin() + static_cast<long>(to_concat));
    auto result = Token::concat(concated);
    if (to_concat == window.size())
        return {result, to_concat, true};

    return {result, to_concat, false};
}

bool canConcatThreeHyphenWords(TokenPtr first, TokenPtr second, TokenPtr third)
{
    if (second->getData() == "-")
    {
        if (first->getTokenType() == ETokenType::WORD
            && third->getTokenType() == ETokenType::WORD)
        {
            return true;
        }
        else if (first->getTokenType() == ETokenType::NUMBER
            && third->getTokenType() == ETokenType::WORD)
            return true;
        else if (first->getTokenType() == ETokenType::WORD
            && third->getTokenType() == ETokenType::NUMBER)
            return true;
    }
    return false;
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

        if (canConcatThreeHyphenWords(window[i], window[i + 1], window[i + 2]))
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

    if (auto [excl, excl_size, possible_more_excls] = concatExclamatingQuestion(window); possible_more_excls && !last)
        return {EConcatStatus::NEED_MORE, nullptr, 0};
    else if (excl)
        return {EConcatStatus::CONCATED, excl, excl_size};

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

    if (auto [dots, dots_size, possible_more_dots] = concatDots(window); possible_more_dots && !last)
        return {EConcatStatus::NEED_MORE, nullptr, 0};
    else if (dots)
        return {EConcatStatus::CONCATED, dots, dots_size};

    return {EConcatStatus::NOT_CONCATED, nullptr, 0};
}
}
