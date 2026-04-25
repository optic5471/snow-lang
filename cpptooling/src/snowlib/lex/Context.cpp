
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#include <snowlib/lex/Context.hpp>

#include <snowlib/src/SourceVault.hpp>
#include <util/ServiceLocator.hpp>

namespace lex {
    Context::Context(const src::FileEntry& file)
        : mFile(file)
        , mBegin(mFile.mContents.begin())
        , mEnd(mFile.mContents.end())
        , mIt(mFile.mContents.begin()) {
    }

    Context::Context(src::FileID file)
        : mFile(*ServiceLocator<src::SourceVault>::fetch().tryGetFileData(file))
        , mBegin(mFile.mContents.begin())
        , mEnd(mFile.mContents.end())
        , mIt(mFile.mContents.begin()) {
    }

    Context::Context(src::Loc startPos)
        : mFile(*ServiceLocator<src::SourceVault>::fetch().tryGetFileData(startPos.mFileID))
        , mBegin(mFile.mContents.begin())
        , mEnd(mFile.mContents.end())
        , mIt(mFile.mContents.begin() + startPos.mOffset) {
    }

    void Context::consume(unsigned char count) {
        for (unsigned char i = 0; i < count; i++) {
            _inc();
        }
    }

    void Context::_inc() {
        if (mIt != mEnd) {
            ++mIt;
        }
    }

    void Context::reverse(unsigned char count) {
        for (unsigned char i = 0; i < count; i++) {
            _dec();
        }
    }

    void Context::_dec() {
        if (mIt != mBegin) {
            --mIt;
        }
    }

    char Context::operator*() const {
        DEBUG_ASSERT(valid(), "Attempting to dereference invalid position");
        *mIt;
    }

    char Context::curr() const {
        DEBUG_ASSERT(valid(), "Attempting to dereference invalid position");
        *mIt;
    }

    bool Context::valid() const {
        return mIt != mEnd;
    }
}
