
// Copyright (c) 2025 Andrew Griffin - All Rights Reserved

#include <testInfra/TestInfra.hpp>

#include <snowlib/diag/Diag.hpp>
#include <snowlib/file/SnowFile.hpp>
#include <util/cmd/manip.hpp>

namespace test {
    TEST_CLASS(PrinterTests, "snowlib", "diag") {
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
            b.textColorBrightBlue("Internal").text(": ").textColorBrightYellow("T0996")
                .text(": Test warning");

            log->expectOneMessageWhichContains(b.str());
        }

        TEST(Diag_WarnFile_PrintsOkay) {
            auto f = file::SnowFile::FromString("asdf");
            diag::make(diag::Type::TEST_Warn, f);
            auto log = Log::testOnly_disableTestEndPoint();

            auto b = util::cmd::manip::Builder();
            b.textColorBrightBlue("In Memory").text(": ").textColorBrightYellow("T0996")
                .text(": Test warning");

            log->expectOneMessageWhichContains(b.str());
        }

        TEST(Diag_WarnLoc_PrintsOkay) {
            auto f = file::SnowFile::FromString("asdf");
            auto l = file::Loc(f, 1, 2, f.getFullContents().c_str() + 1);
            diag::make(diag::Type::TEST_Warn, l);
            auto log = Log::testOnly_disableTestEndPoint();

            auto b = util::cmd::manip::Builder();
            b.textColorBrightBlue("In Memory (1|2)").text(": ").textColorBrightYellow("T0996")
                .text(": Test warning").reset().text("\n")
                .textColorDarkGrey("0001 |").text(" asdf\n")
                .text("     |  ").textColorBrightGreen("^");

            log->expectOneMessageWhichContains(b.str());
        }

        TEST(Diag_WarnLocRange_PrintsOkay) {
            auto f = file::SnowFile::FromString("asdf");
            auto l = file::LocRange(f, 1, 2, f.getFullContents().c_str() + 1, 2);
            diag::make(diag::Type::TEST_Warn, l);
            auto log = Log::testOnly_disableTestEndPoint();

            auto b = util::cmd::manip::Builder();
            b.textColorBrightBlue("In Memory (1|2)").text(": ").textColorBrightYellow("T0996")
                .text(": Test warning").reset().text("\n")
                .textColorDarkGrey("0001 |").text(" asdf\n")
                .text("     |  ").textColorBrightGreen("^").textColorBrightMagenta("~");

            log->expectOneMessageWhichContains(b.str());
        }
    };
}
