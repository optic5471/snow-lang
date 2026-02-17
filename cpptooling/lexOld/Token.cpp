
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#include <snowlib/parse/lex/Token.hpp>
#include <snowlib/parse/lex/Type.hpp>

namespace parse::lex {
    Token::Token(file::LocRange&& loc, Type type)
        : mLoc(loc)
        , mType(type) {
    }

    Token::Token(const file::Loc& loc, size_t len, Type type)
        : mLoc(file::LocRange(loc, len))
        , mType(type) {
    }

    Token::Token(file::LocRange&& loc, Type type, LiteralValue value)
        : mLoc(loc)
        , mType(type)
        , mLiteralValue(value) {
    }

    Token::Token(const file::Loc& loc, size_t len, Type type, LiteralValue value)
        : mLoc(file::LocRange(loc, len))
        , mType(type)
        , mLiteralValue(value) {
    }

    Type Token::type() const {
        return mType;
    }

    const file::LocRange& Token::loc() const {
        return mLoc;
    }

    std::string_view Token::lexeme() const {
        return mLoc.lexeme();
    }

    bool Token::valid() const {
        return mType != Type::Invalid;
    }
}