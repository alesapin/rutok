#include <args.hxx>
#include <cstdlib>
#include <exception>
#include <functional>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <streams/EncodingInputStream.h>
#include <streams/TokenInputStream.h>
#include <streams/EncodingOutputStream.h>
#include <streams/TokenJSONOutputStream.h>
#include <streams/SmallGroupsTokenConcatInputStream.h>
#include <streams/IdenticalConcatInputStream.h>
#include <streams/SentenceInputStream.h>
#include <streams/TokenStringOutputStream.h>
#include <streams/SentenceJSONOutputStream.h>
#include <streams/BaseSentenceOutputStream.h>
#include <streams/SentenceStringOutputStream.h>

using namespace tokenize;

bool isFileExists(const std::string & filename)
{
    std::ifstream infile(filename);
    return infile.good();
}

using OutTokenStreamPtr = std::shared_ptr<BaseTokenOutputStream>;
using OutSentenceStreamPtr = std::shared_ptr<BaseSentenceOutputStream>;

OutTokenStreamPtr getTokenOutputStream(const std::string & format, bool pretty, EncodingOutputStream & out)
{
    OutTokenStreamPtr output_token;
    if (format == "str")
        output_token = std::make_shared<TokenStringOutputStream>(out);
    else if (format == "json")
    {
        if (pretty)
            output_token = std::make_shared<TokenJSONOutputStream>(out, true);
        else
            output_token = std::make_shared<TokenJSONOutputStream>(out, false);
    }
    return output_token;
}


OutSentenceStreamPtr getSentenceOutputStream(const std::string & format, bool pretty, EncodingOutputStream & out)
{
    OutSentenceStreamPtr output_token;
    if (format == "str")
        output_token = std::make_shared<SentenceStringOutputStream>(out);
    else if (format == "json")
    {
        if (pretty)
            output_token = std::make_shared<SentenceJSONOutputStream>(out, true);
        else
            output_token = std::make_shared<SentenceJSONOutputStream>(out, false);
    }
    return output_token;
}

int main(int argc, char** argv)
try {

    const std::string description{
        "Tokenizer for russian language and not only."
        "Can segment sequence of bytes to sequence of tokens."
        "Also can merge some tokens according to russian grammar and select sentences."
    };

    args::ArgumentParser parser(description);

    args::HelpFlag help(parser, "help",
        "Print info about commands and some examples", {'h', "help"});

    args::ValueFlag<std::string> input(
        parser, "input", "Input file name", {'i', "input"});

    args::ValueFlag<std::string> output(
        parser, "output", "Output file name", {'o', "output"});

    args::ValueFlag<std::string> format(
        parser, "format",
        "Output format name",
        {'f', "format"},
        "str");

    args::Flag word_only(
        parser, "word-only",
        "Output only words",
        {'w', "word-only"});

    args::Flag pretty(
        parser, "pretty",
        "Output in pretty format if possible",
        {'p', "pretty"});

    args::ValueFlag<int> min_words(
        parser, "min-words",
        "Output sentences contains at least n words",
        {'m', "min-words"},
        -1);

    args::Flag cyrillic(
        parser, "cyrillic",
        "Output only cyrillic words",
        {'c', "cyrillic"});

    args::Flag latin(
        parser, "latin",
        "Output only latin words",
        {'l', "latin"});

    args::Flag sentence(
        parser, "sentence",
        "Detect sentences",
        {'s', "sentence"});

    try
    {
        parser.ParseCLI(argc, argv);
    }
    catch (const args::Help&)
    {
        std::cout << parser;
        return 0;
    }
    catch (const args::ParseError& e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }

    std::shared_ptr<std::istream> holding_inp_stream = nullptr;
    std::shared_ptr<std::ostream> holding_out_stream = nullptr;
    std::istream * inp = &std::cin;
    std::ostream * out = &std::cout;
    if (input)
    {
        std::string input_file = args::get(input);
        if (!isFileExists(input_file))
            throw std::runtime_error("File " + input_file + " doesn't exists");
        holding_inp_stream = std::make_shared<std::ifstream>(input_file, std::ifstream::in);
        inp = holding_inp_stream.get();
    }
    if (output)
    {
        std::string output_file = args::get(output);
        holding_out_stream = std::make_shared<std::ofstream>(output_file);
        out = holding_out_stream.get();
    }

    EncodingInputStream enc_inp(*inp);
    TokenInputStream base_strm(enc_inp);
    IdenticalConcatInputStream ident(base_strm);
    SmallGroupsTokenConcatInputStream concater(ident);
    int min_words_num = args::get(min_words);

    if (sentence)
    {
        EncodingOutputStream out_enc(*out);
        OutSentenceStreamPtr output_sentence = getSentenceOutputStream(args::get(format), pretty, out_enc);

        output_sentence->start();
        SentenceInputStream sent_inp(concater);
        while (!sent_inp.eof())
        {
            if (auto sent = sent_inp.read(); sent)
            {
                if (word_only)
                    sent = Sentence::toWordsOnly(sent);
                if (sent->wordsCount() < min_words)
                    continue;
                if (cyrillic && !sent->isCyrillic())
                    continue;
                if (latin && !sent->isLatin())
                    continue;
                output_sentence->write(sent);
            }
        }
        output_sentence->finish();
        output_sentence->flush();
    }
    else
    {
        EncodingOutputStream out_enc(*out);
        OutTokenStreamPtr output_token = getTokenOutputStream(args::get(format), pretty, out_enc);

        output_token->start();
        while(!concater.eof())
        {
            if (auto token = concater.read(); token)
            {
                if (word_only && token->getTokenType() != ETokenType::WORD && token->getTokenType() != ETokenType::WORDNUM)
                    continue;
                if (cyrillic && !token->isCyrillic())
                    continue;
                if (latin && !token->isLatin())
                    continue;
                output_token->write(token);
            }
        }
        output_token->finish();
        output_token->flush();
    }

    return 0;

}

catch (const std::exception& ex) {
    std::cerr << "*** " << ex.what() << "\n";
    return EXIT_FAILURE;
}
