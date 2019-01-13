#include <gtest/gtest.h>
#include <core/Sentence.h>
#include <streams/TokenInputStream.h>
#include <streams/SmallGroupsTokenConcatInputStream.h>
#include <streams/SentenceInputStream.h>
#include <streams/EncodingInputStream.h>
#include <sstream>

using namespace tokenize;
TEST(SentenceTest, SimpleFunctionsTest)
{
    std::string str{"Hello world."};
    auto ss = std::istringstream(str, std::ios::binary);
    EncodingInputStream encss(ss);
    TokenInputStream strm(encss);
    auto first = strm.read();
    auto second = strm.read();
    auto third = strm.read();
    auto fourth = strm.read();
    EXPECT_TRUE(strm.eof());
    SentencePtr s = std::make_shared<Sentence>(std::deque<TokenPtr>{first, second, third, fourth});
    EXPECT_EQ(s->tokensCount(), 4);
    EXPECT_EQ(s->wordsCount(), 2);
    EXPECT_EQ(s->charactersCount(), str.length());
    EXPECT_EQ(s->bytesCount(), str.length());

    EXPECT_TRUE(s->isEndsCorrectly());
    EXPECT_TRUE(s->isStartsCorrectly());
    EXPECT_FALSE(s->isQuoted());
    EXPECT_FALSE(s->isWordsOnly());
    EXPECT_FALSE(s->isCyrrilyc());
    EXPECT_TRUE(s->isLatin());
    EXPECT_FALSE(s->isEmpty());

    EXPECT_EQ(s->asText(), str);
    auto words_only_sentence = Sentence::toWordsOnly(s);

    EXPECT_EQ(words_only_sentence->wordsCount(), 2);
    EXPECT_EQ(words_only_sentence->tokensCount(), 3);
    EXPECT_EQ(words_only_sentence->asText(), "Hello world");
    EXPECT_TRUE(words_only_sentence->isWordsOnly());
}

TEST(SentenceTest, SimpleFunctionsTest2)
{
    std::string str{"\"Привет!\""};
    auto ss = std::istringstream(str, std::ios::binary);
    EncodingInputStream encss(ss);
    TokenInputStream strm(encss);
    auto first = strm.read();
    auto second = strm.read();
    auto third = strm.read();
    auto fourth = strm.read();
    EXPECT_TRUE(strm.eof());
    SentencePtr s = std::make_shared<Sentence>(std::deque<TokenPtr>{first, second, third, fourth});
    EXPECT_EQ(s->tokensCount(), 4);
    EXPECT_EQ(s->wordsCount(), 1);
    EXPECT_GT(s->bytesCount(), s->charactersCount());
    EXPECT_EQ(s->charactersCount(), 9);

    EXPECT_TRUE(s->isEndsCorrectly());
    EXPECT_TRUE(s->isStartsCorrectly());
    EXPECT_TRUE(s->isQuoted());
    EXPECT_FALSE(s->isWordsOnly());
    EXPECT_TRUE(s->isCyrrilyc());
    EXPECT_FALSE(s->isLatin());
    EXPECT_FALSE(s->isEmpty());

    EXPECT_EQ(s->asText(), str);
    auto words_only_sentence = Sentence::toWordsOnly(s);

    EXPECT_EQ(words_only_sentence->wordsCount(), 1);
    EXPECT_EQ(words_only_sentence->tokensCount(), 1);
    EXPECT_EQ(words_only_sentence->asText(), "Привет");
    EXPECT_TRUE(words_only_sentence->isWordsOnly());
}

TEST(SentenceTest, SimpliestSentenceInputStreamTest)
{

    std::string str{"Hello world. Привет Мир."};
    auto ss = std::istringstream(str, std::ios::binary);
    EncodingInputStream encss(ss);
    TokenInputStream strm(encss);
    SmallGroupsTokenConcatInputStream concater(strm);
    SentenceInputStream sentence_stream(concater);

    auto sentence1 = sentence_stream.read();
    auto sentence2 = sentence_stream.read();
    EXPECT_TRUE(sentence_stream.eof());

    EXPECT_EQ(sentence1->asText(), "Hello world.");
    EXPECT_EQ(sentence2->asText(), "Привет Мир.");
}

TEST(SentenceTest, AdvancedSentenceInputStreamTest)
{

    std::string str{"Hello! world.Привет? Мир"};
    auto ss = std::istringstream(str, std::ios::binary);
    EncodingInputStream encss(ss);
    TokenInputStream strm(encss);
    SmallGroupsTokenConcatInputStream concater(strm);
    SentenceInputStream sentence_stream(concater);

    auto sentence1 = sentence_stream.read();
    auto sentence2 = sentence_stream.read();
    auto sentence3 = sentence_stream.read();
    auto sentence4 = sentence_stream.read();
    EXPECT_TRUE(sentence_stream.eof());

    EXPECT_EQ(sentence1->asText(), "Hello!");
    EXPECT_EQ(sentence2->asText(), "world.");
    EXPECT_EQ(sentence3->asText(), "Привет?");
    EXPECT_EQ(sentence4->asText(), "Мир");
}

TEST(SentenceTest, HellSentenceInputStreamTest)
{
    std::string str{"Hello! world...Привет?Мир!? пока156	привет ...???"};
    auto ss = std::istringstream(str, std::ios::binary);
    EncodingInputStream encss(ss);
    TokenInputStream strm(encss);
    SmallGroupsTokenConcatInputStream concater(strm);
    SentenceInputStream sentence_stream(concater);

    auto sentence1 = sentence_stream.read();
    auto sentence2 = sentence_stream.read();
    auto sentence3 = sentence_stream.read();
    auto sentence4 = sentence_stream.read();
    auto sentence5 = sentence_stream.read();
    auto sentence6 = sentence_stream.read();
    auto sentence7 = sentence_stream.read();
    auto sentence8 = sentence_stream.read();
    EXPECT_TRUE(sentence_stream.eof());

    EXPECT_EQ(sentence1->asText(), "Hello!");
    EXPECT_EQ(sentence2->asText(), "world...");
    EXPECT_EQ(sentence3->asText(), "Привет?");
    EXPECT_EQ(sentence4->asText(), "Мир!?");
    EXPECT_EQ(sentence5->asText(), "пока156	привет ...???");
}

TEST(SentenceTest, CornerSentenceInputStreamTest)
{
    std::string str{"Академик И. П. Павлов и т.д. Ввели понятие."};
    auto ss = std::istringstream(str, std::ios::binary);
    EncodingInputStream encss(ss);
    TokenInputStream strm(encss);
    SmallGroupsTokenConcatInputStream concater(strm);
    SentenceInputStream sentence_stream(concater);

    auto sentence1 = sentence_stream.read();
    auto sentence2 = sentence_stream.read();
    EXPECT_TRUE(sentence_stream.eof());

    EXPECT_EQ(sentence1->asText(), "Академик И. П. Павлов и т.д.");
    EXPECT_EQ(sentence2->asText(), "Ввели понятие.");
}

TEST(SentenceTest, Corner2SentenceInputStreamTest)
{
    std::string str{"И господин И. Костиков. Однако другие считали, что."};
    auto ss = std::istringstream(str, std::ios::binary);
    EncodingInputStream encss(ss);
    TokenInputStream strm(encss);
    SmallGroupsTokenConcatInputStream concater(strm);
    SentenceInputStream sentence_stream(concater);

    auto sentence1 = sentence_stream.read();
    auto sentence2 = sentence_stream.read();
    EXPECT_TRUE(sentence_stream.eof());

    EXPECT_EQ(sentence1->asText(), "И господин И. Костиков.");
    EXPECT_EQ(sentence2->asText(), "Однако другие считали, что.");
}

TEST(SentenceTest, TrashSentenceInputStreamTest)
{
    std::string str{"...several..."};
    auto ss = std::istringstream(str, std::ios::binary);
    EncodingInputStream encss(ss);
    TokenInputStream strm(encss);
    SmallGroupsTokenConcatInputStream concater(strm);
    SentenceInputStream sentence_stream(concater);

    auto sentence1 = sentence_stream.read();
    EXPECT_TRUE(sentence_stream.eof());

    EXPECT_EQ(sentence1->asText(), "...several...");
}

TEST(SentenceTest, Trash1SentenceInputStreamTest)
{
    std::string str{"The>GOOD!... The<BAD!... The/UGLY!..."};
    auto ss = std::istringstream(str, std::ios::binary);
    EncodingInputStream encss(ss);
    TokenInputStream strm(encss);
    SmallGroupsTokenConcatInputStream concater(strm);
    SentenceInputStream sentence_stream(concater);

    auto sentence1 = sentence_stream.read();
    auto sentence2 = sentence_stream.read();
    auto sentence3 = sentence_stream.read();
    EXPECT_TRUE(sentence_stream.eof());

    EXPECT_EQ(sentence1->asText(), "The>GOOD!...");
    EXPECT_EQ(sentence2->asText(), "The<BAD!...");
    EXPECT_EQ(sentence3->asText(), "The/UGLY!...");
}

TEST(SentenceTest, Trash2SentenceInputStreamTest)
{
    std::string str{"Г. М.: М-м-м... Большую частью."};
    auto ss = std::istringstream(str, std::ios::binary);
    EncodingInputStream encss(ss);
    TokenInputStream strm(encss);
    SmallGroupsTokenConcatInputStream concater(strm);
    SentenceInputStream sentence_stream(concater);

    auto sentence1 = sentence_stream.read();
    auto sentence2 = sentence_stream.read();
    EXPECT_TRUE(sentence_stream.eof());

    EXPECT_EQ(sentence1->asText(), "Г. М.: М-м-м...");
    EXPECT_EQ(sentence2->asText(), "Большую частью.");
}

TEST(SentenceTest, LongSentenceInputStreamTest)
{
    std::ostringstream oss;
    for (size_t i = 0; i < 150; ++i)
        oss << "qqqqqq q ";
    oss << ". ";
    std::string str = oss.str();
    std::string part{"Hello."};
    auto ss = std::istringstream(str + part, std::ios::binary);
    EncodingInputStream encss(ss);
    TokenInputStream strm(encss);
    SmallGroupsTokenConcatInputStream concater(strm);
    SentenceInputStream sentence_stream(concater);

    auto sentence1 = sentence_stream.read();
    auto sentence2 = sentence_stream.read();
    EXPECT_TRUE(sentence_stream.eof());

    str.pop_back();
    EXPECT_EQ(sentence1->asText(), str);
    EXPECT_EQ(sentence2->asText(), part);
}
