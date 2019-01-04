#pragma once
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <type_traits>


namespace tokenize
{

namespace detail {
class TokenImpl;
}

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

enum class EGraphemTag
{
    UNKNOWN = 0x0,
    // word tags
    CYRILLIC = 0x1,
    LATIN = 0x2,
    UPPER_CASE = 0x4,
    LOWER_CASE = 0x8,
    MIXED_CASE = 0x10,
    TITLE_CASE = 0x20,
    // punct tags
    CAN_TERMINATE_SENTENCE = 0x40, // . ? ! ...
    PAIR = 0x80, // { or )
    MULTI_PUNCT = 0x100, // ...
    FAKEPUNCT = 0x200, /// some-word
    // number tags
    NORMAL = 0x80000,
    ROMAN = 0x100000,
    // separator tags
    CAN_TERMINATE_PARAGRAPH = 0x200000,
    MULTI_SEP = 0x4000000,
    HYPH_WORD = 0x10000000,
    MIX_LANG = 0x20000000,
    MULTI_SYMBOL = 0x40000000,
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

std::string toString(EGraphemTag tag, const std::string & sep = ", ");
std::string toString(ETokenType tag, const std::string & sep = ", ");

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
    return result;
}

class Token
{
public:
    char32_t operator[](size_t index) const;
    ETokenType getTokenType() const;
    EGraphemTag getGraphemTag() const;
    const std::string & getData() const;
    size_t getLength() const;

    static std::shared_ptr<Token> createToken(const std::string & data);
    static std::shared_ptr<Token> toUpper(const std::shared_ptr<Token> token);
    static std::shared_ptr<Token> toLower(const std::shared_ptr<Token> token);
    static std::shared_ptr<Token> toTitle(const std::shared_ptr<Token> token);
    static std::shared_ptr<Token> concat(const std::vector<std::shared_ptr<Token>> & tokens);

    bool operator<(const Token & o) const;
    bool operator>(const Token & o) const;
    bool operator==(const Token & o) const;
    bool operator!=(const Token & o) const;
    bool equalsIgnoreCase(const Token & o) const;
    bool startsWith(const std::string prefix) const;
    bool endsWith(const std::string suffix) const;
    bool isUpper() const;
    bool isLower() const;
    bool isTitle() const;

    Token() = default;

    Token(const Token & other);

private:
    std::unique_ptr<detail::TokenImpl> impl;
};

using TokenPtr = std::shared_ptr<Token>;


}
