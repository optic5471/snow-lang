
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#include <snowlib/src/SourceVault.hpp>
#include <testInfra/TestInfra.hpp>

namespace test {
    TEST_CLASS(SourceVaultTests, "snowlib") {
        // getLineCol, getLineAt, getLexeme: beyond file length and missing file errors
        // getLineCol, getLineAt: empty files, no new lines, after last newline
        // getLineAt: gets the line in question with no new line characters
        //      what about /r?
        // getLineCol: on newline, imbetween new lines
        // getLexeme: return the value
        // loadMemory: correct ID, correct new line table

        // Also tests that generating new lines for a file without them works as expected
        TEST(SourceVault_LoadMemory_Functions) {
            src::SourceVault v;
            std::optional<src::FileID> id = v.loadMemory("1234");
            ASSERT_EQ(1, id);
            const src::FileEntry* file = v.tryGetFileData(id.value());
            ASSERT_NOTNULL(file);
            EXPECT_EQ(file->mID, 1);
            EXPECT_EQ(file->mContents, "1234");
            EXPECT_EQ(file->mPath, util::fs::Path("In Memory"));
            EXPECT_TRUE(file->mNewLineTable.empty());
        }

        TEST(SourceVault_FileWithNewlines_GeneratesNewLineTable) {
            src::SourceVault v;
                                                       // 0 1 23456 7 8901234 567890123456 78
            std::optional<src::FileID> id = v.loadMemory("\n\n1234\n\n123456\n12345678980\n12");
            ASSERT_EQ(1, id);
            const src::FileEntry* file = v.tryGetFileData(id.value());
            ASSERT_NOTNULL(file);
            std::vector<uint32_t> expectedNewLineTable = {
                0, 1, 6, 7, 14, 26
            };
            EXPECT_EQ(file->mNewLineTable, expectedNewLineTable);
        }

        TEST(SourceVault_FileWithOnlyNewLines_GeneratesNewLineTable) {
            src::SourceVault v;
            std::optional<src::FileID> id = v.loadMemory("\n\n");
            ASSERT_EQ(1, id);
            const src::FileEntry* file = v.tryGetFileData(id.value());
            ASSERT_NOTNULL(file);
            std::vector<uint32_t> expectedNewLineTable = {
                0, 1
            };
            EXPECT_EQ(file->mNewLineTable, expectedNewLineTable);
        }

        TEST(SourceVault_FileEndingInNewLine_GeneratesNewLineTable) {
            src::SourceVault v;
            std::optional<src::FileID> id = v.loadMemory("\n12\n");
            ASSERT_EQ(1, id);
            const src::FileEntry* file = v.tryGetFileData(id.value());
            ASSERT_NOTNULL(file);
            std::vector<uint32_t> expectedNewLineTable = {
                0, 3
            };
            EXPECT_EQ(file->mNewLineTable, expectedNewLineTable);
        }

        TEST(SourceVault_GetLexeme_StartOfFile_Works) {
            src::SourceVault v;
            std::optional<src::FileID> id = v.loadMemory("1234\n1234");
            ASSERT_EQ(1, id);
            std::string_view lexeme = v.getLexeme({ *id, 0, 2 });
            EXPECT_EQ(lexeme, "12");
        }

        TEST(SourceVault_GetLexeme_EndOfFile_Works) {
            src::SourceVault v;
            std::optional<src::FileID> id = v.loadMemory("1234\n1234");
            ASSERT_EQ(1, id);
            std::string_view lexeme = v.getLexeme({ *id, 7, 2 });
            EXPECT_EQ(lexeme, "34");
        }

        TEST(SourceVault_GetLexeme_MiddleOfFile_Works) {
            src::SourceVault v;
            std::optional<src::FileID> id = v.loadMemory("1234\n1234");
            ASSERT_EQ(1, id);
            std::string_view lexeme = v.getLexeme({ *id, 2, 2 });
            EXPECT_EQ(lexeme, "34");
        }

        TEST(SourceVault_GetLexeme_SpansLines_Works) {
            src::SourceVault v;
            std::optional<src::FileID> id = v.loadMemory("1234\n1234");
            ASSERT_EQ(1, id);
            std::string_view lexeme = v.getLexeme({ *id, 2, 5 });
            EXPECT_EQ(lexeme, "34\n12");
        }

        TEST(SourceVault_GetLineAt_Index0_Works) {
            src::SourceVault v;
            std::optional<src::FileID> id = v.loadMemory("1234\n1234");
            ASSERT_EQ(1, id);
            std::string_view line = v.getLineAt({ *id, 0 });
            EXPECT_EQ(line, "1234");
        }

        TEST(SourceVault_GetLineAt_FirstLine_Works) {
            src::SourceVault v;
            std::optional<src::FileID> id = v.loadMemory("1234\n1234");
            ASSERT_EQ(1, id);
            std::string_view line = v.getLineAt({ *id, 2 });
            EXPECT_EQ(line, "1234");
        }

        TEST(SourceVault_GetLineAt_MiddleOfFile_Works) {
            src::SourceVault v;
            std::optional<src::FileID> id = v.loadMemory("1234\n5678\n90");
            ASSERT_EQ(1, id);
            std::string_view line = v.getLineAt({ *id, 5 });
            EXPECT_EQ(line, "5678");
        }

        TEST(SourceVault_GetLineAt_LastLine_Works) {
            src::SourceVault v;
            std::optional<src::FileID> id = v.loadMemory("1234\n5678");
            ASSERT_EQ(1, id);
            std::string_view line = v.getLineAt({ *id, 5 });
            EXPECT_EQ(line, "5678");
        }

        TEST(SourceVault_GetLineAt_LastIndex_Works) {
            src::SourceVault v;
            std::optional<src::FileID> id = v.loadMemory("1234\n5678");
            ASSERT_EQ(1, id);
            std::string_view line = v.getLineAt({ *id, 8 });
            EXPECT_EQ(line, "5678");
        }

        TEST(SourceVault_GetLineAt_NoNewLines_ReturnsEntireFile) {
            src::SourceVault v;
            std::optional<src::FileID> id = v.loadMemory("12345678");
            ASSERT_EQ(1, id);
            std::string_view line = v.getLineAt({ *id, 5 });
            EXPECT_EQ(line, "12345678");
        }

        TEST(SourceVault_GetLineAt_OnlyNewLineOnLine_ReturnsEmptyString) {
            src::SourceVault v;
            std::optional<src::FileID> id = v.loadMemory("\n\n\n1234");
            ASSERT_EQ(1, id);
            std::string_view line = v.getLineAt({ *id, 1 });
            EXPECT_TRUE(line.empty());
        }

        TEST(SourceVault_GetLineAt_GetNewLinePosition_ReturnsNextLine) {
            src::SourceVault v;
            std::optional<src::FileID> id = v.loadMemory("1234\n5678");
            ASSERT_EQ(1, id);
            std::string_view line = v.getLineAt({ *id, 4 });
            EXPECT_EQ(line, "5678");
        }

        //TEST(SourceVault_SeekBeyondFileLength_Errors) {
        //    AssertHandler log;
        //    src::SourceVault v;
        //    std::optional<src::FileID> id = v.loadMemory("12\n34");
        //    ASSERT_NEMPTY(id);
        //    src::Loc l = { .mFileID = *id, .mOffset = 5 };
        //
        //    auto lineCol = v.getLineCol(l);
        //    auto emptyLineCol = std::tuple<uint32_t, uint32_t>(0, 0);
        //    EXPECT_EQ(lineCol, emptyLineCol);
        //
        //    EXPECT_TRUE(v.getLineAt(l).empty());
        //
        //    EXPECT_TRUE(v.getLexeme({ .mFileID = *id, .mOffset = 5, .mLength = 1 }).empty());
        //    EXPECT_TRUE(v.getLexeme({ .mFileID = *id, .mOffset = 4, .mLength = 1 }) == "4"); // fine
        //    EXPECT_TRUE(v.getLexeme({ .mFileID = *id, .mOffset = 4, .mLength = 2 }).empty());
        //
        //    log.expectXMessagesWhichContain({
        //        "Attempting to reach beyond the file's length!",
        //        "Attempting to reach beyond the file's length!",
        //        "Attempting to reach beyond the file's length!",
        //        "Attempting to reach beyond the file's length!",
        //    });
        //}
    };
}
