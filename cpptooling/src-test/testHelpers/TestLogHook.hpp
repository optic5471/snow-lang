
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#pragma once

#ifdef TEST_ENABLED

#include <testInfra/TestInfra.hpp>

namespace TestHelpers {
    // This merges the interface of the test infra logger to the engine's logger
    class TestLogHook final : public TestInfra::LogHook {
    private:
        bool mQuiet = false;
        bool mBenchmark = false;
        std::string mCurrentClass = "";

    public:
        TestLogHook(bool quiet, bool benchmark);
        ~TestLogHook() override = default;

        void _testRunnerInit(size_t testCount) override;
        void _error(const std::string& message) override;
        void _log(const std::string& message) override;
        void _startTest(const std::string& funcName) override;
        void _endTest(const std::string& funcName, TestInfra::TestResult result, double timeMs) override;
        void _startClass(const std::string& className) override;
        void _endClass(const std::string& className, TestInfra::TestResult result, double timeMs) override;
    };
}

#endif
