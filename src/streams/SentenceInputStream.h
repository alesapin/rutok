#pragma once
#include <core/Sentence.h>
#include <deque>
#include <streams/BaseTokenInputStream.h>

namespace tokenize
{
class SentenceInputStream
{
private:
    std::deque<TokenPtr> pending;
    BaseTokenInputStream & is;
    size_t approximate_sentence_size;
protected:
    bool next();
public:
    SentenceInputStream(BaseTokenInputStream & is_, size_t approximate_sentence_size_=6);
    SentencePtr read();
    bool eof() const;
};

}
