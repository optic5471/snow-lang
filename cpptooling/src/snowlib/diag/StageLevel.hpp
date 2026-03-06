
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#pragma once

#include <cinttypes>
#include <string>

namespace diag {
    enum class Stage : uint8_t {
        Unknown = 0,
        LexicalAnalysis,
        SyntaxAnalysis
    };

    enum class Level : uint8_t {
        Fatal = 0,
        Error,
        Warn,
        Suggest,
        Info,
        Supplemental
    };

    char StageShortName(Stage s);
    std::string ToString(Stage s);
    std::string ToString(Level l);
    bool FromString(const std::string& s, Level& out);
    bool FromString(const std::string& s, Stage& out);
}
