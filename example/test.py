#-*- coding: utf-8 -*-
from __future__ import print_function
from pyrutok import Token, Sentence, Tokenizer, GraphemTag


if __name__ == "__main__":
    str_sentences_count = 0
    for sentence in Tokenizer("Комсомольск-на-Амуре — город в Хабаровском крае России. Население города — 246607 ч. (2019), второй по величине город края и четвёртый на Дальнем Востоке России."):
        str_sentences_count += 1
        for token in sentence:
            if GraphemTag.contains(token.get_graphem_tag(), GraphemTag.TITLE_CASE):
                print(token.to_JSON())
            if GraphemTag.contains(token.get_graphem_tag(), GraphemTag.HYPH_WORD):
                print(token.to_JSON())
    print("Total sentences:", str_sentences_count)

    latin_sentences_count = 0
    with open("./text.txt", 'r') as f:
        for sentence in Tokenizer(f):
            if sentence.is_latin():
                latin_sentences_count += 1
                print(sentence.only_words().as_text())
    print("Total latin sentences:", latin_sentences_count)



