
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#include <snowlib/lex/Token.hpp>

#include <snowlib/lex/TokenType.hpp>
#include <snowlib/src/SourceVault.hpp>
#include <util/ServiceLocator.hpp>

namespace lex {
    Token::Token(src::LocRange loc, TokenType type, std::optional<parse::LiteralValue> value)
        : mLoc(loc)
        , mTokenTypeDesc(ServiceLocator<Desc>::fetch().get(type))
        , mValue(value) {
    }

    bool Token::valid() const {
        return mTokenTypeDesc.mType == TokenType::Invalid;
    }

    TokenType Token::type() const {
        return mTokenTypeDesc.mType;
    }

    const std::string_view Token::lexeme() const {
        return ServiceLocator<src::SourceVault>::fetch().getLexeme(mLoc);
    }

    bool Token::is(TokenType type) const {
        return mTokenTypeDesc.mType == type;
    }

    bool Token::is(std::vector<TokenType> types) const {
        for (const TokenType& type : types) {
            if (mTokenTypeDesc.mType == type) {
                return true;
            }
        }
        return false;
    }
}
