
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#include <util/Assertions.hpp>
#include <util/cmd/ArgParse.hpp>
#include <util/Log.hpp>
#include <snowlib/main.hpp>
#include <testInfra/TestRunner.hpp>

// This will read directly from argc argv for the commands to run
int mainAsCommandInput(int argc, const char** argv) {
    ASSERT_IN_PUBLISH(testonly_snowlibinit(), "Failed to initialize internal compiler");
    util::cmd::ArgParse argparse = test::makeArgParse();
    util::cmd::ParseResult result = argparse.parse(argc, argv);
    if (result.mFailed) {
        Log::error(result.mErrorMessage);
        return 1;
    }
    else if (!result.mErrorMessage.empty()) {
        Log::log(result.mErrorMessage);
    }
    return 0;
}


// -------------------------------------------
// This will block allowing repeatable inputs
class App {
public:
    static bool running;
    static void run(util::cmd::ArgParse& parser, std::string exeName) {
        running = true;
        exeName.push_back(' ');
        while (running) {
            std::string input;
            std::getline(std::cin, input);
            input = exeName + input;
            util::cmd::ParseResult result = parser.parse(input);
            if (result.mFailed) {
                Log::error(result.mErrorMessage);
            }
            else if (!result.mErrorMessage.empty()) {
                Log::log(result.mErrorMessage);
            }
        }
    }
};
bool App::running = false;

util::cmd::ParseResult exitApp(const util::cmd::ParsedCmdLine&) {
    App::running = false;
    return {};
}

int mainAsStandingApp(const char* exeName) {
    ASSERT_IN_PUBLISH(testonly_snowlibinit(), "Failed to initialize internal compiler");
    util::cmd::ArgParse argparse("Runs tests on the snow compiler");
    test::bindArgParse(argparse);
    argparse.addSubCommand(util::cmd::Command("exit", &exitApp, "Exit the application"));
    App::run(argparse, exeName);
    return 0;
}


// -------------------------------------------
int main([[maybe_unused]] int argc, [[maybe_unused]] const char** argv) {
    //return mainAsCommandInput(argc, argv);
    return mainAsStandingApp(argv[0]);
}