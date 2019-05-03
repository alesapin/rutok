#pragma once
#include <core/Sentence.h>
#include <streams/TokenJSONOutputStream.h>
#include <streams/BaseCharOutputStream.h>
#include <streams/BaseSentenceOutputStream.h>

namespace tokenize
{
class SentenceJSONOutputStream : public BaseSentenceOutputStream
{
private:
    std::unique_ptr<TokenJSONOutputStream> holder;
    TokenJSONOutputStream & output;
public:
    SentenceJSONOutputStream(TokenJSONOutputStream & output_)
        : output(output_) {}
    SentenceJSONOutputStream(BaseCharOutputStream & output_)
        : holder(std::make_unique<TokenJSONOutputStream>(output_))
        , output(*holder) {}

    void write(SentencePtr sentence);
    void start();
    void finish();
    void flush();
};
}
