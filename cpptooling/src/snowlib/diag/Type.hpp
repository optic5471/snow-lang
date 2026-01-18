
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#pragma once

#include <cstdint>

namespace diag {
    // DO NOT CHANGE ORDER
    // Diagnostic numerical values should be preserved
    enum class Type : uint16_t {
        // -------------------------------
        // 0-999:     Defaults
        Unknown = 0,

#ifdef TEST_ENABLED
        TEST_Warn = 996,
        TEST_UnaryError = 997,
        TEST_BinaryInfo = 998,
        TEST_Suggestion = 999,
#endif

        InternalFatal = 1,


        // -------------------------------
        // 1000-1999: Tokenization
        Tok_Unknown = 1000,


        // -------------------------------
        // 2000-2999: Syntax Analysis
        SA_Unknown = 2000,
    };
}