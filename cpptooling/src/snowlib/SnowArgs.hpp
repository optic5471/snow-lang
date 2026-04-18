
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#pragma once

#include <optional>
#include <string>

namespace diag {
    enum class LogLevel : char;
}

enum class CompilerDebugQuery : char {
    None = 0,
    Ast = 1 << 0,
    Tokens = 1 << 1,
    Types = 1 << 2
};

struct SnowArgs {
    diag::LogLevel logLevel;
    bool forceCompilingDebug = false;
    bool allWarningsAreErrors = false;
    bool timeCompilation = false;
    char compilerDebugQuery = 0;
    std::optional<std::string> compilerDebugQueryFile;
};
