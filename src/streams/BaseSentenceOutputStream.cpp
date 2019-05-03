#include <streams/BaseSentenceOutputStream.h>
namespace tokenize
{

void BaseSentenceOutputStream::write(const Sentence * sentence)
{
    for (size_t i = 0; i < sentence->tokensCount(); ++i)
        output->write(sentence->getTokenPtr(i));
}
}
