#pragma once
#include <streams/BaseSentenceOutputStream.h>
#include <streams/TokenStringOutputStream.h>
namespace tokenize
{
class SentenceStringOutputStream : public BaseSentenceOutputStream
{
public:
    SentenceStringOutputStream(BaseCharOutputStream & output_)
        : BaseSentenceOutputStream(std::make_unique<TokenStringOutputStream>(output_))
    {}

};
}
