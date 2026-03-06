
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#include <snowlib/src/SourceVault.hpp>
#include <testInfra/TestInfra.hpp>

namespace test {
    TEST_CLASS(SourceVaultTests, "snowlib", TestInfra::Tags::Ignore) {
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

        void _lineCol(src::SourceVault& v, uint32_t pos, uint32_t l, uint32_t c) {
            auto [line, col] = v.getLineCol({ 1, pos });
            EXPECT_EQ(line, l);
            EXPECT_EQ(col, c);
        }

        TEST(SourceVault_PosEmptyFile_Works) {
            src::SourceVault v;
            std::optional<src::FileID> id = v.loadMemory("");
            ASSERT_EQ(1, id);
            _lineCol(v, 0, 1, 0);
        }

        void _lineColForLine(src::SourceVault& v, uint32_t lineNum, uint32_t beginOfLine, uint32_t endOfLine) {
            // begin
            _lineCol(v, beginOfLine, lineNum, 1);

            // end
            _lineCol(v, endOfLine, lineNum, endOfLine - beginOfLine + 1);

            // middle
            uint32_t x = ((endOfLine - beginOfLine) / 2);
            _lineCol(v, beginOfLine + x, lineNum, x + 1);
        }

        TEST(SourceVault_PosOneLineFile_Works) {
            src::SourceVault v;
            std::optional<src::FileID> id = v.loadMemory("12345");
            ASSERT_EQ(1, id);
            _lineColForLine(v, 1, 0, 4);
        }

        TEST(SourceVault_PosTwoLineFile_WorksOnBothLines) {
            src::SourceVault v;                        // 012345 67890
            std::optional<src::FileID> id = v.loadMemory("12345\n67890");
            ASSERT_EQ(1, id);
            _lineColForLine(v, 1, 0, 5);
            _lineColForLine(v, 2, 6, 10);
        }

        TEST(SourceVault_PosThreeLineFile_WorksOnAllLines) {
            src::SourceVault v;                        // 012345 678901 23456
            std::optional<src::FileID> id = v.loadMemory("12345\n67890\nabcde");
            ASSERT_EQ(1, id);
            _lineColForLine(v, 1, 0, 5);
            _lineColForLine(v, 2, 6, 11);
            _lineColForLine(v, 3, 12, 16);
        }

        TEST(SourceVault_PosMultiLineFile_WorksOnLastLine) {
            src::SourceVault v;                        // 012345 678901 234567 89012
            std::optional<src::FileID> id = v.loadMemory("12345\n67890\nabcde\nfghik");
            ASSERT_EQ(1, id);
            _lineColForLine(v, 1, 0, 5);
            _lineColForLine(v, 2, 6, 11);
            _lineColForLine(v, 3, 12, 17);
            _lineColForLine(v, 4, 18, 22);
        }

        TEST(SourceVault_PosStress_Works) {
            std::string fileData;
            for (size_t i = 0; i < 50; ++i) {
                //           01234567890123456789
                fileData += "0123456789abcdefghi\n";
            }

            src::SourceVault v;
            std::optional<src::FileID> id = v.loadMemory(fileData);
            ASSERT_EQ(1, id);
            for (size_t i = 0; i < 49; ++i) {
                _lineColForLine(v, i + 1, (i * 20), ((i + 1) * 20));
            }
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

        TEST(SourceVault_GetLineAt_GetNewLinePosition_ReturnsLineItsOn) {
            src::SourceVault v;
            std::optional<src::FileID> id = v.loadMemory("1234\n5678");
            ASSERT_EQ(1, id);
            std::string_view line = v.getLineAt({ *id, 4 });
            EXPECT_EQ(line, "1234");
        }


        // then errors


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
