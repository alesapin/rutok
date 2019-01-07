#pragma once
#include <streams/BaseTokenConcatInputStream.h>
namespace tokenize
{
/** Concat sings like:
 * 1) ...
 * 2) !?
 * 3) some-what
 * 4) Komsomolsk-on-Amur
 */
class SmallGroupsTokenConcatInputStream : public BaseTokenConcatInputStream
{
protected:
    ConcatResult concat(const std::deque<TokenPtr> & window, bool last) override;
public:
    SmallGroupsTokenConcatInputStream(BaseTokenInputStream & in_);
};

}
