#include <gtest/gtest.h>
#include <unicorn/string.hpp>
#include <unicorn/character.hpp>
#include <core/Token.h>
#include <unordered_map>

using namespace RS;
using namespace RS::Unicorn;
using namespace tokenize;

size_t getLengthViaToken(const std::string & str)
{
    auto token = Token::createToken(str);
    return token->getLength();
}

TEST(TokenTest, Length)
{
    std::string str{"hello world"};
    EXPECT_EQ(str.length(), getLengthViaToken(str));
    std::string rustr{"Привет мир"};
    EXPECT_EQ(10, getLengthViaToken(rustr));
    std::string rustr_newline{"Привер\nмир!"};
    EXPECT_EQ(11, getLengthViaToken(rustr_newline));
    std::string exotic_unicode{"益ツ"};
    EXPECT_EQ(2, getLengthViaToken(exotic_unicode));
}
ETokenType getTypeViaToken(const std::string & str)
{
    auto token = Token::createToken(str);
    return token->getTokenType();
}
EGraphemTag getGraphemTagViaToken(const std::string & str)
{
    auto token = Token::createToken(str);
    return token->getGraphemTag();
}

bool graphemTagContains(const std::string & str, EGraphemTag tag)
{
    return contains(getGraphemTagViaToken(str), tag);
}

GC getCharGC(const std::string & str, size_t i)
{
    return char_general_category(str_char_at(str, i));
}

TEST(TokenTest, TokenType)
{
    std::string str{"hello"};
    EXPECT_EQ(ETokenType::WORD, getTypeViaToken(str));
    std::string rustr{"Привет"};
    EXPECT_EQ(ETokenType::WORD, getTypeViaToken(rustr));
    std::string diacritic{"ö"};
    EXPECT_EQ(ETokenType::WORD, getTypeViaToken(diacritic));

    std::string punct{"!"};
    EXPECT_EQ(ETokenType::PUNCT, getTypeViaToken(punct));
    std::string punct2{"…;?\"&@*\\/#"};
    EXPECT_EQ(ETokenType::PUNCT, getTypeViaToken(punct2));

    std::string sym{">"};
    EXPECT_EQ(ETokenType::SYMBOL, getTypeViaToken(sym));
    std::string sym1{"|^$`"};
    EXPECT_EQ(ETokenType::SYMBOL, getTypeViaToken(sym1));

    std::string exotic_unicode{"ツ"};
    EXPECT_EQ(ETokenType::HIEROGLYPH, getTypeViaToken(exotic_unicode));
    std::string strange_letter{"傍點"};
    EXPECT_EQ(ETokenType::HIEROGLYPH, getTypeViaToken(strange_letter));

    std::string seprator{" \n	"};
    EXPECT_EQ(ETokenType::SEPARATOR, getTypeViaToken(seprator));
    std::string seprator2{"\t\n\r"};
    EXPECT_EQ(ETokenType::SEPARATOR, getTypeViaToken(seprator2));
    std::string seprator3{R"***(
)***"};
    EXPECT_EQ(ETokenType::SEPARATOR, getTypeViaToken(seprator3));

    std::string number{"123"};
    EXPECT_EQ(ETokenType::NUMBER, getTypeViaToken(number));
    std::string number2{"0563"};
    EXPECT_EQ(ETokenType::NUMBER, getTypeViaToken(number2));

    std::string wordnum{"су542"};
    EXPECT_EQ(ETokenType::WORDNUM, getTypeViaToken(wordnum));
    std::string wordnum2{"с4ц5"};
    EXPECT_EQ(ETokenType::WORDNUM, getTypeViaToken(wordnum2));
    std::string wordnum3{"555555L"};
    EXPECT_EQ(ETokenType::WORDNUM, getTypeViaToken(wordnum3));

    std::string unkn{"А Б Р"};
    EXPECT_EQ(ETokenType::UNKNOWN, getTypeViaToken(unkn));
    std::string unkn2{"Николай,"};
    EXPECT_EQ(ETokenType::UNKNOWN, getTypeViaToken(unkn2));
    std::string unkn3{"5 > 3"};
    EXPECT_EQ(ETokenType::UNKNOWN, getTypeViaToken(unkn3));
    std::string unkn4;
    EXPECT_EQ(ETokenType::UNKNOWN, getTypeViaToken(unkn4));

}


TEST(TokenTest, GraphemInfoWord)
{
    std::string word_latin{"hello"};
    EXPECT_EQ(EGraphemTag::LATIN | EGraphemTag::LOWER_CASE, getGraphemTagViaToken(word_latin));
    std::string word_cyr{"привет"};
    EXPECT_EQ(EGraphemTag::CYRILLIC | EGraphemTag::LOWER_CASE, getGraphemTagViaToken(word_cyr));
    std::string word_mix{"приll"};
    EXPECT_EQ(EGraphemTag::LOWER_CASE | EGraphemTag::MIX_LANG, getGraphemTagViaToken(word_mix));

    std::string word_latin_cap{"HELLO"};
    EXPECT_EQ(EGraphemTag::LATIN | EGraphemTag::UPPER_CASE, getGraphemTagViaToken(word_latin_cap));
    std::string word_cyr_cap{"ПРИВЕТ"};
    EXPECT_EQ(EGraphemTag::CYRILLIC | EGraphemTag::UPPER_CASE, getGraphemTagViaToken(word_cyr_cap));
    std::string word_mix_cap{"ПРИLL"};
    EXPECT_EQ(EGraphemTag::UPPER_CASE | EGraphemTag::MIX_LANG, getGraphemTagViaToken(word_mix_cap));

    std::string word_latin_title{"Hello"};
    EXPECT_EQ(EGraphemTag::LATIN | EGraphemTag::TITLE_CASE, getGraphemTagViaToken(word_latin_title));
    std::string word_cyr_title{"Привет"};
    EXPECT_EQ(EGraphemTag::CYRILLIC | EGraphemTag::TITLE_CASE, getGraphemTagViaToken(word_cyr_title));
    std::string word_mix_title{"Приll"};
    EXPECT_EQ(EGraphemTag::TITLE_CASE | EGraphemTag::MIX_LANG, getGraphemTagViaToken(word_mix_title));

    std::string word_latin_mixed{"HelLo"};
    EXPECT_EQ(EGraphemTag::LATIN | EGraphemTag::MIXED_CASE, getGraphemTagViaToken(word_latin_mixed));
    std::string word_cyr_mixed{"ПрВЕт"};
    EXPECT_EQ(EGraphemTag::CYRILLIC | EGraphemTag::MIXED_CASE, getGraphemTagViaToken(word_cyr_mixed));
    std::string word_mix_mixed{"ПриlL"};
    EXPECT_EQ(EGraphemTag::MIXED_CASE | EGraphemTag::MIX_LANG, getGraphemTagViaToken(word_mix_mixed));
}

TEST(TokenTest, GraphemInfoPunct)
{
    std::string dot{"."};
    EXPECT_EQ(EGraphemTag::CAN_TERMINATE_SENTENCE, getGraphemTagViaToken(dot));
    std::string end{"?!…"};
    EXPECT_EQ(EGraphemTag::MULTI_PUNCT | EGraphemTag::CAN_TERMINATE_SENTENCE, getGraphemTagViaToken(end));
    std::string end1{"…"};
    EXPECT_EQ(EGraphemTag::CAN_TERMINATE_SENTENCE, getGraphemTagViaToken(end1));
    std::string end2{";"};
    EXPECT_EQ(EGraphemTag::CAN_TERMINATE_SENTENCE, getGraphemTagViaToken(end2));
    std::string end3{"..."};
    EXPECT_EQ(EGraphemTag::MULTI_PUNCT | EGraphemTag::CAN_TERMINATE_SENTENCE, getGraphemTagViaToken(end3));

    std::string pair1{"{"};
    EXPECT_EQ(EGraphemTag::PAIR, getGraphemTagViaToken(pair1));

    std::string pair2{"\""};
    EXPECT_EQ(EGraphemTag::PAIR, getGraphemTagViaToken(pair2));

    std::string pair3{"''"};
    EXPECT_EQ(EGraphemTag::PAIR | EGraphemTag::MULTI_PUNCT, getGraphemTagViaToken(pair3));

    std::string empty{""};
    EXPECT_EQ(EGraphemTag::UNKNOWN, getGraphemTagViaToken(empty));

}


TEST(TokenTest, GraphemInfoSep)
{
    std::string sep{"\n"};
    EXPECT_EQ(EGraphemTag::CAN_TERMINATE_PARAGRAPH, getGraphemTagViaToken(sep));

    std::string sep2{"\n\r"};
    EXPECT_EQ(EGraphemTag::CAN_TERMINATE_PARAGRAPH | EGraphemTag::MULTI_SEP, getGraphemTagViaToken(sep2));

    std::string sep3{"                  "};
    EXPECT_EQ(EGraphemTag::MULTI_SEP, getGraphemTagViaToken(sep3));

    std::string sep4{"\t\t\n "};
    EXPECT_EQ(EGraphemTag::MULTI_SEP | EGraphemTag::CAN_TERMINATE_PARAGRAPH, getGraphemTagViaToken(sep4));
    std::string sep5{"\t\t"};
    EXPECT_EQ(EGraphemTag::MULTI_SEP, getGraphemTagViaToken(sep5));
}

TokenPtr merge(const std::vector<std::string> & strings)
{
    std::vector<TokenPtr> tokens;
    for (const auto & str : strings)
        tokens.push_back(Token::createToken(str));
    return Token::concat(tokens);
}

TEST(TokenTest, TestConcat)
{
    auto merged1 = merge({"при", "вет"});
    EXPECT_EQ(ETokenType::WORD, merged1->getTokenType());
    EXPECT_EQ(EGraphemTag::CYRILLIC | EGraphemTag::LOWER_CASE, merged1->getGraphemTag());

    auto merged2 = merge({"hell", "o"});
    EXPECT_EQ(ETokenType::WORD, merged2->getTokenType());
    EXPECT_EQ(EGraphemTag::LATIN | EGraphemTag::LOWER_CASE, merged2->getGraphemTag());

    auto merged3 = merge({"hell", "вет"});
    EXPECT_EQ(ETokenType::WORD, merged3->getTokenType());
    EXPECT_EQ(EGraphemTag::MIX_LANG | EGraphemTag::LOWER_CASE, merged3->getGraphemTag());

    auto merged4 = merge({"ПРИ", "ВЕТ"});
    EXPECT_EQ(ETokenType::WORD, merged4->getTokenType());
    EXPECT_EQ(EGraphemTag::CYRILLIC | EGraphemTag::UPPER_CASE, merged4->getGraphemTag());

    auto merged5 = merge({"ПРИ", "llo"});
    EXPECT_EQ(ETokenType::WORD, merged5->getTokenType());
    EXPECT_EQ(EGraphemTag::MIX_LANG | EGraphemTag::MIXED_CASE, merged5->getGraphemTag());

    auto merged6 = merge({"П", "р", "в", "е", "т"});
    EXPECT_EQ(ETokenType::WORD, merged6->getTokenType());
    EXPECT_EQ(EGraphemTag::CYRILLIC | EGraphemTag::TITLE_CASE, merged6->getGraphemTag());

    auto merged7 = merge({"Что", "-", "то"});
    EXPECT_EQ(ETokenType::WORD, merged7->getTokenType());
    EXPECT_EQ(EGraphemTag::CYRILLIC | EGraphemTag::TITLE_CASE | EGraphemTag::HYPH_WORD, merged7->getGraphemTag());

    auto merged8 = merge({"КОМСОМОЛЬСК", "-", "НА", "-", "АМУРЕ"});
    EXPECT_EQ(ETokenType::WORD, merged8->getTokenType());
    EXPECT_EQ(EGraphemTag::CYRILLIC | EGraphemTag::UPPER_CASE | EGraphemTag::HYPH_WORD, merged8->getGraphemTag());

    auto merged9 = merge({".", ".", "."});
    EXPECT_EQ(ETokenType::PUNCT, merged9->getTokenType());
    EXPECT_EQ(EGraphemTag::MULTI_PUNCT | EGraphemTag::CAN_TERMINATE_SENTENCE, merged9->getGraphemTag());

    auto merged10 = merge({" ", " ", "    ", " "});
    EXPECT_EQ(ETokenType::SEPARATOR, merged10->getTokenType());
    EXPECT_EQ(EGraphemTag::MULTI_SEP, merged10->getGraphemTag());

    auto merged11 = merge({" ", "\n", "    ", " "});
    EXPECT_EQ(ETokenType::SEPARATOR, merged10->getTokenType());
    EXPECT_EQ(EGraphemTag::MULTI_SEP | EGraphemTag::CAN_TERMINATE_PARAGRAPH, merged11->getGraphemTag());

    auto merged12 = merge({"ту", "156"});
    EXPECT_EQ(ETokenType::WORDNUM, merged12->getTokenType());
    EXPECT_EQ(EGraphemTag::UNKNOWN, merged12->getGraphemTag());

    auto merged13 = merge({"ту", "-", "156", "-", "самолёт"});
    EXPECT_EQ(ETokenType::WORDNUM, merged13->getTokenType());
    EXPECT_EQ(EGraphemTag::HYPH_WORD, merged13->getGraphemTag());

    auto merged14 = merge({"ту", ":", "222"});
    EXPECT_EQ(ETokenType::UNKNOWN, merged14->getTokenType());
    EXPECT_EQ(EGraphemTag::UNKNOWN, merged14->getGraphemTag());

    auto merged15 = merge({"Б", " ", "Ю"});
    EXPECT_EQ(ETokenType::UNKNOWN, merged15->getTokenType());
    EXPECT_EQ(EGraphemTag::UNKNOWN, merged15->getGraphemTag());

    auto merged16 = merge({"=", "<", ">"});
    EXPECT_EQ(ETokenType::SYMBOL, merged16->getTokenType());
    EXPECT_EQ(EGraphemTag::MULTI_SYMBOL, merged16->getGraphemTag());

    auto merged17 = merge({"=", "xxx", ">"});
    EXPECT_EQ(ETokenType::UNKNOWN, merged17->getTokenType());
    EXPECT_EQ(EGraphemTag::UNKNOWN, merged17->getGraphemTag());

    auto merged18 = merge({"", "", "", ""});
    EXPECT_EQ(ETokenType::UNKNOWN, merged18->getTokenType());
    EXPECT_EQ(EGraphemTag::UNKNOWN, merged18->getGraphemTag());
}

TEST(TokenTest, SimpleHash)
{
    std::unordered_map<Token, size_t, TokenHasher> counter;
    TokenPtr first = Token::createToken("hello");
    TokenPtr second = Token::createToken("world");
    TokenPtr third = Token::createToken("hello");
    TokenPtr fourth = Token::createToken("111");
    counter[*first] = 1;
    counter[*second] = 1;
    counter[*first] += 1;
    counter[*third] += 1;
    EXPECT_EQ(counter.size(), 2);
    EXPECT_EQ(counter[*first], 3);
    EXPECT_EQ(counter[*second], 1);
    EXPECT_EQ(counter[*third], 3);
    EXPECT_EQ(counter[*fourth], 0);
    EXPECT_EQ(counter.size(), 3);
}
