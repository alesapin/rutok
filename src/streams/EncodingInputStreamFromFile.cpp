#include <streams/EncodingInputStreamFromFile.h>
namespace tokenize
{

bool EncodingInputStreamFromFile::next()
{
    if (feof(fileptr))
        return false;

    if (buffer_filled_length == buffer_size)
        return false;

    auto readen = fread(buffer.get() + buffer_filled_length, sizeof(char), static_cast<long>(buffer_size - buffer_filled_length), fileptr);
    buffer_filled_length += static_cast<size_t>(readen);

    if (encoding.empty())
        encoding = detectCharset(buffer.get(), buffer_filled_length);

    return true;

}
}
