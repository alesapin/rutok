#include <streams/SmallGroupsTokenConcatInputStream.h>
namespace tokenize
{

SmallGroupsTokenConcatInputStream::SmallGroupsTokenConcatInputStream(BaseTokenInputStream & in_)
    : BaseTokenConcatInputStream(in_, 3)
{
}

namespace
{
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


    std::vector<const Token *> concated;
    for (size_t i = 0; i < to_concat; ++i)
    {
        concated.emplace_back(window[i].get());
    }
    auto result = Token::concat(concated);
    if (to_concat == window.size())
        return {std::move(result), to_concat, true};

    return {std::move(result), to_concat, false};
}

bool canConcatThreeHyphenWords(const Token * first, const Token * second, const Token * third)
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

bool canConcatWordWrap(const Token * first, const Token * second, const Token * third, const Token * fourth)
{
    return first->getTokenType() == ETokenType::WORD
        && second->getData() == "-"
        && third->getData() == "\n"
        && fourth->getTokenType() == ETokenType::WORD;
}

bool canConcatWordWrap(const Token * first, const Token * second, const Token * third, const Token * fourth, const Token * fifth)
{
    return first->getTokenType() == ETokenType::WORD
        && second->getData() == "-"
        && third->getData() == "\n"
        && fourth->getTokenType() == ETokenType::SEPARATOR
        && fifth->getTokenType() == ETokenType::WORD;
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

        if (canConcatThreeHyphenWords(window[i].get(), window[i + 1].get(), window[i + 2].get()))
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
        std::vector<const Token *> to_concat;
        for (size_t i = 0; i < concat_size; ++i)
            to_concat.emplace_back(window[i].get());
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
        return {EConcatStatus::CONCATED, std::move(excl), excl_size};

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
        return {EConcatStatus::CONCATED, std::move(token), size};

    if (window.size() < 4)
    {
        if (last)
            return {EConcatStatus::NOT_CONCATED, nullptr, 0};
        else
            return {EConcatStatus::NEED_MORE, nullptr, 0};
    }
    if (canConcatWordWrap(window[0].get(), window[1].get(), window[2].get(), window[3].get()))
        return {EConcatStatus::CONCATED, Token::concat({window[0].get(), window[3].get()}, EGraphemTag::WRAPPED_WORD), 4};

    if (window.size() < 5)
    {
        if (last)
            return {EConcatStatus::NOT_CONCATED, nullptr, 0};
        else
            return {EConcatStatus::NEED_MORE, nullptr, 0};
    }
    if (canConcatWordWrap(window[0].get(), window[1].get(), window[2].get(), window[3].get(), window[4].get()))
        return {EConcatStatus::CONCATED, Token::concat({window[0].get(), window[4].get()}, EGraphemTag::WRAPPED_WORD), 5};

    return {EConcatStatus::NOT_CONCATED, nullptr, 0};
}
}
