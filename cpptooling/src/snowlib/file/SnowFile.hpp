
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#pragma once

#include <string>

#include <snowlib/ast/BaseNode.hpp>
#include <util/ChunkFileReader.hpp>
#include <util/FileSystem.hpp>

namespace file {
    class SnowFile {
        bool mErrored = false;
        SnowFile() = default;

        util::fs::Path mLocalPath;
        util::fs::Path mFullPath;

        // This only loaded during Lexical Analysis and Syntax Analysis stages after that it is unloaded for memory
        std::optional<std::string> mMemoryFileReader;
        std::unique_ptr<util::fs::ChunkFileReader> mFileReader;

        // This is loaded after Lexical Analysis and Syntax Analysis stages
        std::shared_ptr<ast::BaseNode> mAst;

    public:
        static SnowFile FromString(const std::string& str);
        //static SnowFile FromPath(const util::fs::Path& path, const Package& owningPackage);

        const std::shared_ptr<ast::BaseNode>& getAst() const;
        const std::string& getFullContents() const; // TODO: Well the chunk reader is nice, but it needs to keep the last chunk around too, we dont want to iterate and then go backwards a bit

        const util::fs::Path& localPath() const; // the path from the package, including the package folder
        void setErrored() const;
        bool isErrored() const;
    };
}
