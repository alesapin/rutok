#include "Tag.h"
#include <sstream>
namespace tokenize {

namespace {
std::string toStringSingle(EGraphemTag tag)
{
    switch(tag)
    {
    case EGraphemTag::UNKNOWN: return "UNKNOWN";
    case EGraphemTag::CYRILLIC: return "CYRILLIC";
    case EGraphemTag::LATIN: return "LATIN";
    case EGraphemTag::UPPER_CASE: return "UPPER_CASE";
    case EGraphemTag::LOWER_CASE: return "LOWER_CASE";
    case EGraphemTag::MIXED_CASE: return "MIXED_CASE";
    case EGraphemTag::TITLE_CASE: return "TITLE_CASE";
    case EGraphemTag::CAN_TERMINATE_SENTENCE: return "CAN_TERMINATE_SENTENCE";
    case EGraphemTag::PAIR: return "PAIR";
    case EGraphemTag::MULTI_PUNCT: return "MULTI_PUNCT";
    case EGraphemTag::FAKEPUNCT: return "FAKEPUNCT";
    case EGraphemTag::NORMAL: return "NORMAL";
    case EGraphemTag::ROMAN: return "ROMAN";
    case EGraphemTag::CAN_TERMINATE_PARAGRAPH: return "CAN_TERMINATE_PARAGRAPH";
    case EGraphemTag::MULTI_SEP: return "MULTI_SEP";
    case EGraphemTag::HYPH_WORD: return "HYPH_WORD";
    case EGraphemTag::MIX_LANG: return "MIX_LANG";
    case EGraphemTag::MULTI_SYMBOL: return "MULTI_SYMBOL";
    case EGraphemTag::SPACED: return "SPACED";
    case EGraphemTag::MUST_TERMINATE_SENTENCE: return "MUST_TERMINATE_SENTENCE";
    }
}

}

std::string toString(EGraphemTag tag_bag, const std::string & sep)
{
    std::ostringstream os;
    std::vector<EGraphemTag> tags = toTagSet(tag_bag);
    for (size_t i = 0; i < tags.size() - 1; ++i)
        os << toStringSingle(tags[i]) << sep;
    os << toStringSingle(tags.back());
    return os.str();
}

std::string toString(ETokenType tag)
{
    switch(tag)
    {
    case ETokenType::UNKNOWN: return "UNKNOWN";
    case ETokenType::WORD: return "WORD";
    case ETokenType::PUNCT: return "PUNCT";
    case ETokenType::SEPARATOR: return "SEPARATOR";
    case ETokenType::NUMBER: return "NUMBER";
    case ETokenType::WORDNUM: return "WORDNUM";
    case ETokenType::HIEROGLYPH: return "HIEROGLYPH";
    case ETokenType::SYMBOL: return "SYMBOL";
    }
}

}
