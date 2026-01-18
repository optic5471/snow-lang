
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#pragma once

#include <string>

#include <snowlib/ast/BaseNode.hpp>
#include <util/FileSystem.hpp>

namespace file {
    class SnowFile {
        bool mErrored = false;
        SnowFile() = default;

        util::fs::Path mLocalPath;
        util::fs::Path mFullPath;

        // This only loaded during Tokenization and Syntax Analysis stages after that it is unloaded for memory
        std::string mFullContents;

        // This is loaded after Tokenization and Syntax Analysis stages
        std::shared_ptr<ast::BaseNode> mAst;

    public:
        SnowFile FromString(const std::string& str);
        //SnowFile FromPath(const util::fs::Path& path);

        void unloadFullContents(std::shared_ptr<ast::BaseNode> ast);
        const std::shared_ptr<ast::BaseNode>& getAst() const;
        const std::string& getFullContents() const;

        const util::fs::Path& localPath() const; // the path from the package, including the package folder
        void setErrored() const;
        bool isErrored() const;
    };
}
