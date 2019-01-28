#include <unicorn/options.hpp>
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

using namespace RS;
using namespace RS::Unicorn;
using namespace tokenize;

bool isFileExists(const std::string & filename)
{
    std::ifstream infile(filename);
    return infile.good();
}

using StreamPtr = std::shared_ptr<BaseTokenOutputStream>;

int main(int argc, char** argv)
try {

    const Ustring description{
        "Tokenizer for russian language and not only."
        "Can segment sequence of bytes to sequence of tokens."
        "Also can merge some tokens according to russian grammar and select sentences."
    };

    // Parse the command line options

    Options opt("Rutok");
    opt.add(str_wrap(description, 0, 75));
    opt.add("input", "Input file (default is standard input)", Options::abbrev="i", Options::defvalue="-");
    opt.add("output", "Output file (default is standard output)", Options::abbrev="o", Options::defvalue="-");
    opt.add("format", "Output format (default is str)", Options::abbrev="f", Options::defvalue="str");
    opt.add("sentence", "Separate sentences (default is str)", Options::abbrev="s", Options::boolean=false);
    opt.add("word-only", "Output words only (default is str)", Options::abbrev="w", Options::boolean=false);
    opt.add("pretty", "Output more pretty if possible", Options::abbrev="p", Options::boolean=false);
    opt.add("cyrillic", "Output only cyrrlic tokens", Options::abbrev="c", Options::boolean=false);
    opt.add("min-words", "Output sentences more than words", Options::abbrev="m", Options::integer=1);
    opt.add("to-lower", "Print everything in lowercase", Options::abbrev="l", Options::boolean=false);

    if (opt.parse(argc, argv))
        return 0;

    std::shared_ptr<std::istream> holding_inp_stream = nullptr;
    std::shared_ptr<std::ostream> holding_out_stream = nullptr;
    std::istream * inp = &std::cin;
    std::ostream * out = &std::cout;
    if (opt.has("input"))
    {
        std::string input_file = opt.get<std::string>("input");
        if (!isFileExists(input_file))
            throw std::runtime_error("File " + input_file + " doesn't exists");
        holding_inp_stream = std::make_shared<std::ifstream>(input_file, std::ifstream::in);
        inp = holding_inp_stream.get();
    }
    if (opt.has("output"))
    {
        std::string output_file = opt.get<std::string>("output");
        holding_out_stream = std::make_shared<std::ofstream>(output_file);
        out = holding_out_stream.get();
    }

    EncodingInputStream enc_inp(*inp);
    TokenInputStream base_strm(enc_inp);
    IdenticalConcatInputStream ident(base_strm);
    SmallGroupsTokenConcatInputStream concater(ident);
    int min_words = opt.get<int>("min-words");
    bool cyrillic_only = opt.get<bool>("cyrillic");
    bool to_lower = opt.get<bool>("to-lower");

    /// bad have to rewritten
    if (opt.has("sentence") && opt.get<bool>("sentence"))
    {
        SentenceInputStream sent_inp(concater);
        while (!sent_inp.eof())
        {
            auto sentence = sent_inp.read();
            if (opt.has("word-only") && opt.get<bool>("word-only"))
                sentence = Sentence::toWordsOnly(sentence);
            if (sentence->wordsCount() < min_words)
                continue;
            if (cyrillic_only && !sentence->isCyrillic())
                continue;
            (*out) << sentence->asText(to_lower) << std::endl;
        }
    }
    else
    {
        EncodingOutputStream out_enc(*out);
        std::shared_ptr<BaseTokenOutputStream> output_token;
        if (opt.get<std::string>("format") == "str")
            output_token = std::make_shared<TokenStringOutputStream>(out_enc, concater);
        else if (opt.get<std::string>("format") == "json")
        {
            if (opt.has("pretty") && opt.get<bool>("pretty"))
                output_token = std::make_shared<TokenJSONOutputStream>(out_enc, concater, true);
            else
                output_token = std::make_shared<TokenJSONOutputStream>(out_enc, concater, false);
        }

        output_token->start();
        while(!output_token->eos())
            output_token->write();
        output_token->finish();
        output_token->flush();
    }

    return 0;

}

catch (const std::exception& ex) {
    std::cerr << "*** " << ex.what() << "\n";
    return EXIT_FAILURE;
}
