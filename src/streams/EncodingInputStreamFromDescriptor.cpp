#include <streams/EncodingInputStreamFromDescriptor.h>
#include <unistd.h>
namespace tokenize
{

bool EncodingInputStreamFromDescriptor::next()
{
    if (buffer_filled_length == buffer_size)
        return false;

    size_t bytes_read = 0;
    while (!bytes_read)
    {

        ssize_t res = 0;
        res = ::read(fd, buffer.get() + buffer_filled_length, static_cast<long>(buffer_size - buffer_filled_length));
        if (!res)
            break;

        if (-1 == res && errno != EINTR)
            throw std::runtime_error("Cannot read from fd");

        if (res > 0)
            bytes_read += res;
    }
    if (!bytes_read)
    {
        feof = true;
        return false;
    }

    if (encoding.empty())
        encoding = detectCharset(buffer.get(), buffer_filled_length);

    buffer_filled_length += bytes_read;
    return true;

}
}
