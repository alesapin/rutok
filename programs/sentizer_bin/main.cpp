#include <args.hxx>
#include <iostream>
#include <exception>
#include <memory>
#include <fstream>

#include <streams/EncodingInputStreamFromStream.h>
#include <streams/TokenInputStream.h>
#include <streams/EncodingOutputStream.h>
#include <streams/SmallGroupsTokenConcatInputStream.h>
#include <streams/IdenticalConcatInputStream.h>
#include <streams/SentenceInputStream.h>
#include <streams/TokenStringOutputStream.h>

static constexpr auto DESCRIPTION = R"***(Program for sentence segmentation of cyrrilic texts.
Able to remove all non-word characters and found sentences end.
)***";

static constexpr auto EXAMPLE = R"***(Example:
    ./sentizer --input=f.txt --output=r.txt --cyrillic --lower --word-only
)***";

using namespace tokenize;

bool isFileExists(const std::string & filename)
{
    std::ifstream infile(filename);
    return infile.good();
}

int main(int argc, char ** argv)
try
{
    args::ArgumentParser parser(DESCRIPTION, EXAMPLE);
    args::HelpFlag help(parser, "help",
        "Print info about commands and some examples", {'h', "help"});
    args::Flag cyrillic(parser, "cyrillic", "Output only cyrillic words", {'c', "cyrillic"});
    args::Flag latin(parser, "latin", "Output only latin words", {'e', "latin"});
    args::Flag lower(parser, "lower", "Print each word in lower case", {'l', "lower"});
    args::Flag upper(parser, "upper", "Print each word in upper case", {'u', "upper"});
    args::Flag word_only(parser, "word-only", "Output only words", {'w', "word-only"});

    args::ValueFlag<int> min_words(
        parser,
        "min-words",
        "Output sentences contains at least n words",
        {'m', "min-words"},
        -1
    );
    args::ValueFlag<std::string> input(parser, "input", "Input file name", {'i', "input"});
    args::ValueFlag<std::string> output(parser, "output", "Output file name", {'o', "output"});

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

    EncodingInputStreamFromStream enc_inp(*inp);
    TokenInputStream base_strm(enc_inp);
    IdenticalConcatInputStream ident(base_strm);
    SmallGroupsTokenConcatInputStream concater(ident);
    SentenceInputStream sent_inp(concater);
    std::unique_ptr<EncodingOutputStream> strm = nullptr;

    int min_words_num = args::get(min_words);
    std::string encoding;
    while (!sent_inp.eof())
    {
        auto sentence = sent_inp.read();
        if (min_words_num != -1
            && sentence->wordsCount() < min_words_num)
            continue;
        if (cyrillic && !sentence->isCyrillic())
            continue;
        else if(latin && !sentence->isLatin())
            continue;
        if (word_only)
            sentence = Sentence::toWordsOnly(std::move(sentence));

        if (encoding.empty())
        {
            encoding = enc_inp.getEncoding();
            strm = std::make_unique<EncodingOutputStream>(*out, 4096, encoding);
        }

        strm->write(sentence->asText(args::get(lower), args::get(upper)) + "\n");
    }
    if (strm)
        strm->flush();
}
catch (const std::exception& ex)
{
    std::cerr << "*** " << ex.what() << "\n";
    return 1;
}
