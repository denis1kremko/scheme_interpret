#pragma once

#include <variant>
#include <optional>
#include <istream>
#include <iostream>
#include <sstream>

struct SymbolToken {
    std::string name;

    bool operator==(const SymbolToken &other) const {
        if (name == other.name) {
            return true;
        }

        return false;
    }
};

struct QuoteToken {
    bool operator==(const QuoteToken &) const {
        return true;
    }
};

struct DotToken {
    bool operator==(const DotToken &) const {
        return true;
    }
};

enum class BracketToken { OPEN, CLOSE };

struct ConstantToken {
    int value;

    bool operator==(const ConstantToken &other) const {
        if (value == other.value) {
            return true;
        }

        return false;
    }
};

typedef std::variant<ConstantToken, BracketToken, SymbolToken, QuoteToken, DotToken> Token;

// Интерфейс позволяющий читать токены по одному из потока.
class Tokenizer {
public:
    explicit Tokenizer(std::istream *in) : counter_(0), in_(in) {
        Next();
    }

    bool IsEnd() {
        if (in_->eof() && counter_ == 0) {
            return true;
        }

        return false;
    }

    void Next() {
        while (in_->peek() == ' ' || in_->peek() == '\n') {
            in_->ignore(1);
        }

        if (in_->eof()) {
            return;
        }

        if (IsBracket(in_->peek())) {
            // Скобка
            char bracket = in_->get();
            token_ = (bracket == '(') ? Token{BracketToken::OPEN} : Token{BracketToken::CLOSE};
            ++counter_;

        } else if (IsSign(in_->peek())) {
            char sign = in_->get();

            if (std::isdigit(in_->peek())) {
                // за ним число
                int value = GetNumber();
                if (sign == '-') {
                    token_ = Token{ConstantToken{(-1) * value}};
                } else {
                    token_ = Token{ConstantToken{value}};
                }
            } else {
                // просто символ
                token_ = Token{SymbolToken{std::string(1, sign)}};
            }
            ++counter_;

        } else if (IsNum(in_->peek())) {
            // просто число
            token_ = Token{ConstantToken{GetNumber()}};
            ++counter_;
        } else if (in_->peek() == '.') {
            // точка
            token_ = Token{DotToken{}};
            in_->ignore(1);
            ++counter_;
        } else if (in_->peek() == '\'') {
            // запятая
            token_ = Token{QuoteToken{}};
            in_->ignore(1);
            ++counter_;
        } else if (ProperStartSymbol(in_->peek())) {
            // строка символов
            token_ = Token{SymbolToken{GetString()}};
            ++counter_;
        } else {
            throw std::logic_error("logic error");
        }
    }

    Token GetToken() {
        --counter_;

        return token_;
    }

private:
    bool IsSign(const char c) {
        return c == '-' || c == '+';
    }

    bool IsBracket(const char c) {
        return c == '(' || c == ')';
    }

    bool IsNum(const char c) {
        return c >= '0' && c <= '9';
    }

    bool ProperStartSymbol(const char c) {
        return std::isalpha(c) || c == '<' || c == '=' || c == '>' || c == '*' || c == '#' ||
               c == '/';
    }

    bool ProperInnerSymbol(const char c) {
        return std::isalpha(c) || c == '<' || c == '=' || c == '>' || c == '*' || c == '#' ||
               IsNum(c) || c == '?' || c == '!' || c == '-' || c == '/';
    }

    int GetNumber() {
        int value;
        *in_ >> value;

        return value;
    }

    std::string GetString() {
        std::string str;
        while (ProperInnerSymbol(in_->peek())) {
            str.push_back(in_->get());
        }

        return str;
    }

    int counter_;

    Token token_;
    std::istream *in_;
};