
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#ifdef TEST_ENABLED

#include <testHelpers/TestLogHook.hpp>
#include <util/cmd/manip.hpp>
#include <util/Log.hpp>

namespace TestHelpers {
    TestLogHook::TestLogHook(bool quiet, bool benchmark)
        : mQuiet(quiet)
        , mBenchmark(benchmark) {
    }

    void TestLogHook::_testRunnerInit(size_t) {
    }

    void TestLogHook::_error(const std::string& message) {
        Log::infraError(message);
    }

    void TestLogHook::_log(const std::string& message) {
        if (!mQuiet) {
            Log::log(message);
        }
    }

    void TestLogHook::_startTest(const std::string& funcName) {
        if (!mQuiet) {
            Log::log("\t${}", funcName);
        }
    }

    void TestLogHook::_endTest(const std::string& funcName, TestInfra::TestResult result, double timeMs) {
        std::string resultName = util::cmd::manip::textColorGreen() + "Passed" + util::cmd::manip::textColorDefault();
        if (result == TestInfra::TestResult::Fail) {
            resultName = util::cmd::manip::textColorBrightRed() + "Failed" + util::cmd::manip::textColorDefault();
        }
        else if (result == TestInfra::TestResult::None) {
            resultName = util::cmd::manip::textColorBrightYellow() + "Inconclusive" + util::cmd::manip::textColorDefault();
        }

        if (!mQuiet) {
            if (mBenchmark) {
                Log::log("\t\t${} in ${} ms", resultName,  timeMs);
            }
            else {
                Log::log("\t\t${}", resultName);
            }
        }
        else if (result != TestInfra::TestResult::Pass) {
            if (mBenchmark) {
                Log::log("${} : ${}\n\t${} in ${} ms", mCurrentClass, funcName, resultName, timeMs);
            }
            else {
                Log::log("${} : ${}\n\t${}", mCurrentClass, funcName, resultName);
            }
        }
    }

    void TestLogHook::_startClass(const std::string& className) {
        if (!mQuiet) {
            Log::log(className);
        }
        else {
            mCurrentClass = className;
        }
    }

    void TestLogHook::_endClass(const std::string&, TestInfra::TestResult result, double timeMs) {
        if (!mQuiet && mBenchmark) {
            std::string resultName = util::cmd::manip::textColorGreen() + "Passed" + util::cmd::manip::textColorDefault();
            if (result == TestInfra::TestResult::Fail) {
                resultName = util::cmd::manip::textColorBrightRed() + "Failed" + util::cmd::manip::textColorDefault();
            }
            else if (result == TestInfra::TestResult::None) {
                resultName = util::cmd::manip::textColorBrightYellow() + "Inconclusive" + util::cmd::manip::textColorDefault();
            }

            Log::log("\t${} in ${} ms", resultName, timeMs);
        }
    }
}

#endif
