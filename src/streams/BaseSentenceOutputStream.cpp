#include <streams/BaseSentenceOutputStream.h>
namespace tokenize
{

void BaseSentenceOutputStream::write(SentencePtr sentence)
{
    for (size_t i = 0; i < sentence->tokensCount(); ++i)
        output.write(sentence->getToken(i));
}
}
