#include <streams/BaseTokenConcatInputStream.h>
namespace tokenize
{


BaseTokenConcatInputStream::BaseTokenConcatInputStream(BaseTokenInputStream & in_, size_t approx_window_size_)
    : in(in_)
    , approx_window_size(approx_window_size_)
{
}

void BaseTokenConcatInputStream::dropNFromQueueHead(size_t n_first)
{
    if (n_first > pending.size())
        throw std::runtime_error("Can't drop " + std::to_string(n_first) + " while queue size is " + std::to_string(pending.size()));
    while(n_first--)
        pending.pop_front();
}

bool BaseTokenConcatInputStream::eof() const
{
    return in.eof() && pending.empty();
}

bool BaseTokenConcatInputStream::next()
{
    if (in.eof())
        return false;

    for (size_t i = 0; i < approx_window_size && !in.eof(); ++i)
    {
        auto res = in.read();
        if (res != nullptr)
            pending.push_back(res);
    }

    return !pending.empty();
}

TokenPtr BaseTokenConcatInputStream::read()
{
    while (pending.empty())
        if (!next())
            return nullptr;

    bool last = in.eof();

    ConcatResult conc = concat(pending, last);
    while (conc.operation_status == EConcatStatus::NEED_MORE)
    {
        if (!next())
            last = true;
        conc = concat(pending, last);
    }
    TokenPtr result = nullptr;
    if (conc.operation_status == EConcatStatus::NOT_CONCATED)
    {
        result = pending.front();
        pending.pop_front();
    }
    else
    {
        result = conc.concat_result;
        dropNFromQueueHead(conc.number_of_concated_tokens);
    }

    return result;
}

}

