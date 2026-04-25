
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#pragma once

#include <optional>
#include <string>

#include <snowlib/diag/DiagLogger.hpp>
#include <util/EnumBitset.hpp>

namespace lex {
    class Token;
}

namespace ast {
    class BaseNode;
}

enum class CompilerDebugQuery : char {
    None = 0,
    Ast,
    Tokens,
    Types,

    _count
};

struct SnowArgs {
    struct CompilerArgs {
        diag::LogLevel logLevel = diag::LogLevel::Default;
        bool forceCompilingDebug = false;
        bool allWarningsAreErrors = false;
        bool timeCompilation = false;
    };

    struct DebugCompilerArgs {
        util::EnumBitset<CompilerDebugQuery> compilerDebugQuery;
        std::optional<std::string> compilerDebugQueryFile;

        // outputs
        std::vector<lex::Token> mOutputTokens;
        std::shared_ptr<ast::BaseNode> mAst;
        //std::vector<parse::Type> mTypes;
    };

    CompilerArgs mCompilerArgs;
    DebugCompilerArgs mDebugCompilerArgs;
};
