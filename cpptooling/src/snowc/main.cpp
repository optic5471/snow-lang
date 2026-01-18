
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#include <util/cmd/ArgParse.hpp>
#include <util/Log.hpp>

int main(int argc, const char** argv) {
    util::cmd::ArgParse argparse("snowc is not implemented");
    util::cmd::ParseResult result = argparse.parse(argc, argv);
    if (result.mFailed) {
        Log::error(result.mErrorMessage);
    }
    else if (!result.mErrorMessage.empty()) {
        Log::log(result.mErrorMessage);
    }
    return 0;
}
