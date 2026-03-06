
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#include <snowlib/src/SourceVault.hpp>

#include <utility>

namespace src {
    std::tuple<uint32_t, uint32_t> SourceVault::getLineCol(Loc l) const {
        const FileEntry* file = tryGetFileData(l.mFileID);
        if (!file) {
            DEBUG_FAIL("How do you have a loc for a non-existant file?");
            return { 0, 0 };
        }

        if (file->mContents.empty()) {
            return { 1, 0 };
        }
        else if (file->mContents.size() <= l.mOffset) {
            DEBUG_FAIL("Attempting to reach beyond the file's length!");
            return { 0, 0 };
        }

        // one line only
        if (file->mNewLineTable.empty()) {
            return { 1, l.mOffset + 1 };
        }

        auto it = std::lower_bound(file->mNewLineTable.begin(), file->mNewLineTable.end(), l.mOffset + 1);
        uint32_t line = static_cast<uint32_t>(it - file->mNewLineTable.begin()) + 1;

        if (line == 1) {
            return { 1, l.mOffset + 1 };
        }
        else if (it == file->mNewLineTable.end()) {
            return { line, (l.mOffset - file->mNewLineTable.back()) + 1 };
        }

        return {
            line,
            l.mOffset - *it
        };
    }

    std::string_view SourceVault::getLineAt(Loc l) const {
        const FileEntry* file = tryGetFileData(l.mFileID);

        if (!file) {
            DEBUG_FAIL("How do you have a loc for a non-existant file?");
            return "";
        }
        else if (file->mContents.size() < l.mOffset) {
            DEBUG_FAIL("Attempting to reach beyond the file's length!");
            return "";
        }

        uint32_t start = 0;
        uint32_t end = 0;
        if (file->mNewLineTable.empty()) {
            end = file->mContents.size();
        }
        else {
            auto [line, col] = getLineCol(l);
            if (line == 0 || col == 0) {
                return "";
            }

            if (line == 1) {
                start = 0;
            }
            else {
                // -2 since the entry in the table is the start of line 2
                // +1 to not include the new line character itself
                start = file->mNewLineTable[line - 2] + 1;
            }

            if (file->mNewLineTable.size() == (line - 1)) {
                end = file->mContents.size();
            }
            else {
                end = file->mNewLineTable[line - 1];
            }
        }

        if (start == end - 1) {
            return "";
        }

        while (start < file->mContents.size() && file->mContents[start] == '\r') {
            start++;
        }
        while (end > 1 && file->mContents[end - 1] == '\r') {
            end--;
        }

        return std::string_view(file->mContents.c_str() + start, end - start);
    }

    std::string_view SourceVault::getLexeme(LocRange l) const {
        const FileEntry* file = tryGetFileData(l.mFileID);
        if (!file) {
            DEBUG_FAIL("How do you have a loc for a non-existant file?");
            return "";
        }
        else if (file->mContents.size() < l.mOffset + l.mLength) {
            DEBUG_FAIL("Attempting to reach beyond the file's length!");
            return "";
        }
        const char* start = file->mContents.c_str() + l.mOffset;
        return std::string_view(start, l.mLength);
    }

    const FileEntry* SourceVault::tryGetFileData(FileID id) const {
        auto it = mFiles.find(id);
        if (it == mFiles.end()) {
            return nullptr;
        }
        return &it->second;
    }

    std::optional<FileID> SourceVault::loadFile([[maybe_unused]]const util::fs::Path& path) {
        // load file from disk into memory
        DEBUG_FAIL("Not implemented, also how do I reconcile multiple packages?");
        return {};
    }

    std::optional<FileID> SourceVault::loadMemory(const std::string& str) {
        ASSERT_IN_PUBLISH(mNextID < 4294967290, "Too many files are being compiled, compile less than 4 billion files please.");
        ASSERT_IN_PUBLISH(str.size() < 4294967290, "Memory file contents exceeds maximum size of 4GB. Break up this file to compile it.");

        mNextID++;
        mFiles.emplace(mNextID, FileEntry{
            .mNewLineTable = _generateNewLineTable(str),
            .mID = mNextID,
            .mContents = str,
            .mPath = "In Memory"
        });
        return mNextID;
    }

    std::vector<uint32_t> SourceVault::_generateNewLineTable(const std::string& file) const {
        std::vector<uint32_t> ret;
        uint32_t index = 0;
        for (auto it = file.begin(); it != file.end(); it++) {
            if (*it == '\n') {
                ret.emplace_back(index);
            }
            index++;
        }
        return ret;
    }
}
