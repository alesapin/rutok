#include <gtest/gtest.h>
#include <unicorn/string.hpp>
#include <unicorn/character.hpp>
#include <streams/TokenInputStream.h>
#include <sstream>

using namespace RS;
using namespace RS::Unicorn;
using namespace tokenize;

std::istringstream strToSteam(const std::string & str)
{
    return std::istringstream(str, std::ios::binary);
}

void test_simple_stream(const std::string & str, size_t buf_size)
{
    auto ss = strToSteam(str);
    TokenInputStream strm(ss, buf_size);
    auto first = strm.read();
    auto space1 = strm.read();
    auto second = strm.read();
    auto space2 = strm.read();
    auto third = strm.read();
    EXPECT_TRUE(strm.eof());
    EXPECT_EQ(strm.read(), nullptr);

    EXPECT_EQ(space1->getData(), " ");
    EXPECT_EQ(space2->getData(), " ");
    EXPECT_EQ(space1->getTokenType(), space2->getTokenType());
    EXPECT_EQ(space1->getTokenType(), ETokenType::SEPARATOR);


    EXPECT_EQ(first->getData(), "hello");
    EXPECT_EQ(second->getData(), "beautiful");
    EXPECT_EQ(third->getData(), "world");

    EXPECT_EQ(first->getTokenType(), ETokenType::WORD);
    EXPECT_EQ(second->getTokenType(), ETokenType::WORD);
    EXPECT_EQ(third->getTokenType(), ETokenType::WORD);

    EXPECT_EQ(first->getGraphemTag(), EGraphemTag::LATIN | EGraphemTag::LOWER_CASE);
    EXPECT_EQ(second->getGraphemTag(), EGraphemTag::LATIN | EGraphemTag::LOWER_CASE);
    EXPECT_EQ(third->getGraphemTag(), EGraphemTag::LATIN | EGraphemTag::LOWER_CASE);
}

TEST(TokenStreamTest, WordSequence)
{
    std::string str{"hello beautiful world"};
    test_simple_stream(str, 4096);
    test_simple_stream(str, str.length());
    test_simple_stream(str, str.length() - 1);
    test_simple_stream(str, 3);
    test_simple_stream(str, 2);
    test_simple_stream(str, 1);
    try {
        auto ss = strToSteam(str);
        TokenInputStream strm{ss, 0};
        EXPECT_TRUE(false);
    }
    catch(std::runtime_error & err)
    {
        EXPECT_TRUE(true);
    }
}

TEST(TokenStreamTest, StrangeSequences)
{
    auto ss = strToSteam(R"***(Привет, хElLo,>	world,...
ту156)***");
    TokenInputStream strm(ss);
    auto privet = strm.read();
    auto comma1 = strm.read();
    auto space1 = strm.read();
    auto hello = strm.read();
    auto comma2 = strm.read();
    auto greater = strm.read();
    auto tab = strm.read();
    auto world = strm.read();
    auto comma3 = strm.read();
    EXPECT_FALSE(strm.eof());
    auto dot1 = strm.read();
    EXPECT_FALSE(strm.eof());
    auto dot2 = strm.read();
    EXPECT_FALSE(strm.eof());
    auto dot3 = strm.read();
    EXPECT_FALSE(strm.eof());
    auto newline = strm.read();
    EXPECT_FALSE(strm.eof());
    auto ty = strm.read();
    EXPECT_TRUE(strm.eof());
    EXPECT_TRUE(strm.read() == nullptr);

    EXPECT_EQ(privet->getData(), "Привет");
    EXPECT_EQ(privet->getTokenType(), ETokenType::WORD);
    EXPECT_EQ(privet->getGraphemTag(), EGraphemTag::CYRILLIC | EGraphemTag::TITLE_CASE);

    EXPECT_EQ(comma1->getData(), ",");
    EXPECT_EQ(comma1->getTokenType(), ETokenType::PUNCT);
    EXPECT_EQ(*comma1, *comma2);
    EXPECT_EQ(*comma1, *comma3);

    EXPECT_EQ(dot1->getData(), ".");
    EXPECT_EQ(dot1->getTokenType(), ETokenType::PUNCT);
    EXPECT_EQ(dot1->getGraphemTag(), EGraphemTag::CAN_TERMINATE_SENTENCE);
    EXPECT_EQ(*dot1, *dot2);
    EXPECT_EQ(*dot1, *dot3);

    EXPECT_EQ(space1->getData(), " ");
    EXPECT_EQ(space1->getTokenType(), ETokenType::SEPARATOR);

    EXPECT_EQ(tab->getData(), "	");
    EXPECT_EQ(tab->getTokenType(), ETokenType::SEPARATOR);

    EXPECT_EQ(greater->getData(), ">");
    EXPECT_EQ(greater->getTokenType(), ETokenType::SYMBOL);

    EXPECT_EQ(ty->getData(), "ту156");
    EXPECT_EQ(ty->getTokenType(), ETokenType::WORDNUM);

    EXPECT_EQ(hello->getData(), "хElLo");
    EXPECT_EQ(hello->getTokenType(), ETokenType::WORD);
    EXPECT_EQ(hello->getGraphemTag(), EGraphemTag::MIXED_CASE | EGraphemTag::MIX_LANG);
}

TEST(TokenStreamTest, CornerCases)
{
    std::string long_str(10000, 'q');
    long_str += '\n';
    auto ss = strToSteam(long_str);
    TokenInputStream strm(ss, 7000);

    auto qqqq = strm.read();
    auto newline = strm.read();
    EXPECT_TRUE(strm.eof());

    EXPECT_EQ(qqqq->getData().length(), 10000);
    EXPECT_EQ(qqqq->getTokenType(), ETokenType::WORD);

    EXPECT_EQ(newline->getTokenType(), ETokenType::SEPARATOR);
    EXPECT_EQ(newline->getGraphemTag(), EGraphemTag::CAN_TERMINATE_PARAGRAPH);
}
