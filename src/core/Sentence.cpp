#include <core/Sentence.h>
#include <algorithm>
#include <sstream>
#include <numeric>
#include <iostream>
namespace tokenize
{

Sentence::Sentence(const std::deque<TokenPtr> & tokens_)
    : tokens(tokens_)
{
}

size_t Sentence::tokensCount() const
{
    return tokens.size();
}

size_t Sentence::wordsCount() const
{
    return static_cast<size_t>(std::count_if(
        tokens.begin(),
        tokens.end(),
        [](TokenPtr token) { return token->getTokenType() == ETokenType::WORD; }));
}

size_t Sentence::charactersCount() const
{
    return std::accumulate(
        tokens.begin(),
        tokens.end(),
        size_t{0},
        [](size_t current, TokenPtr token) { return token->getLength() + current; });
}

size_t Sentence::bytesCount() const
{
    return std::accumulate(
        tokens.begin(),
        tokens.end(),
        size_t{0},
        [](size_t current, TokenPtr token) { return token->getBytesLength() + current; });
}

bool Sentence::isEndsCorrectly() const
{
    if (tokens.empty())
        return true;

    TokenPtr last = tokens.back();
    if (last->getTokenType() == ETokenType::PUNCT && last->getData() == "\"")
    {
        if (tokens.size() > 1)
            last = tokens[tokens.size() - 2];
        else
            return false;
    }
    return last->getTokenType() == ETokenType::PUNCT
        && (contains(last->getGraphemTag(), EGraphemTag::CAN_TERMINATE_SENTENCE)
            || contains(last->getGraphemTag(), EGraphemTag::MUST_TERMINATE_SENTENCE));
}

bool Sentence::isStartsCorrectly() const
{
    if (tokens.empty())
        return true;

    TokenPtr first = tokens.front();
    if (first->getTokenType() == ETokenType::PUNCT && first->getData() == "\"")
    {
        if (tokens.size() > 1)
            first = tokens[1];
        else
            return false;
    }
    return (first->getTokenType() == ETokenType::WORD
        || first->getTokenType() == ETokenType::WORDNUM)
        && contains(first->getGraphemTag(), EGraphemTag::TITLE_CASE);

}

bool Sentence::isQuoted() const
{
    if (tokens.size() < 2)
        return false;
    TokenPtr first = tokens.front();
    TokenPtr last = tokens.back();
    return first->getTokenType() == ETokenType::PUNCT
        && first->getData() == "\""
        && *first == *last;
}

bool Sentence::isWordsOnly() const
{
    return std::all_of(
        tokens.begin(),
        tokens.end(),
        [](TokenPtr token)
        {
            return token->getTokenType() == ETokenType::WORD
                || (token->getTokenType() == ETokenType::SEPARATOR && token->getData() == " ");
        });
}

bool Sentence::isCyrrilyc() const
{
    return std::all_of(
        tokens.begin(),
        tokens.end(),
        [](TokenPtr token)
        {
            if (token->getTokenType() == ETokenType::WORD
                || token->getTokenType() == ETokenType::WORDNUM)
                return contains(token->getGraphemTag(), EGraphemTag::CYRILLIC);
            return true;
        });
}

bool Sentence::isLatin() const
{
    return std::all_of(
        tokens.begin(),
        tokens.end(),
        [](TokenPtr token)
        {
            if (token->getTokenType() == ETokenType::WORD
                || token->getTokenType() == ETokenType::WORDNUM)
                return contains(token->getGraphemTag(), EGraphemTag::LATIN);
            return true;
        });
}

bool Sentence::isEmpty() const
{
    return tokens.empty();
}


std::string Sentence::asText() const
{
    std::ostringstream oss;
    for (auto token : tokens)
        oss << token->getData();
    return oss.str();
}

SentencePtr Sentence::toWordsOnly(SentencePtr sentence)
{
    std::deque<TokenPtr> new_tokens;
    for (size_t i = 0; i < sentence->tokens.size(); ++i)
    {
        auto token = sentence->tokens[i];
        if (token->getTokenType() == ETokenType::WORD || token->getTokenType() == ETokenType::WORDNUM)
        {
            new_tokens.emplace_back(std::make_shared<Token>(*token));
            new_tokens.emplace_back(Token::createDefaultSeparator());
        }
    }
    if (!new_tokens.empty())
        new_tokens.pop_back();
    SentencePtr result = std::make_shared<Sentence>();
    result->tokens.swap(new_tokens);
    return result;
}
}
