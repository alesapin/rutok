#include <gtest/gtest.h>
#include <core/Sentence.h>
#include <streams/TokenInputStream.h>
#include <sstream>

using namespace tokenize;
TEST(SentenceTest, SimpleFunctionsTest)
{
    std::string str{"Hello world."};
    auto ss = std::istringstream(str, std::ios::binary);
    TokenInputStream strm(ss);
    auto first = strm.read();
    auto second = strm.read();
    auto third = strm.read();
    auto fourth = strm.read();
    EXPECT_TRUE(strm.eof());
    Sentence s({first, second, third, fourth});
    EXPECT_EQ(s.tokensCount(), 4);
    EXPECT_EQ(s.wordsCount(), 2);
    EXPECT_EQ(s.charactersCount(), str.length());
    EXPECT_EQ(s.bytesCount(), str.length());

    EXPECT_TRUE(s.isEndsCorrectly());
    EXPECT_TRUE(s.isStartsCorrectly());
    EXPECT_FALSE(s.isQuoted());
    EXPECT_FALSE(s.isWordsOnly());
    EXPECT_FALSE(s.isCyrrilyc());
    EXPECT_TRUE(s.isLatin());
    EXPECT_FALSE(s.isEmpty());

    EXPECT_EQ(s.asText(), str);
    auto words_only_sentence = Sentence::toWordsOnly(s);

    EXPECT_EQ(words_only_sentence.wordsCount(), 2);
    EXPECT_EQ(words_only_sentence.tokensCount(), 3);
    EXPECT_EQ(words_only_sentence.asText(), "Hello world");
    EXPECT_TRUE(words_only_sentence.isWordsOnly());
}

TEST(SentenceTest, SimpleFunctionsTest2)
{
    std::string str{"\"Привет!\""};
    auto ss = std::istringstream(str, std::ios::binary);
    TokenInputStream strm(ss);
    auto first = strm.read();
    auto second = strm.read();
    auto third = strm.read();
    auto fourth = strm.read();
    EXPECT_TRUE(strm.eof());
    Sentence s({first, second, third, fourth});
    EXPECT_EQ(s.tokensCount(), 4);
    EXPECT_EQ(s.wordsCount(), 1);
    EXPECT_GT(s.bytesCount(), s.charactersCount());
    EXPECT_EQ(s.charactersCount(), 9);

    EXPECT_TRUE(s.isEndsCorrectly());
    EXPECT_TRUE(s.isStartsCorrectly());
    EXPECT_TRUE(s.isQuoted());
    EXPECT_FALSE(s.isWordsOnly());
    EXPECT_TRUE(s.isCyrrilyc());
    EXPECT_FALSE(s.isLatin());
    EXPECT_FALSE(s.isEmpty());

    EXPECT_EQ(s.asText(), str);
    auto words_only_sentence = Sentence::toWordsOnly(s);

    EXPECT_EQ(words_only_sentence.wordsCount(), 1);
    EXPECT_EQ(words_only_sentence.tokensCount(), 1);
    EXPECT_EQ(words_only_sentence.asText(), "Привет");
    EXPECT_TRUE(words_only_sentence.isWordsOnly());
}
