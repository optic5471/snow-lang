
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#pragma once

#include <optional>
#include <unordered_map>

#include <snowlib/diag/Diag.hpp>
#include <snowlib/parse/lex/Type.hpp>
#include <util/HashedString.hpp>

namespace file {
    class SnowFile;
    class Loc;
    class LocRange;
}

namespace parse::lex {
    class Token;

    class Context {
    public:
        // Options that control how lexing will happen
        struct Options {
            bool stripComments = true; // does not remove documentation comments
            bool stripDocComments = false; // does not remove normal comments
            bool suppressDiags = false; // dont call any diags while lexing
            bool stripWhitespace = true; // removes whitespace characters
        };

    private:
        // this keeps track of where the lexer is
        struct State {
            // current parse position
            // TODO: What if we dont have the whole file parsed because its massive, I need a custom iterable class type that dynamically loads from the file probably
            const char* pos = nullptr;
            // current line number (1 based)
            size_t line = 0;
            // current column number (1 based)
            size_t col = 0;
        };

        Options mOptions;
        State mState;
        const file::SnowFile& mFile;

        std::optional<State> mCurrentTokenStart = {};

    public:
        Context(const file::SnowFile& file, Options options = {});

        // options
        bool stripComments() const;
        void stripComments(bool value);
        bool stripDocComments() const;
        void stripDocComments(bool value);
        bool suppressDiags() const;
        void suppressDiags(bool value);
        bool stripWhitespace() const;
        void stripWhitespace(bool value);

        const file::SnowFile& file() const;
        bool valid() const; // returns true if not pointing at EOF or NULL
        void consume(size_t count = 1); // consumes 'count' characters from the stream
        // reverses 'count' characters from the stream
        // this can be expensive and could result in exceptions
        // use very sparingly
        //void reverse(size_t count = 1);
        char curr() const; // get current character, no consumption
        char operator*() const; // same as curr()
        std::optional<char> peek(size_t count = 1); // peeks 'count' characters ahead if valid
        size_t len() const; // gets the current length of the current token (or 0 if not building)

        // Returns a loc based on the current building token's start point
        std::optional<file::Loc> startLoc() const;

        // Returns a loc range based on the current building token's start point
        // infers length from current - tokenstart
        std::optional<file::LocRange> tokenLoc() const;
        // Returns a loc range based on the current building token's start point with provided length
        std::optional<file::LocRange> tokenLoc(size_t len) const;

        // Returns a loc at the current parser position
        file::Loc currLoc() const;

        // Returns a loc based on the current building token's start point, offset by the value
        std::optional<file::Loc> offsetTokenLoc(size_t offset) const;
        // Returns a loc range based on the current building token's start point, offset by the value, of len length
        std::optional<file::LocRange> offsetTokenLoc(size_t offset, size_t len) const;

        // common diags
        template <typename... tArgs>
        void diag(diag::Type type, const file::Loc& loc, tArgs&&... args) {
            if (!mOptions.suppressDiags) {
                diag::make(type, loc, std::forward<tArgs>(args)...);
            }
        }
        template <typename... tArgs>
        void diagUnary(
            diag::Type type,
            const file::Loc& loc,
            const file::LocRange& a,
            tArgs&&... args) {
            if (!mOptions.suppressDiags) {
                diag::make(type, loc, a, std::forward<tArgs>(args)...);
            }
        }
        template <typename... tArgs>
        void diagBinary(
            diag::Type type,
            const file::Loc& loc,
            const file::LocRange& a,
            const file::LocRange& b,
            tArgs&&... args) {
            if (!mOptions.suppressDiags) {
                diag::make(type, loc, a, b, std::forward<tArgs>(args)...);
            }
        }
    };

    class Lexer {
        static std::unordered_map<util::HashedString, Type> mKeywords;
        static std::unordered_map<util::HashedString, Type> mSymbols;

        static Token _next(Context& ctx);
        static std::optional<uint64_t> _fastAtoiBase2(std::string_view str);
        static std::optional<uint64_t> _fastAtoiBase10(std::string_view str);
        static std::optional<uint64_t> _fastAtoiBase16(std::string_view str);

    public:
        static std::optional<Token> lexToken(Context& ctx);
    };
}
