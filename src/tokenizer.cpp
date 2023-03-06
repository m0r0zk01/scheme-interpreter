#include "error.h"
#include "tokenizer.h"

// Tokens

bool SymbolToken::operator==(const SymbolToken& other) const {
    return this->name == other.name;
}

bool QuoteToken::operator==(const QuoteToken&) const {
    return true;
}

bool DotToken::operator==(const DotToken&) const {
    return true;
}

bool ConstantToken::operator==(const ConstantToken& other) const {
    return this->value == other.value;
}

bool BooleanToken::operator==(const BooleanToken& other) const {
    return this->value == other.value;
}

// Tokenizer Main

Tokenizer::Tokenizer(std::istream* in) : in_(in) {
    parsers_.emplace_back(new Tokenizer::BooleanTokenParser());
    parsers_.emplace_back(new Tokenizer::SymbolTokenParser());
    parsers_.emplace_back(new Tokenizer::QuoteTokenParser());
    parsers_.emplace_back(new Tokenizer::DotTokenParser());
    parsers_.emplace_back(new Tokenizer::BracketTokenParser());
    parsers_.emplace_back(new Tokenizer::ConstantTokenParser());
    Next();
}

bool Tokenizer::IsEnd() {
    return is_end_;
}

void Tokenizer::Next() {
    int c = 0;
    while ((c = in_->peek()) != EOF) {
        if (std::isspace(c)) {
            in_->get();
        } else {
            break;
        }
    }
    if (in_->eof()) {
        is_end_ = true;
        return;
    }
    std::string cur;
    std::vector<size_t> mask(parsers_.size(), std::string::npos);
    size_t cnt_alive = parsers_.size();
    size_t ind = 0;
    while ((c = in_->peek()) != EOF) {
        for (size_t i = 0; i < parsers_.size(); ++i) {
            if (mask[i] < std::string::npos) {
                continue;
            }
            if (!parsers_[i]->Next(cur, c)) {
                --cnt_alive;
                mask[i] = ind;
            }
        }
        if (!cnt_alive) {
            break;
        }
        cur += c;
        in_->get();
        ++ind;
    }
    for (size_t i = 0; i < parsers_.size(); ++i) {
        if ((mask[i] == ind || (in_->eof() && mask[i] == std::string::npos)) &&
            parsers_[i]->Validate(cur)) {
            parsers_[i]->Set(cur, &cur_token_);
            return;
        }
    }
    throw SyntaxError("Parsing failed!");
}

Token Tokenizer::GetToken() {
    return cur_token_;
}

// SymbolTokenParser

bool Tokenizer::SymbolTokenParser::Next(std::string_view pref, char next_char) {
    if (pref.empty()) {
        return IsHeadSymbol(next_char) || next_char == '+' || next_char == '-';
    }
    return !(pref[0] == '+' || pref[0] == '-') && IsTailSymbol(next_char);
}

void Tokenizer::SymbolTokenParser::Set(std::string_view s, Token* out) {
    SymbolToken res = {std::string(s)};
    *out = res;
}

bool Tokenizer::SymbolTokenParser::IsHeadSymbol(char c) {
    return kAbcHead.find(c) != std::string::npos;
}

bool Tokenizer::SymbolTokenParser::IsTailSymbol(char c) {
    return kAbcTail.find(c) != std::string::npos;
}

// QuoteTokenParser

bool Tokenizer::QuoteTokenParser::Next(std::string_view pref, char next_char) {
    if (pref.empty()) {
        return next_char == '\'';
    }
    return false;
}

void Tokenizer::QuoteTokenParser::Set(std::string_view, Token* out) {
    *out = QuoteToken();
}

// DotTokenParser

bool Tokenizer::DotTokenParser::Next(std::string_view pref, char next_char) {
    if (pref.empty()) {
        return next_char == '.';
    }
    return false;
}

void Tokenizer::DotTokenParser::Set(std::string_view, Token* out) {
    *out = DotToken();
}

// BracketTokenParser

bool Tokenizer::BracketTokenParser::Next(std::string_view pref, char next_char) {
    if (pref.empty()) {
        return next_char == '(' || next_char == ')';
    }
    return false;
}

void Tokenizer::BracketTokenParser::Set(std::string_view s, Token* out) {
    *out = s == "(" ? BracketToken::OPEN : BracketToken::CLOSE;
}

// ConstantTokenParser

bool Tokenizer::ConstantTokenParser::Next(std::string_view pref, char next_char) {
    if (pref.empty()) {
        return next_char == '+' || next_char == '-' || std::isdigit(next_char);
    }
    return std::isdigit(next_char);
}

bool Tokenizer::ConstantTokenParser::Validate(std::string_view s) {
    if (!s.empty() && (s[0] == '+' || s[0] == '-')) {
        return s.size() > 1;
    } else if (!s.empty()) {
        return true;
    }
    return false;
}

void Tokenizer::ConstantTokenParser::Set(std::string_view s, Token* out) {
    ConstantToken res = {std::atoi(s.data())};
    *out = res;
}

// BooleanTokenParser

bool Tokenizer::BooleanTokenParser::Next(std::string_view pref, char next_char) {
    if (pref.empty()) {
        return next_char == '#';
    } else if (pref.size() == 1) {
        return next_char == 't' || next_char == 'f';
    }
    return false;
}

void Tokenizer::BooleanTokenParser::Set(std::string_view s, Token* out) {
    BooleanToken res = {s == "#t"};
    *out = res;
}
