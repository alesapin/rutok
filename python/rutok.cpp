#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <core/Tag.h>
#include <core/Token.h>
#include <core/Sentence.h>
#include <pybind11/stl.h>
#include <cstdio>
#include <deque>
#include <pybind11/iostream.h>
#include <streams/BaseCharInputStream.h>
#include <streams/EncodingInputStreamFromDescriptor.h>
#include <streams/EncodingInputStreamFromString.h>
#include <streams/TokenInputStream.h>
#include <streams/BaseTokenInputStream.h>
#include <streams/BaseTokenConcatInputStream.h>
#include <streams/IdenticalConcatInputStream.h>
#include <streams/SmallGroupsTokenConcatInputStream.h>
#include <streams/SentenceInputStream.h>

namespace py = pybind11;
using namespace tokenize;

class CommonInputStreamFromFD
{
    EncodingInputStreamFromDescriptor enc_inp;
    TokenInputStream tok_inp;
    IdenticalConcatInputStream ident_inp;
    SmallGroupsTokenConcatInputStream small_inp;
    SentenceInputStream sent_inp;

public:
    CommonInputStreamFromFD(int fd, size_t buffer_size)
        : enc_inp(fd, buffer_size)
        , tok_inp(enc_inp, buffer_size)
        , ident_inp(tok_inp)
        , small_inp(ident_inp)
        , sent_inp(small_inp)
    {}

    SentencePtr read()
    {
        if (!sent_inp.eof())
            return sent_inp.read();

        return nullptr;
    }
};

class CommonInputStreamFromString
{
private:
    EncodingInputStreamFromString enc_inp;
    TokenInputStream tok_inp;
    IdenticalConcatInputStream ident_inp;
    SmallGroupsTokenConcatInputStream small_inp;
    SentenceInputStream sent_inp;

public:
    CommonInputStreamFromString(const std::string & s, size_t buffer_size)
        : enc_inp(s, buffer_size)
        , tok_inp(enc_inp, buffer_size)
        , ident_inp(tok_inp)
        , small_inp(ident_inp)
        , sent_inp(small_inp)
    {}

    SentencePtr read()
    {
        if (!sent_inp.eof())
            return sent_inp.read();

        return nullptr;
    }
};

PYBIND11_MODULE(rutokpy, m) {
    py::enum_<ETokenType>(m, "TokenType", py::arithmetic())
        .value("UNKNOWN", ETokenType::UNKNOWN)
        .value("WORD", ETokenType::WORD)
        .value("PUNCT", ETokenType::PUNCT)
        .value("SEPARATOR", ETokenType::SEPARATOR)
        .value("NUMBER", ETokenType::NUMBER)
        .value("WORDNUM", ETokenType::WORDNUM)
        .value("HIEROGLYPH", ETokenType::HIEROGLYPH)
        .value("SYMBOL", ETokenType::SYMBOL);

    py::enum_<EGraphemTag>(m, "GraphemTag", py::arithmetic())
        .value("UNKNOWN", EGraphemTag::UNKNOWN)
        .value("CYRILLIC", EGraphemTag::CYRILLIC)
        .value("LATIN", EGraphemTag::LATIN)
        .value("UPPER_CASE", EGraphemTag::UPPER_CASE)
        .value("LOWER_CASE", EGraphemTag::LOWER_CASE)
        .value("MIXED_CASE", EGraphemTag::MIXED_CASE)
        .value("TITLE_CASE", EGraphemTag::TITLE_CASE)
        .value("SPACED", EGraphemTag::SPACED)
        .value("CAN_TERMINATE_SENTENCE", EGraphemTag::CAN_TERMINATE_SENTENCE)
        .value("PAIR", EGraphemTag::PAIR)
        .value("MULTI_PUNCT", EGraphemTag::MULTI_PUNCT)
        .value("FAKEPUNCT", EGraphemTag::FAKEPUNCT)
        .value("MUST_TERMINATE_SENTENCE", EGraphemTag::MUST_TERMINATE_SENTENCE)
        .value("NORMAL", EGraphemTag::NORMAL)
        .value("ROMAN", EGraphemTag::ROMAN)
        .value("CAN_TERMINATE_PARAGRAPH", EGraphemTag::CAN_TERMINATE_PARAGRAPH)
        .value("MULTI_SEP", EGraphemTag::MULTI_SEP)
        .value("HYPH_WORD", EGraphemTag::HYPH_WORD)
        .value("MIX_LANG", EGraphemTag::MIX_LANG)
        .value("MULTI_SYMBOL", EGraphemTag::MULTI_SYMBOL)
        .value("WRAPPED_WORD", EGraphemTag::WRAPPED_WORD)
        .def_static("to_tag_set", &toTagSet<EGraphemTag>)
        .def_static("contains", &contains<EGraphemTag>);

    py::enum_<ESemanticTag>(m, "SemanticTag", py::arithmetic())
        .value("UNKNOWN", ESemanticTag::UNKNOWN)
        .value("SENTENCE_END", ESemanticTag::SENTENCE_END)
        .value("PARAGRAPH_END", ESemanticTag::PARAGRAPH_END)
        .def_static("to_tag_set", &toTagSet<ESemanticTag>)
        .def_static("contains", &contains<ESemanticTag>);

    py::class_<Token>(m, "TokenImpl")
        .def("__getitem__", [](const Token & tok, size_t index) { return tok.at(index); }, py::is_operator())
        .def(py::init(&Token::createToken))
        .def("__len__", &Token::getLength)
        .def("__str__", &Token::getData)
        .def("__eq__", &Token::operator==)
        .def("__ne__", &Token::operator!=)
        .def("__lt__", &Token::operator<)
        .def("__gt__", &Token::operator>)
        .def("equals_ignorecase", &Token::equalsIgnoreCase)
        .def("is_upper", &Token::isUpper)
        .def("is_lower", &Token::isLower)
        .def("is_title", &Token::isTitle)
        .def("is_latin", &Token::isLatin)
        .def("is_cyrillic", &Token::isCyrillic)
        .def("get_token_type", &Token::getTokenType)
        .def("get_graphem_tag", &Token::getGraphemTag)
        .def("get_semantic_tag", &Token::getSemanticTag)
        .def("get_escaped_data", &Token::getEscapedData)
        .def("get_length_in_bytes", &Token::getBytesLength)
        .def_static("upper", &Token::toUpper)
        .def_static("lower", &Token::toLower)
        .def_static("title", &Token::toTitle)
        .def_static("refine", &Token::refine)
        .def_static("concat", &Token::concat);

    py::class_<TokenHasher>(m, "TokenHasherImpl")
        .def(py::init())
        .def("hashit", &TokenHasher::operator());

    py::class_<CommonInputStreamFromFD>(m, "CommonInputStreamFromFD")
        .def(py::init<int, size_t>())
        .def("read", &CommonInputStreamFromFD::read);

    py::class_<CommonInputStreamFromString>(m, "CommonInputStreamFromString")
        .def(py::init<const std::string &, size_t>())
        .def("read", &CommonInputStreamFromString::read);


    py::class_<Sentence>(m, "SentenceImpl")
        .def("tokens_count", &Sentence::tokensCount)
        .def("words_count", &Sentence::wordsCount)
        .def("characters_count", &Sentence::charactersCount)
        .def("__len__", &Sentence::tokensCount)
        .def("bytes_count", &Sentence::bytesCount)
        .def("is_ends_correctly", &Sentence::isEndsCorrectly)
        .def("is_starts_correctly", &Sentence::isStartsCorrectly)
        .def("is_quoted", &Sentence::isQuoted)
        .def("is_words_only", &Sentence::isWordsOnly)
        .def("is_cyrillic", &Sentence::isCyrillic)
        .def("is_latin", &Sentence::isLatin)
        .def("empty", &Sentence::isEmpty)
        .def("get_token", &Sentence::getTokenRef)
        .def("as_text", &Sentence::asText)
        .def_static("to_words", &Sentence::toWordsOnly);
}
