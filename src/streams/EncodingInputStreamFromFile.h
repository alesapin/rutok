#pragma once
#include <streams/EncodingInputStreamBase.h>
#include <cstdio>

namespace tokenize
{
class EncodingInputStreamFromFile final : public EncodingInputStreamBase
{
private:
    FILE * fileptr;
private:
    bool next() override;
    bool inner_eof() const override
    {
        return feof(fileptr);
    }

public:
    EncodingInputStreamFromFile(FILE * f, size_t size = 4096)
        : EncodingInputStreamBase(size)
        , fileptr(f)
    {
    }
};
}
