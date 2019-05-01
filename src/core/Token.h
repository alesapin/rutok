#pragma once
#include <core/Tag.h>
#include <memory>
#include <string>
#include <vector>
#include <type_traits>


namespace tokenize
{

class Token
{
public:
    char32_t operator[](size_t index) const;
    char32_t at(size_t i) const;
    ETokenType getTokenType() const;
    EGraphemTag getGraphemTag() const;
    ESemanticTag getSemanticTag() const;
    const std::string & getData() const;
    std::string getEscapedData() const;
    size_t getLength() const;
    size_t getBytesLength() const;

    static std::unique_ptr<Token> createToken(const std::string & data);
    static std::unique_ptr<Token> toUpper(const Token * token);
    static std::unique_ptr<Token> toLower(const Token * token);
    static std::unique_ptr<Token> toTitle(const Token * token);
    static std::unique_ptr<Token> concat(const std::vector<const Token *> & tokens, EGraphemTag additional=EGraphemTag::UNKNOWN);
    static std::unique_ptr<Token> refine(const Token * token);
    static std::unique_ptr<Token> createDefaultSeparator();

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
    bool isLatin() const;
    bool isCyrillic() const;

    void setSemanticTag(ESemanticTag tag);

    Token() = default;
    ~Token() = default;

private:
    std::string data;
    size_t length;
    ETokenType type_tag = ETokenType::UNKNOWN;
    EGraphemTag graphem_tag = EGraphemTag::UNKNOWN;
    ESemanticTag semantic_tag = ESemanticTag::UNKNOWN;
};

struct TokenHasher
{
    size_t operator()(const Token & token) const;
};

using TokenPtr = std::unique_ptr<Token>;

}
