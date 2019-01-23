#include <properties/charProperties.h>
#include <unicorn/library.hpp>
namespace tokenize
{
using namespace RS;
using namespace RS::Unicorn;
using namespace RS::Unicorn::Literals;

bool charIsPunct(char32_t chr)
{
    return char_is_punctuation(chr);
}
bool charIsNumber(char32_t chr)
{
    return char_is_digit(chr);
}
bool charIsSymbol(char32_t chr)
{
    return char_general_category(chr) == GC::Sk
        || char_general_category(chr) == GC::Sm
        || char_general_category(chr) == GC::Sc;
}
bool charIsSeparator(char32_t chr)
{
    return char_is_separator(chr) || char_is_control(chr);
}
bool charIsHierglyph(char32_t chr)
{
    return char_general_category(chr) == GC::So
        || char_general_category(chr) == GC::Lo;

}
bool charIsLetter(char32_t chr)
{
    return char_is_letter(chr);
}
ETokenType detectCharType(char32_t chr)
{
    if (charIsPunct(chr))
        return ETokenType::PUNCT;
    else if (charIsSymbol(chr))
        return ETokenType::SYMBOL;
    else if (charIsNumber(chr))
        return ETokenType::NUMBER;
    else if (charIsSeparator(chr))
        return ETokenType::SEPARATOR;
    else if (charIsLetter(chr))
        return ETokenType::WORD;
    else if (charIsHierglyph(chr))
        return ETokenType::HIEROGLYPH;
    return ETokenType::UNKNOWN;
}

}
