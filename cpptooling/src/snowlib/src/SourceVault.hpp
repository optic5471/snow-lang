
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#pragma once

#include <optional>
#include <tuple>
#include <unordered_map>

#include <snowlib/src/Loc.hpp>
#include <util/Singleton.hpp>
#include <util/FileSystem.hpp>

namespace src {
    struct FileEntry {
        // contains the list of newlines in the file
        // index + 1 is the line number, value is the offset in the file for the new line character
        std::vector<uint32_t> mNewLineTable;
        FileID mID;
        const std::string mContents; // the entire file's contents
        util::fs::Path mPath;
    };

    class SourceVault : public Singleton<SourceVault> {
        FileID mNextID = 0;
        std::unordered_map<FileID, FileEntry> mFiles;

        std::vector<uint32_t> _generateNewLineTable(const std::string& file) const;
        std::string _stripCarriageReturnAtNewlines(const std::string& fileContents) const;

    public:
        std::tuple<uint32_t, uint32_t> getLineCol(Loc l) const;
        std::string_view getLineAt(Loc l) const;
        std::string_view getLexeme(LocRange l) const;

        const FileEntry* tryGetFileData(FileID id) const;
        std::optional<FileID> loadFile(const util::fs::Path& path);
        std::optional<FileID> loadMemory(const std::string& str);
    };
}
