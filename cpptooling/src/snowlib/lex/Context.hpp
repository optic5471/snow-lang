
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#pragma once

#include <string>

#include <snowlib/src/Loc.hpp>

namespace src {
    struct FileEntry;
}

namespace lex {
    class Context {
        const src::FileEntry& mFile;
        const std::string::const_iterator mBegin;
        const std::string::const_iterator mEnd;
        std::string::const_iterator mIt;

        void _inc();
        void _dec();

    public:
        Context(const src::FileEntry& file);
        Context(src::FileID file);
        Context(src::Loc startPos);

        void consume(unsigned char count = 1);
        void reverse(unsigned char count = 1);

        char operator*() const;
        char curr() const;
        bool valid() const;
    };
}
