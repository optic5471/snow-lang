
// Copyright (c) 2025 Andrew Griffin - All Rights Reserved

#ifdef TEST_ENABLED

#include <util/cmd/ArgParse.hpp>
#include <util/cmd/manip.hpp>
#include <util/Log.hpp>
#include <util/String.hpp>
#include <testHelpers/TestLogHook.hpp>
#include <testInfra/TestRunner.hpp>
#include <testInfra/TestInfra.hpp>

namespace test {
    static const char* sTestDesc = "Runs tests. Usage: <exe> <options> [tagsOrNamesForTest]...\n";

    util::cmd::ParseResult _runTest(const util::cmd::ParsedCmdLine& cmd);

    #define _addArgParseOptions(_to_) \
        _to_ \
            .addSwitch(util::cmd::Switch("all", 'a', "Runs all tests except those tagged with ignore. Do not provide a tag with switch")) \
            .addOption(util::cmd::Option("count", 'c', util::cmd::ValueType::Integer, "Runs the tests count times. Must be non-zero positive value")) \
            .addSwitch(util::cmd::Switch("quiet", 'q', "Outputs failure errors, or just the success message")) \
            .addSwitch(util::cmd::Switch("bench", 'b', "Outputs the time it took to execute the tests")) \
            .addSwitch(util::cmd::Switch("bench-all", "Outputs the time it took to execute each test, class, and total"));

    util::cmd::ArgParse makeArgParse() {
        auto argparse = util::cmd::ArgParse(&_runTest, sTestDesc);
        _addArgParseOptions(argparse);
        return argparse;
    }

    void bindArgParse(util::cmd::ArgParse& argparse) {
        util::cmd::Command& command = argparse.addSubCommand(util::cmd::Command("test", &_runTest, sTestDesc));
        _addArgParseOptions(command);
    }

    #undef _addArgParseOptions

    util::cmd::ParseResult _runTest(const util::cmd::ParsedCmdLine& cmd) {
        bool runAll = false;
        if (const util::cmd::Switch* a = cmd.mRunningCommand.tryGetSwitch('a')) {
            runAll = a->parsed();
        }

        int count = 1;
        if (const util::cmd::Option* c = cmd.mRunningCommand.tryGetOption('c'); c && c->parsed()) {
            std::pair<bool, int> i = c->tryGetValue<int>();
            if (i.first) {
                if (i.second <= 0) {
                    return util::cmd::ParseResult("'--count' option must be non-zero positive");
                }
                else {
                    count = i.second;
                }
            }
        }

        bool quiet = false;
        if (const util::cmd::Switch* a = cmd.mRunningCommand.tryGetSwitch('q')) {
            quiet = a->parsed();
        }

        bool benchmark = false;
        if (const util::cmd::Switch* benchSwitch = cmd.mRunningCommand.tryGetSwitch('b')) {
            benchmark = benchSwitch->parsed();
        }

        bool benchmarkAll = false;
        if (const util::cmd::Switch* benchAllSwitch = cmd.mRunningCommand.tryGetSwitch("bench-all")) {
            benchmark |= benchAllSwitch->parsed();
            benchmarkAll = benchAllSwitch->parsed();
        }

        if (runAll && !cmd.mArguments.empty()) {
            return util::cmd::ParseResult("Use either test tags/names or the any switch, not both");
        }

        if (!runAll && cmd.mArguments.empty()) {
            return cmd.mRunningCommand.printHelp(util::cmd::ParseResult("No arguments provided"));
        }

        TestHelpers::TestLogHook logger(quiet, benchmarkAll);
        TestInfra::TestRunResult result;
        double timeTaken = 0.0;
        if (runAll) {
            for (int i = 0; i < count; ++i) {
                result = TestInfra::TestRunner::runAllTests();
                timeTaken += result.mTimeToRun;
                if (result.mOveralResult != TestInfra::TestResult::Pass) {
                    break;
                }
            }
        }
        else {
            std::vector<std::string> tags;
            tags.reserve(cmd.mArguments.size());
            for (auto&& p : cmd.mArguments) {
                tags.emplace_back(p.second);
            }

            for (int i = 0; i < count; ++i) {
                std::vector<std::string> tag = tags;
                result = TestInfra::TestRunner::runTests(std::move(tag));
                timeTaken += result.mTimeToRun;
                if (result.mOveralResult != TestInfra::TestResult::Pass) {
                    break;
                }
            }
        }

        switch (result.mOveralResult) {
            case TestInfra::TestResult::None: {
                util::cmd::manip::Builder b("\n\n");
                b.backgroundColorBrightYellow("            ")
                    .textColorBrightYellow(util::format("\n(${}/${}) Test results were inconclusive!\n", result.mTestsRun - result.mTestsFailed, result.mTestsRun));

                if (benchmark) {
                    b.textColorBrightYellow(util::format("Completed in ${} sec.\n", timeTaken));
                }

                b.backgroundColorBrightYellow("            ")
                    .text("\n");
                Log::log(b.str());
                break;
            }
            case TestInfra::TestResult::Fail: {
                util::cmd::manip::Builder b("\n\n");
                b.backgroundColorBrightRed("            ")
                    .textColorBrightRed(util::format("\n(${}/${}) ${} tests failed!\n", result.mTestsRun - result.mTestsFailed, result.mTestsRun, result.mTestsFailed));

                if (benchmark) {
                    b.textColorBrightRed(util::format("Completed in ${} sec.\n", timeTaken));
                }

                b.backgroundColorBrightRed("            ")
                    .text("\n");
                Log::log(b.str());
                break;
            }
            case TestInfra::TestResult::Pass: {
                util::cmd::manip::Builder b("\n\n");
                b.backgroundColorBrightGreen("            ")
                    .textColorBrightGreen(util::format("\n(${}/${}) All tests passed!\n", result.mTestsRun - result.mTestsFailed, result.mTestsRun));

                if (benchmark) {
                    b.textColorBrightGreen(util::format("Completed in ${} sec.\n", timeTaken));
                }

                b.backgroundColorBrightGreen("            ")
                    .text("\n");
                Log::log(b.str());
                break;
            }
        }
        return util::cmd::ParseResult();
    }
}

#endif