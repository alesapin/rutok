#pragma once
#include <core/Sentence.h>
#include <streams/BaseCharOutputStream.h>
#include <streams/BaseTokenOutputStream.h>


namespace tokenize
{
class BaseSentenceOutputStream
{
protected:
    std::unique_ptr<BaseTokenOutputStream> output;
public:
    BaseSentenceOutputStream(std::unique_ptr<BaseTokenOutputStream> holder_)
        : output(std::move(holder_)) {}

    virtual void write(const Sentence * sentence);
    virtual void start() { output->start(); };
    virtual void finish() { output->finish(); };
    virtual void flush() { output->flush(); };

    virtual ~BaseSentenceOutputStream() = default;
};

}
