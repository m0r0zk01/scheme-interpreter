#include <string>
#include <tuple>
#include <variant>
#include "object.h"
#include "parser.h"
#include "error.h"
#include "tokenizer.h"

ObjectPtr CastToken(Token& token, Tokenizer* tokenizer) {
    ObjectPtr res;
    if (ConstantToken* t = std::get_if<ConstantToken>(&token)) {
        res = As<Object>(MakeNumber(t->value));
    } else if (BooleanToken* t = std::get_if<BooleanToken>(&token)) {
        res = As<Object>(MakeBoolean(t->value));
    } else if (SymbolToken* t = std::get_if<SymbolToken>(&token)) {
        res = As<Object>(Heap::Make<Symbol>().From(t->name));
    } else if (QuoteToken* t = std::get_if<QuoteToken>(&token)) {
        res = As<Object>(
            Heap::Make<Cell>().From(Heap::Make<Symbol>().From("'"),
                                    Heap::Make<Cell>().From(Read(tokenizer), ObjectPtr(nullptr))));
    } else {
        throw SyntaxError("Parsing failed!");
    }
    return res;
}

bool IsOpenBracket(Token& token) {
    BracketToken* bracket_token = std::get_if<BracketToken>(&token);
    return bracket_token && *bracket_token == BracketToken::OPEN;
}

bool IsCloseBracket(Token& token) {
    BracketToken* bracket_token = std::get_if<BracketToken>(&token);
    return bracket_token && *bracket_token == BracketToken::CLOSE;
}

ObjectPtr Read(Tokenizer* tokenizer) {
    if (tokenizer->IsEnd()) {
        throw SyntaxError("Parsing Failed!");
    }

    Token token = tokenizer->GetToken();
    tokenizer->Next();
    ObjectPtr res;
    if (IsOpenBracket(token)) {
        res = ReadList(tokenizer);
    } else {
        res = CastToken(token, tokenizer);
    }
    return res;
}

CellPtr ReadList(Tokenizer* tokenizer) {
    CellPtr res = Heap::Make<Cell>().From();
    Token first = tokenizer->GetToken();
    if (IsCloseBracket(first)) {
        tokenizer->Next();
        return nullptr;
    }
    res->SetFirst(Read(tokenizer));

    std::vector<ObjectPtr> tail;
    size_t dot_index = std::string::npos;
    while (true) {
        if (tokenizer->IsEnd()) {
            throw SyntaxError("Parsing failed!");
        }
        Token next = tokenizer->GetToken();
        if (IsCloseBracket(next)) {
            tokenizer->Next();
            break;
        } else if (Is<DotToken>(next)) {
            tokenizer->Next();
            if (dot_index < std::string::npos) {
                throw SyntaxError("Parsing failed!");
            } else {
                dot_index = tail.size();
            }
        } else {
            tail.push_back(Read(tokenizer));
        }
    }
    if (dot_index != std::string::npos && dot_index + 1 != tail.size()) {
        throw SyntaxError("Parsing failed!");
    } else {
        CellPtr cur = res;
        for (size_t i = 0; i + 1 < tail.size(); ++i) {
            cur->SetSecond(As<Object>(Heap::Make<Cell>().From()));
            cur = As<Cell>(cur->GetSecond());
            cur->SetFirst(tail[i]);
        }
        if (dot_index != std::string::npos) {
            cur->SetSecond(tail.back());
        } else if (!tail.empty()) {
            cur->SetSecond(As<Object>(Heap::Make<Cell>().From()));
            cur = As<Cell>(cur->GetSecond());
            cur->SetFirst(tail.back());
        }
    }
    return res;
}
