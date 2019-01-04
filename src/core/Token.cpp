#include "Token.h"
#include <unicorn/library.hpp>
#include <tuple>
#include <sstream>
namespace tokenize
{
using namespace RS;
using namespace RS::Unicorn;
using namespace RS::Unicorn::Literals;

namespace detail {
struct TokenImpl
{
    Ustring data;
    size_t length;
    ETokenType type_tag = ETokenType::UNKNOWN;
    EGraphemTag graphem_tag = EGraphemTag::UNKNOWN;
    char32_t at(size_t i) const
    {
        return str_char_at(data, i);
    }

};
}

char32_t Token::operator[](size_t i) const
{
    return impl->at(i);
}

ETokenType Token::getTokenType() const
{
    return impl->type_tag;
}


EGraphemTag Token::getGraphemTag() const
{
    return impl->graphem_tag;
}

const std::string & Token::getData() const
{
    return impl->data;
}

size_t Token::getLength() const
{
    return impl->length;
}

bool Token::operator<(const Token & o) const
{
    return std::make_tuple(impl->length, impl->data, impl->type_tag, impl->graphem_tag) < std::make_tuple(o.impl->length, o.impl->data, o.impl->type_tag, o.impl->graphem_tag);
}

bool Token::operator>(const Token & o) const
{
    return !(*this < o) && !(*this == o);
}

bool Token::operator==(const Token & o) const
{
    return
        getLength() == o.getLength()
        && getGraphemTag() == o.getGraphemTag()
        && getTokenType() == o.getTokenType()
        && getData() == o.getData();
}

bool Token::operator!=(const Token & o) const
{
    return !(*this == o);
}

bool Token::equalsIgnoreCase(const Token & o) const
{

    StringCompare<Strcmp::equal | Strcmp::icase> cmp_ei;
    return getLength() == o.getLength()
        && getGraphemTag() == o.getGraphemTag()
        && getTokenType() == o.getTokenType()
        && cmp_ei(getData(), o.getData());
}

bool Token::startsWith(const std::string prefix) const
{
    return str_starts_with(getData(), prefix);
}

bool Token::endsWith(const std::string suffix) const
{
    return str_ends_with(getData(), suffix);
}

bool Token::isUpper() const
{
    return contains(getGraphemTag(), EGraphemTag::UPPER_CASE);
}

bool Token::isLower() const
{
    return contains(getGraphemTag(), EGraphemTag::LOWER_CASE);
}

bool Token::isTitle() const
{
    return contains(getGraphemTag(), EGraphemTag::TITLE_CASE);
}

Token::Token(const Token & other)
{
    impl = std::make_unique<detail::TokenImpl>(*other.impl);
}

namespace
{

ETokenType detectTokenType(const Ustring & str)
{
    ETokenType result = ETokenType::UNKNOWN;
    for (const auto & sym : utf_range(str))
    {
        if (char_is_punctuation(sym))
        {
            if (result == ETokenType::UNKNOWN)
                result = ETokenType::PUNCT;
            else if (result != ETokenType::PUNCT)
                return ETokenType::UNKNOWN;
        }
        else if (char_is_digit(sym))
        {
            if (result == ETokenType::UNKNOWN)
                result = ETokenType::NUMBER;
            else if (result == ETokenType::WORDNUM || result == ETokenType::WORD)
                result = ETokenType::WORDNUM;
            else if (result != ETokenType::NUMBER)
                return ETokenType::UNKNOWN;
        }
        else if (char_is_separator(sym) || char_is_control(sym))
        {
            if (result == ETokenType::UNKNOWN)
                result = ETokenType::SEPARATOR;
            else if (result != ETokenType::SEPARATOR)
                return ETokenType::UNKNOWN;
        }
        else if (char_general_category(sym) == GC::Sk
            || char_general_category(sym) == GC::Sm || char_general_category(sym) == GC::Sc)
        {
            if (result == ETokenType::UNKNOWN)
                result = ETokenType::SYMBOL;
            else if (result != ETokenType::SYMBOL)
                return ETokenType::UNKNOWN;
        }
        else if (char_general_category(sym) == GC::So
            || char_general_category(sym) == GC::Lo
            )
        {
            if (result == ETokenType::UNKNOWN)
                result = ETokenType::HIEROGLYPH;
            else if (result != ETokenType::HIEROGLYPH)
                return ETokenType::UNKNOWN;
        }
        else if (char_is_letter(sym))
        {
            if (result == ETokenType::UNKNOWN)
                result = ETokenType::WORD;
            else if (result == ETokenType::WORDNUM || result == ETokenType::NUMBER)
                result = ETokenType::WORDNUM;
            else if (result != ETokenType::WORD)
                return ETokenType::UNKNOWN;
        }
    }
    return result;
}
bool isUpperCaseStr(const Ustring & str)
{
    for (const auto & sym : utf_range(str))
        if (sym != '-' && !char_is_uppercase(sym))
            return false;
    return true;
}

bool isLowerCaseStr(const Ustring & str)
{
    for (const auto & sym : utf_range(str))
        if (sym != '-' && !char_is_lowercase(sym))
            return false;
    return true;
}

bool isTitleCaseStr(const Ustring & str)
{
    if (str.length() < 2) return false;
    auto range = utf_range(str);
    auto start = range.begin();
    if (!char_is_uppercase(*start))
        return false;

    for (auto it = ++start; it != range.end(); ++it)
        if (*it != '-' && !char_is_lowercase(*it))
            return false;
    return true;
}

bool checkScript(const Ustring & str, const Ustring & scriptname)
{
    for (const auto & sym : utf_range(str))
    {
        if (sym != '-' && char_script(sym) != scriptname)
            return false;
    }
    return true;
}


EGraphemTag detectGraphemTag(const Ustring & str, ETokenType token_type, size_t length)
{
    EGraphemTag result = EGraphemTag::UNKNOWN;
    switch (token_type)
    {
    case ETokenType::WORD:
        if (isUpperCaseStr(str))
            result |= EGraphemTag::UPPER_CASE;
        else if (isLowerCaseStr(str))
            result |= EGraphemTag::LOWER_CASE;
        else if (isTitleCaseStr(str))
            result |= EGraphemTag::TITLE_CASE;
        else
            result |= EGraphemTag::MIXED_CASE;
        if (checkScript(str, "Latn"))
            result |= EGraphemTag::LATIN;
        else if (checkScript(str, "Cyrs"))
            result |= EGraphemTag::CYRILLIC;
        else
            result |= EGraphemTag::MIX_LANG;
        break;
    case ETokenType::PUNCT:
        if (length > 1)
        {
            result |= EGraphemTag::MULTI_PUNCT;
            if (str_find_first_of("?!.…;", str).valid())
                result |= EGraphemTag::CAN_TERMINATE_SENTENCE;
            else if (str_find_first_not_of("(){}[]\"\'", str).valid())
                result |= EGraphemTag::PAIR;
        }
        else
        {
            if (str_find_first_of("?!.…;", str).valid())
                result |= EGraphemTag::CAN_TERMINATE_SENTENCE;
            else if (str[0] == ')' || str[0] == '(' || str[0] == '{'
                || str[0] == '}' || str[0] == '[' || str[0] == ']'
                || str[0] == '"' || str[0] == '\'')
                result |= EGraphemTag::PAIR;
        }
        break;
    case ETokenType::SEPARATOR:
        if (length > 1)
        {
            result |= EGraphemTag::MULTI_SEP;
            for (const auto & chr : str)
                if (char_is_line_break(chr))
                    result |= EGraphemTag::CAN_TERMINATE_PARAGRAPH;
        }
        else
        {
            if (char_is_line_break(str[0]))
                result |= EGraphemTag::CAN_TERMINATE_PARAGRAPH;
        }
        break;

    case ETokenType::SYMBOL:
        if (length > 1)
            result |= EGraphemTag::MULTI_SYMBOL;
        break;
    default:
        break;
    }
    return result;
}

}

std::shared_ptr<Token> Token::createToken(const std::string & data)
{
    auto result = std::make_shared<Token>();
    auto impl = std::make_unique<detail::TokenImpl>();
    impl->data = data;
    size_t length = str_length(data);
    impl->length = length;
    impl->type_tag = detectTokenType(data);
    impl->graphem_tag = detectGraphemTag(data, impl->type_tag, length);
    result->impl = std::move(impl);
    return result;
}

std::shared_ptr<Token> Token::toUpper(const std::shared_ptr<Token> token)
{
    auto result = std::make_shared<Token>(*token);
    if (token->getTokenType() == ETokenType::WORD)
    {
        str_uppercase_in(result->impl->data);
        result->impl->graphem_tag = exclude(result->impl->graphem_tag, EGraphemTag::LOWER_CASE | EGraphemTag::MIXED_CASE | EGraphemTag::TITLE_CASE);
        result->impl->graphem_tag |= EGraphemTag::UPPER_CASE;
    }
    return result;
}
std::shared_ptr<Token> Token::toLower(const std::shared_ptr<Token> token)
{
    auto result = std::make_shared<Token>(*token);
    if (token->getTokenType() == ETokenType::WORD)
    {
        str_lowercase_in(result->impl->data);
        result->impl->graphem_tag = exclude(result->impl->graphem_tag, EGraphemTag::UPPER_CASE | EGraphemTag::MIXED_CASE | EGraphemTag::TITLE_CASE);
        result->impl->graphem_tag |= EGraphemTag::LOWER_CASE;
    }
    return result;
}
std::shared_ptr<Token> Token::toTitle(const std::shared_ptr<Token> token)
{
    auto result = std::make_shared<Token>(*token);
    if (token->getTokenType() == ETokenType::WORD)
    {
        str_titlecase_in(result->impl->data);
        result->impl->graphem_tag = exclude(result->impl->graphem_tag, EGraphemTag::UPPER_CASE | EGraphemTag::MIXED_CASE | EGraphemTag::LOWER_CASE);
        result->impl->graphem_tag |= EGraphemTag::TITLE_CASE;
    }

    return result;
}


TokenPtr Token::concat(const std::vector<TokenPtr> & tokens)
{
    auto result = std::make_shared<Token>();
    result->impl = std::make_unique<detail::TokenImpl>();
    ETokenType type_tag = ETokenType::UNKNOWN;
    EGraphemTag graphem_tag = EGraphemTag::UNKNOWN;
    bool first = true;
    size_t sum_length = 0;
    for (auto token : tokens)
    {
        if (type_tag == ETokenType::UNKNOWN && first)
        {
            first = false;
            type_tag = token->getTokenType();
        }
        else if (type_tag == ETokenType::NUMBER)
        {
            switch(token->getTokenType())
            {
            case ETokenType::NUMBER:
                break;
            case ETokenType::WORD:
            case ETokenType::WORDNUM:
                type_tag = ETokenType::WORDNUM;
            default:
                type_tag = ETokenType::UNKNOWN;
            }
        }
        else if (type_tag == ETokenType::WORD)
        {
            switch(token->getTokenType())
            {

            case ETokenType::WORD:
                break;
            case ETokenType::WORDNUM:
            case ETokenType::NUMBER:
                type_tag = ETokenType::WORDNUM;
                break;
            case ETokenType::PUNCT:
                if (token->getLength() == 1 && (*token)[0] == '-')
                {
                    graphem_tag |= EGraphemTag::HYPH_WORD;
                    break;
                }
            default:
                type_tag = ETokenType::UNKNOWN;
            }
        }
        else if (type_tag == ETokenType::WORDNUM)
        {
            switch(token->getTokenType())
            {

            case ETokenType::WORD:
            case ETokenType::WORDNUM:
            case ETokenType::NUMBER:
                break;
            case ETokenType::PUNCT:
                if (token->getLength() == 1 && (*token)[0] == '-')
                {
                    graphem_tag |= EGraphemTag::HYPH_WORD;
                    break;
                }
            default:
                type_tag = ETokenType::UNKNOWN;
            }
        }
        else if (type_tag == ETokenType::SEPARATOR)
        {
            if (token->getTokenType() != ETokenType::SEPARATOR)
                type_tag = ETokenType::UNKNOWN;
        }
        else if (type_tag == ETokenType::PUNCT)
        {
            if (token->getTokenType() != ETokenType::PUNCT)
                type_tag = ETokenType::UNKNOWN;
        }
        else if (type_tag == ETokenType::HIEROGLYPH)
        {
            if (token->getTokenType() != ETokenType::HIEROGLYPH)
                type_tag = ETokenType::UNKNOWN;
        }
        else if (type_tag == ETokenType::SYMBOL)
        {
            if (token->getTokenType() != ETokenType::SYMBOL)
                type_tag = ETokenType::UNKNOWN;
        }


        str_append(result->impl->data, token->getData());
        sum_length += token->getLength();
    }
    result->impl->length = sum_length;
    result->impl->type_tag = type_tag;

    if (type_tag != ETokenType::UNKNOWN)
        result->impl->graphem_tag = graphem_tag | detectGraphemTag(result->getData(), type_tag, result->getLength());

    return result;
}

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
    }
}

std::string toStringSingle(ETokenType tag)
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

std::string toString(EGraphemTag tag_bag, const std::string & sep)
{
    std::ostringstream os;
    std::vector<EGraphemTag> tags = toTagSet(tag_bag);
    for (size_t i = 0; i < tags.size() - 1; ++i)
        os << toStringSingle(tags[i]) << sep;
    os << toStringSingle(tags.back());
    return os.str();
}
std::string toString(ETokenType tag_bag, const std::string & sep)
{
    std::ostringstream os;
    std::vector<ETokenType> tags = toTagSet(tag_bag);
    for (size_t i = 0; i < tags.size() - 1; ++i)
        os << toStringSingle(tags[i]) << sep;
    os << toStringSingle(tags.back());
    return os.str();
}


}
