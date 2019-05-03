#pragma once
#include <streams/EncodingInputStreamBase.h>

namespace tokenize
{
class EncodingInputStreamFromStream : public EncodingInputStreamBase
{
private:
    std::istream & is;
private:
    bool next() override;

    bool inner_eof() const override
    {
        return is.eof();
    }
public:
    EncodingInputStreamFromStream(std::istream & is_, size_t size = 4096)
        : EncodingInputStreamBase(size)
        , is(is_)
    {
    }
};

}
