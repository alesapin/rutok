#pragma once
#include <core/Sentence.h>
#include <streams/TokenJSONOutputStream.h>
#include <streams/BaseCharOutputStream.h>
#include <streams/BaseSentenceOutputStream.h>

namespace tokenize
{
class SentenceJSONOutputStream : public BaseSentenceOutputStream
{
public:
    SentenceJSONOutputStream(BaseCharOutputStream & output_, bool pretty)
        : BaseSentenceOutputStream(std::make_unique<TokenJSONOutputStream>(output_, pretty))
    {}

    void write(SentencePtr sentence) override;
};
}
