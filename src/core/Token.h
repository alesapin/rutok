#pragma once
#include <core/Tag.h>
#include <memory>
#include <string>
#include <vector>
#include <type_traits>


namespace tokenize
{

namespace detail {
struct TokenImpl;
}

class Token
{
public:
    char32_t operator[](size_t index) const;
    ETokenType getTokenType() const;
    EGraphemTag getGraphemTag() const;
    const std::string & getData() const;
    size_t getLength() const;
    size_t getBytesLength() const;

    static std::shared_ptr<Token> createToken(const std::string & data);
    static std::shared_ptr<Token> toUpper(const std::shared_ptr<Token> token);
    static std::shared_ptr<Token> toLower(const std::shared_ptr<Token> token);
    static std::shared_ptr<Token> toTitle(const std::shared_ptr<Token> token);
    static std::shared_ptr<Token> concat(const std::vector<std::shared_ptr<Token>> & tokens, EGraphemTag additional=EGraphemTag::UNKNOWN);
    static std::shared_ptr<Token> createDefaultSeparator();

    bool operator<(const Token & o) const;
    bool operator>(const Token & o) const;
    bool operator==(const Token & o) const;
    bool operator!=(const Token & o) const;
    bool equalsIgnoreCase(const Token & o) const;
    bool startsWith(const std::string prefix) const;
    bool endsWith(const std::string suffix) const;
    bool isUpper() const;
    bool isLower() const;
    bool isTitle() const;

    Token() = default;
    ~Token();

    Token(const Token & other);

private:
    std::unique_ptr<detail::TokenImpl> impl;
};

struct TokenHasher {
    size_t operator()(const Token & token) const;
};

using TokenPtr = std::shared_ptr<Token>;


}
