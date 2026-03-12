
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#include <testInfra/TestInfra.hpp>

#include <snowlib/diag/Diag.hpp>
#include <test/testHelpers/TestAssertHandler.hpp>
#include <util/cmd/manip.hpp>

namespace test {
    TEST_CLASS(PrinterTests, "snowlib", "diag", TestInfra::Tags::Ignore) {
        TEST_START() {
            Log::testOnly_enableTestEndPoint();
        }

        TEST_STOP() {
            Log::testOnly_forceDisableTestEndPoint();
        }

        TEST(Diag_TestCaptureWorks) {
            diag::internal::testOnly_beginCapture();
            diag::make(diag::Type::TEST_Suggestion);
            diag::make(diag::Type::TEST_Warn);
            auto cap = diag::internal::testOnly_endCapture();
            ASSERT_EQ(cap.size(), 2);
            EXPECT_EQ(cap[0], diag::Type::TEST_Suggestion);
            EXPECT_EQ(cap[1], diag::Type::TEST_Warn);
        }

        TEST(Diag_WarnNoFile_PrintsOkay) {
            diag::make(diag::Type::TEST_Warn);
            auto log = Log::testOnly_disableTestEndPoint();

            auto b = util::cmd::manip::Builder();
            b.textColorBrightBlue("Internal").text(": ").textColorBrightYellow("L0996")
                .text(": Test warning");

            log->expectOneMessageWhichContains(b.str());
        }

        TEST(Diag_WarnFile_PrintsOkay) {
            src::SourceVault v;
            auto f = v.loadMemory("asdf");
            diag::make(diag::Type::TEST_Warn, *f);
            auto log = Log::testOnly_disableTestEndPoint();

            auto b = util::cmd::manip::Builder();
            b.textColorBrightBlue("In Memory").text(": ").textColorBrightYellow("L0996")
                .text(": Test warning");

            log->expectOneMessageWhichContains(b.str());
        }

        TEST(Diag_WarnLoc_PrintsOkay) {
            src::SourceVault v;
            auto f = v.loadMemory("asdf");
            auto l = src::Loc{ *f, 1 };
            diag::make(diag::Type::TEST_Warn, l);
            auto log = Log::testOnly_disableTestEndPoint();

            auto b = util::cmd::manip::Builder();
            b.textColorBrightBlue("In Memory (1|2)").text(": ").textColorBrightYellow("L0996")
                .text(": Test warning").reset().text("\n")
                .textColorDarkGrey("   1 |").text(" asdf\n")
                .textColorDarkGrey("     | ").text(" ").textColorBrightGreen("^");

            log->expectOneMessageWhichContains(b.str());
        }

        TEST(Diag_WarnLocRange_PrintsOkay) {
            src::SourceVault v;
            auto f = v.loadMemory("asdf");
            auto l = src::LocRange{ *f, 1, 2 };
            diag::make(diag::Type::TEST_Warn, l);
            auto log = Log::testOnly_disableTestEndPoint();

            auto b = util::cmd::manip::Builder();
            b.textColorBrightBlue("In Memory (1|2)").text(": ").textColorBrightYellow("L0996")
                .text(": Test warning").reset().text("\n")
                .textColorDarkGrey("   1 |").text(" asdf\n")
                .textColorDarkGrey("     | ").text(" ").textColorBrightGreen("^").textColorBrightMagenta("~");

            log->expectOneMessageWhichContains(b.str());
        }

        TEST(Diag_Suggest_PrintsOkay) {
            diag::make(diag::Type::TEST_Suggestion);
            auto log = Log::testOnly_disableTestEndPoint();

            auto b = util::cmd::manip::Builder();
            b.textColorBrightBlue("Internal").text(": ").textColorBrightYellow("U0999")
                .text(": Suggest doing X");

            log->expectOneMessageWhichContains(b.str());
        }

        TEST(Diag_Unary_PrintsOkay) {
            src::SourceVault v;
            //               0123456789012
            std::string s = "unary p unary";
            auto f = v.loadMemory(s);
            auto l = src::Loc{ *f, 6 };

            diag::make(diag::Type::TEST_UnaryError, l, "thingy");
            auto log = Log::testOnly_disableTestEndPoint();

            auto b = util::cmd::manip::Builder();
            b.textColorBrightBlue("In Memory (1|7)").text(": ").textColorBrightRed("S0997")
                .text(": Test error with '").textColorBrightCyan("thingy").text("' value").reset().text("\n")
                .textColorDarkGrey("   1 |").text(" unary p unary\n")
                .textColorDarkGrey("     | ").text("      ").textColorBrightGreen("^");

            log->expectOneMessageWhichContains(b.str());
        }

        TEST(Diag_UnaryRangeBefore_PrintsOkay) {
            src::SourceVault v;
            //               0123456789012
            std::string s = "unary p unary";
            auto f = v.loadMemory(s);
            auto l = src::Loc(*f, 6);
            auto r = src::LocRange(*f, 0, 5);

            diag::make(diag::Type::TEST_UnaryError, l, r, "thingy");
            auto log = Log::testOnly_disableTestEndPoint();

            auto b = util::cmd::manip::Builder();
            b.textColorBrightBlue("In Memory (1|7)").text(": ").textColorBrightRed("S0997")
                .text(": Test error with '").textColorBrightCyan("thingy").text("' value").reset().text("\n")
                .textColorDarkGrey("   1 |").text(" unary p unary\n")
                .textColorDarkGrey("     | ");
            for (size_t i = 0; i < 5; ++i) {
                b.textColorBrightMagenta("~");
            }
            b.text(" ").textColorBrightGreen("^");

            log->expectOneMessageWhichContains(b.str());
        }

        TEST(Diag_UnaryRangeAfter_PrintsOkay) {
            src::SourceVault v;
            //               0123456789012
            std::string s = "unary p unary";
            auto f = v.loadMemory(s);
            auto l = src::Loc(*f, 6);
            auto r = src::LocRange(*f, 8, 5);

            diag::make(diag::Type::TEST_UnaryError, l, r, "thingy");
            auto log = Log::testOnly_disableTestEndPoint();

            auto b = util::cmd::manip::Builder();
            b.textColorBrightBlue("In Memory (1|7)").text(": ").textColorBrightRed("S0997")
                .text(": Test error with '").textColorBrightCyan("thingy").text("' value").reset().text("\n")
                .textColorDarkGrey("   1 |").text(" unary p unary\n")
                .textColorDarkGrey("     | ").text("      ").textColorBrightGreen("^").text(" ");
            for (size_t i = 0; i < 5; ++i) {
                b.textColorBrightMagenta("~");
            }

            log->expectOneMessageWhichContains(b.str());
        }

        TEST(Diag_UnaryRangeMix_PrintsOkay) {
            src::SourceVault v;
            //               0123456789012
            std::string s = "unary p unary";
            auto f = v.loadMemory(s);
            auto l = src::Loc(*f, 6);
            auto r = src::LocRange(*f, 3, 7);

            diag::make(diag::Type::TEST_UnaryError, l, r, "thingy");
            auto log = Log::testOnly_disableTestEndPoint();

            auto b = util::cmd::manip::Builder();
            b.textColorBrightBlue("In Memory (1|7)").text(": ").textColorBrightRed("S0997")
                .text(": Test error with '").textColorBrightCyan("thingy").text("' value").reset().text("\n")
                .textColorDarkGrey("   1 |").text(" unary p unary\n")
                .textColorDarkGrey("     | ").text("   ");
            for (size_t i = 0; i < 3; ++i) {
                b.textColorBrightMagenta("~");
            }
            b.textColorBrightGreen("^");
            for (size_t i = 0; i < 3; ++i) {
                b.textColorBrightMagenta("~");
            }

            log->expectOneMessageWhichContains(b.str());
        }

        TEST(Diag_UnaryRangeDual_PrintsOkay) {
            src::SourceVault v;
            //               0123456789012
            std::string s = "unary p unary";
            auto f = v.loadMemory(s);
            auto l = src::Loc(*f, 6);
            auto r1 = src::LocRange(*f, 0, 5);
            auto r2 = src::LocRange(*f, 8, 5);

            diag::make(diag::Type::TEST_UnaryError, l, r1, r2, "thingy");
            auto log = Log::testOnly_disableTestEndPoint();

            auto b = util::cmd::manip::Builder();
            b.textColorBrightBlue("In Memory (1|7)").text(": ").textColorBrightRed("S0997")
                .text(": Test error with '").textColorBrightCyan("thingy").text("' value").reset().text("\n")
                .textColorDarkGrey("   1 |").text(" unary p unary\n")
                .textColorDarkGrey("     | ");
            for (size_t i = 0; i < 5; ++i) {
                b.textColorBrightMagenta("~");
            }
            b.text(" ").textColorBrightGreen("^").text(" ");
            for (size_t i = 0; i < 5; ++i) {
                b.textColorBrightMagenta("~");
            }

            log->expectOneMessageWhichContains(b.str());
        }

        TEST(Diag_Binary_PrintsOkay) {
            diag::make(diag::Type::TEST_BinaryInfo, "foo", "bar");
            auto log = Log::testOnly_disableTestEndPoint();

            auto b = util::cmd::manip::Builder();
            b.textColorBrightBlue("Internal").text(": ").textColorBrightCyan("U0998")
                .text(": Test binary error of '").textColorBrightCyanOn()
                .textUnderline("foo").textColorDefault().text("' and '").textColorBrightRed("bar").text("'");

            log->expectOneMessageWhichContains(b.str());
        }

        // exactly the same test as Diag_WarnLocRange_PrintsOkay
        TEST(Diag_MultiLineFile_PrintsOkay) {
            src::SourceVault v;                      //    0123456789012345678 9 01234 5 67890
            auto f = v.loadMemory("when in the summer\r\nasdf\r\nI find myself wondering");
            auto l = src::LocRange(*f, 21, 2);
            diag::make(diag::Type::TEST_Warn, l);
            auto log = Log::testOnly_disableTestEndPoint();

            auto b = util::cmd::manip::Builder();
            b.textColorBrightBlue("In Memory (2|2)").text(": ").textColorBrightYellow("L0996")
                .text(": Test warning").reset().text("\n")
                .textColorDarkGrey("   2 |").text(" asdf\n")
                .textColorDarkGrey("     | ").text(" ").textColorBrightGreen("^").textColorBrightMagenta("~");

            log->expectOneMessageWhichContains(b.str());
        }

        TEST(Diag_ArgsThenRange_Fails) {
            src::SourceVault v;
            AssertHandler handler;
            std::string s = "unary p unary";
            auto f = v.loadMemory(s);
            auto l = src::Loc(*f, 6);
            auto r = src::LocRange(*f, 0, 5);

            diag::make(diag::Type::TEST_UnaryError, l, "thingy", r);
            handler.expectOneMessageWhichContains("Range was provided after arguments. Please define all ranges before arguments");
        }

        TEST(Diag_RangeArgRange_Fails) {
            src::SourceVault v;
            AssertHandler handler;
            std::string s = "unary p unary";
            auto f = v.loadMemory(s);
            auto r1 = src::LocRange(*f, 0, 5);
            auto r2 = src::LocRange(*f, 0, 5);

            diag::make(diag::Type::TEST_UnaryError, r1, "thingy", r2);
            handler.expectOneMessageWhichContains("Range was provided after arguments. Please define all ranges before arguments");
        }

        TEST(Diag_TooManyRanges_Fails) {
            src::SourceVault v;
            AssertHandler handler;
            std::string s = "unary p unary";
            auto f = v.loadMemory(s);
            auto r1 = src::LocRange(*f, 0, 5);
            auto r2 = src::LocRange(*f, 0, 5);
            auto r3 = src::LocRange(*f, 0, 5);

            diag::make(diag::Type::TEST_UnaryError, r1, r2, r3, "thingy");
            handler.expectOneMessageWhichContains("Too many ranges have been provided");
        }

        TEST(Diag_LocAndRangeDifferentLine_Fails) {
            src::SourceVault v;
            AssertHandler handler;
            std::string s = "unary p unary\nasdf";
            auto f = v.loadMemory(s);
            auto l = src::Loc(*f, 6);
            auto r1 = src::LocRange(*f, 14, 2);
            diag::make(diag::Type::TEST_UnaryError, l, r1, "Thingy");
            handler.expectOneMessageWhichContains("A diag provided with ranges must come from the same line, use supplemental diags for multi-line");
        }
        TEST(Diag_LocAndRange2DifferentLine_Fails) {
            src::SourceVault v;
            AssertHandler handler;
            std::string s = "unary p unary\nasdf";
            auto f = v.loadMemory(s);
            auto l = src::Loc(*f, 6);
            auto r1 = src::LocRange(*f, 0, 2);
            auto r2 = src::LocRange(*f, 14, 2);
            diag::make(diag::Type::TEST_UnaryError, l, r1, r2, "Thingy");
            handler.expectOneMessageWhichContains("A diag provided with ranges must come from the same line, use supplemental diags for multi-line");
        }

        TEST(Diag_LocAndRangeDifferentFile_Fails) {
            src::SourceVault v;
            AssertHandler handler;
            std::string s = "unary p unary";
            std::string s2 = "22222";
            auto f1 = v.loadMemory(s);
            auto f2 = v.loadMemory(s);
            auto l = src::Loc(*f1, 6);
            auto r1 = src::LocRange(*f2, 0, 2);
            diag::make(diag::Type::TEST_UnaryError, l, r1, "Thingy");
            handler.expectOneMessageWhichContains("A diag provided with ranges, must have ranges and loc come from the same file");
        }
        TEST(Diag_LocAndRange2DifferentFile_Fails) {
            src::SourceVault v;
            AssertHandler handler;
            std::string s = "unary p unary";
            std::string s2 = "22222";
            auto f1 = v.loadMemory(s);
            auto f2 = v.loadMemory(s);
            auto l = src::Loc(*f1, 6);
            auto r1 = src::LocRange(*f1, 0, 2);
            auto r2 = src::LocRange(*f2, 2, 2);
            diag::make(diag::Type::TEST_UnaryError, l, r1, r2, "Thingy");
            handler.expectOneMessageWhichContains("A diag provided with ranges, must have ranges and loc come from the same file");
        }

        TEST(Diag_FileAndRange_Fails) {
            src::SourceVault v;
            AssertHandler handler;
            std::string s = "unary p unary";
            auto f = v.loadMemory(s);
            auto r1 = src::LocRange(*f, 0, 2);
            diag::make(diag::Type::TEST_UnaryError, *f, r1, "Thingy");
            handler.expectOneMessageWhichContains("A diag provided with only a file cannot use ranges");
        }

        TEST(Diag_RangesWithDifferentLines_Fails) {
            src::SourceVault v;
            AssertHandler handler;
            std::string s = "unary p unary\nasdf";
            auto f = v.loadMemory(s);
            auto r1 = src::LocRange(*f, 0, 2);
            auto r2 = src::LocRange(*f, 14, 2);
            diag::make(diag::Type::TEST_UnaryError, r1, r2, "Thingy");
            handler.expectOneMessageWhichContains("A diag provided with ranges must come from the same line, use supplemental diags for multi-line");
        }

        TEST(Diag_RangesWithDifferentFiles_Fails) {
            src::SourceVault v;
            AssertHandler handler;
            std::string s = "unary p unary\nasdf";
            auto f1 = v.loadMemory(s);
            auto f2 = v.loadMemory(s);
            auto r1 = src::LocRange(*f1, 0, 2);
            auto r2 = src::LocRange(*f2, 0, 2);
            diag::make(diag::Type::TEST_UnaryError, r1, r2, "Thingy");
            handler.expectOneMessageWhichContains("A diag provided with ranges, must have both ranges come from the same file");
        }
    };
}
