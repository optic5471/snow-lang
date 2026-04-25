
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#pragma once

#include <optional>

#include <snowlib/src/Loc.hpp>

namespace lex {
    class Context;
    class Token;

    class Lexer {
        static std::optional<uint64_t> _fastAtoiBase2(src::LocRange loc);
        static std::optional<uint64_t> _fastAtoiBase10(src::LocRange loc);
        static std::optional<uint64_t> _fastAtoiBase16(src::LocRange loc);
        static std::optional<double> _fastAtod(src::LocRange loc);

        static Token _startsWithInteger(Context& ctx);
        static Token _startsWithLetter(Context& ctx);

        static Token _parseWysiwygString(Context& ctx);
        static Token _parseFloat(Context& ctx);
        static Token _parseNormalString(Context& ctx);
        static Token _parseFormatString(Context& ctx);
        static Token _parseTripleComment(Context& ctx);
        static Token _parseBangComment(Context& ctx);
        static Token _parseEndOfLineComment(Context& ctx);
        static Token _parseMultiLinComment(Context& ctx);

        static void _tryConvertIdentifierToKeyword(Token& token);

    public:
        static std::optional<Token> getNextToken(Context& ctx);
    };
}
