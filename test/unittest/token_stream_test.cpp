#include <gtest/gtest.h>
#include <unicorn/string.hpp>
#include <unicorn/character.hpp>
#include <unicorn/mbcs.hpp>
#include <streams/TokenInputStream.h>
#include <streams/TokenStringOutputStream.h>
#include <streams/TokenJSONOutputStream.h>
#include <streams/SmallGroupsTokenConcatInputStream.h>
#include <streams/IdenticalConcatInputStream.h>
#include <streams/EncodingOutputStream.h>
#include <streams/EncodingInputStream.h>
#include <sstream>

using namespace RS;
using namespace RS::Unicorn;
using namespace tokenize;
using IStringStreamPtr = std::shared_ptr<std::istringstream>;
using EncodingStreamPtr = std::shared_ptr<EncodingInputStream>;

std::pair<EncodingStreamPtr, IStringStreamPtr> strToSteam(const std::string & str)
{
    IStringStreamPtr iss = std::make_shared<std::istringstream>(str, std::ios::binary);
    EncodingStreamPtr result = std::make_shared<EncodingInputStream>(*iss);
    return std::make_pair(result, iss);
}

void test_simple_stream(const std::string & str, size_t buf_size)
{
    auto [ss, iss] = strToSteam(str);
    TokenInputStream strm(*ss, buf_size);
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

TEST(TokenInputStreamTest, WordSequence)
{
    std::string str{"hello beautiful world"};
    test_simple_stream(str, 4096);
    test_simple_stream(str, str.length());
    test_simple_stream(str, str.length() - 1);
    test_simple_stream(str, 3);
    test_simple_stream(str, 2);
    test_simple_stream(str, 1);
    try {
        auto [ss, iss] = strToSteam(str);
        TokenInputStream strm{*ss, 0};
        EXPECT_TRUE(false);
    }
    catch(std::runtime_error & err)
    {
        EXPECT_TRUE(true);
    }
}

TEST(TokenInputStreamTest, StrangeSequences)
{
    auto [ss, iss] = strToSteam(R"***(Привет, хElLo,>	world,...
ту156)***");
    TokenInputStream strm(*ss);
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

TEST(TokenInputStreamTest, CornerCases)
{
    std::string long_str(10000, 'q');
    long_str += '\n';
    auto [ss, iss] = strToSteam(long_str);
    TokenInputStream strm(*ss, 7000);

    auto qqqq = strm.read();
    auto newline = strm.read();
    EXPECT_TRUE(strm.eof());

    EXPECT_EQ(qqqq->getData().length(), 10000);
    EXPECT_EQ(qqqq->getTokenType(), ETokenType::WORD);

    EXPECT_EQ(newline->getTokenType(), ETokenType::SEPARATOR);
    EXPECT_EQ(newline->getGraphemTag(), EGraphemTag::CAN_TERMINATE_PARAGRAPH);
}

TEST(TokenInputStreamTest, TestStrangeCase)
{
    auto [ss1, iss] = strToSteam("hello.world.by");
    TokenInputStream concater1(*ss1);

    auto hello = concater1.read();
    auto dot = concater1.read();
    auto world = concater1.read();
    auto dot1 = concater1.read();
    auto by = concater1.read();
    EXPECT_TRUE(concater1.eof());

    EXPECT_EQ(hello->getData(), "hello");
    EXPECT_EQ(dot->getData(), ".");
    EXPECT_EQ(world->getData(), "world");
    EXPECT_EQ(dot1->getData(), ".");
    EXPECT_EQ(by->getData(), "by");
}


TEST(TokenOutputStreamTest, SimpleOutput)
{

    std::string str{"hello beautiful world"};
    auto [ss, iss] = strToSteam(str);
    TokenInputStream strm(*ss);
    std::ostringstream oss;
    EncodingOutputStream encoutput(oss);
    TokenStringOutputStream outstrm(encoutput);
    while(!strm.eof())
        outstrm.write(strm.read());
    std::string result = R"***([hello, WORD, LATIN, LOWER_CASE]
[ , SEPARATOR]
[beautiful, WORD, LATIN, LOWER_CASE]
[ , SEPARATOR]
[world, WORD, LATIN, LOWER_CASE]
)***";
    outstrm.flush();
    EXPECT_EQ(result, oss.str());
}

TEST(TokenOutputStreamTest, SimpleJSON)
{

    std::string str{"hello "};
    auto [ss, iss] = strToSteam(str);
    TokenInputStream strm(*ss);
    std::ostringstream oss;
    EncodingOutputStream encoutput(oss);
    TokenJSONOutputStream outstrm(encoutput, false);
    outstrm.start();
    while(!strm.eof())
        outstrm.write(strm.read());
    outstrm.finish();
    outstrm.flush();
    std::string result = "[{\"text\":\"hello\",\"token_type\":\"WORD\",\"graphem_tags\":[\"LATIN\",\"LOWER_CASE\"],\"semantic_tags\":[]},{\"text\":\" \",\"token_type\":\"SEPARATOR\",\"graphem_tags\":[],\"semantic_tags\":[]}]";
    EXPECT_EQ(result, oss.str());
}

TEST(TokenOutputStreamTest, PrettyJSON)
{
    std::string str{"hello "};
    auto [ss, iss] = strToSteam(str);
    TokenInputStream strm(*ss);
    std::ostringstream oss;
    EncodingOutputStream encoutput(oss);
    TokenJSONOutputStream outstrm(encoutput, true);
    outstrm.start();
    while(!strm.eof())
        outstrm.write(strm.read());
    outstrm.finish();
    outstrm.flush();
    std::string result = R"***([{
        "text":"hello",
        "token_type":"WORD",
        "graphem_tags": ["LATIN","LOWER_CASE"],
        "semantic_tags": []
    }, {
        "text":" ",
        "token_type":"SEPARATOR",
        "graphem_tags": [],
        "semantic_tags": []
    }])***";
    EXPECT_EQ(result, oss.str());
}


TEST(TokenConcatInputStreamTest, TestHyphCases)
{
    auto [ss1, iss1] = strToSteam("hello-world");
    TokenInputStream strm1(*ss1);
    SmallGroupsTokenConcatInputStream concater1(strm1);
    auto token = concater1.read();
    EXPECT_TRUE(concater1.eof());
    EXPECT_EQ(token->getData(), "hello-world");
    EXPECT_EQ(token->getTokenType(), ETokenType::WORD);
    EXPECT_EQ(token->getGraphemTag(), EGraphemTag::LATIN | EGraphemTag::LOWER_CASE | EGraphemTag::HYPH_WORD);

    auto [ss2, iss] = strToSteam("комсомольск-на-амуре-что-бывает");
    TokenInputStream strm2(*ss2);
    SmallGroupsTokenConcatInputStream concater2(strm2);
    auto token1 = concater2.read();
    EXPECT_TRUE(concater2.eof());
    EXPECT_EQ(token1->getData(), "комсомольск-на-амуре-что-бывает");
    EXPECT_EQ(token1->getTokenType(), ETokenType::WORD);
    EXPECT_EQ(token1->getGraphemTag(), EGraphemTag::CYRILLIC | EGraphemTag::LOWER_CASE | EGraphemTag::HYPH_WORD);

    auto [ss3, iss3] = strToSteam("комсомольск-на-амуре-что");
    TokenInputStream strm3(*ss3);
    SmallGroupsTokenConcatInputStream concater3(strm3);
    auto token2 = concater3.read();
    EXPECT_TRUE(concater3.eof());
    EXPECT_EQ(token2->getData(), "комсомольск-на-амуре-что");
    EXPECT_EQ(token2->getTokenType(), ETokenType::WORD);
    EXPECT_EQ(token2->getGraphemTag(), EGraphemTag::CYRILLIC | EGraphemTag::LOWER_CASE | EGraphemTag::HYPH_WORD);

    auto [ss4, iss4] = strToSteam("комсомольск-на-амуре-что-");
    TokenInputStream strm4(*ss4);
    SmallGroupsTokenConcatInputStream concater4(strm4);
    auto token4 = concater4.read();
    auto token5 = concater4.read();

    EXPECT_TRUE(concater4.eof());

    EXPECT_EQ(token4->getData(), "комсомольск-на-амуре-что");
    EXPECT_EQ(token4->getTokenType(), ETokenType::WORD);
    EXPECT_EQ(token4->getGraphemTag(), EGraphemTag::CYRILLIC | EGraphemTag::LOWER_CASE | EGraphemTag::HYPH_WORD);

    EXPECT_EQ(token5->getData(), "-");
    EXPECT_EQ(token5->getTokenType(), ETokenType::PUNCT);

    auto [ss5, iss5] = strToSteam("комсомольск-на-амуре--xnj");
    TokenInputStream strm5(*ss5);
    SmallGroupsTokenConcatInputStream concater5(strm5);
    auto token6 = concater5.read();
    auto token7 = concater5.read();
    auto token8 = concater5.read();
    auto token9 = concater5.read();

    EXPECT_TRUE(concater5.eof());

    EXPECT_EQ(token6->getData(), "комсомольск-на-амуре");
    EXPECT_EQ(token6->getTokenType(), ETokenType::WORD);
    EXPECT_EQ(token6->getGraphemTag(), EGraphemTag::CYRILLIC | EGraphemTag::LOWER_CASE | EGraphemTag::HYPH_WORD);

    EXPECT_EQ(token7->getData(), "-");
    EXPECT_EQ(token7->getTokenType(), ETokenType::PUNCT);

    EXPECT_EQ(token8->getData(), "-");
    EXPECT_EQ(token8->getTokenType(), ETokenType::PUNCT);

    EXPECT_EQ(token9->getData(), "xnj");
    EXPECT_EQ(token9->getTokenType(), ETokenType::WORD);
    EXPECT_EQ(token9->getGraphemTag(), EGraphemTag::LATIN | EGraphemTag::LOWER_CASE);


    auto [ss6, iss6] = strToSteam("-hello-world-156");
    TokenInputStream strm6(*ss6);
    SmallGroupsTokenConcatInputStream concater6(strm6);
    auto token10 = concater6.read();
    auto token11 = concater6.read();

    EXPECT_TRUE(concater6.eof());

    EXPECT_EQ(token10->getData(), "-");
    EXPECT_EQ(token11->getData(), "hello-world-156");
    EXPECT_EQ(token11->getTokenType(), ETokenType::WORDNUM);
    EXPECT_EQ(token11->getGraphemTag(), EGraphemTag::HYPH_WORD);
}

void testPunctStr(const std::string & str, size_t num_chars)
{
    auto [ss1, iss1] = strToSteam(str);
    TokenInputStream strm1(*ss1);
    IdenticalConcatInputStream strm2(strm1);
    SmallGroupsTokenConcatInputStream concater1(strm2);
    auto token = concater1.read();
    EXPECT_EQ(token->getData().length(), num_chars);
    EXPECT_EQ(token->getTokenType(), ETokenType::PUNCT);
    EXPECT_EQ(token->getGraphemTag(), EGraphemTag::MULTI_PUNCT | EGraphemTag::MUST_TERMINATE_SENTENCE);
    EXPECT_TRUE(concater1.eof());
}


TEST(TokenConcatInputStreamTest, TestDotCases)
{
    testPunctStr("...", 3);
    testPunctStr("......", 6);
    testPunctStr(".........", 9);

    auto [ss1, iss1] = strToSteam("hello.....");
    TokenInputStream strm1(*ss1);
    IdenticalConcatInputStream strm2(strm1);
    SmallGroupsTokenConcatInputStream concater1(strm2);

    auto hello = concater1.read();
    auto five_dots = concater1.read();
    EXPECT_TRUE(concater1.eof());

    EXPECT_EQ(hello->getData(), "hello");
    EXPECT_EQ(five_dots->getData(), ".....");
}

TEST(TokenConcatInputStreamTest, TestExclQuestion)
{
    auto [ss1, iss1] = strToSteam("hello?!");
    TokenInputStream strm1(*ss1);
    SmallGroupsTokenConcatInputStream concater1(strm1);
    auto hello = concater1.read();
    auto exclq = concater1.read();
    EXPECT_TRUE(concater1.eof());
    EXPECT_EQ(hello->getData(), "hello");
    EXPECT_EQ(exclq->getData(), "?!");
    EXPECT_EQ(exclq->getTokenType(), ETokenType::PUNCT);
    EXPECT_EQ(exclq->getGraphemTag(), EGraphemTag::MUST_TERMINATE_SENTENCE | EGraphemTag::MULTI_PUNCT);

    auto [ss2, iss2] = strToSteam("?!\?\?\?!?");
    TokenInputStream strm2(*ss2);
    SmallGroupsTokenConcatInputStream concater2(strm2);
    auto exclq1 = concater2.read();
    EXPECT_TRUE(concater2.eof());

    EXPECT_EQ(exclq1->getData(), "?!\?\?\?!?");
}

TEST(TokenConcatInputStreamTest, TestWordWrap)
{
    {
        auto [ss1, iss1] = strToSteam(R"***(че-
ловек)***");
        TokenInputStream strm1(*ss1);
        IdenticalConcatInputStream strm2(strm1);
        SmallGroupsTokenConcatInputStream concater1(strm2);
        auto word = concater1.read();
        EXPECT_TRUE(concater1.eof());
        EXPECT_EQ(word->getData(), "человек");
        EXPECT_TRUE(contains(word->getGraphemTag(), EGraphemTag::WRAPPED_WORD));
    }
    {
        auto [ss1, iss1] = strToSteam(R"***(че-
        ловек)***");
        TokenInputStream strm1(*ss1);
        IdenticalConcatInputStream strm2(strm1);
        SmallGroupsTokenConcatInputStream concater1(strm2);
        auto word = concater1.read();
        EXPECT_TRUE(concater1.eof());
        EXPECT_EQ(word->getData(), "человек");
        EXPECT_TRUE(contains(word->getGraphemTag(), EGraphemTag::WRAPPED_WORD));
    }
    {
        auto [ss1, iss1] = strToSteam(R"***(когда находится че-
ловек в изоляции, а приговора еще нет.)***");
        TokenInputStream strm1(*ss1);
        IdenticalConcatInputStream strm2(strm1);
        SmallGroupsTokenConcatInputStream concater1(strm2);
        auto word1 = concater1.read();
        auto sepr = concater1.read();
        auto word2 = concater1.read();
        auto sepr2 = concater1.read();
        auto word3 = concater1.read();
        EXPECT_TRUE(contains(word3->getGraphemTag(), EGraphemTag::WRAPPED_WORD));
        EXPECT_EQ(word3->getData(), "человек");

    }

}

void simpleEncodingCheck(const std::string & str, const std::string & readen, size_t buf_size, size_t read_size)
{
    auto [_, ss1] = strToSteam(str);
    EncodingInputStream iss(*ss1, buf_size);
    std::string result;
    std::string tmp;
    while(iss.read(tmp, read_size))
        result += tmp;
    EXPECT_EQ(result, readen);
}

TEST(EncodingStreamTest, TestEncoding)
{
    simpleEncodingCheck("hello!?", "hello!?", 100, 100);
    simpleEncodingCheck("привет!?", "привет!?", 100, 100);
    std::string to_export{"привет!?"};
    std::string result;
    export_string(to_export, result, "windows-1251");
    simpleEncodingCheck(result, to_export, 100, 2);
    simpleEncodingCheck(result, to_export, 3, 100);
    simpleEncodingCheck(result, to_export, 3, 1);

    to_export = "абырвлаг";
    export_string(to_export, result, "KOI8-R");

    simpleEncodingCheck(result, to_export, 100, 100);
}
