#include <streams/EncodingInputStreamFromStream.h>
namespace tokenize
{
bool EncodingInputStreamFromStream::next()
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
}
