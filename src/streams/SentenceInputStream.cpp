#include <streams/SentenceInputStream.h>
#include <iostream>

namespace tokenize
{

namespace
{
enum class ESentenceStatus
{
    TERMINATES,
    NOT_TERMINATES,
    UNKNOWN,
};
enum class ESearchStatus
{
    FOUND,
    NOT_FOUND,
    NEED_MORE,
};

SentencePtr buildSentence(std::deque<TokenPtr> & tokens, size_t to)
{
    auto begin = tokens.begin();
    auto end = tokens.begin() + static_cast<long>(to);
    std::deque<TokenPtr> new_tokens(
        std::make_move_iterator(begin),
        std::make_move_iterator(end));

    if (!new_tokens.empty() && new_tokens.front()->getData() == " ")
        new_tokens.pop_front();

    if (!new_tokens.empty() && new_tokens.back()->getData() == " ")
        new_tokens.pop_back();

    tokens.erase(begin, end);
    return std::make_shared<Sentence>(std::move(new_tokens));
}

ESentenceStatus isSentenceEndAtSecond(
    const Token * first_word,
    const Token * second_word,
    const Token * third_word)
{
    if(second_word->getTokenType() != ETokenType::PUNCT
        || (
            !contains(second_word->getGraphemTag(), EGraphemTag::CAN_TERMINATE_SENTENCE)
            && !contains(second_word->getGraphemTag(), EGraphemTag::MUST_TERMINATE_SENTENCE))
       )
    {
        return ESentenceStatus::NOT_TERMINATES;
    }

    if (second_word->getTokenType() == ETokenType::PUNCT
        && contains(second_word->getGraphemTag(), EGraphemTag::MUST_TERMINATE_SENTENCE))
        return ESentenceStatus::TERMINATES;

    if (first_word->getTokenType() == ETokenType::WORD && first_word->getLength() == 1)
    {
        if (contains(first_word->getGraphemTag(), EGraphemTag::UPPER_CASE))
            return ESentenceStatus::NOT_TERMINATES;

        if (third_word->getLength() == 1)
            return ESentenceStatus::NOT_TERMINATES;

        if (!contains(third_word->getGraphemTag(), EGraphemTag::TITLE_CASE))
            return ESentenceStatus::NOT_TERMINATES;
    }
    return ESentenceStatus::TERMINATES;
}

std::pair<long, ESearchStatus> getWordPositionFrom(const std::deque<TokenPtr> & tokens, long start)
{
    for (size_t i = static_cast<size_t>(start); i < tokens.size(); ++i)
    {
        if (tokens[i]->getTokenType() == ETokenType::WORD
            || tokens[i]->getTokenType() == ETokenType::WORDNUM)
            return {static_cast<long>(i), ESearchStatus::FOUND};
        else if (tokens[i]->getTokenType() == ETokenType::PUNCT
            && (contains(tokens[i]->getGraphemTag(), EGraphemTag::CAN_TERMINATE_SENTENCE)
                || contains(tokens[i]->getGraphemTag(), EGraphemTag::MUST_TERMINATE_SENTENCE)))
            return {-1, ESearchStatus::NOT_FOUND};
    }

    return {-1, ESearchStatus::NEED_MORE};
}

std::tuple<long, ESearchStatus> getPunctPositionFrom(const std::deque<TokenPtr> & tokens, long start)
{
    for (size_t i = static_cast<size_t>(start); i < tokens.size(); ++i)
    {
        if (tokens[i]->getTokenType() == ETokenType::PUNCT
            && (
                contains(tokens[i]->getGraphemTag(), EGraphemTag::MUST_TERMINATE_SENTENCE)
                || contains(tokens[i]->getGraphemTag(), EGraphemTag::CAN_TERMINATE_SENTENCE)))
            return {static_cast<long>(i), ESearchStatus::FOUND};
        else if (tokens[i]->getTokenType() == ETokenType::WORD
            || tokens[i]->getTokenType() == ETokenType::WORDNUM)
            return {-1, ESearchStatus::NOT_FOUND};
    }

    return {-1, ESearchStatus::NEED_MORE};
}


}
SentenceInputStream::SentenceInputStream(BaseTokenInputStream & is_, size_t approximate_sentence_size_)
    : is(is_)
    , approximate_sentence_size(approximate_sentence_size_)
{
}

bool SentenceInputStream::next()
{
    if (is.eof())
        return false;

    size_t read_util = approximate_sentence_size;

    while(read_util-- && !is.eof())
    {
        auto res = is.read();
        if (res != nullptr)
            pending.emplace_back(std::move(res));
    }

    return !pending.empty();
}



SentencePtr SentenceInputStream::read()
{
    if (pending.empty())
        if(!next())
            return nullptr;

    bool last = is.eof();
    long start = 0;
    SentencePtr result = nullptr;
    while(static_cast<size_t>(start) < pending.size())
    {
        auto [first_word, first_status] = getWordPositionFrom(pending, start);
        if (first_word == -1)
        {
            if(first_status == ESearchStatus::NEED_MORE && next())
                continue;
            else
            {
                last = true;
                break;
            }
        }
        auto [second_punct, second_status] = getPunctPositionFrom(pending, first_word + 1);
        if (second_punct == -1)
        {
            if (second_status == ESearchStatus::NOT_FOUND)
            {
                start = first_word + 1;
                continue;
            }
            else if(second_status == ESearchStatus::NEED_MORE && next())
            {
                continue;
            }
            else
            {
                last = true;
                break;
            }
        }

        auto [third_word, third_status] = getWordPositionFrom(pending, second_punct + 1);
        if (third_word == -1)
        {
            if (second_status == ESearchStatus::NOT_FOUND)
            {
                start = first_word + 1;
                continue;
            }
            else if(third_status == ESearchStatus::NEED_MORE && next())
                continue;
            else
            {
                last = true;
                break;
            }
        }
        const Token * first = pending[static_cast<size_t>(first_word)].get();
        const Token * second = pending[static_cast<size_t>(second_punct)].get();
        const Token * third = pending[static_cast<size_t>(third_word)].get();
        if (isSentenceEndAtSecond(first, second, third) == ESentenceStatus::TERMINATES)
        {
            result = buildSentence(pending, static_cast<size_t>(second_punct) + 1);
            break;
        }
        else
        {
            if (next())
            {
                start = third_word;
                continue;
            }
            else
            {
                last = true;
                break;
            }
        }
    }
    if (last)
        result = buildSentence(pending, pending.size());

    return result;
}

bool SentenceInputStream::eof() const
{
    return is.eof() && pending.empty();
}

}
