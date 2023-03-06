#pragma once

#include <cctype>
#include <cstdlib>
#include <istream>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_set>
#include <variant>
#include <vector>

struct SymbolToken {
    std::string name;

    bool operator==(const SymbolToken& other) const;
};

struct QuoteToken {
    bool operator==(const QuoteToken&) const;
};

struct DotToken {
    bool operator==(const DotToken&) const;
};

enum class BracketToken { OPEN, CLOSE };

struct ConstantToken {
    int value;

    bool operator==(const ConstantToken& other) const;
};

struct BooleanToken {
    bool value;

    bool operator==(const BooleanToken& other) const;
};

using Token =
    std::variant<ConstantToken, BracketToken, SymbolToken, QuoteToken, DotToken, BooleanToken>;

template <typename T>
bool Is(const Token& token) {
    return std::get_if<T>(&token);
}

class Tokenizer {
public:
    Tokenizer(std::istream* in);

    bool IsEnd();

    void Next();

    Token GetToken();

private:
    // Parsers

    class TokenParser {
    public:
        virtual bool Next(std::string_view pref, char next_char) = 0;
        virtual void Set(std::string_view s, Token* out) = 0;
        virtual bool Validate(std::string_view s) {
            return !s.empty();
        }
        virtual ~TokenParser(){};

    protected:
        bool is_valid_ = false;
    };

    class SymbolTokenParser : public TokenParser {
    public:
        bool Next(std::string_view pref, char next_char);
        void Set(std::string_view s, Token* out);

    private:
        static constexpr std::string_view kAbcHead =
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ<=>*/#";
        static constexpr std::string_view kAbcTail =
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ<=>*/#0123456789?!-+";

        bool IsHeadSymbol(char c);
        bool IsTailSymbol(char c);
    };

    class QuoteTokenParser : public Tokenizer::TokenParser {
    public:
        bool Next(std::string_view pref, char next_char);
        void Set(std::string_view s, Token* out);
    };

    class DotTokenParser : public Tokenizer::TokenParser {
    public:
        bool Next(std::string_view pref, char next_char);
        void Set(std::string_view s, Token* out);
    };

    class BracketTokenParser : public Tokenizer::TokenParser {
    public:
        bool Next(std::string_view pref, char next_char);
        void Set(std::string_view s, Token* out);
    };

    class ConstantTokenParser : public Tokenizer::TokenParser {
    public:
        bool Next(std::string_view pref, char next_char);
        bool Validate(std::string_view s);
        void Set(std::string_view s, Token* out);
    };

    class BooleanTokenParser : public Tokenizer::TokenParser {
    public:
        bool Next(std::string_view pref, char next_char);
        void Set(std::string_view s, Token* out);
    };

    std::vector<std::unique_ptr<TokenParser>> parsers_;

    bool is_end_ = false;
    std::istream* in_;
    Token cur_token_;
};
