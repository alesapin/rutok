#pragma once
#include <streams/BaseTokenConcatInputStream.h>
namespace tokenize
{
/** Concat sings like:
 * 1) ...
 * 2) !!!!!!!
 * 3) \t\t\t\t\t\t
 * 4) \n\n\n\n\n
 * 5) --
 */
class IdenticalConcatInputStream : public BaseTokenConcatInputStream
{
protected:
    ConcatResult concat(const std::deque<TokenPtr> & window, bool last) override;
public:
    IdenticalConcatInputStream(BaseTokenInputStream & in_);
};
}
