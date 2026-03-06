
// Copyright (c) 2025 Andrew Griffin - All Rights Reserved

#include <functional>
#include <unordered_set>

#include <testHelpers/TestFileSystem.hpp>
#include <testInfra/TestInfra.hpp>

namespace {
    size_t timeStampMod = 0;
    const float AvailableUnusedRatio = 0.8f;
}

namespace test {
    const size_t TestFileSystem::OneGig = 1'000'000'000;

    void TestFileSystem::TestFileStream::_flushFile() {
        if (mFlusherFn) {
            mFlusherFn();
        }
    }

    size_t TestFileSystem::TestFileStream::_getPendingWriteCursor() {
        return 0;
    }

    TestFileSystem::TestFileStream::~TestFileStream() {
        if (isOpen()) {
            mFile.reset();
        }
    }

    bool TestFileSystem::TestFileStream::isOpen() const {
        return !!mFile;
    }

    bool TestFileSystem::TestFileStream::close() {
        if (isOpen()) {
            _flushFile();
            mFile.reset();
        }
        return true;
    }

    std::optional<size_t> TestFileSystem::TestFileStream::fileSize() {
        if (isOpen()) {
            return mFile->mData.size() + _getPendingWriteCursor();
        }
        return {};
    }

    bool TestFileSystem::TestFileStream::setCursor(size_t position) {
        if (isOpen()) {
            _flushFile();
            if (position > mFile->mData.size()) {
                return false;
            }
            mCursor = position;
            return true;
        }
        return false;
    }

    bool TestFileSystem::TestFileStream::setCursorBegin() {
        if (isOpen()) {
            _flushFile();
            mCursor = 0;
            return true;
        }
        return false;
    }

    bool TestFileSystem::TestFileStream::setCursorEnd() {
        if (isOpen()) {
            _flushFile();
            mCursor = mFile->mData.size();
            return true;
        }
        return false;
    }

    std::optional<size_t> TestFileSystem::TestFileStream::getCursor() {
        if (isOpen()) {
            return mCursor + _getPendingWriteCursor();
        }
        return {};
    }

    std::optional<char> TestFileSystem::TestReadFileStream::peek() {
        if (TestFileStream::isOpen() && mCursor != mFile->mData.size()) {
            return mFile->mData[mCursor];
        }
        return {};
    }

    std::optional<char> TestFileSystem::TestReadFileStream::consume() {
        if (TestFileStream::isOpen() && mCursor != mFile->mData.size()) {
            mCursor++;
            return mFile->mData[mCursor - 1];
        }
        return {};
    }

    size_t TestFileSystem::TestReadFileStream::reverse(std::int64_t n) {
        if (n < 0) {
            return 0;
        }
        if (TestFileStream::isOpen()) {
            if (static_cast<std::int64_t>(mCursor) < n) {
                size_t v = mCursor;
                mCursor = 0;
                return v;
            }
            else {
                mCursor -= n;
                return n;
            }
        }
        return 0;
    }

    bool TestFileSystem::TestReadFileStream::readUntil(char delimiter) {
        if (TestFileStream::isOpen() && mCursor != mFile->mData.size()) {
            char c = mFile->mData[mCursor];
            while (c != delimiter && mCursor != mFile->mData.size()) {
                mCursor++;
                c = mFile->mData[mCursor];
            }
            return true;
        }
        return false;
    }

    size_t TestFileSystem::TestReadFileStream::readSome(size_t n, std::vector<char>& buffer) {
        if (n > buffer.size()) {
            return 0;
        }
        else if (n <= 0) {
            return 0;
        }
        else if (TestFileStream::isOpen() && mCursor != mFile->mData.size()) {
            for (size_t i = 0; i < n; ++i) {
                char c = mFile->mData[mCursor];
                mCursor++;
                buffer[i] = c;
                if (mCursor == mFile->mData.size()) {
                    _validateReadSome(i, buffer);
                    return i + 1;
                }
            }
            _validateReadSome(n, buffer);
            return n;
        }
        return 0;
    }

    TestFileSystem::TestReadCharFileStream::TestReadCharFileStream(std::shared_ptr<File> file) {
        mFile = file;
    }

    void TestFileSystem::TestReadCharFileStream::_validateReadSome(size_t n, std::vector<char>& buffer) {
        // read this in character mode (no /0) and in windows format (no /r)
        for (size_t i = 0; i < n; ++i) {
            if (buffer[i] == '\0') {
                for (size_t j = i; j < n; ++j) {
                    buffer[j] = '\0';
                }
                return;
            }
            else if (buffer[i] == '\r') {
                for (size_t j = i + 1; j < n; ++j) {
                    buffer[j - 1] = buffer[j];
                }
                i--;
                n--;
            }
        }
    }

    std::optional<std::string> TestFileSystem::TestReadCharFileStream::readEntireFileToMemory() {
        if (TestFileStream::isOpen()) {
            mCursor = mFile->mData.size();

            std::string str;
            for (auto&& c : mFile->mData) {
                if (c == '\0') {
                    return str; // char mode reading
                }
                else if (c == '\r') {
                    continue; // windows mode
                }
                str.push_back(c);
            }
            return str;
        }
        return {};
    }

    std::optional<std::string> TestFileSystem::TestReadCharFileStream::readLine() {
        if (TestFileStream::isOpen()) {
            if (mCursor == mFile->mData.size()) {
                return "";
            }

            std::string ret;
            char c = mFile->mData[mCursor];
            mCursor++;
            if (mCursor == mFile->mData.size()) {
                ret.push_back(c);
                return ret;
            }

            while (c != '\n') {
                if (c != '\r') {
                    ret.push_back(c);
                }
                else if (c == '\0') {
                    mCursor = mFile->mData.size();
                    break;
                }

                if (mCursor == mFile->mData.size()) {
                    break;
                }
                c = mFile->mData[mCursor];
                mCursor++;
            }
            if (c == '\n') {
                if (!ret.empty() && ret.back() == '\r') {
                    ret.pop_back();
                }
            }
            return std::move(ret);
        }
        return {};
    }

    std::optional<std::string> TestFileSystem::TestReadCharFileStream::read(size_t n) {
        if (TestFileStream::isOpen()) {
            std::vector<char> data;
            data.resize(n);
            size_t readN = TestReadFileStream::readSome(n, data);

            // make it behave like a char stream and in windows mode
            for (size_t i = 0; i < readN; ++i) {
                if (data[i] == '\r') {
                    for (size_t j = i + 1; j < readN; ++j) {
                        data[j - 1] = data[j];
                    }
                    readN--;
                    i--;
                }
                else if (data[i] == '\0') {
                    data.resize(i);
                    mCursor = mFile->mData.size();
                    break;
                }
            }

            std::string ret;
            ret.reserve(data.size());
            for (auto c : data) {
                ret.push_back(c);
            }
            return std::move(ret);
        }
        return {};
    }

    TestFileSystem::TestReadByteFileStream::TestReadByteFileStream(std::shared_ptr<File> file) {
        mFile = file;
    }

    std::optional<std::vector<char>> TestFileSystem::TestReadByteFileStream::readEntireFileToMemory() {
        if (TestFileStream::isOpen()) {
            mCursor = mFile->mData.size();
            std::vector<char> str;
            str.reserve(mFile->mData.size());
            for (auto&& c : mFile->mData) {
                str.emplace_back(c);
            }
            return std::move(str);
        }
        return {};
    }

    std::optional<std::vector<char>> TestFileSystem::TestReadByteFileStream::read(size_t n) {
        if (TestFileStream::isOpen()) {
            std::vector<char> data;
            data.resize(n);
            size_t readN = TestReadFileStream::readSome(n, data);
            data.resize(readN);
            return std::move(data);
        }
        return {};
    }

    TestFileSystem::TestWriteFileStream::TestWriteFileStream()
        : util::fs::WriteFileStream()
        , TestFileStream() {
        mFlusherFn = [this]() {
            flush();
        };
    }

    TestFileSystem::TestWriteFileStream::~TestWriteFileStream() {
        if (isOpen()) {
            flush();
            mFile.reset();
        }
    }

    size_t TestFileSystem::TestWriteFileStream::_getPendingWriteCursor() {
        return mPendingWrite.size();
    }

    bool TestFileSystem::TestWriteFileStream::push(char c) {
        if (TestFileStream::isOpen()) {
            mPendingWrite.emplace_back(c);
            if (!mPendingWriteCursor) {
                mPendingWriteCursor = mCursor;
            }
            mCursor++;
            return true;
        }
        return false;
    }

    bool TestFileSystem::TestWriteFileStream::flush() {
        if (TestFileStream::isOpen()) {
            if (!mPendingWrite.empty()) {
                ASSERT_NEMPTY(mPendingWriteCursor, "Cannot write pending, no cursor");
                size_t p = mPendingWriteCursor.value();
                mPendingWriteCursor = {};
                for (auto&& c : mPendingWrite) {
                    if (p == mFile->mData.size()) {
                        mFile->mData.push_back(c);
                    }
                    else {
                        mFile->mData[p] = c;
                    }
                    p++;
                }
                mPendingWrite.clear();
            }
            return true;
        }
        return false;
    }

    TestFileSystem::TestWriteCharFileStream::TestWriteCharFileStream(std::shared_ptr<File> file) {
        mFile = file;
    }

    bool TestFileSystem::TestWriteCharFileStream::writeLine(const std::string& data) {
        if (TestFileStream::isOpen()) {
            for (auto&& c : data) {
                mPendingWrite.emplace_back(c);
            }
            if (!mPendingWriteCursor) {
                mPendingWriteCursor = mCursor;
            }
            if (!data.empty() && data.back() != '\n' && data.back() != '\r') {
                mPendingWrite.emplace_back('\n');
                mCursor++;
            }
            mCursor += data.size();
            return true;
        }
        return false;
    }

    bool TestFileSystem::TestWriteCharFileStream::write(const std::string& data) {
        if (TestFileStream::isOpen()) {
            for (auto&& c : data) {
                mPendingWrite.emplace_back(c);
            }
            if (!mPendingWriteCursor) {
                mPendingWriteCursor = mCursor;
            }
            mCursor += data.size();
            return true;
        }
        return false;
    }

    TestFileSystem::TestWriteByteFileStream::TestWriteByteFileStream(std::shared_ptr<File> file) {
        mFile = file;
    }

    bool TestFileSystem::TestWriteByteFileStream::write(const std::vector<char>& data) {
        if (TestFileStream::isOpen()) {
            if (!mPendingWriteCursor) {
                mPendingWriteCursor = mCursor;
            }
            mCursor += data.size();
            for (auto&& c : data) {
                mPendingWrite.emplace_back(c);
            }
            return true;
        }
        return false;
    }

    TestFileSystem::File::File(const std::string& name, std::shared_ptr<Directory> parent)
        : mName(name)
        , mTimeStamp(::timeStampMod++)
        , mParent(parent) {
    }

    void TestFileSystem::File::updateTimeStamp() {
        mTimeStamp = ::timeStampMod++;
    }

    bool TestFileSystem::File::operator==(const File& other) const {
        // done this way to break point easier
        if (other.mData == mData && other.mName == mName) {
            return true;
        }
        return false;
    }

    TestFileSystem::Directory::Directory(const std::string& name, std::shared_ptr<Directory> parent)
        : mName(name)
        , mTimeStamp(::timeStampMod++)
        , mParent(parent) {
    }

    bool TestFileSystem::Directory::operator==(const Directory& other) const {
        // done this way to break point easier
        if (mDirs.size() != other.mDirs.size()) {
            return false;
        }
        else if (mFiles.size() != other.mFiles.size()) {
            return false;
        }
        else if (mName != other.mName) {
            return false;
        }

        for (auto&& dir : mDirs) {
            auto it = other.mDirs.find(dir.first);
            if (it == other.mDirs.end()) {
                return false;
            }
            else if (*dir.second != *it->second) {
                return false;
            }
        }
        for (auto&& f : mFiles) {
            auto it = other.mFiles.find(f.first);
            if (it == other.mFiles.end()) {
                return false;
            }
            else if (*f.second != *it->second) {
                return false;
            }
        }

        return true;
    }

    void TestFileSystem::Directory::updateTimeStamp() {
        mTimeStamp = ::timeStampMod++;
    }

    TestFileSystem::Mount::Mount(const std::string& name, size_t driveSize)
        : mDirectory(std::make_shared<Directory>("/", nullptr))
        , mName(name)
        , mCapacity(driveSize)
        , mAvailableSize(static_cast<size_t>(driveSize * ::AvailableUnusedRatio)) // TODO have files actually make this go down
        , mUnusedSize(driveSize) {
    }

    bool TestFileSystem::Mount::operator==(const Mount& other) const {
        // done this way to break point easier
        if (mName != other.mName) {
            return false;
        }
        else if (*mDirectory != *other.mDirectory) {
            return false;
        }
        return true;
    }

    TestFileSystem::MainMount::MainMount(const std::string& name, size_t driveSize)
        : Mount(name, driveSize) {
    }

    bool TestFileSystem::MainMount::operator==(const MainMount& other) const {
        // done this way to break point easier
        if (mName != other.mName) {
            return false;
        }
        else if (*mDirectory != *other.mDirectory) {
            return false;
        }
        else if (mMounts.size() != other.mMounts.size()) {
            return false;
        }

        for (auto&& m : mMounts) {
            auto it = other.mMounts.find(m.first);
            if (it == other.mMounts.end()) {
                return false;
            }
            if (it->second != m.second) {
                return false;
            }
        }
        return true;
    }

    TestFileSystem::Mount* TestFileSystem::_tryGetMount(const util::fs::Path& path) {
        auto pit = path.crbegin();
        if (pit == path.crend()) {
            return nullptr;
        }

        if (*pit == "/") {
            ++pit;
            if (pit == path.crend()) {
                return &mMainDrive;
            }

            if (*pit == "mnt") {
                ++pit;
                if (pit == path.crend()) {
                    return nullptr;
                }

                util::HashedString name = *pit;
                auto it = mMainDrive.mMounts.find(name);
                if (it == mMainDrive.mMounts.end()) {
                    return nullptr; // not a valid drive
                }
                return &it->second;
            }
            else {
                return &mMainDrive;
            }
        }
        return nullptr;
    }

    std::shared_ptr<TestFileSystem::Directory> TestFileSystem::_tryGetOrMakeDirectory(const util::fs::Path& path, MakeDirOpt make) {
        auto iter = path.rbegin();
        if (iter == path.rend()) {
            return nullptr;
        }

        util::fs::Path realPath = path;
        if (*iter != "/") {
            // not absolue, convert to absolute
            realPath = mCwd / path;
        }

        Mount* mount = _tryGetMount(realPath);
        if (!mount) {
            return nullptr;
        }

        auto it = realPath.crbegin();
        if (mount->mName == "/") {
            ++it;
        }
        else {
            ++it; // /
            ++it; // mnt
            ++it; // name
        }

        std::shared_ptr<Directory> dir = mount->mDirectory;
        for (; it != realPath.crend(); ++it) {
            if (it->starts_with("..")) {
                dir = dir->mParent;
                if (!dir) {
                    return nullptr; // not a path
                }
            }
            else if (it->starts_with("./")) {
                continue; // current directory
            }
            else if (realPath.hasFilename() && *it == realPath.filename()) {
                return dir; // we have reached a file, return the enclosing directory
            }

            auto d = dir->mDirs.find(*it);
            auto f = dir->mFiles.find(*it);
            if (f != dir->mFiles.end()) {
                return dir; // found a file, return the enclosing directory
            }

            if (d != dir->mDirs.end()) {
                dir = d->second;
                continue;
            }
            else if (make == MakeDirOpt::None) {
                return nullptr; // not a path
            }
            else if ((it + 1) == realPath.crend()) {
                auto newDir = dir->mDirs.emplace(*it, std::make_shared<Directory>(*it, dir));
                dir = newDir.first->second;
                return dir;
            }
            else if (make == MakeDirOpt::MakeRecursively) {
                auto newDir = dir->mDirs.emplace(*it, std::make_shared<Directory>(*it, dir));
                dir = newDir.first->second;
            }
            else {
                return nullptr;
            }
        }
        return dir;
    }

    util::fs::FsResult TestFileSystem::_moveFileImpl(
        const util::fs::Path& source,
        const util::fs::Path& destination,
        util::fs::FileChangeOptions options,
        bool copy) {
        if (!source.hasFilename()) {
            return util::fs::FsResult::Error;
        }

        std::shared_ptr<Directory> srcDir = _tryGetOrMakeDirectory(source);
        if (!srcDir) {
            return util::fs::FsResult::Error;
        }

        std::shared_ptr<File> sourceFile = nullptr;
        {
            auto srcFileIt = srcDir->mFiles.find(source.filename().value());
            if (srcFileIt == srcDir->mFiles.end()) {
                return util::fs::FsResult::Error;
            }
            sourceFile = srcFileIt->second;
        }

        std::shared_ptr<Directory> destDir = _tryGetOrMakeDirectory(destination);
        if (!destDir) {
            if (options & util::fs::FileChangeOptions::MakeDirDest) {
                destDir = _tryGetOrMakeDirectory(destination, MakeDirOpt::MakeRecursively);
                if (!destDir) {
                    return util::fs::FsResult::Error;
                }
            }
            else {
                return util::fs::FsResult::Error;
            }
        }

        bool checkForOverwrite = false;
        std::shared_ptr<File> destinationFile = nullptr;
        {
            if (destination.hasFilename()) {
                auto destFileIt = destDir->mFiles.find(destination.filename().value());
                if (destFileIt == destDir->mFiles.end()) {
                    auto destFileItE = destDir->mFiles.emplace(destination.filename().value(), std::make_shared<File>(destination.filename().value(), destDir));
                    destinationFile = destFileItE.first->second;
                }
                else {
                    destinationFile = destFileIt->second;
                    checkForOverwrite = true;
                }
            }
            else {
                auto destFileIt = destDir->mFiles.find(source.filename().value());
                if (destFileIt == destDir->mFiles.end()) {
                    auto destFileItE = destDir->mFiles.emplace(source.filename().value(), std::make_shared<File>(source.filename().value(), destDir));
                    destinationFile = destFileItE.first->second;
                }
                else {
                    destinationFile = destFileIt->second;
                    checkForOverwrite = true;
                }
            }
        }

        bool performOp = false;
        if (checkForOverwrite) {
            if (options & util::fs::FileChangeOptions::Overwrite) {
                performOp = true;
            }
            else if (options & util::fs::FileChangeOptions::OverwriteIfNewer) {
                if (destinationFile->mTimeStamp < sourceFile->mTimeStamp) {
                    performOp = true;
                }
                else {
                    return util::fs::FsResult::NoActionTaken;
                }
            }
        }
        else {
            performOp = true;
        }

        if (performOp) {
            if (copy) {
                destinationFile->mData = sourceFile->mData;
                destinationFile->mTimeStamp = ::timeStampMod++;
            }
            else {
                destinationFile->mData = std::move(sourceFile->mData);
                destinationFile->mTimeStamp = std::move(sourceFile->mTimeStamp);
                sourceFile->mParent->mFiles.erase(sourceFile->mName);
                sourceFile.reset();
            }
            return util::fs::FsResult::Success;
        }
        else {
            return util::fs::FsResult::Error;
        }
    }

    util::fs::FsResult TestFileSystem::_moveDirImpl(
        const util::fs::Path& source,
        const util::fs::Path& destination,
        util::fs::FileChangeOptions options,
        bool copy) {
        std::shared_ptr<Directory> srcDir = _tryGetOrMakeDirectory(source);
        if (!srcDir) {
            return util::fs::FsResult::Error;
        }

        bool checkForOverwrite = false;
        std::shared_ptr<Directory> destDir = _tryGetOrMakeDirectory(destination);
        if (!destDir) {
            auto parentPath = destination.parent();
            if (options & util::fs::FileChangeOptions::MakeDirDest) {
                destDir = _tryGetOrMakeDirectory(destination, MakeDirOpt::MakeRecursively);
                if (!destDir) {
                    return util::fs::FsResult::Error;
                }
            }
            // check for parent directory existing, then we can add a new directory for this
            else if (parentPath.has_value()) {
                parentPath = destination.parent();
                if (parentPath) {
                    destDir = _tryGetOrMakeDirectory(parentPath.value());
                    if (!destDir) {
                        return util::fs::FsResult::Error;
                    }
                    else {
                        destDir = _tryGetOrMakeDirectory(destination, MakeDirOpt::MakeRecursively);
                        if (!destDir) {
                            return util::fs::FsResult::Error;
                        }
                    }
                }
                else {
                    return util::fs::FsResult::Error;
                }
            }
            else {
                return util::fs::FsResult::Error;
            }
        }
        else {
            checkForOverwrite = true;
        }

        bool performOp = false;
        if (checkForOverwrite) {
            if (options & util::fs::FileChangeOptions::Overwrite) {
                performOp = true;
            }
            else if (options & util::fs::FileChangeOptions::OverwriteIfNewer) {
                if (destDir->mTimeStamp < srcDir->mTimeStamp) {
                    performOp = true;
                }
                else {
                    return util::fs::FsResult::NoActionTaken;
                }
            }
        }
        else {
            performOp = true;
        }

        if (performOp) {
            //size_t srcSize = _getDirectorySize(srcDir);
            //size_t destSize = _getDirectorySize(destDir);
            //Mount* dm = _tryGetMount(destination);
            //Mount* sm = _tryGetMount(source);
            //dm->mAvailableSize += srcSize;
            //dm->mAvailableSize -= destSize;
            if (copy) {
                destDir->mDirs = srcDir->mDirs;
                destDir->mFiles = srcDir->mFiles;
                destDir->mTimeStamp = ::timeStampMod++;
            }
            else {
                //sm->mAvailableSize -= srcSize;
                destDir->mDirs = std::move(srcDir->mDirs);
                destDir->mFiles = std::move(srcDir->mFiles);
                destDir->mTimeStamp = std::move(srcDir->mTimeStamp);
                srcDir->mParent->mDirs.erase(srcDir->mName);
            }
            return util::fs::FsResult::Success;
        }
        else {
            return util::fs::FsResult::Error;
        }
    }

    size_t TestFileSystem::_getDirectorySize(std::shared_ptr<Directory> dir) {
        if (!dir) {
            return 0;
        }
        size_t ret = 0;
        std::function<void(std::shared_ptr<Directory>)> callback = [&](std::shared_ptr<Directory> d) {
            for (auto&& f : d->mFiles) {
                ret += f.second->mData.size();
            }
            for (auto&& d2 : d->mDirs) {
                callback(d2.second);
            }
        };
        callback(dir);
        return ret;
    }

    TestFileSystem::TestFileSystem()
        : mMainDrive("/", OneGig)
        , mCwd("/home/")
        , mTmp("/tmp/")
        , mTimeStampBase(util::fs::StdFileDateTime::clock::now()) {
        clear();
        util::fs::FileSystem::setNewFileSystemDefault(this);
    }

    TestFileSystem::~TestFileSystem() {
        util::fs::FileSystem::setNewFileSystemDefault();
    }

    util::fs::FsResult TestFileSystem::copyFile(
        const util::fs::Path& source,
        const util::fs::Path& destination,
        util::fs::FileChangeOptions options) {
        return _moveFileImpl(source, destination, options, true);
    }

    util::fs::FsResult TestFileSystem::copyDir(
        const util::fs::Path& source,
        const util::fs::Path& destination,
        util::fs::FileChangeOptions options) {
        return _moveDirImpl(source, destination, options, true);
    }

    util::fs::FsResult TestFileSystem::mkdir(const util::fs::Path& path, bool recursively) {
        if (path.hasFilename()) {
            return util::fs::FsResult::Error;
        }

        if (isFile(path)) {
            return util::fs::FsResult::Error;
        }

        std::shared_ptr<Directory> dir = _tryGetOrMakeDirectory(path);
        if (dir) {
            return util::fs::FsResult::NoActionTaken;
        }

        dir = _tryGetOrMakeDirectory(path, recursively ? MakeDirOpt::MakeRecursively : MakeDirOpt::Make);
        if (dir) {
            return util::fs::FsResult::Success;
        }
        return util::fs::FsResult::Error;
    }

    std::optional<util::fs::Path> TestFileSystem::cwd() {
        return mCwd;
    }

    bool TestFileSystem::cwd(const util::fs::Path& path) {
        if (exists(path)) {
            mCwd = path;
            return true;
        }
        return false;
    }

    std::optional<util::fs::Path> TestFileSystem::tempDir() {
        return mTmp;
    }

    bool TestFileSystem::exists(const util::fs::Path& path) {
        std::shared_ptr<Directory> dir = _tryGetOrMakeDirectory(path);
        if (!dir) {
            return false;
        }

        if (path.hasFilename()) {
            auto it = dir->mFiles.find(path.filename().value());
            if (it == dir->mFiles.end()) {
                return false;
            }
        }
        else {
            if (dir->mName == path[0]) {
                return true;
            }
            auto it = dir->mDirs.find(path[0]);
            if (it == dir->mDirs.end()) {
                return false;
            }
        }
        return true;
    }

    std::optional<uint64_t> TestFileSystem::fileSize(const util::fs::Path& path) {
        if (!path.hasFilename()) {
            return {};
        }
        std::shared_ptr<Directory> dir = _tryGetOrMakeDirectory(path);
        if (!dir) {
            return {};
        }
        auto it = dir->mFiles.find(path.filename().value());
        return it->second->mData.size();
    }

    std::optional<util::fs::StdFileDateTime> TestFileSystem::fileTimeStamp(const util::fs::Path& path) {
        if (!path.hasFilename()) {
            return {};
        }
        std::shared_ptr<Directory> dir = _tryGetOrMakeDirectory(path);
        if (!dir) {
            return {};
        }
        auto it = dir->mFiles.find(path.filename().value());
        if (it == dir->mFiles.end()) {
            return {};
        }
        return mTimeStampBase + std::chrono::seconds(it->second->mTimeStamp);
    }

    std::optional<util::fs::StdFileDateTime> TestFileSystem::dirTimeStamp(const util::fs::Path& path) {
        if (path.hasFilename()) {
            return {};
        }
        std::shared_ptr<Directory> dir = _tryGetOrMakeDirectory(path);
        if (!dir) {
            return {};
        }
        return mTimeStampBase + std::chrono::seconds(dir->mTimeStamp);
    }

    bool TestFileSystem::touchFile(const util::fs::Path& path) {
        if (!path.hasFilename()) {
            return false;
        }
        std::shared_ptr<Directory> dir = _tryGetOrMakeDirectory(path);
        if (!dir) {
            return false;
        }
        auto it = dir->mFiles.find(path.filename().value());
        if (it == dir->mFiles.end()) {
            dir->mFiles.emplace(path.filename().value(), std::make_shared<File>(path.filename().value(), dir));
            return true;
        }
        it->second->mTimeStamp = ::timeStampMod++;
        return true;
    }

    util::fs::FsResult TestFileSystem::deleteFile(const util::fs::Path& path) {
        if (!path.hasFilename()) {
            return util::fs::FsResult::Error;
        }
        auto dir = _tryGetOrMakeDirectory(path);
        if (!dir) {
            return util::fs::FsResult::NoActionTaken;
        }
        auto it = dir->mFiles.find(path.filename().value());
        if (it == dir->mFiles.end()) {
            return util::fs::FsResult::NoActionTaken;
        }
        dir->mFiles.erase(it->second->mName);
        return util::fs::FsResult::Success;
    }

    util::fs::FsResult TestFileSystem::deleteDir(const util::fs::Path& path) {
        if (path.hasFilename()) {
            return util::fs::FsResult::Error;
        }
        auto dir = _tryGetOrMakeDirectory(path);
        if (!dir) {
            return util::fs::FsResult::NoActionTaken;
        }
        if (dir->mParent) {
            dir->mParent->mDirs.erase(dir->mName);
            return util::fs::FsResult::Success;
        }
        return util::fs::FsResult::Error;
    }

    std::optional<util::fs::PartitionData> TestFileSystem::statParition(const util::fs::Path& path) {
        Mount* mount = _tryGetMount(path);
        if (!mount) {
            return {};
        }
        return util::fs::PartitionData{
            util::fs::Path("/mnt/") / mount->mName,
            mount->mCapacity,
            mount->mUnusedSize,
            mount->mAvailableSize
        };
    }

    bool TestFileSystem::isFile(const util::fs::Path& path) {
        if (!path.hasFilename()) {
            return false;
        }
        auto dir = _tryGetOrMakeDirectory(path);
        if (!dir) {
            return false;
        }
        auto it = dir->mFiles.find(path[0]);
        if (it == dir->mFiles.end()) {
            auto dit = dir->mDirs.find(path[0]);
            if (dit == dir->mDirs.end()) {
                return false;
            }
            return false;
        }
        return true;
    }

    bool TestFileSystem::isDir(const util::fs::Path& path) {
        if (path.hasFilename()) {
            return false;
        }
        auto dir = _tryGetOrMakeDirectory(path);
        if (!dir) {
            return false;
        }
        auto it = dir->mFiles.find(path[0]);
        if (it != dir->mFiles.end()) {
            return false;
        }
        return true;
    }

    std::optional<bool> TestFileSystem::emptyFile(const util::fs::Path& path) {
        if (!path.hasFilename()) {
            return {};
        }
        auto dir = _tryGetOrMakeDirectory(path);
        if (!dir) {
            return {};
        }
        auto it = dir->mFiles.find(path[0]);
        if (it == dir->mFiles.end()) {
            return {};
        }
        return it->second->mData.empty();
    }

    std::optional<bool> TestFileSystem::emptyDir(const util::fs::Path& path) {
        if (path.hasFilename()) {
            return {};
        }
        auto dir = _tryGetOrMakeDirectory(path);
        if (!dir) {
            return {};
        }
        return dir->mDirs.empty() && dir->mFiles.empty();
    }

    util::fs::FsResult TestFileSystem::renameFile(
        const util::fs::Path& path,
        const std::string& newName,
        util::fs::FileChangeOptions options) {
        if (options & util::fs::FileChangeOptions::MakeDirDest) {
            return util::fs::FsResult::Error;
        }
        util::fs::Path dest = path;
        dest[0] = newName;
        return _moveFileImpl(path, dest, options, false);
    }

    util::fs::FsResult TestFileSystem::renameDir(
        const util::fs::Path& path,
        const std::string& newName,
        util::fs::FileChangeOptions options) {
        if (options & util::fs::FileChangeOptions::MakeDirDest) {
            return util::fs::FsResult::Error;
        }
        util::fs::Path dest = path;
        dest[0] = newName;
        return _moveDirImpl(path, dest, options, false);
    }

    util::fs::FsResult TestFileSystem::moveFile(
        const util::fs::Path& source,
        const util::fs::Path& destination,
        util::fs::FileChangeOptions options) {
        return _moveFileImpl(source, destination, options, false);
    }

    util::fs::FsResult TestFileSystem::moveDir(
        const util::fs::Path& source,
        const util::fs::Path& destination,
        util::fs::FileChangeOptions options) {
        return _moveDirImpl(source, destination, options, false);
    }

    util::fs::DirectoryIterable TestFileSystem::iterate(const util::fs::Path& path, bool recursively) {
        if (path.hasFilename()) {
            return _makeDirIter(path);
        }
        auto dir = _tryGetOrMakeDirectory(path);
        if (!dir) {
            return _makeDirIter(path);
        }
        std::vector<util::fs::Path> paths;
        util::fs::Path currPath = path;
        std::function<void(std::shared_ptr<Directory>)> callback = [&](std::shared_ptr<Directory> d) {
            for (auto&& f : d->mFiles) {
                paths.emplace_back(currPath / f.second->mName);
            }
            for (auto&& dd : d->mDirs) {
                paths.emplace_back(currPath / dd.second->mName);
                if (recursively) {
                    currPath /= dd.second->mName;
                    callback(dd.second);
                    currPath.pop();
                }
            }
        };
        callback(dir);
        return _makeDirIter(path, std::move(paths));
    }

    std::unique_ptr<util::fs::ReadCharFileStream> TestFileSystem::openTextFileReader(const util::fs::Path& path) {
        auto dir = _tryGetOrMakeDirectory(path);
        if (!dir) {
            return nullptr;
        }
        auto it = dir->mFiles.find(path[0]);
        if (it == dir->mFiles.end()) {
            return nullptr;
        }
        return std::make_unique<TestReadCharFileStream>(it->second);
    }

    std::unique_ptr<util::fs::ReadByteFileStream> TestFileSystem::openBinaryFileReader(const util::fs::Path& path) {
        auto dir = _tryGetOrMakeDirectory(path);
        if (!dir) {
            return nullptr;
        }
        auto it = dir->mFiles.find(path[0]);
        if (it == dir->mFiles.end()) {
            return nullptr;
        }
        return std::make_unique<TestReadByteFileStream>(it->second);
    }

    std::unique_ptr<util::fs::WriteCharFileStream> TestFileSystem::openTextFileWriter(const util::fs::Path& path) {
        auto dir = _tryGetOrMakeDirectory(path);
        if (!dir) {
            return nullptr;
        }
        auto it = dir->mFiles.find(path[0]);
        if (it == dir->mFiles.end()) {
            it = dir->mFiles.emplace(path[0], std::make_shared<File>(path[0], dir)).first;
        }
        return std::make_unique<TestWriteCharFileStream>(it->second);
    }

    std::unique_ptr<util::fs::WriteByteFileStream> TestFileSystem::openBinaryFileWriter(const util::fs::Path& path) {
        auto dir = _tryGetOrMakeDirectory(path);
        if (!dir) {
            return nullptr;
        }
        auto it = dir->mFiles.find(path[0]);
        if (it == dir->mFiles.end()) {
            it = dir->mFiles.emplace(path[0], std::make_shared<File>(path[0], dir)).first;
        }
        return std::make_unique<TestWriteByteFileStream>(it->second);
    }

    util::fs::Path TestFileSystem::mount(const std::string& mountName, size_t size) {
        mMainDrive.mMounts.emplace(mountName, Mount(mountName, size));
        return util::fs::Path("/mnt/") / mountName;
    }

    void TestFileSystem::demount(const std::string& mountName) {
        auto it = mMainDrive.mMounts.find(mountName);
        if (it != mMainDrive.mMounts.end()) {
            mMainDrive.mMounts.erase(it);
        }
    }

    void TestFileSystem::clear() {
        mMainDrive.mMounts.clear();
        mMainDrive.mDirectory = std::make_shared<Directory>("/", nullptr);
        ASSERT_NOTNULL(_tryGetOrMakeDirectory("/home/", MakeDirOpt::MakeRecursively));
        ASSERT_NOTNULL(_tryGetOrMakeDirectory("/tmp/", MakeDirOpt::MakeRecursively));
    }

    void TestFileSystem::assertFileIdentical(const util::fs::Path& a, const util::fs::Path& b) {
        auto aDir = _tryGetOrMakeDirectory(a);
        ASSERT_NOTNULL(aDir);
        auto bDir = _tryGetOrMakeDirectory(b);
        ASSERT_NOTNULL(bDir);
        auto aIt = aDir->mFiles.find(a.filename().value());
        ASSERT_NEQ(aIt, aDir->mFiles.end());
        auto bIt = bDir->mFiles.find(b.filename().value());
        ASSERT_NEQ(bIt, bDir->mFiles.end());
        ASSERT_EQ(aIt->second->mData, bIt->second->mData);
    }

    void TestFileSystem::assertDirIdentical(const util::fs::Path& a, const util::fs::Path& b) {
        auto aDir = _tryGetOrMakeDirectory(a);
        ASSERT_NOTNULL(aDir);
        auto bDir = _tryGetOrMakeDirectory(b);
        ASSERT_NOTNULL(bDir);
        std::unordered_set<util::HashedString> foundInA;
        for (auto&& af : aDir->mFiles) {
            auto bf = bDir->mFiles.find(af.first);
            ASSERT_NEQ(bf, bDir->mFiles.end());
            ASSERT_EQ(af.second->mData, bf->second->mData);
            foundInA.emplace(af.first);
        }
        for (auto&& bf : bDir->mFiles) {
            auto it = foundInA.find(bf.first);
            ASSERT_NEQ(it, foundInA.end());
        }
        foundInA.clear();
        for (auto&& ad : aDir->mDirs) {
            auto bd = bDir->mDirs.find(ad.first);
            ASSERT_NEQ(bd, bDir->mDirs.end());
            assertDirIdentical(a / ad.first.getStr(), b / bd->first.getStr());
            foundInA.emplace(ad.first);
        }
        for (auto&& bd : bDir->mDirs) {
            auto it = foundInA.find(bd.first);
            ASSERT_NEQ(it, foundInA.end());
        }
    }

    void TestFileSystem::assertPathIdentical(const util::fs::Path& a, const util::fs::Path& b) {
        if (isFile(a) && isFile(b)) {
            assertFileIdentical(a, b);
        }
        else if (isDir(a) && isDir(b)) {
            assertDirIdentical(a, b);
        }
        else {
            ASSERT_FAIL("Path a and b are not the same type");
        }
    }
}
