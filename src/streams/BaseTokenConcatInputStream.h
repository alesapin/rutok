#pragma once
#include <streams/BaseTokenInputStream.h>
#include <streams/TokenInputStream.h>
#include <core/Token.h>
#include <deque>

namespace tokenize
{
enum class EConcatStatus
{
    NOT_CONCATED,
    CONCATED,
    NEED_MORE,
};

struct ConcatResult
{
    EConcatStatus operation_status;
    TokenPtr concat_result;
    size_t number_of_concated_tokens;
};

class BaseTokenConcatInputStream : public BaseTokenInputStream
{
private:
    void dropNFromQueueHead(size_t n_first);
protected:
    std::deque<TokenPtr> pending;
    BaseTokenInputStream & in;
    size_t approx_window_size;

protected:
    bool next() override;
    /** 1) Must return NOT_CONCATED or CONCATED if last specified
     * 2) If CONCATED returned than concated tokens starts from zero index in window
     */
    virtual ConcatResult concat(const std::deque<TokenPtr> & window, bool last) = 0;

public:
    BaseTokenConcatInputStream(BaseTokenInputStream & in_, size_t approx_window_size_=3);
    size_t alreadyReadBytes() const override { return in.alreadyReadBytes(); }
    TokenPtr read() override;
    bool eof() const override;
};
}
