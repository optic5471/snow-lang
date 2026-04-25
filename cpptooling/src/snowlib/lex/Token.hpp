
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#pragma once

#include <optional>

#include <snowlib/src/Loc.hpp>
#include <snowlib/parse/LiteralValue.hpp>

namespace lex {
    class Desc;
    enum class TokenType : short;

    class Token {
        src::LocRange mLoc;
        const Desc& mTokenTypeDesc;
        std::optional<parse::LiteralValue> mValue;

    public:
        Token(src::LocRange loc, TokenType type, std::optional<parse::LiteralValue> value = std::nullopt);

        bool valid() const;
        TokenType type() const;
        const std::string_view lexeme() const;

        bool is(TokenType type) const;
        bool is(std::vector<TokenType> types) const;
    };
}