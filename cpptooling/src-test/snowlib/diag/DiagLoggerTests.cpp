
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#include <testInfra/TestInfra.hpp>
#include <snowlib/diag/DiagLogger.hpp>

#include <chrono>
#include <thread>

#include <test/testHelpers/TestLogEndPoint.hpp>

namespace test {
    struct InputReplayer {
        int x = 0;
        int y = 0;
        std::vector<std::vector<char>> mBuffer;

        InputReplayer() {
            // 50 lines
            for (size_t i = 0; i < 50; ++i) {
                mBuffer.push_back(std::vector<char>());
                std::vector<char>& buf = mBuffer.back();

                // 80 columns
                for (size_t j = 0; j < 80; ++j) {
                    buf.push_back(0);
                }
            }
        }

        void compare(const std::string& input) {
            InputReplayer replayer;
            replayer.replay(input);
            for (size_t i = 0; i < 50; ++i) {
                for (size_t j = 0; j < 80; ++j) {
                    ASSERT_EQ(replayer.mBuffer[y][x], mBuffer[y][x], "Mismatched replay");
                }
            }
        }

        void replay(const std::string& input) {
            const char tag = '\033';
            for (size_t i = 0; i < input.size(); ++i) {
                char c = input[i];
                if (c == tag) {
                    char t = input[i + 3];

                    // clear line
                    if (t == 'K') {
                        for (size_t j = 0; j < mBuffer[y].size(); ++j) {
                            mBuffer[y][j] = '\0';
                        }
                    }
                    // cursor up
                    else if (t == 'F') {
                        y -= input[i + 2] - '0';
                    }
                    i += 3;
                }
                // this will emulate windows
                else if (c == '\n') {
                    y++;
                    x = 0;
                }
                else if (c == '\r') {
                    x = 0;
                }
                else {
                    mBuffer[y][x] = c;
                    x++;
                }
            }
        }
    };

    TEST_CLASS(DiagLoggerTests) {
        TEST_START() {
            Log::testOnly_enableTestEndPoint();
        }

        TEST_STOP() {
            Log::testOnly_forceDisableTestEndPoint();
        }

        TEST(DiagLogger_PrintLogs) {
            diag::DiagLogger logger(true, diag::LogLevel::Default);
            logger.writeInfo("first");
            logger.writeError("second");
            logger.writeVerbose("third");
            auto output = Log::testOnly_disableTestEndPoint();
            output->expectXMessagesWhichContain({
                "first",
                "second"
            });
        }

        TEST(DiagLogger_PrintLogs_ErrorsOnly) {
            diag::DiagLogger logger(true, diag::LogLevel::ErrorsOnly);
            logger.writeInfo("first");
            logger.writeError("second");
            logger.writeVerbose("third");
            auto output = Log::testOnly_disableTestEndPoint();
            output->expectXMessagesWhichContain({
                "second"
            });
        }

        TEST(DiagLogger_PrintLogs_Verbose) {
            diag::DiagLogger logger(true, diag::LogLevel::Verbose);
            logger.writeInfo("first");
            logger.writeError("second");
            logger.writeVerbose("third");
            auto output = Log::testOnly_disableTestEndPoint();
            output->expectXMessagesWhichContain({
                "first",
                "second",
                "third"
            });
        }

        TEST(DiagLogger_PrintLogs_HandlesCorrectlyInReplayer) {
            diag::DiagLogger logger(true);
            logger.writeInfo("first");
            logger.writeError("second");
            logger.writeVerbose("third");
            InputReplayer replayer;
            auto output = Log::testOnly_disableTestEndPoint();
            for (const auto& o : output->getLogs()) {
                replayer.replay(o.second);
            }
            replayer.compare("first\nsecond\n");
        }

        TEST(DiagLogger_PrintLogsThenProgress_HandlesCorrectly) {
            diag::DiagLogger logger(true);
            logger.initProgress(10, "Message");
            logger.writeInfo("first");
            logger.incProgress();
            logger.writeError("second");
            logger.incProgress();
            logger.writeVerbose("third");
            logger.incProgress();
            InputReplayer replayer;
            auto output = Log::testOnly_disableTestEndPoint();
            for (const auto& o : output->getLogs()) {
                replayer.replay(o.second);
            }
            replayer.compare("first\nsecond\n\nMessage\n3/10 (30%) [*******------------------]\n");
        }

        TEST(DiagLogger_5PercDone) {
            diag::DiagLogger logger(true);
            logger.initProgress(20, "", 1, 0);
            auto output = Log::testOnly_disableTestEndPoint();
            InputReplayer replayer;
            for (const auto& o : output->getLogs()) {
                replayer.replay(o.second);
            }
            replayer.compare("\n\n1/20 (5%) [*------------------------]\n");
        }

        TEST(DiagLogger_20PercDone) {
            diag::DiagLogger logger(true);
            logger.initProgress(20, "", 4, 0);
            auto output = Log::testOnly_disableTestEndPoint();
            InputReplayer replayer;
            for (const auto& o : output->getLogs()) {
                replayer.replay(o.second);
            }
            replayer.compare("\n\n4/20 (20%) [*****--------------------]\n");
        }

        TEST(DiagLogger_100PercDone) {
            diag::DiagLogger logger(true);
            logger.initProgress(20, "", 20, 0);
            auto output = Log::testOnly_disableTestEndPoint();
            InputReplayer replayer;
            for (const auto& o : output->getLogs()) {
                replayer.replay(o.second);
            }
            replayer.compare("\n\n20/20 (100%) [*************************]\n");
        }

        TEST(DiagLogger_WithFailsTo100_Is100) {
            diag::DiagLogger logger(true);
            logger.initProgress(20, "", 19, 1);
            auto output = Log::testOnly_disableTestEndPoint();
            InputReplayer replayer;
            for (const auto& o : output->getLogs()) {
                replayer.replay(o.second);
            }
            replayer.compare("\n\n20/20 (100% - 1 Failed) [*************************]\n");
        }

        TEST(DiagLogger_Over100_Is100) {
            diag::DiagLogger logger(true);
            logger.initProgress(10, "", 19, 1);
            auto output = Log::testOnly_disableTestEndPoint();
            InputReplayer replayer;
            for (const auto& o : output->getLogs()) {
                replayer.replay(o.second);
            }
            replayer.compare("\n\n20/10 (100% - 1 Failed) [*************************]\n");
        }

        TEST(DiagLogger_FailsLessThan100_Works) {
            diag::DiagLogger logger(true);
            logger.initProgress(20, "", 10, 5);
            auto output = Log::testOnly_disableTestEndPoint();
            InputReplayer replayer;
            for (const auto& o : output->getLogs()) {
                replayer.replay(o.second);
            }
            replayer.compare("\n\n15/20 (75% - 5 Failed) [******************-------]\n");
        }

        TEST(DiagLogger_LogProgressInit_Handles) {
            diag::DiagLogger logger(true);
            logger.writeInfo("log");
            logger.initProgress(20, "Message", 10, 5);
            auto output = Log::testOnly_disableTestEndPoint();
            InputReplayer replayer;
            for (const auto& o : output->getLogs()) {
                replayer.replay(o.second);
            }
            replayer.compare("log\nMessage\n15/20 (75% - 5 Failed) [******************-------]\n");
        }

        TEST(DiagLogger_ProgressInitLog_Handles) {
            diag::DiagLogger logger(true);
            logger.initProgress(20, "Message", 10, 5);
            logger.writeInfo("log");
            auto output = Log::testOnly_disableTestEndPoint();
            InputReplayer replayer;
            for (const auto& o : output->getLogs()) {
                replayer.replay(o.second);
            }
            replayer.compare("log\nMessage\n15/20 (75% - 5 Failed) [******************-------]\n");
        }

        TEST(DiagLogger_ProgressInitLogDeinit_Handles) {
            diag::DiagLogger logger(true);
            logger.initProgress(20, "Message", 10, 5);
            logger.writeInfo("log");
            logger.clearProgress();
            auto output = Log::testOnly_disableTestEndPoint();
            InputReplayer replayer;
            for (const auto& o : output->getLogs()) {
                replayer.replay(o.second);
            }
            replayer.compare("log\n");
        }

        TEST(DiagLogger_ProgressInitDeinit_Handles) {
            diag::DiagLogger logger(true);
            logger.writeInfo("log");
            logger.initProgress(20, "Message", 10, 5);
            logger.clearProgress();
            logger.writeInfo("log2");
            auto output = Log::testOnly_disableTestEndPoint();
            InputReplayer replayer;
            for (const auto& o : output->getLogs()) {
                replayer.replay(o.second);
            }
            replayer.compare("log\nlog2\n");
        }

        TEST(DiagLogger_LongRunningTest, TestInfra::Tags::Ignore) {
            Log::testOnly_forceDisableTestEndPoint();
            diag::DiagLogger logger("./DiagLogger_LongRunningTest.txt");
            for (size_t i = 0; i < 500; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                logger.writeInfo(util::format("Before window log: ${}", i));
            }

            logger.initProgress(500, "This message is the window message");
            for (size_t i = 0; i < 500; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                logger.writeInfo(util::format("With window but no change log: ${}", i + 500));
            }

            for (size_t i = 0; i < 500; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                logger.incProgress();
                logger.writeInfo(util::format("With window and change log: ${}", i + 1000));
            }

            logger.clearProgress();
            for (size_t i = 0; i < 500; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                logger.writeInfo(util::format("After window log: ${}", i + 1500));
            }
        }
    };
}
