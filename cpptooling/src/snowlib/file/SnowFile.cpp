// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#include <snowlib/file/SnowFile.hpp>

namespace file {
    SnowFile SnowFile::FromString(const std::string& str) {
        SnowFile f;
        f.mFullPath = "In Memory";
        f.mLocalPath = "In Memory";
        f.mFullContents = str;
        return f;
    }

    const util::fs::Path& SnowFile::localPath() const {
        return mLocalPath;
    }

    void SnowFile::setErrored() const {
        SnowFile* t = const_cast<SnowFile*>(this);
        t->mErrored = true;
    }

    bool SnowFile::isErrored() const {
        return mErrored;
    }

    void SnowFile::unloadFullContents(std::shared_ptr<ast::BaseNode> ast) {
        mFullContents.clear();
        mAst = ast;
    }

    const std::shared_ptr<ast::BaseNode>& SnowFile::getAst() const {
        return mAst;
    }

    const std::string& SnowFile::getFullContents() const {
        return mFullContents;
    }
}
