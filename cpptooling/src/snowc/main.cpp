
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#include <snowlib/main.hpp>
#include <util/cmd/ArgParse.hpp>
#include <util/Log.hpp>

int main(int argc, const char** argv) {
    //snowlibinit();

    util::cmd::ArgParse argparse("snowc is not implemented");

    // Normal arguments: These are used with SnowArgs object
    // -v, --verbose: flag, send snowlibinit LogLevel::Verbose
    // --error-only: flag, send snowlibinit LogLevel::ErrorsOnly
    // --force-debug: flag, forces debug mode compilation, will not allow packages to override this
    // --werror: flag, all warnings are errors
    // --version: Prints version of compiler
    // -t, --time: Times various parts of the compilation, broken down by file, project, stage, and a summary printed at the end with the stage timings, most expensive files, ...
    //
    // Compiler Debugging:
    // query --ast <file-to-compile>: dumps the file's completed ast
    // query --types <file-to-compile>: dumps the collected type information from the file
    // query --tokens <file-to-compile>: dumps the tokens from the file
    // can be combined to print the stages together as well

    util::cmd::ParseResult result = argparse.parse(argc, argv);
    if (result.mFailed) {
        Log::error(result.mErrorMessage);
    }
    else if (!result.mErrorMessage.empty()) {
        Log::log(result.mErrorMessage);
    }
    return 0;
}
