#pragma once
#include <type_traits>
#include <vector>
#include <string>
#include <sstream>

namespace tokenize
{
enum class ETokenType
{
    UNKNOWN,
    WORD,
    PUNCT,
    SEPARATOR,
    NUMBER,
    WORDNUM,
    HIEROGLYPH,
    SYMBOL,
};

enum class EGraphemTag : uint64_t
{
    UNKNOWN = 0x0,
    // word tags
    CYRILLIC = 0x1,
    LATIN = 0x2,
    UPPER_CASE = 0x4,
    LOWER_CASE = 0x8,
    MIXED_CASE = 0x10,
    TITLE_CASE = 0x20,
    SPACED = 0x40,
    // punct tags
    CAN_TERMINATE_SENTENCE = 0x80, // .
    PAIR = 0x100, // { or )
    MULTI_PUNCT = 0x200, // ...
    FAKEPUNCT = 0x400, /// some-word
    MUST_TERMINATE_SENTENCE = 0x800, // ? ! ...
    // number tags
    NORMAL = 0x80000,
    ROMAN = 0x100000,
    // separator tags
    CAN_TERMINATE_PARAGRAPH = 0x200000,
    MULTI_SEP = 0x4000000,
    HYPH_WORD = 0x10000000,
    MIX_LANG = 0x20000000,
    MULTI_SYMBOL = 0x40000000,
    WRAPPED_WORD = 0x80000000,
};

enum class ESemanticTag
{
    UNKNOWN = 0x0,
    SENTENCE_END = 0x1,
    PARAGRAPH_END = 0x2,
};

template<typename T>
inline T operator|(T left, T right) {
    static_assert(std::is_enum<T>::value, "Type of operator| must be enum");
    auto left_val = static_cast<std::underlying_type_t<T>>(left);
    auto right_val = static_cast<std::underlying_type_t<T>>(right);
    auto res_val = left_val | right_val;
    return static_cast<T>(res_val);
}
template<typename T>
inline T operator|=(T& left, T right) {
    static_assert(std::is_enum<T>::value, "Type of operator|= must be enum");
    auto left_val = static_cast<std::underlying_type_t<T>>(left);
    auto right_val = static_cast<std::underlying_type_t<T>>(right);
    auto res_val = left_val | right_val;
    return left = static_cast<T>(res_val);
}
template<typename T>
inline bool intersect(T left, T right) {
    static_assert(std::is_enum<T>::value, "Type of intersect must be enum");
    auto left_val = static_cast<std::underlying_type_t<T>>(left);
    auto right_val = static_cast<std::underlying_type_t<T>>(right);
    auto res_val = left_val & right_val;
    return static_cast<bool>(res_val);
}

template<typename T>
inline bool contains(T left, T right) {
    static_assert(std::is_enum<T>::value, "Type of contains must be enum");
    auto left_val = static_cast<std::underlying_type_t<T>>(left);
    auto right_val = static_cast<std::underlying_type_t<T>>(right);
    auto res_val = left_val & right_val;
    return res_val == right_val;
}

namespace detail {
inline std::string toStringSingle(EGraphemTag tag)
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
    case EGraphemTag::WRAPPED_WORD : return "WRAPPED_WORD";
    }
}

inline std::string toStringSingle(ESemanticTag tag)
{
    switch(tag)
    {
    case ESemanticTag::UNKNOWN: return "UNKNOWN";
    case ESemanticTag::SENTENCE_END: return "SENTENCE_END";
    case ESemanticTag::PARAGRAPH_END: return "PARAGRAPH_END";
    }
}

}

std::string toString(ETokenType tag);

template<typename T>
std::string toString(T tag_bag, const std::string & sep = ", ")
{
    std::ostringstream os;
    std::vector<T> tags = toTagSet(tag_bag);
    for (size_t i = 0; i < tags.size() - 1; ++i)
        os << detail::toStringSingle(tags[i]) << sep;
    os << detail::toStringSingle(tags.back());
    return os.str();
}

template<typename T>
inline T exclude(T left, T right) {
    static_assert(std::is_enum<T>::value, "Type of exclude must be enum");
    auto left_val = static_cast<std::underlying_type_t<T>>(left);
    auto right_val = static_cast<std::underlying_type_t<T>>(right);
    auto res_val = left_val & (~right_val);
    return static_cast<T>(res_val);
}

template<typename T>
std::vector<T> toTagSet(T tag) {
    static_assert(std::is_enum<T>::value, "Type of toTagSet must be enum");
    std::underlying_type_t<T> value = static_cast<std::underlying_type_t<T>>(tag);
    std::vector<T> result;
    for (size_t i = 1; i < 1ul << 63; i <<= 1) {
        if (value & i) {
            result.push_back(static_cast<T>(i));
        }
    }

    if (result.empty())
        result.push_back(static_cast<T>(0));

    return result;
}

}
