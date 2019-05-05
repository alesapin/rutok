[![Build Status](https://travis-ci.com/alesapin/rutok.svg?branch=master)](https://travis-ci.com/alesapin/rutok)
# Графематический анализатор для русского языка

Позволяет разбивать текст в любых кодировках на последовательность токенов.

## Требования
* Компилятор с поддержкой C++11 и выше
* CMake >= 2.8.12

## Установка

### CLI
```
$ git clone https://github.com/alesapin/rutok
$ cd rutok && mkdir build && cd build
$ cmake ..
$ make -j `nproc`
$ programs/rutok --sentence
мама мыла раму.
[мама, WORD, CYRILLIC, LOWER_CASE]
[ , SEPARATOR]
[мыла, WORD, CYRILLIC, LOWER_CASE]
[ , SEPARATOR]
[раму, WORD, CYRILLIC, LOWER_CASE]
[., PUNCT, CAN_TERMINATE_SENTENCE, SENTENCE_END]
[\n, SEPARATOR, CAN_TERMINATE_PARAGRAPH]
```

### Python API

```
$ git clone https://github.com/alesapin/rutok
$ git submodule update --init --recursive
$ sudo pip install .
```

## [[Примеры][https://github.com/alesapin/rutok/example]]


