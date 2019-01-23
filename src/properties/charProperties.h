#pragma once
#include <core/Tag.h>

namespace tokenize {
bool charIsPunct(char32_t chr);
bool charIsNumber(char32_t chr);
bool charIsSymbol(char32_t chr);
bool charIsSeparator(char32_t chr);
bool charIsHierglyph(char32_t chr);
bool charIsLetter(char32_t chr);
ETokenType detectCharType(char32_t chr);
}
