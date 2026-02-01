
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#pragma once

namespace parse::lex {
    enum class Type : uint8_t {
        Invalid = 0,
        Identifier,
        Operator,
        MergeMarker, // >>>>>>> ======= <<<<<<<

        KeywordStart,
        KeywordStop,

        SymbolStart,
        SymbolStop,

        LiteralStart,
        LiteralStop,

        CompilerMarkersStart,
        CompilerMarkersStop,

        eof // last
    };
}
