
// Copyright (c) 2025 Andrew Griffin - All Rights Reserved

#include <testHelpers/TestFileSystem.hpp>
#include <testInfra/TestInfra.hpp>

using namespace util::fs;

namespace test {
    TEST_CLASS(TestFileSystemTests) {
        TestFileSystem mSys;
        Path mRoot = "/";

        // for expected
        std::shared_ptr<TestFileSystem::Directory> addDir(
            std::shared_ptr<TestFileSystem::Directory> parent,
            const std::string& name) {
            ASSERT_NOTNULL(parent);
            auto it = parent->mDirs.find(name);
            ASSERT_EQ(it, parent->mDirs.end());
            return parent->mDirs.emplace(
                name,
                std::make_shared<TestFileSystem::Directory>(name, parent)
            ).first->second;
        }

        std::shared_ptr<TestFileSystem::File> addFile(
            std::shared_ptr<TestFileSystem::Directory> parent,
            const std::string& name,
            const std::string& contents) {
            ASSERT_NOTNULL(parent);
            auto it = parent->mFiles.find(name);
            ASSERT_EQ(it, parent->mFiles.end());
            std::shared_ptr<TestFileSystem::File> f = parent->mFiles.emplace(
                name,
                std::make_shared<TestFileSystem::File>(name, parent)
            ).first->second;
            f->mData = contents;
            return f;
        }

        // -----------------------------
        // for functioning fs
        void makeTestFile(const Path& filePath, const std::string& contents) {
            ASSERT_TRUE(filePath.hasFilename());
            auto dir = mSys._tryGetOrMakeDirectory(filePath, TestFileSystem::MakeDirOpt::MakeRecursively);
            ASSERT_NOTNULL(dir);
            auto it = dir->mFiles.find(filePath[0]);
            if (it == dir->mFiles.end()) {
                it = dir->mFiles.emplace(filePath[0], std::make_shared<TestFileSystem::File>(filePath[0], dir)).first;
            }
            it->second->mData = contents;
            it->second->updateTimeStamp();
        }

        void makeTestDir(const Path& path) {
            ASSERT_FALSE(path.hasFilename());
            auto dir = mSys._tryGetOrMakeDirectory(path, TestFileSystem::MakeDirOpt::MakeRecursively);
            ASSERT_NOTNULL(dir);
        }

        TestFileSystem::MainMount makeExpected() {
            TestFileSystem::MainMount m;
            m.mID = "Expected";
            m.mDirectory->mDirs.emplace("home", std::make_shared<TestFileSystem::Directory>("home", m.mDirectory));
            m.mDirectory->mDirs.emplace("tmp", std::make_shared<TestFileSystem::Directory>("tmp", m.mDirectory));
            return m;
        }

        void validate(const TestFileSystem::MainMount& drive) {
            EXPECT_EQ(drive, mSys.mMainDrive);
        }

        TEST_STOP() {
            mSys.clear();
        }

        TEST(TestFS_CopyFile) {
            makeTestFile(mRoot / "hello.txt", "hello world");
            makeTestFile(mRoot / "over.txt", "gamer");
            TestFileSystem::MainMount expected = makeExpected();
            auto hellotxtFile = addFile(expected.mDirectory, "hello.txt", "hello world");
            addFile(expected.mDirectory, "over.txt", "gamer");
            validate(expected);

            // straight copy
            ASSERT_EQ(FsResult::Success, copyFile(mRoot / "hello.txt", mRoot / "hello2.txt", FileChangeOptions::None));
            addFile(expected.mDirectory, "hello2.txt", "hello world");
            validate(expected);

            // dont overwrite
            ASSERT_EQ(FsResult::Error, copyFile(mRoot / "hello.txt", mRoot / "over.txt", FileChangeOptions::None));

            // dont overwrite if newer
            ASSERT_EQ(FsResult::NoActionTaken, copyFile(mRoot / "hello.txt", mRoot / "over.txt", FileChangeOptions::OverwriteIfNewer));

            // overwrite if newer
            ASSERT_EQ(FsResult::Success, copyFile(mRoot / "over.txt", mRoot / "hello.txt", FileChangeOptions::OverwriteIfNewer));
            hellotxtFile->mData = "gamer";
            validate(expected);

            // overwrite
            ASSERT_EQ(FsResult::Success, copyFile(mRoot / "hello2.txt", mRoot / "hello.txt", FileChangeOptions::Overwrite));
            hellotxtFile->mData = "hello world";
            validate(expected);

            // source doesnt exist
            ASSERT_EQ(FsResult::Error, copyFile(mRoot / "blah.txt", mRoot / "hello.txt", FileChangeOptions::None));

            // source is not a file
            ASSERT_EQ(FsResult::Error, copyFile(mRoot / "hello", mRoot / "hello2.txt", FileChangeOptions::None));

            // destination doesnt exist
            ASSERT_EQ(FsResult::Success, copyFile(mRoot / "hello.txt", mRoot / "hello3.txt", FileChangeOptions::None));
            addFile(expected.mDirectory, "hello3.txt", "hello world");
            validate(expected);

            // destination is a directory (that exists)
            makeTestDir(mRoot / "dirA");
            ASSERT_EQ(FsResult::Success, copyFile(mRoot / "hello.txt", mRoot / "dirA", FileChangeOptions::None));
            auto dirA = addDir(expected.mDirectory, "dirA");
            addFile(dirA, "hello.txt", "hello world");
            validate(expected);

            // destination is a directory that doesnt exist
            ASSERT_EQ(FsResult::Error, copyFile(mRoot / "hello.txt", mRoot / "dirB", FileChangeOptions::None));

            // destination is a directory that doesnt exist and create flag
            ASSERT_EQ(FsResult::Success, copyFile(mRoot / "hello.txt", mRoot / "dirB", FileChangeOptions::MakeDirDest));
            auto dirB = addDir(expected.mDirectory, "dirB");
            addFile(dirB, "hello.txt", "hello world");
            validate(expected);
        }

        TEST(TestFS_CopyDir) {
            makeTestDir(mRoot / "dirA");
            makeTestFile(mRoot / "dirA" / "hello.txt", "blah");
            TestFileSystem::MainMount expected = makeExpected();
            auto dirA = addDir(expected.mDirectory, "dirA");
            addFile(dirA, "hello.txt", "blah");

            // straight copy
            ASSERT_EQ(FsResult::Success, copyDir(mRoot / "dirA", mRoot / "dirB", FileChangeOptions::None));
            auto dirB = addDir(expected.mDirectory, "dirB");
            addFile(dirB, "hello.txt", "blah");
            validate(expected);

            // dont overwrite
            ASSERT_EQ(FsResult::Error, copyDir(mRoot / "dirA", mRoot / "dirB", FileChangeOptions::None));

            // dont overwrite if newer
            ASSERT_EQ(FsResult::NoActionTaken, copyDir(mRoot / "dirA", mRoot / "dirB", FileChangeOptions::OverwriteIfNewer));

            // overwrite if newer
            ASSERT_EQ(FsResult::Success, copyDir(mRoot / "dirB", mRoot / "dirA", FileChangeOptions::OverwriteIfNewer));
            validate(expected);

            // overwrite
            ASSERT_EQ(FsResult::Success, copyDir(mRoot / "dirB", mRoot / "dirA", FileChangeOptions::Overwrite));
            validate(expected);

            // source doesnt exist
            ASSERT_EQ(FsResult::Error, copyDir(mRoot / "dirC", mRoot / "dirA", FileChangeOptions::None));

            // source is a file
            makeTestFile(mRoot / "file.txt", "blank");
            addFile(expected.mDirectory, "file.txt", "blank");
            ASSERT_EQ(FsResult::Error, copyDir(mRoot / "file.txt", mRoot / "dirA", FileChangeOptions::None));

            // destination doesnt exist (parent does)
            makeTestDir(mRoot / "dirC");
            ASSERT_EQ(FsResult::Success, copyDir(mRoot / "dirA", mRoot / "dirC" / "childDir", FileChangeOptions::None));
            auto dirC = addDir(expected.mDirectory, "dirC");
            auto C_childDir = addDir(dirC, "childDir");
            addFile(C_childDir, "hello.txt", "blah");
            validate(expected);

            // destination doesnt exist (parent doesnt either)
            ASSERT_EQ(FsResult::Error, copyDir(mRoot / "dirA", mRoot / "dirD" / "childDir", FileChangeOptions::None));

            // destination is a file
            ASSERT_EQ(FsResult::Error, copyDir(mRoot / "dirA", mRoot / "file.txt", FileChangeOptions::None));

            // destination doesnt exist but create flag set
            ASSERT_EQ(FsResult::Success, copyDir(mRoot / "dirA", mRoot / "dirD" / "childDir", FileChangeOptions::MakeDirDest));
            auto dirD = addDir(expected.mDirectory, "dirD");
            auto D_childDir = addDir(dirD, "childDir");
            addFile(D_childDir, "hello.txt", "blah");
            validate(expected);

            // recursively copy
            makeTestDir(mRoot / "A");
            makeTestDir(mRoot / "A" / "B");
            makeTestDir(mRoot / "A" / "B" / "C");
            makeTestDir(mRoot / "A" / "B" / "C" / "D");
            makeTestFile(mRoot / "A" / "file.txt", "a");
            makeTestFile(mRoot / "A" / "B" / "file.txt", "b");
            makeTestFile(mRoot / "A" / "B" / "C" / "file.txt", "c");
            makeTestFile(mRoot / "A" / "B" / "C" / "D" / "file.txt", "d");

            auto A = addDir(expected.mDirectory, "A");
            auto copyA = addDir(expected.mDirectory, "copyA");
            std::vector<std::shared_ptr<TestFileSystem::Directory>> toMake = { A, copyA };
            for (auto&& a : toMake) {
                auto B = addDir(a, "B");
                auto C = addDir(B, "C");
                auto D = addDir(C, "D");
                addFile(a, "file.txt", "a");
                addFile(B, "file.txt", "b");
                addFile(C, "file.txt", "c");
                addFile(D, "file.txt", "d");
            }

            ASSERT_EQ(FsResult::Success, copyDir(mRoot / "A", mRoot / "copyA", FileChangeOptions::MakeDirDest));
            validate(expected);
        }

        TEST(TestFS_MkDir) {
            TestFileSystem::MainMount expected = makeExpected();
            addDir(expected.mDirectory, "A");

            // normal
            ASSERT_EQ(FsResult::Success, mkdir(mRoot / "A", false));
            validate(expected);

            // path is a file
            ASSERT_EQ(FsResult::Error, mkdir(mRoot / "B.txt", false));

            // path already exists
            ASSERT_EQ(FsResult::NoActionTaken, mkdir(mRoot / "A", false));

            // parent does not exist
            ASSERT_EQ(FsResult::Error, mkdir(mRoot / "B" / "C", false));

            // parent does not exist with recursive flag
            ASSERT_EQ(FsResult::Success, mkdir(mRoot / "B" / "C", true));
            auto B = addDir(expected.mDirectory, "B");
            addDir(B, "C");
            validate(expected);
        }

        TEST(TestFS_BuiltInPaths) {
            auto dir = cwd();
            auto tmp = tempDir();
            ASSERT_TRUE(dir.has_value());
            EXPECT_FALSE(dir.value().empty());
            ASSERT_TRUE(tmp.has_value());
            EXPECT_FALSE(tmp.value().empty());
        }

        TEST(TestFS_Exists) {
            makeTestFile(mRoot / "hello.txt", "");
            makeTestDir(mRoot / "A");
            makeTestDir(mRoot / "A" / "B");
            makeTestFile(mRoot / "A" / "B" / "hello.txt", "abcd"); // 4 bytes

            // exist
            EXPECT_TRUE(exists(mRoot / "hello.txt"));
            EXPECT_TRUE(exists(mRoot / "A"));
            EXPECT_TRUE(exists(mRoot / "A" / "B"));
            EXPECT_TRUE(exists(mRoot / "A" / "B" / "hello.txt"));
            EXPECT_FALSE(exists({}));
            EXPECT_FALSE(exists(mRoot / "C"));
            EXPECT_FALSE(exists(mRoot / "C.txt"));

            // size
            EXPECT_EQ(fileSize(mRoot / "hello.txt"), 0);
            EXPECT_EMPTY(fileSize(mRoot / "A"));
            EXPECT_EMPTY(fileSize(mRoot / "noExist"));
            EXPECT_EQ(fileSize(mRoot / "A" / "B" / "hello.txt"), 4); // 4 bytes
        }

        TEST(TestFS_TimeStampAndTouch) {
            makeTestFile(mRoot / "hello.txt", "");
            makeTestDir(mRoot / "A");

            // validate it all exists
            ASSERT_NEQ(mSys.mMainDrive.mDirectory->mDirs.find("A"), mSys.mMainDrive.mDirectory->mDirs.end());
            ASSERT_NEQ(mSys.mMainDrive.mDirectory->mFiles.find("hello.txt"), mSys.mMainDrive.mDirectory->mFiles.end());

            auto base = mSys.mTimeStampBase;
            auto hello = base + std::chrono::seconds(mSys.mMainDrive.mDirectory->mFiles.find("hello.txt")->second->mTimeStamp);
            auto A = base + std::chrono::seconds(mSys.mMainDrive.mDirectory->mDirs.find("A")->second->mTimeStamp);

            // normal
            EXPECT_EQ(hello, fileTimeStamp(mRoot / "hello.txt"));
            EXPECT_EQ(A, dirTimeStamp(mRoot / "A"));

            // doesnt exist
            EXPECT_EMPTY(fileTimeStamp(mRoot / "blank.txt"));
            EXPECT_EMPTY(dirTimeStamp(mRoot / "B"));

            // is not correct type
            EXPECT_EMPTY(fileTimeStamp(mRoot / "A"));
            EXPECT_EMPTY(dirTimeStamp(mRoot / "hello.txt"));

            // touch dir
            EXPECT_FALSE(touchFile(mRoot / "A"));

            // touch non-exist but parent exists
            EXPECT_TRUE(touchFile(mRoot / "blank.txt"));

            // touch non-exist with no parent
            EXPECT_FALSE(touchFile(mRoot / "blank" / "blank.txt"));

            // touch
            EXPECT_TRUE(touchFile(mRoot / "hello.txt"));
            auto hello2 = base + std::chrono::seconds(mSys.mMainDrive.mDirectory->mFiles.find("hello.txt")->second->mTimeStamp);
            EXPECT_NEQ(hello, hello2, "Did not update file timestamp on touch");
        }

        TEST(TestFS_Delete) {
            makeTestFile(mRoot / "hello.txt", "");
            makeTestDir(mRoot / "A");
            TestFileSystem::MainMount expected = makeExpected();
            addFile(expected.mDirectory, "hello.txt", "");
            addDir(expected.mDirectory, "A");
            validate(expected);

            // wrong type
            ASSERT_EQ(FsResult::Error, deleteFile(mRoot / "A"));
            ASSERT_EQ(FsResult::Error, deleteDir(mRoot / "hello.txt"));

            // doesnt exist
            ASSERT_EQ(FsResult::NoActionTaken, deleteFile(mRoot / "foo.txt"));
            ASSERT_EQ(FsResult::NoActionTaken, deleteDir(mRoot / "B"));

            // delete
            ASSERT_EQ(FsResult::Success, deleteFile(mRoot / "hello.txt"));
            expected.mDirectory->mFiles.clear();
            validate(expected);

            ASSERT_EQ(FsResult::Success, deleteDir(mRoot / "A"));
            expected.mDirectory->mDirs.erase("A");
            validate(expected);
        }

        TEST(TestFS_StatIsAndEmpty) {
            Path p1 = "/";
            Path p2 = "../";
            EXPECT_NEMPTY(statPartition(p1));
            EXPECT_EMPTY(statPartition(p2));

            makeTestFile(mRoot / "emptyFile.txt", "");
            makeTestDir(mRoot / "emptyDir");
            makeTestDir(mRoot / "A");
            makeTestFile(mRoot / "A" / "file.txt", "asdf");

            // is_ doesnt exist
            EXPECT_FALSE(isFile(mRoot / "blank.txt"));
            EXPECT_FALSE(isDir(mRoot / "B"));

            // is_ wrong type
            EXPECT_FALSE(isFile(mRoot / "A"));
            EXPECT_FALSE(isDir(mRoot / "emptyFile.txt"));

            // is_ does exist
            EXPECT_EQ(isFile(mRoot / "emptyFile.txt"), true);
            EXPECT_EQ(isDir(mRoot / "A"), true);

            // empty_ doesnt exist
            EXPECT_EMPTY(emptyFile(mRoot / "blank.txt"));
            EXPECT_EMPTY(emptyDir(mRoot / "B"));

            // empty_ wrong type
            EXPECT_EMPTY(emptyFile(mRoot / "A"));
            EXPECT_EMPTY(emptyDir(mRoot / "emptyFile.txt"));

            // empty_ does exist and is not empty
            EXPECT_EQ(emptyFile(mRoot / "A" / "file.txt"), false);
            EXPECT_EQ(emptyDir(mRoot / "A"), false);

            // empty_ does exist and is empty
            EXPECT_EQ(emptyFile(mRoot / "emptyFile.txt"), true);
            EXPECT_EQ(emptyDir(mRoot / "emptyDir"), true);
        }

        TEST(TestFS_RenameFile) {
            makeTestFile(mRoot / "hello.txt", "hello world");
            makeTestFile(mRoot / "over.txt", "gamer");
            makeTestFile(mRoot / "over2.txt", "gamer2");

            // straight rename
            ASSERT_EQ(FsResult::Success, renameFile(mRoot / "hello.txt", "hello2.txt", FileChangeOptions::None));
            TestFileSystem::MainMount expected = makeExpected();
            addFile(expected.mDirectory, "over.txt", "gamer");
            addFile(expected.mDirectory, "over2.txt", "gamer2");
            addFile(expected.mDirectory, "hello2.txt", "hello world");
            validate(expected);

            // dont overwrite
            ASSERT_EQ(FsResult::Error, renameFile(mRoot / "hello2.txt", "over.txt", FileChangeOptions::None));

            // dont overwrite if newer
            ASSERT_EQ(FsResult::Error, renameFile(mRoot / "over.txt", "hello2.txt", FileChangeOptions::None));

            // overwrite if newer
            ASSERT_TRUE(touchFile(mRoot / "hello2.txt"));
            ASSERT_EQ(FsResult::Success, renameFile(mRoot / "hello2.txt", "over.txt", FileChangeOptions::OverwriteIfNewer));
            expected.mDirectory->mFiles.clear();
            addFile(expected.mDirectory, "over.txt", "hello world");
            addFile(expected.mDirectory, "over2.txt", "gamer2");
            validate(expected);

            // overwrite
            ASSERT_EQ(FsResult::Success, renameFile(mRoot / "over2.txt", "over.txt", FileChangeOptions::Overwrite));
            expected.mDirectory->mFiles.clear();
            addFile(expected.mDirectory, "over.txt", "gamer2");
            validate(expected);

            // source doesnt exist
            ASSERT_EQ(FsResult::Error, renameFile(mRoot / "blank.txt", "thing.txt", FileChangeOptions::None));

            // source is not a file
            makeTestDir(mRoot / "A");
            ASSERT_EQ(FsResult::Error, renameFile(mRoot / "A", "B", FileChangeOptions::None));
        }

        TEST(TestFS_RenameDir) {
            makeTestDir(mRoot / "dirA");
            makeTestFile(mRoot / "dirA" / "hello.txt", "blah");
            TestFileSystem::MainMount expected = makeExpected();
            auto dir = addDir(expected.mDirectory, "dirA");
            addFile(dir, "hello.txt", "blah");

            // straight rename
            ASSERT_EQ(FsResult::Success, renameDir(mRoot / "dirA", "dirB", FileChangeOptions::None));
            expected.mDirectory->mDirs.erase("dirA");
            dir = addDir(expected.mDirectory, "dirB");
            addFile(dir, "hello.txt", "blah");
            validate(expected);

            // dont overwrite
            makeTestDir(mRoot / "dirC");
            makeTestFile(mRoot / "dirC" / "hello.txt", "blah");
            ASSERT_EQ(FsResult::Error, renameDir(mRoot / "dirB", "dirC", FileChangeOptions::None));

            // dont overwrite if newer
            ASSERT_EQ(FsResult::NoActionTaken, renameDir(mRoot / "dirB", "dirC", FileChangeOptions::OverwriteIfNewer));

            // overwrite if newer
            ASSERT_EQ(FsResult::Success, renameDir(mRoot / "dirC", "dirB", FileChangeOptions::OverwriteIfNewer));
            expected = makeExpected();
            dir = addDir(expected.mDirectory, "dirB");
            addFile(dir, "hello.txt", "blah");
            validate(expected);

            // overwrite
            makeTestDir(mRoot / "dirD");
            makeTestFile(mRoot / "dirD" / "file.txt", "");
            ASSERT_EQ(FsResult::Success, renameDir(mRoot / "dirB", "dirD", FileChangeOptions::Overwrite));
            expected = makeExpected();
            dir = addDir(expected.mDirectory, "dirD");
            addFile(dir, "hello.txt", "blah");
            validate(expected);

            // source doesnt exist
            ASSERT_EQ(FsResult::Error, renameDir(mRoot / "blank", "dirD", FileChangeOptions::None));

            // source is a file
            makeTestFile(mRoot / "file.txt", "blank");
            addFile(expected.mDirectory, "file.txt", "blank");
            ASSERT_EQ(FsResult::Error, renameDir(mRoot / "file.txt", "dirD", FileChangeOptions::None));
        }

        TEST(TestFS_MoveFile) {
            makeTestFile(mRoot / "hello.txt", "hello world");
            makeTestFile(mRoot / "over.txt", "gamer");
            TestFileSystem::MainMount expected = makeExpected();
            addFile(expected.mDirectory, "hello.txt", "hello world");
            addFile(expected.mDirectory, "over.txt", "gamer");

            // straight move
            ASSERT_EQ(FsResult::Success, moveFile(mRoot / "hello.txt", mRoot / "hello2.txt", FileChangeOptions::None));
            addFile(expected.mDirectory, "hello2.txt", "hello world");
            expected.mDirectory->mFiles.erase("hello.txt");
            validate(expected);

            // dont overwrite
            ASSERT_EQ(FsResult::Error, moveFile(mRoot / "hello2.txt", mRoot / "over.txt", FileChangeOptions::None));

            // dont overwrite if newer
            ASSERT_TRUE(touchFile(mRoot / "hello2.txt"));
            ASSERT_EQ(FsResult::NoActionTaken, moveFile(mRoot / "over.txt", mRoot / "hello2.txt", FileChangeOptions::OverwriteIfNewer));

            // overwrite if newer
            ASSERT_EQ(FsResult::Success, moveFile(mRoot / "hello2.txt", mRoot / "over.txt", FileChangeOptions::OverwriteIfNewer));
            expected.mDirectory->mFiles.erase("over.txt");
            expected.mDirectory->mFiles.erase("hello2.txt");
            addFile(expected.mDirectory, "over.txt", "hello world");
            validate(expected);

            // overwrite
            makeTestFile(mRoot / "over2.txt", "gamer2");
            ASSERT_EQ(FsResult::Success, moveFile(mRoot / "over.txt", mRoot / "over2.txt", FileChangeOptions::Overwrite));
            expected.mDirectory->mFiles.erase("over.txt");
            addFile(expected.mDirectory, "over2.txt", "hello world");
            validate(expected);

            // source doesnt exist
            ASSERT_EQ(FsResult::Error, moveFile(mRoot / "blah.txt", mRoot / "hello.txt", FileChangeOptions::None));

            // source is not a file
            ASSERT_EQ(FsResult::Error, moveFile(mRoot / "hello", mRoot / "hello2.txt", FileChangeOptions::None));

            // destination doesnt exist
            ASSERT_EQ(FsResult::Success, moveFile(mRoot / "over2.txt", mRoot / "hello3.txt", FileChangeOptions::None));
            expected.mDirectory->mFiles.erase("over2.txt");
            addFile(expected.mDirectory, "hello3.txt", "hello world");
            validate(expected);

            // destination is a directory (that exists)
            makeTestDir(mRoot / "dirA");
            ASSERT_EQ(FsResult::Success, moveFile(mRoot / "hello3.txt", mRoot / "dirA", FileChangeOptions::None));
            auto dirA = addDir(expected.mDirectory, "dirA");
            addFile(dirA, "hello3.txt", "hello world");
            expected.mDirectory->mFiles.erase("hello3.txt");
            validate(expected);

            // destination is a directory that doesnt exist
            ASSERT_EQ(FsResult::Error, moveFile(mRoot / "dirA" / "hello3.txt", mRoot / "dirB", FileChangeOptions::None));

            // destination is a directory that doesnt exist and create flag
            ASSERT_EQ(FsResult::Success, moveFile(mRoot / "dirA" / "hello3.txt", mRoot / "dirB", FileChangeOptions::MakeDirDest));
            auto dirB = addDir(expected.mDirectory, "dirB");
            addFile(dirB, "hello3.txt", "hello world");
            dirA->mFiles.clear();
            validate(expected);
        }

        TEST(TestFS_MoveDir) {
            makeTestDir(mRoot / "dirA");
            makeTestFile(mRoot / "dirA" / "hello.txt", "blah");
            TestFileSystem::MainMount expected = makeExpected();
            auto dir = addDir(expected.mDirectory, "dirA");
            addFile(dir, "hello.txt", "blah");

            // straight rename
            ASSERT_EQ(FsResult::Success, moveDir(mRoot / "dirA", mRoot / "dirB", FileChangeOptions::None));
            expected.mDirectory->mDirs.erase("dirA");
            dir = addDir(expected.mDirectory, "dirB");
            addFile(dir, "hello.txt", "blah");
            validate(expected);

            // dont overwrite
            makeTestDir(mRoot / "dirC");
            makeTestFile(mRoot / "dirC" / "hello.txt", "blah");
            ASSERT_EQ(FsResult::Error, moveDir(mRoot / "dirB", mRoot / "dirC", FileChangeOptions::None));

            // dont overwrite if newer
            ASSERT_EQ(FsResult::NoActionTaken, moveDir(mRoot / "dirB", mRoot / "dirC", FileChangeOptions::OverwriteIfNewer));

            // overwrite if newer
            ASSERT_EQ(FsResult::Success, moveDir(mRoot / "dirC", mRoot / "dirB", FileChangeOptions::OverwriteIfNewer));
            validate(expected);

            // overwrite
            makeTestDir(mRoot / "dirD");
            makeTestFile(mRoot / "dirD" / "file.txt", "");
            ASSERT_EQ(FsResult::Success, moveDir(mRoot / "dirB", mRoot / "dirD", FileChangeOptions::Overwrite));
            expected.mDirectory->mDirs.erase("dirB");
            dir = addDir(expected.mDirectory, "dirD");
            addFile(dir, "hello.txt", "blah");
            validate(expected);

            // source doesnt exist
            ASSERT_EQ(FsResult::Error, moveDir(mRoot / "blank", mRoot / "dirD", FileChangeOptions::None));

            // source is a file
            makeTestFile(mRoot / "file.txt", "blank");
            addFile(expected.mDirectory, "file.txt", "blank");
            ASSERT_EQ(FsResult::Error, moveDir(mRoot / "file.txt", mRoot / "dirD", FileChangeOptions::None));

            // destination doesnt exist (parent does)
            makeTestDir(mRoot / "dirC");
            ASSERT_EQ(FsResult::Success, moveDir(mRoot / "dirD", mRoot / "dirC" / "childDir", FileChangeOptions::None));
            expected.mDirectory->mDirs.erase("dirD");
            auto dirC = addDir(expected.mDirectory, "dirC");
            auto C_childDir = addDir(dirC, "childDir");
            addFile(C_childDir, "hello.txt", "blah");
            validate(expected);

            // destination doesnt exist (parent doesnt either)
            ASSERT_EQ(FsResult::Error, moveDir(mRoot / "dirC", mRoot / "dirA" / "childDir", FileChangeOptions::None));

            // destination is a file
            ASSERT_EQ(FsResult::Error, moveDir(mRoot / "dirC", mRoot / "file.txt", FileChangeOptions::None));

            // destination doesnt exist but create flag set
            ASSERT_EQ(FsResult::Success, moveDir(mRoot / "dirC", mRoot / "dirD" / "childDir", FileChangeOptions::MakeDirDest));
            expected.mDirectory->mDirs.erase("dirC");
            auto dirD = addDir(expected.mDirectory, "dirD");
            auto D_childDir = addDir(dirD, "childDir");
            auto D_childDir2 = addDir(D_childDir, "childDir");
            addFile(D_childDir2, "hello.txt", "blah");
            validate(expected);

            // recursively copy
            makeTestDir(mRoot / "A");
            makeTestDir(mRoot / "A" / "B");
            makeTestDir(mRoot / "A" / "B" / "C");
            makeTestDir(mRoot / "A" / "B" / "C" / "D");
            makeTestFile(mRoot / "A" / "file.txt", "a");
            makeTestFile(mRoot / "A" / "B" / "file.txt", "b");
            makeTestFile(mRoot / "A" / "B" / "C" / "file.txt", "c");
            makeTestFile(mRoot / "A" / "B" / "C" / "D" / "file.txt", "d");
            auto A = addDir(expected.mDirectory, "movedA");
            auto B = addDir(A, "B");
            auto C = addDir(B, "C");
            auto D = addDir(C, "D");
            addFile(A, "file.txt", "a");
            addFile(B, "file.txt", "b");
            addFile(C, "file.txt", "c");
            addFile(D, "file.txt", "d");
            ASSERT_EQ(FsResult::Success, moveDir(mRoot / "A", mRoot / "movedA", FileChangeOptions::MakeDirDest));
            validate(expected);
        }

        TEST(TestFS_Iterate) {
            auto same = [](const std::vector<Path>& a, const std::vector<Path>& b) {
                for (auto itA : a) {
                    bool found = false;
                    for (auto itB : b) {
                        if (itA == itB) {
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        EXPECT_FAIL("Wasnt found");
                        return;
                    }
                }
                };

            makeTestDir(mRoot / "A");
            makeTestDir(mRoot / "A" / "B");
            makeTestDir(mRoot / "A" / "B" / "C");
            makeTestDir(mRoot / "A" / "B" / "C" / "D");
            makeTestFile(mRoot / "A" / "aFile.txt", "a");
            makeTestFile(mRoot / "A" / "B" / "bFile.txt", "b");
            makeTestFile(mRoot / "A" / "B" / "C" / "cFile.txt", "c");
            makeTestFile(mRoot / "A" / "B" / "C" / "D" / "dFile.txt", "d");
            makeTestFile(mRoot / "A" / "B" / "C" / "D" / "dFile2.txt", "d2");

            // normal
            std::vector<Path> DPaths = {
                mRoot / "A" / "B" / "C" / "D" / "dFile2.txt",
                mRoot / "A" / "B" / "C" / "D" / "dFile.txt"
            };
            auto localIt = iterate(mRoot / "A" / "B" / "C" / "D", false);
            ASSERT_TRUE(localIt.valid());
            same(localIt.paths(), DPaths);

            std::vector<Path> DPathsAct;
            for (auto it : localIt) {
                DPathsAct.emplace_back(it);
            }
            same(DPathsAct, DPaths);

            // recursive
            std::vector<Path> entirePaths = {
                mRoot / "A" / "B",
                mRoot / "A" / "B" / "C",
                mRoot / "A" / "B" / "C" / "D",
                mRoot / "A" / "aFile.txt",
                mRoot / "A" / "B" / "bFile.txt",
                mRoot / "A" / "B" / "C" / "cFile.txt",
                mRoot / "A" / "B" / "C" / "D" / "dFile.txt",
                mRoot / "A" / "B" / "C" / "D" / "dFile2.txt"
            };
            auto recIt = iterate(mRoot / "A" / "B" / "C" / "D", true);
            ASSERT_TRUE(recIt.valid());
            same(recIt.paths(), entirePaths);

            std::vector<Path> entirePathsAct;
            for (auto it : recIt) {
                entirePathsAct.emplace_back(it);
            }
            same(entirePathsAct, entirePaths);
        }

        void _strToVec(const std::string& data, std::vector<char>& ret) {
            for (auto&& c : data) {
                ret.emplace_back(c);
            }
        }

        void _validateFileContents(const Path& path, const std::vector<char>& contents) {
            std::vector<char> data;
            auto dir = mSys._tryGetOrMakeDirectory(path);
            ASSERT_NOTNULL(dir);
            auto it = dir->mFiles.find(path.filename().value());
            ASSERT_NEQ(it, dir->mFiles.end());
            data.reserve(it->second->mData.size());
            for (auto&& c : it->second->mData) {
                data.emplace_back(c);
            }
            ASSERT_EQ(data, contents);
        }

        void _testWriteFile(std::unique_ptr<WriteFileStream>&& file) {
            ASSERT_NOTNULL(file);

            // isOpen()
            ASSERT_TRUE(file->isOpen());

            // fileSize() and push()
            ASSERT_EQ(file->fileSize(), 0);
            ASSERT_TRUE(file->push('a'));
            ASSERT_EQ(file->fileSize(), 1);
            for (int i = 0; i < 9; ++i) {
                ASSERT_TRUE(file->push('a'));
            }
            ASSERT_EQ(file->fileSize(), 10);

            // setCursor() and getCursor()
            ASSERT_TRUE(file->setCursor(5));
            ASSERT_EQ(file->getCursor(), 5);

            // setCursorBegin()
            ASSERT_TRUE(file->setCursorBegin());
            ASSERT_EQ(file->getCursor(), 0);

            // setCursorEnd()
            ASSERT_TRUE(file->setCursorEnd());
            ASSERT_EQ(file->getCursor(), 10);

            // flush()
            ASSERT_TRUE(file->flush());

            // close() and retest closed
            ASSERT_TRUE(file->close());
            ASSERT_FALSE(file->isOpen());
            ASSERT_EMPTY(file->fileSize());
            ASSERT_FALSE(file->push('a'));
            ASSERT_FALSE(file->setCursor(5));
            ASSERT_EMPTY(file->getCursor());
            ASSERT_FALSE(file->setCursorBegin());
            ASSERT_FALSE(file->setCursorEnd());
            ASSERT_FALSE(file->flush());
            ASSERT_TRUE(file->close());
        }

        TEST(TestFS_WriteCharFile) {
            _testWriteFile(openTextFileWriter(mRoot / "generaltest.txt"));

            // writeLine()
            {
                std::unique_ptr<WriteCharFileStream> writer = openTextFileWriter(mRoot / "spectest.txt");
                ASSERT_NOTNULL(writer);
                std::vector<char> validate;
                EXPECT_TRUE(writer->writeLine("testLine"));
                _strToVec("testLine\n", validate);
                EXPECT_TRUE(writer->writeLine("testLine\n\r"));
                _strToVec("testLine\n\r", validate);
                EXPECT_TRUE(writer->writeLine("testLine\n"));
                _strToVec("testLine\n", validate);
                EXPECT_TRUE(writer->flush());
                _validateFileContents(mRoot / "spectest.txt", validate);
            }

            // write()
            {
                std::unique_ptr<WriteCharFileStream> writer = openTextFileWriter(mRoot / "spectest2.txt");
                ASSERT_NOTNULL(writer);
                std::vector<char> validate;
                EXPECT_TRUE(writer->write("testLine"));
                _strToVec("testLine", validate);
                EXPECT_TRUE(writer->write("testLine\n\r"));
                _strToVec("testLine\n\r", validate);
                EXPECT_TRUE(writer->write("testLine\n"));
                _strToVec("testLine\n", validate);
                EXPECT_TRUE(writer->flush());
                _validateFileContents(mRoot / "spectest2.txt", validate);
            }
        }

        TEST(TestFS_WriteByteFile) {
            _testWriteFile(openBinaryFileWriter(mRoot / "generaltest.exe"));

            // write()
            {
                std::unique_ptr<WriteByteFileStream> writer = openBinaryFileWriter(mRoot / "spectest.exe");
                ASSERT_NOTNULL(writer);
                std::vector<char> validate;
                EXPECT_TRUE(writer->write(std::vector<char>{'t', 'e', 's', 't'}));
                _strToVec("test", validate);
                EXPECT_TRUE(writer->write(std::vector<char>{'\0', '\n', '\t', 'd'}));
                validate.emplace_back('\0');
                _strToVec("\n\td", validate);
                EXPECT_TRUE(writer->flush());
                _validateFileContents(mRoot / "spectest.exe", validate);
            }
        }

        const char* loremipsum = "Lorem ipsum dolor sit amet, consectetur adipiscing elit,\nsed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident,\nsunt in culpa qui officia deserunt mollit anim id est laborum.";
        const char* loremipsumL1 = "Lorem ipsum dolor sit amet, consectetur adipiscing elit,";
        const char* loremipsumL2 = "sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident,";
        const char* loremipsumL3 = "sunt in culpa qui officia deserunt mollit anim id est laborum.";
        void _makeFileForReading(const util::fs::Path& path) {
            makeTestFile(path, loremipsum);
        }

        void _testReadFile(std::unique_ptr<ReadFileStream>&& file) {
            ASSERT_NOTNULL(file);

            // isOpen()
            ASSERT_TRUE(file->isOpen());

            // fileSize()
            ASSERT_EQ(file->fileSize(), 445);

            // setCursorEnd() and getCursor()
            ASSERT_TRUE(file->setCursorEnd());
            ASSERT_EQ(file->getCursor(), 445);

            // setCursorBegin()
            ASSERT_TRUE(file->setCursorBegin());
            ASSERT_EQ(file->getCursor(), 0);

            // setCursor()
            ASSERT_TRUE(file->setCursor(5));
            ASSERT_EQ(file->getCursor(), 5);

            // peek()
            ASSERT_EQ(file->peek(), ' ');
            ASSERT_EQ(file->getCursor(), 5);

            // consume()
            ASSERT_EQ(file->consume(), ' ');
            ASSERT_EQ(file->getCursor(), 6);

            // reverse()
            ASSERT_EQ(file->reverse(3), 3);
            ASSERT_EQ(file->getCursor(), 3);

            // readUntil()
            ASSERT_TRUE(file->readUntil(' '));
            ASSERT_EQ(file->getCursor(), 5);

            // readSome()
            std::vector<char> buffer;
            ASSERT_EQ(file->readSome(6, buffer), 0);
            buffer.resize(6);
            ASSERT_EQ(file->readSome(0, buffer), 0);
            ASSERT_EQ(file->readSome(6, buffer), 6);
            std::vector<char> expectedBuffer = { ' ', 'i', 'p', 's', 'u', 'm' };
            ASSERT_EQ(expectedBuffer, buffer);

            // close()
            ASSERT_TRUE(file->close());
            ASSERT_FALSE(file->isOpen());
            ASSERT_EMPTY(file->fileSize());
            ASSERT_EMPTY(file->peek());
            ASSERT_EQ(file->reverse(), 0);
            ASSERT_FALSE(file->readUntil(' '));
            ASSERT_EQ(file->readSome(6, buffer), 0);
            ASSERT_FALSE(file->setCursor(5));
            ASSERT_EMPTY(file->getCursor());
            ASSERT_FALSE(file->setCursorBegin());
            ASSERT_FALSE(file->setCursorEnd());
            ASSERT_TRUE(file->close());
        }

        TEST(TestFS_ReadCharFile) {
            _makeFileForReading(mRoot / "test.txt");
            _testReadFile(openTextFileReader(mRoot / "test.txt"));

            // readEntireFileToMemory()
            {
                std::unique_ptr<ReadCharFileStream> reader = openTextFileReader(mRoot / "test.txt");
                EXPECT_EQ(reader->readEntireFileToMemory(), std::string(loremipsum));
            }

            // readLine()
            {
                std::unique_ptr<ReadCharFileStream> reader = openTextFileReader(mRoot / "test.txt");
                EXPECT_EQ(reader->readLine(), loremipsumL1);
                EXPECT_EQ(reader->readLine(), loremipsumL2);
                EXPECT_EQ(reader->readLine(), loremipsumL3);
            }

            // read()
            {
                {
                    std::unique_ptr<ReadCharFileStream> reader = openTextFileReader(mRoot / "test.txt");
                    EXPECT_EQ(reader->read(16), "Lorem ipsum dolo");
                }
            }
        }

        TEST(TestFS_ReadByteFile) {
            _makeFileForReading(mRoot / "test.txt");
            _testReadFile(openBinaryFileReader(mRoot / "test.txt"));

            // readEntireFileToMemory()
            {
                std::unique_ptr<ReadByteFileStream> reader = openBinaryFileReader(mRoot / "test.txt");
                std::vector<char> buf;
                _strToVec(loremipsum, buf);
                EXPECT_EQ(reader->readEntireFileToMemory(), buf);
            }

            // readSome(size_t)
            {
                std::unique_ptr<ReadByteFileStream> reader = openBinaryFileReader(mRoot / "test.txt");
                std::vector<char> buf;
                _strToVec("Lorem ipsum dolo", buf);
                EXPECT_EQ(reader->read(16), buf);
            }
        }
    };
}
