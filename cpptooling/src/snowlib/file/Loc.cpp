// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#include <snowlib/file/Loc.hpp>
#include <snowlib/file/SnowFile.hpp>
#include <util/Assertions.hpp>

namespace file {
    Loc::Loc(const SnowFile& file, size_t line, size_t col, const char* pos)
        : mLine(line)
        , mColumn(col)
        , mFile(file)
        , mPos(pos) {
    }

    size_t Loc::line() const {
        return mLine;
    }

    size_t Loc::col() const {
        return mColumn;
    }

    const SnowFile& Loc::file() const {
        return mFile;
    }

    std::string Loc::getFullLine() const {
        std::string ret;

        // first find either the beginning of the file or the previous new line or return carriage character
        const char* beg = mFile.getFullContents().c_str();
        const char* end = mFile.getFullContents().c_str() + mFile.getFullContents().size();
        const char* pos = mPos;
        while (pos != beg && (*pos) != '\n' && (*pos) != '\r') {
            pos--;
        }

        if ((*pos) == '\n' || (*pos) == '\r') {
            pos++;
        }

        // now read until the next new line character or the end of the file
        while (pos != end && (*pos) != '\n' && (*pos) != '\r') {
            ret.push_back(*pos);
            pos++;
        }

        return ret;
    }


    // ------------------------------------------
    LocRange::LocRange(const SnowFile& file, size_t line, size_t col, const char* pos, size_t len)
        : Loc(file, line, col, pos)
        , mLength(len) {

        const std::string& contents = file.getFullContents();
        if ((pos + len) >= (contents.c_str() + contents.size())) {
            FAIL_IN_PUBLISH("This LocRange is a range which exceeds the file's contents");
        }
        else {
            for (size_t i = 0; i < len; ++i) {
                if (*(pos + i) == '\n' || *(pos + i) == '\r') {
                    FAIL_IN_PUBLISH("This LocRange is a range which spans a newline character");
                }
            }
        }
    }

    size_t LocRange::len() const {
        return mLength;
    }

    std::string LocRange::lexeme() const {
        std::string ret;
        for (size_t i = 0; i < mLength; ++i) {
            ret.push_back(*(mPos + i));
        }
        return ret;
    }
}
