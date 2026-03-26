
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#pragma once

#include <functional>
#include <unordered_map>
#include <variant>

#include <util/FileSystem.hpp>
#include <util/HashedString.hpp>
#include <util/Singleton.hpp>

namespace test {
    class TestFileSystemTests;

    // An in memory filesystem used for tests
    // Assumes linux based filesystem, root is /
    class TestFileSystem : public util::fs::FileSystem {
        static const size_t OneGig;
        friend class TestFileSystemTests;
        struct Directory;

        struct File {
            File(const std::string& name, std::shared_ptr<Directory> parent);
            bool operator==(const File& other) const;
            bool operator!=(const File& other) const { return !(*this == other); }
            void updateTimeStamp();
            std::string mName;
            std::string mData;
            size_t mTimeStamp;
            std::shared_ptr<Directory> mParent;
        };

        struct Directory {
            Directory() = default;
            Directory(const std::string& name, std::shared_ptr<Directory> parent);
            bool operator==(const Directory& other) const;
            bool operator!=(const Directory& other) const { return !(*this == other); }
            void updateTimeStamp();
            std::unordered_map<util::HashedString, std::shared_ptr<File>> mFiles;
            std::unordered_map<util::HashedString, std::shared_ptr<Directory>> mDirs;
            std::string mName;
            size_t mTimeStamp;
            std::shared_ptr<Directory> mParent;
        };

        struct Mount {
            Mount(const std::string& name, size_t driveSize);
            bool operator==(const Mount& other) const;
            bool operator!=(const Mount& other) const { return !(*this == other); }
            std::shared_ptr<Directory> mDirectory;
            std::string mName;
            size_t mCapacity;
            size_t mAvailableSize; // 0.8 of unused, represents drive fragmentation and stuff
            size_t mUnusedSize; // total unused
        };

        struct MainMount : public Mount {
            MainMount(const std::string& name = "/", size_t driveSize = TestFileSystem::OneGig);
            bool operator==(const MainMount& other) const;
            bool operator!=(const MainMount& other) const { return !(*this == other); }
            std::unordered_map<util::HashedString, Mount> mMounts; // /mnt/<name> folders
            std::string mID = "TFS";
        };

        MainMount mMainDrive; // everything except the /mnt/* folders
        util::fs::Path mCwd;
        util::fs::Path mTmp;
        util::fs::StdFileDateTime mTimeStampBase;
        size_t mTimeStampNext = 0;

        enum class MakeDirOpt {
            None,
            Make,
            MakeRecursively
        };

        Mount* _tryGetMount(const util::fs::Path& path);
        std::shared_ptr<Directory> _tryGetOrMakeDirectory(const util::fs::Path& path, MakeDirOpt make = MakeDirOpt::None);
        util::fs::FsResult _moveFileImpl(
            const util::fs::Path& source,
            const util::fs::Path& destination,
            util::fs::FileChangeOptions options,
            bool copy);
        util::fs::FsResult _moveDirImpl(
            const util::fs::Path& source,
            const util::fs::Path& destination,
            util::fs::FileChangeOptions options,
            bool copy);
        size_t _getDirectorySize(std::shared_ptr<Directory> dir);

        class TestFileStream : virtual public util::fs::FileStream {
        protected:
            std::shared_ptr<File> mFile;
            size_t mCursor = 0;
            std::function<void()> mFlusherFn;
            void _flushFile();
            virtual size_t _getPendingWriteCursor();
        public:
            ~TestFileStream() override;
            bool isOpen() const override;
            bool close() override;
            std::optional<size_t> fileSize() override;
            bool setCursor(size_t position) override;
            bool setCursorBegin() override;
            bool setCursorEnd() override;
            std::optional<size_t> getCursor() override;
        };
        class TestReadFileStream : virtual public util::fs::ReadFileStream, protected TestFileStream {
        protected:
            virtual void _validateReadSome(size_t n, std::vector<char>& buffer) = 0; // char and byte read differently
        public:
            std::optional<char> peek() override;
            std::optional<char> consume() override;
            size_t reverse(std::int64_t n = 1) override;
            bool readUntil(char delimiter) override;
            size_t readSome(size_t n, std::vector<char>& buffer) override;
        };
        class TestWriteFileStream : virtual public util::fs::WriteFileStream, protected TestFileStream {
        protected:
            std::vector<char> mPendingWrite;
            std::optional<size_t> mPendingWriteCursor;
            size_t _getPendingWriteCursor() override;
        public:
            TestWriteFileStream();
            ~TestWriteFileStream() override;
            bool push(char c) override;
            bool flush() override;
        };
        class TestReadCharFileStream : virtual public util::fs::ReadCharFileStream, protected TestReadFileStream {
        protected:
            void _validateReadSome(size_t n, std::vector<char>& buffer) override;
        public:
            TestReadCharFileStream(std::shared_ptr<File> file);
            std::optional<std::string> readEntireFileToMemory() override;
            std::optional<std::string> readLine() override;
            std::optional<std::string> read(size_t n) override;
        };
        class TestReadByteFileStream : virtual public util::fs::ReadByteFileStream, protected TestReadFileStream {
        protected:
            void _validateReadSome(size_t, std::vector<char>&) override {} // byte reading reads everything
        public:
            TestReadByteFileStream(std::shared_ptr<File> file);
            std::optional<std::vector<char>> readEntireFileToMemory() override;
            std::optional<std::vector<char>> read(size_t n) override;
        };
        class TestWriteCharFileStream : virtual public util::fs::WriteCharFileStream, protected TestWriteFileStream {
        public:
            TestWriteCharFileStream(std::shared_ptr<File> file);
            bool writeLine(const std::string& data) override;
            bool write(const std::string& data) override;
        };
        class TestWriteByteFileStream : virtual public util::fs::WriteByteFileStream, protected TestWriteFileStream {
        public:
            TestWriteByteFileStream(std::shared_ptr<File> file);
            bool write(const std::vector<char>& data) override;
        };

    public:
        TestFileSystem();
        virtual ~TestFileSystem();
        virtual util::fs::FsResult copyFile(
            const util::fs::Path& source,
            const util::fs::Path& destination,
            util::fs::FileChangeOptions options = util::fs::FileChangeOptions::None) override;
        virtual util::fs::FsResult copyDir(
            const util::fs::Path& source,
            const util::fs::Path& destination,
            util::fs::FileChangeOptions options = util::fs::FileChangeOptions::None) override;
        virtual util::fs::FsResult mkdir(const util::fs::Path& path, bool recursively = false) override;
        virtual std::optional<util::fs::Path> cwd() override;
        virtual bool cwd(const util::fs::Path& path) override;
        virtual std::optional<util::fs::Path> tempDir() override;
        virtual bool exists(const util::fs::Path& path) override;
        virtual std::optional<uint64_t> fileSize(const util::fs::Path& path) override;
        virtual std::optional<util::fs::StdFileDateTime> fileTimeStamp(const util::fs::Path& path) override;
        virtual std::optional<util::fs::StdFileDateTime> dirTimeStamp(const util::fs::Path& path) override;
        virtual bool touchFile(const util::fs::Path& path) override;
        virtual util::fs::FsResult deleteFile(const util::fs::Path& path) override;
        virtual util::fs::FsResult deleteDir(const util::fs::Path& path) override;
        virtual std::optional<util::fs::PartitionData> statParition(const util::fs::Path& path) override;
        virtual bool isFile(const util::fs::Path& path) override;
        virtual bool isDir(const util::fs::Path& path) override;
        virtual std::optional<bool> emptyFile(const util::fs::Path& path) override;
        virtual std::optional<bool> emptyDir(const util::fs::Path& path) override;
        virtual util::fs::FsResult renameFile(
            const util::fs::Path& path,
            const std::string& newName,
            util::fs::FileChangeOptions options = util::fs::FileChangeOptions::None) override;
        virtual util::fs::FsResult renameDir(
            const util::fs::Path& path,
            const std::string& newName,
            util::fs::FileChangeOptions options = util::fs::FileChangeOptions::None) override;
        virtual util::fs::FsResult moveFile(
            const util::fs::Path& source,
            const util::fs::Path& destination,
            util::fs::FileChangeOptions options = util::fs::FileChangeOptions::None) override;
        virtual util::fs::FsResult moveDir(
            const util::fs::Path& source,
            const util::fs::Path& destination,
            util::fs::FileChangeOptions options = util::fs::FileChangeOptions::None) override;
        virtual util::fs::DirectoryIterable iterate(const util::fs::Path& path, bool recursively = false) override;
        virtual std::unique_ptr<util::fs::ReadCharFileStream> openTextFileReader(const util::fs::Path& path) override;
        virtual std::unique_ptr<util::fs::ReadByteFileStream> openBinaryFileReader(const util::fs::Path& path) override;
        virtual std::unique_ptr<util::fs::WriteCharFileStream> openTextFileWriter(const util::fs::Path& path) override;
        virtual std::unique_ptr<util::fs::WriteByteFileStream> openBinaryFileWriter(const util::fs::Path& path) override;


        // not part of the interface
        virtual util::fs::Path mount(const std::string& mountName, size_t size);
        virtual void demount(const std::string& mountName);
        virtual void clear();

        // compares two files to be the same (but at different paths) or two directories
        // (which are at different paths) to be the same (with all children as well)
        virtual void assertPathIdentical(const util::fs::Path& a, const util::fs::Path& b);
        virtual void assertFileIdentical(const util::fs::Path& a, const util::fs::Path& b);
        virtual void assertDirIdentical(const util::fs::Path& a, const util::fs::Path& b);
    };
}
