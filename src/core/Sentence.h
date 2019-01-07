#pragma once
#include <core/Token.h>
#include <vector>

namespace tokenize
{
class Sentence
{
private:
    std::vector<TokenPtr> tokens;
public:
    Sentence() = default;
    Sentence(const std::vector<TokenPtr> tokens_);

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
    bool isCyrrilyc() const;
    /// All words in sentence are latin
    bool isLatin() const;

    bool isEmpty() const;

    /// Return sentence as text
    std::string asText() const;

    static Sentence toWordsOnly(const Sentence & sentence);
};
}
