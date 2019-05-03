#pragma once
#include <streams/EncodingInputStreamBase.h>

namespace tokenize
{
class EncodingInputStreamFromDescriptor: public EncodingInputStreamBase
{
private:
    int fd;
    bool feof = false;
private:
    bool next() override;
    bool inner_eof() const override
    {
        return feof;
    }
public:
    EncodingInputStreamFromDescriptor(int fd_, size_t size = 4096)
        : EncodingInputStreamBase(size)
        , fd(fd_)
    {
    }

};

}
