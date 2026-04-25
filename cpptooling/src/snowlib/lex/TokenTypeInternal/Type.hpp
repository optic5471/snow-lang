
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#pragma once

namespace lex {
    enum class MetaType : char {
        None = 0,
        Symbol,
        Keyword,
        Literal,
        Comment,
        DocComment,
        CompilerValue
    };

    enum class TokenType : short {
        Invalid = 0,
        EndOfLineComment,
        MultiLineComment,
        TripleComment,
        BangComment,
        Variadic,
        ExclusiveRange,
        InclusiveRange,
        Identifier,


        // ----------------------------
        META_CompilerValuesStart,

        Comp_FileName,
        Comp_FilePathFull, // full path from the file system root
        Comp_FilePathPackage, // path from package root
        Comp_LineNumber,
        Comp_FunctionName,
        Comp_FullFunctionDecl, // Entire declaration of the function from the file
        Comp_TimeStamp, // timestamp at the current time when compiling, NOT static at the beginning of compilation
        Comp_CompileTimestamp, // timestamp of compilation, remains static throughout compilation
        Comp_CompilerVendor,
        Comp_CompilerVersion,
        Comp_SnowVersion,
        Comp_ModuleName,
        Comp_PackageName,
        Comp_ObjectName, // class, interface, enum, ...

        META_CompilerValuesEnd,


        // ----------------------------
        META_KeywordStart, // alpha sorted for ease

        K_alias,
        K_arptr,
        K_as,
        K_async,
        K_await,
        K_binop,
        K_bool,
        K_break,
        K_class,
        K_continue,
        K_copy,
        K_defer,
        K_deinit,
        K_do,
        K_dynlib,
        K_else,
        K_enum,
        K_exe,
        K_export,
        K_f32,
        K_f64,
        K_fallthrough,
        K_fn,
        K_foreach,
        K_friend,
        K_generic,
        K_has,
        K_i8,
        K_i16,
        K_i32,
        K_i64,
        K_if,
        K_implements,
        K_import,
        K_in,
        K_inf,
        K_init,
        K_interface,
        K_is,
        K_let,
        K_lib,
        K_module,
        K_move,
        K_mut,
        K_nan,
        K_otherwise,
        K_package,
        K_panic,
        K_postop,
        K_preop,
        K_ptr,
        K_pub,
        K_ref,
        K_return,
        K_rune,
        K_self, // variable name
        K_Self, // type name
        K_static,
        K_string,
        K_switch,
        K_typeof,
        K_u8,
        K_u16,
        K_u32,
        K_u64,
        K_undefined,
        K_unsafe,
        K_use,
        K_where,
        K_while,
        K_yield,

        META_KeywordEnd,


        // ----------------------------
        META_SymbolStart,

        S_question,
        S_closeParen,
        S_openParen,
        S_colon,
        S_openCurly,
        S_closeCurly,
        S_openBrace,
        S_closeBrace,
        S_equal,
        S_plus,
        S_minus,
        S_asterisk,
        S_ampersand,
        S_carot,
        S_percent,
        S_dollar,
        S_pound,
        S_at,
        S_bang,
        S_tilde,
        S_grave,
        S_bar,
        S_doubleQuote,
        S_singleQuote,
        S_semicolon,
        S_forwardSlash, // /
        S_backSlash, // \ a
        S_period,
        S_comma,
        S_greater,
        S_less,

        META_SymbolEnd
    };
}
