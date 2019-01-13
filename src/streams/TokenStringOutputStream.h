#pragma once
#include <streams/BaseTokenOutputStream.h>
#include <streams/BaseCharOutputStream.h>
#include <string>
#include <deque>
#include <iostream>
#include <memory>


namespace tokenize
{

class TokenStringOutputStream : public BaseTokenOutputStream
{
private:
    BaseCharOutputStream & os;
    std::deque<std::string> pending;
    size_t summary_pending_len = 0;
    std::string separator;

protected:
    bool next() override;

public:
    TokenStringOutputStream(
        BaseCharOutputStream & os_,
        BaseTokenInputStream & input_,
        size_t buffer_size_ = 4096,
        const std::string & separator_ = "\n");
    bool write() override;
    bool eos() const override;
    void flush() override;
};
}
