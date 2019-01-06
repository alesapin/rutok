#pragma once
#include <core/Tag.h>

namespace tokenize {
extern bool charIsPunct(char32_t chr);
extern bool charIsNumber(char32_t chr);
extern bool charIsSymbol(char32_t chr);
extern bool charIsSeparator(char32_t chr);
extern bool charIsHierglyph(char32_t chr);
extern bool charIsLetter(char32_t chr);
extern ETokenType detectCharType(char32_t chr);
}
