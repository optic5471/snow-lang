
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#pragma once

#include <string>

namespace file {
    class SnowFile;

    class Loc {
    protected:
        size_t mLine;
        size_t mColumn;
        const SnowFile& mFile;
        const char* mPos;

    public:
        Loc(const SnowFile& file, size_t line, size_t col, const char* pos);

        size_t line() const;
        size_t col() const;
        const SnowFile& file() const;

        // warning this expensive
        std::string getFullLine() const;
    };


    // ------------------------------
    class LocRange : public Loc {
        size_t mLength;

    public:
        // if the range passes over a newline, this will throw. Also if this exceeds the file length
        LocRange(const SnowFile& file, size_t line, size_t col, const char* pos, size_t len);
        LocRange(const Loc& loc, size_t len);

        size_t len() const;
        std::string lexeme() const;
    };
}