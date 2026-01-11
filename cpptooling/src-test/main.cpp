
// Copyright (c) 2025 Andrew Griffin - All Rights Reserved

#include <util/cmd/ArgParse.hpp>
#include <util/Log.hpp>
#include <testInfra/TestRunner.hpp>

int main(int argc, const char** argv) {
    // main start point for program
    util::cmd::ArgParse argparse = test::makeArgParse();

    util::cmd::ParseResult result = argparse.parse(argc, argv);
    if (result.mFailed) {
        Log::error(result.mErrorMessage);
    }
    else if (!result.mErrorMessage.empty()) {
        Log::log(result.mErrorMessage);
    }
    return 0;
}
