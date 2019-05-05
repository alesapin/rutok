#include <core/Sentence.h>
#include <algorithm>
#include <sstream>
#include <numeric>
#include <iostream>
namespace tokenize
{
namespace
{
void terminateSentence(std::deque<TokenPtr> & tokens)
{
    if (tokens.empty()) return;
    bool found = false;
    for (long i = tokens.size() - 1; i >= 0; --i)
    {
        if (contains(tokens[i]->getGraphemTag(), EGraphemTag::CAN_TERMINATE_SENTENCE) || contains(tokens[i]->getGraphemTag(), EGraphemTag::MUST_TERMINATE_SENTENCE))
        {
            found = true;
            tokens[i]->setSemanticTag(ESemanticTag::SENTENCE_END);
            break;
        }
    }
    if (!found)
        tokens.back()->setSemanticTag(ESemanticTag::SENTENCE_END);
}
}

Sentence::Sentence(std::deque<TokenPtr> && tokens_)
    : tokens(std::move(tokens_))
{
    terminateSentence(tokens);
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
        [](const TokenPtr & token) { return token->getTokenType() == ETokenType::WORD; }));
}

size_t Sentence::charactersCount() const
{
    return std::accumulate(
        tokens.begin(),
        tokens.end(),
        size_t{0},
        [](size_t current, const TokenPtr & token) { return token->getLength() + current; });
}

size_t Sentence::bytesCount() const
{
    return std::accumulate(
        tokens.begin(),
        tokens.end(),
        size_t{0},
        [](size_t current, const TokenPtr & token) { return token->getBytesLength() + current; });
}

bool Sentence::isEndsCorrectly() const
{
    if (tokens.empty())
        return true;

    auto last = tokens.back().get();
    if (last->getTokenType() == ETokenType::PUNCT && last->getData() == "\"")
    {
        if (tokens.size() > 1)
            last = tokens[tokens.size() - 2].get();
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

    auto first = tokens.front().get();
    if (first->getTokenType() == ETokenType::PUNCT && first->getData() == "\"")
    {
        if (tokens.size() > 1)
            first = tokens[1].get();
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
    auto first = tokens.front().get();
    auto last = tokens.back().get();
    return first->getTokenType() == ETokenType::PUNCT
        && first->getData() == "\""
        && *first == *last;
}

bool Sentence::isWordsOnly() const
{
    return std::all_of(
        tokens.begin(),
        tokens.end(),
        [](const TokenPtr & token)
        {
            return token->getTokenType() == ETokenType::WORD
                || (token->getTokenType() == ETokenType::SEPARATOR && token->getData() == " ");
        });
}

bool Sentence::isCyrillic() const
{
    return std::all_of(
        tokens.begin(),
        tokens.end(),
        [](const TokenPtr & token)
        {
            return token->isCyrillic();
        });
}

bool Sentence::isLatin() const
{
    return std::all_of(
        tokens.begin(),
        tokens.end(),
        [](const TokenPtr & token)
        {
            return token->isLatin();
        });
}

bool Sentence::isEmpty() const
{
    return tokens.empty();
}


std::string Sentence::asText(bool to_lower, bool to_upper) const
{
    std::ostringstream oss;
    for (auto & token : tokens)
        if (to_lower)
            oss << Token::toLower(token.get())->getData();
        else if (to_upper)
            oss << Token::toUpper(token.get())->getData();
        else
            oss << token->getData();
    return oss.str();
}

SentencePtr Sentence::toWordsOnly(const Sentence * sentence)
{
    std::deque<TokenPtr> new_tokens;
    for (size_t i = 0; i < sentence->tokens.size(); ++i)
    {
        auto token = sentence->tokens[i].get();
        if (token->getTokenType() == ETokenType::WORD || token->getTokenType() == ETokenType::WORDNUM || token->getTokenType() == ETokenType::NUMBER)
        {
            new_tokens.emplace_back(Token::refine(token));
            new_tokens.emplace_back(Token::createDefaultSeparator());
        }
    }
    if (!new_tokens.empty())
        new_tokens.pop_back();
    SentencePtr result = std::make_unique<Sentence>();
    result->tokens.swap(new_tokens);
    return result;
}
}
