#pragma once
#include <core/Sentence.h>
#include <streams/BaseCharOutputStream.h>
#include <streams/BaseTokenOutputStream.h>


namespace tokenize
{
class BaseSentenceOutputStream
{
protected:
    std::unique_ptr<BaseTokenOutputStream> holder;
    BaseTokenOutputStream & output;
public:
    BaseSentenceOutputStream(std::unique_ptr<BaseTokenOutputStream> holder_)
        : holder(std::move(holder_))
        , output(*holder) {}

    virtual void write(SentencePtr sentence);
    virtual void start() { output.start(); };
    virtual void finish() { output.finish(); };
    virtual void flush() { output.flush(); };
};

}
