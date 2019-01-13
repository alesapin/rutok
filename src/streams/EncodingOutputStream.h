#pragma once

#include <streams/BaseCharOutputStream.h>
#include <sstream>

namespace tokenize
{
class EncodingOutputStream : public BaseCharOutputStream
{
private:
    std::ostream & os;
    std::string encoding;
    std::ostringstream buffer;
    bool need_conversion;
    size_t current_size{0};

public:
    EncodingOutputStream(
        std::ostream & os_,
        size_t buffer_size_ = 4096,
        std::string encoding_= "UTF-8");

    void write(const std::string & data) override;
    void flush() override;
};
}
