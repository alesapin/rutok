#pragma once
#include <core/Token.h>
#include <deque>
#include <memory>

namespace tokenize
{
class Sentence
{
private:
    std::deque<TokenPtr> tokens;
public:
    Sentence() = default;
    Sentence(const std::deque<TokenPtr> & tokens_);

    /// Total tokens in sentence
    size_t tokensCount() const;
    /// Total amount of words in sentence
    size_t wordsCount() const;
    /// Total charactes in sentence in unicode sence
    size_t charactersCount() const;
    /// Total amount of bytes in sentence
    size_t bytesCount() const;

    /// Contains .?...! at the end of the sentence
    bool isEndsCorrectly() const;
    /// Starts with TITLE_CASE word
    bool isStartsCorrectly() const;
    /// Quoted
    bool isQuoted() const;
    /// Contain tokens with words only
    bool isWordsOnly() const;
    /// All words in sentence are cyrrlic
    bool isCyrillic() const;
    /// All words in sentence are latin
    bool isLatin() const;

    bool isEmpty() const;

    /// Return sentence as text
    std::string asText(bool to_lower=false, bool to_upper=false) const;

    static std::shared_ptr<Sentence> toWordsOnly(std::shared_ptr<Sentence> sentence);
};
using SentencePtr = std::shared_ptr<Sentence>;
}
