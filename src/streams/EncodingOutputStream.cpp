#include <streams/EncodingOutputStream.h>
#include <unicorn/library.hpp>

namespace tokenize
{
EncodingOutputStream::EncodingOutputStream(
    std::ostream & os_,
    size_t buffer_size_,
    std::string encoding_)
    : BaseCharOutputStream(buffer_size_)
    , os(os_)
    , encoding(encoding_)
{
    need_conversion = static_cast<bool>(encoding != "UTF-8" && encoding != "utf-8");
}


void EncodingOutputStream::flush()
{
    if (current_size != 0)
    {
        std::string result;
        if(need_conversion)
            RS::Unicorn::export_string(buffer.str(), result, encoding);
        else
            result = buffer.str();
        os << result;
        buffer.clear();
        current_size = 0;
    }
}

void EncodingOutputStream::write(const std::string & data)
{
    if (current_size < buffer_size)
    {
        buffer << data;
        current_size += data.size();
    }
    else
        flush();
}
}
