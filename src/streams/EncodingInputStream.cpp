#include <streams/EncodingInputStream.h>
#include <cstring>
#include <unicorn/library.hpp>
#include <charsetdetect.h>

namespace tokenize
{
using namespace RS;
using namespace RS::Unicorn;
using namespace RS::Unicorn::Literals;

namespace
{
std::string detectCharset(char * buf, size_t size)
{
    csd_t csd = csd_open();
    if (csd == reinterpret_cast<csd_t>(-1)) {
        throw std::runtime_error("Can't open csd");
    }
    int result = csd_consider(csd, buf, size);
    if (result < 0)
        throw std::runtime_error("Buffer is too small to detect charset");

    const char * enc_name = csd_close(csd);
    if (enc_name == nullptr)
        throw std::runtime_error("Unknown encoding");
    return std::string{enc_name};
}
}

bool EncodingInputStream::next()
{
    if (is.eof())
        return false;

    if (buffer_filled_length == buffer_size)
        return false;


    is.read(buffer.get() + buffer_filled_length, static_cast<long>(buffer_size - buffer_filled_length));
    buffer_filled_length += static_cast<size_t>(is.gcount());

    if (encoding.empty())
        encoding = detectCharset(buffer.get(), buffer_filled_length);

    return true;
}

bool EncodingInputStream::read(std::string & data, size_t max_size)
{
    if (buffer_filled_length == 0 && !next())
        return false;

    while(max_size >= buffer_filled_length && next());

    std::string tmp;
    if (max_size < buffer_filled_length)
    {
        tmp.resize(max_size);
        std::memcpy(&tmp[0], buffer.get(), max_size);
        size_t rest = buffer_filled_length - max_size;
        // overlapping buffers
        std::memmove(buffer.get(), buffer.get() + max_size, rest);
        buffer_filled_length = rest;
    }
    else
    {
        tmp.resize(buffer_filled_length);
        std::memcpy(&tmp[0], buffer.get(), buffer_filled_length);
        buffer_filled_length = 0;
    }
    if (encoding != "UTF-8" && encoding != "utf-8" && encoding != "utf8" && encoding != "ASCII")
        import_string(tmp, data, encoding);
    else
        data.swap(tmp);

    return true;
}

bool EncodingInputStream::eof() const
{
    return is.eof() && buffer_filled_length == 0;
}
}
