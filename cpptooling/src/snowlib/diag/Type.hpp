
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#pragma once

#include <cstdint>

namespace diag {
    // DO NOT CHANGE ORDER
    // Diagnostic numerical values should be preserved
    // All errors support nothing, a file, a Loc, or a Loc with up to 2 LocRanges on the same line
    // Naming Scheme:
    // MetaType_ArgCount_Name or MetaType_Name (when arg count is 0)
    // Ex)
    //   - Tok_MissingLetter, prints no arguments and is from Tokenizer
    //   - SA_1_MismatchClosing, prints with 1 argument and is from Syntax Analysis
    // When an argument count is provided, place a comment above for what each argument is
    enum class Type : uint16_t {
        // -------------------------------
        // 0-999:     Compiler Reserved
        // -------------------------------
        Unknown = 0,
        InternalFatal = 1,


#ifdef TEST_ENABLED
        TEST_Warn = 996,
        TEST_UnaryError = 997,
        TEST_BinaryInfo = 998,
        TEST_Suggestion = 999,
#endif

        // -------------------------------
        // 1000-1999: Tokenization
        // -------------------------------
        Tok_Unknown = 1000,


        // -------------------------------
        // 2000-2999: Syntax Analysis
        // -------------------------------
        SA_Unknown = 2000,
    };
}