#include <streams/SentenceJSONOutputStream.h>
namespace tokenize
{

void SentenceJSONOutputStream::write(SentencePtr sentence)
{
    output->start();
    for (size_t i = 0; i < sentence->tokensCount(); ++i)
        output->write(sentence->getTokenPtr(i));
    output->finish();
}

}
