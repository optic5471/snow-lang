
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#include <snowlib/diag/StageLevel.hpp>

#include <unordered_map>

#include <util/Assertions.hpp>

namespace diag {
    std::unordered_map<Stage, std::string> StageMap = {
        { Stage::Unknown, "unknown" },
        { Stage::Tokenization, "tokenization" },
        { Stage::SyntaxAnalysis, "syntax-analysis" },
    };

    std::unordered_map<Stage, char> StageShortNameMap = {
        { Stage::Unknown, 'U' },
        { Stage::Tokenization, 'T' },
        { Stage::SyntaxAnalysis, 'A' },
    };

    std::unordered_map<Level, std::string> LevelMap = {
        { Level::Fatal, "fatal" },
        { Level::Error, "error" },
        { Level::Warn, "warning" },
        { Level::Suggest, "suggestion" },
        { Level::Info, "info" },
        { Level::Supplemental, "supplemental" },
    };

    char StageShortName(Stage s) {
        auto it = StageShortNameMap.find(s);
        if (it == StageShortNameMap.end()) {
            DEBUG_FAIL("Unknown diag stage, was a new one added?");
            return 'U';
        }
        return it->second;
    }

    std::string ToString(Stage s) {
        auto it = StageMap.find(s);
        if (it == StageMap.end()) {
            DEBUG_FAIL("Unknown diag stage, was a new one added?");
            return "unknown";
        }
        return it->second;
    }

    std::string ToString(Level l) {
        auto it = LevelMap.find(l);
        if (it == LevelMap.end()) {
            DEBUG_FAIL("Unknown diag level, was a new one added?");
            return "fatal-unknown";
        }
        return it->second;
    }

    bool FromString(const std::string& s, Stage& out) {
        for (auto&& it : StageMap) {
            if (s == it.second) {
                out = it.first;
                return true;
            }
        }
        DEBUG_FAIL("Unknown diag stage, was a new one added?");
        return false;
    }

    bool FromString(const std::string& s, Level& out) {
        for (auto&& it : LevelMap) {
            if (s == it.second) {
                out = it.first;
                return true;
            }
        }
        DEBUG_FAIL("Unknown diag level, was a new one added?");
        return false;
    }
}