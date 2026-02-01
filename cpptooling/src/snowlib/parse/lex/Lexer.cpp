
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#pragma once

#include <snowlib/file/Loc.hpp>
#include <snowlib/file/SnowFile.hpp>
#include <snowlib/parse/lex/Lexer.hpp>
#include <snowlib/parse/lex/Token.hpp>

namespace parse::lex {
    Context::Context(const file::SnowFile& file, Options options)
        : mOptions(options)
        , mFile(file)
        , mState({ file.getFullContents().c_str(), 1, 1 }) {
    }

    bool Context::stripComments() const {
        return mOptions.stripComments;
    }

    bool Context::stripDocComments() const {
        return mOptions.stripDocComments;
    }

    bool Context::stripWhitespace() const {
        return mOptions.stripWhitespace;
    }

    bool Context::suppressDiags() const {
        return mOptions.suppressDiags;
    }

    void Context::stripComments(bool value) {
        mOptions.stripComments = value;
    }

    void Context::stripDocComments(bool value) {
        mOptions.stripDocComments = value;
    }

    void Context::stripWhitespace(bool value) {
        mOptions.stripWhitespace = value;
    }

    void Context::suppressDiags(bool value) {
        mOptions.suppressDiags = value;
    }

    const file::SnowFile& Context::file() const {
        return mFile;
    }

    bool Context::valid() const {
        return mState.pos != nullptr && mState.pos != '\0';
    }

    void Context::consume(size_t count) {
        DEBUG_ASSERT(count > 0, "Cannot consume 0 characters");
        DEBUG_ASSERT(valid(), "Cannot consume on invalid or expired context");

        for (size_t i = 0; i < count; ++i) {
            if (*mState.pos == '\n') {
                mState.line++;
            }
            mState.col++;
            mState.pos++;
            DEBUG_ASSERT(valid(), "Consumption has exceeded the file bounds");
        }
    }

    //void Context::reverse(size_t count) {
    //    DEBUG_ASSERT(count > 0, "Cannot reverse 0 characters");
    //    DEBUG_ASSERT(valid(), "Cannot reverse an invalid or expired context");
    //    const char* fileStart = mFile.getFullContents().c_str();
    //
    //    for (size_t i = 0; i < count; ++i) {
    //        DEBUG_ASSERT(mState.pos != fileStart, "Reversing would has exceeded the file bounds");
    //        if (*mState.pos == '\n') {
    //            mState.line--;
    //
    //            // now find column
    //
    //        }
    //        mState.col--;
    //        mState.pos--;
    //    }
    //}

    char Context::curr() const {
        DEBUG_ASSERT(valid(), "Cannot dereference an invalid or expired context");
        return *mState.pos;
    }

    char Context::operator*() const {
        return curr();
    }

    std::optional<char> Context::peek(size_t count) {
        DEBUG_ASSERT(count > 0, "Cannot peek 0 characters");
        DEBUG_ASSERT(valid(), "Cannot peek on invalid or expired context");
        const char* val = mState.pos;

        for (size_t i = 0; i < count; ++i) {
            val++;
            if (*val != '\0') {
                return {};
            }
        }
        return *val;
    }

    size_t Context::len() const {
        if (mCurrentTokenStart) {
            return mState.pos - mCurrentTokenStart->pos;
        }
        return 0;
    }

    std::optional<file::Loc> Context::startLoc() const {
        if (mCurrentTokenStart) {
            return file::Loc(mFile, mCurrentTokenStart->line, mCurrentTokenStart->col, mCurrentTokenStart->pos);
        }
        return {};
    }

    std::optional<file::LocRange> Context::tokenLoc() const {
        if (mCurrentTokenStart) {
            return file::LocRange(
                mFile,
                mCurrentTokenStart->line,
                mCurrentTokenStart->col,
                mCurrentTokenStart->pos,
                mState.pos - mCurrentTokenStart->pos
            );
        }
        return {};
    }

    std::optional<file::LocRange> Context::tokenLoc(size_t len) const {
        if (mCurrentTokenStart) {
            return file::LocRange(
                mFile,
                mCurrentTokenStart->line,
                mCurrentTokenStart->col,
                mCurrentTokenStart->pos,
                len
            );
        }
        return {};
    }

    file::Loc Context::currLoc() const {
        return file::Loc(mFile, mState.line, mState.col, mState.pos);
    }

    std::optional<file::Loc> Context::offsetTokenLoc(size_t offset) const {
        if (mCurrentTokenStart) {
            return file::Loc(
                mFile,
                mCurrentTokenStart->line,
                mCurrentTokenStart->col + offset,
                mCurrentTokenStart->pos + offset
            );
        }
        return {};
    }

    std::optional<file::LocRange> Context::offsetTokenLoc(size_t offset, size_t len) const {
        if (mCurrentTokenStart) {
            return file::LocRange(
                mFile,
                mCurrentTokenStart->line,
                mCurrentTokenStart->col + offset,
                mCurrentTokenStart->pos + offset,
                len
            );
        }
        return {};
    }


    // ----------------------------------------------------------------------------------------
    std::optional<uint64_t> Lexer::_fastAtoiBase2(std::string_view str) {
        uint64_t ret = 0;
        char i = 0;
        for (char c : str) {
            if (c == '_' || c == '`') {
                continue;
            }

            if (i++ >= 64) {
                return {}; // overflow, u64 base 2 max is 1111111111111111111111111111111111111111111111111111111111111111
            }

            ret = (ret << 1) + (c - '0');
        }
        return ret;
    }

    std::optional<uint64_t> Lexer::_fastAtoiBase10(std::string_view str) {
        uint64_t ret = 0;
        bool firstNonZero = false;
        for (char c : str) {
            if (c == '_' || c == '`') {
                continue;
            }
            if (c == '0' && !firstNonZero) {
                continue;
            }
            firstNonZero = true;

            // multiplcation overflow check
            if (ret > (std::numeric_limits<uint64_t>::max() / 10)) {
                return {}; // next digit causes overflow, u64 base 10 max is 18446744073709551615
            }

            ret *= 10;
            char v = c - '0';
            if (ret > (std::numeric_limits<uint64_t>::max() - v)) {
                return {}; // exceeds max for this digit, overflow, u64 base 10 max is 18446744073709551615
            }

            ret += v;
        }
        return ret;
    }

    std::optional<uint64_t> Lexer::_fastAtoiBase16(std::string_view str) {
        uint64_t ret = 0;
        char i = 0;
        for (char c : str) {
            if (c == '_' || c == '`') {
                continue;
            }

            if (i++ >= 16) {
                return {}; // overflow, u64 base 16 max is FFFFFFFFFFFFFFFF
            }

            if (c >= 'a' && c <= 'f') {
                ret = (ret * 16) + ((c - 'a') + 10);
            }
            else if (c >= 'A' && c <= 'F') {
                ret = (ret * 16) + ((c - 'A') + 10);
            }
            else if (c >= '0' && c <= '9') {
                ret = (ret * 16) + (c - '0');
            }
        }
        return ret;
    }

    std::optional<Token> Lexer::lexToken(Context& ctx) {
        DEBUG_FAIL("not implemented");
    }

    Token Lexer::_next(Context& ctx) {
        DEBUG_FAIL("not implemented");
    }
}
