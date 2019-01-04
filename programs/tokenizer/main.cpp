#include <unicorn/library.hpp>
#include <cstdlib>
#include <exception>
#include <functional>
#include <iostream>
#include <map>
#include <string>

using namespace RS;
using namespace RS::Unicorn;
using namespace RS::Unicorn::Literals;

int main(int argc, char** argv)
try {

    const Ustring description =
        "This program reads a text file, splits it into words, "
        "converts each word to normalization form NFC and lower case, "
        "and writes out a list of all words encountered more than once, "
        "in descending order of frequency.";

    // Parse the command line options

    Options opt("Unicorn Demo");
    opt.add(str_wrap(description, 0, 75));
    opt.add("input", "Input file (default is standard input)", Options::abbrev="i", Options::defvalue="-");
    opt.add("output", "Output file (default is standard output)", Options::abbrev="o", Options::defvalue="-");
    opt.add("encoding", "Input encoding", Options::abbrev="e", Options::defvalue="UTF-8");

    if (opt.parse(argc, argv))
        return 0;

    auto input_file = opt.get<Ustring>("input");
    auto output_file = opt.get<Ustring>("output");
    auto input_encoding = opt.get<Ustring>("encoding");

    // Read all the words in the input file

    Ustring prev;
    for (auto & line : read_lines(input_file, IO::standin | IO::notempty | IO::striplf, input_encoding))
    {
        auto sentences = sentence_range(line);
        size_t range_size = range_count(sentences);

        size_t current_sentence = 0;
        for (auto & sentence : sentences)
        {
            if (current_sentence == range_size - 1)
                prev = u_str(sentence);
            else
            {
                Ustring current;
                if (prev.back() == '-')
                    current = current_sentence == 0 ? prev + u_str(sentence) : u_str(sentence);
                else
                    current = current_sentence == 0 ? prev + ' ' + u_str(sentence) : u_str(sentence);
                for (auto & word : word_range(current, Segment::alpha))
                    std::cout << u_str(word) << ' ';
                std::cout << std::endl;
            }
            current_sentence++;
        }
        auto words = word_range(prev, Segment::alpha);
        size_t words_size = range_count(words);
        size_t current_word = 0;
        for (auto & word : words)
        {
            std::cout << u_str(word);
            if (!(current_word == words_size - 1 && prev.back() == '-'))
                std::cout << ' ';
            current_word++;
        }

        if (prev.back() == '.' || prev.back() == '!' || prev.back() == '?' || prev.back() == ':')
            std::cout << std::endl;
        prev = "";
    }
    return 0;

}

catch (const std::exception& ex) {
    std::cerr << "*** " << ex.what() << "\n";
    return EXIT_FAILURE;
}
