
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#include <snowlib/lex/TokenTypeInternal/Desc.hpp>

#include <snowlib/lex/TokenTypeInternal/Type.hpp>
#include <util/Assertions.hpp>

namespace lex {
    std::unordered_map<TokenType, Desc> Desc::sDescriptions = {};

    const Desc& Desc::get(TokenType t) {
        auto it = sDescriptions.find(t);
        if (it == sDescriptions.end()) {
            FAIL_IN_PUBLISH("Invalid token type, was a new one added?");
        }
        return it->second;
    }

    void Desc::init() {
        auto add = [&](TokenType t, MetaType m, std::string_view tostr, std::optional<std::string_view> k = std::nullopt) {
            sDescriptions.emplace(t, Desc{ t, m, tostr, k });
        };

        add(TokenType::Invalid, MetaType::None, "Invalid");
        add(TokenType::EndOfLineComment, MetaType::Comment, "EndOfLineComment");
        add(TokenType::MultiLineComment, MetaType::Comment, "MultiLineComment");
        add(TokenType::TripleComment, MetaType::DocComment, "TripleComment");
        add(TokenType::BangComment, MetaType::DocComment, "BangComment");
        add(TokenType::Variadic, MetaType::None, "Variadic");
        add(TokenType::ExclusiveRange, MetaType::None, "ExclusiveRange");
        add(TokenType::InclusiveRange, MetaType::None, "InclusiveRange");
        add(TokenType::Identifier, MetaType::None, "Identifier");

        // Compiler values
        add(TokenType::Comp_FileName, MetaType::CompilerValue, "FileName");
        add(TokenType::Comp_FilePathFull, MetaType::CompilerValue, "FilePathFull");
        add(TokenType::Comp_FilePathPackage, MetaType::CompilerValue, "FilePathPackage");
        add(TokenType::Comp_LineNumber, MetaType::CompilerValue, "LineNumber");
        add(TokenType::Comp_FunctionName, MetaType::CompilerValue, "FunctionName");
        add(TokenType::Comp_FullFunctionDecl, MetaType::CompilerValue, "FullFunctionDecl");
        add(TokenType::Comp_TimeStamp, MetaType::CompilerValue, "TimeStamp");
        add(TokenType::Comp_CompileTimestamp, MetaType::CompilerValue, "CompileTimeStamp");
        add(TokenType::Comp_CompilerVendor, MetaType::CompilerValue, "CompilerVendor");
        add(TokenType::Comp_CompilerVersion, MetaType::CompilerValue, "CompilerVersion");
        add(TokenType::Comp_SnowVersion, MetaType::CompilerValue, "SnowVersion");
        add(TokenType::Comp_ModuleName, MetaType::CompilerValue, "ModuleName");
        add(TokenType::Comp_PackageName, MetaType::CompilerValue, "PackageName");
        add(TokenType::Comp_ObjectName, MetaType::CompilerValue, "ObjectName");

        // Keywords
        add(TokenType::K_alias, MetaType::Keyword, "Alias", "alias");
        add(TokenType::K_arptr, MetaType::Keyword, "ArrayPointer", "arptr");
        add(TokenType::K_as, MetaType::Keyword, "As", "as");
        add(TokenType::K_async, MetaType::Keyword, "Async", "async");
        add(TokenType::K_binop, MetaType::Keyword, "BinOp", "binop");
        add(TokenType::K_bool, MetaType::Keyword, "Bool", "bool");
        add(TokenType::K_break, MetaType::Keyword, "Break", "break");
        add(TokenType::K_class, MetaType::Keyword, "Class", "class");
        add(TokenType::K_continue, MetaType::Keyword, "Continue", "continue");
        add(TokenType::K_copy, MetaType::Keyword, "Copy", "copy");
        add(TokenType::K_defer, MetaType::Keyword, "Defer", "defer");
        add(TokenType::K_deinit, MetaType::Keyword, "Deinit", "deinit");
        add(TokenType::K_do, MetaType::Keyword, "Do", "do");
        add(TokenType::K_dynlib, MetaType::Keyword, "DynamicLibrary", "dynlib");
        add(TokenType::K_else, MetaType::Keyword, "Else", "else");
        add(TokenType::K_enum, MetaType::Keyword, "Enum", "enum");
        add(TokenType::K_exe, MetaType::Keyword, "Executable", "exe");
        add(TokenType::K_export, MetaType::Keyword, "Export", "export");
        add(TokenType::K_f32, MetaType::Keyword, "f32", "f32");
        add(TokenType::K_f64, MetaType::Keyword, "f64", "f64");
        add(TokenType::K_fallthrough, MetaType::Keyword, "Fallthrough", "fallthrough");
        add(TokenType::K_fn, MetaType::Keyword, "Function", "fn");
        add(TokenType::K_foreach, MetaType::Keyword, "Foreach", "foreach");
        add(TokenType::K_friend, MetaType::Keyword, "Friend", "friend");
        add(TokenType::K_generic, MetaType::Keyword, "Generic", "generic");
        add(TokenType::K_has, MetaType::Keyword, "Has", "has");
        add(TokenType::K_i8, MetaType::Keyword, "i8", "i8");
        add(TokenType::K_i16, MetaType::Keyword, "i16", "i16");
        add(TokenType::K_i32, MetaType::Keyword, "i32", "i32");
        add(TokenType::K_i64, MetaType::Keyword, "i64", "i64");
        add(TokenType::K_if, MetaType::Keyword, "If", "if");
        add(TokenType::K_implements, MetaType::Keyword, "Implements", "implements");
        add(TokenType::K_import, MetaType::Keyword, "Import", "import");
        add(TokenType::K_in, MetaType::Keyword, "In", "in");
        add(TokenType::K_inf, MetaType::Keyword, "Infinite", "inf");
        add(TokenType::K_init, MetaType::Keyword, "Init", "init");
        add(TokenType::K_interface, MetaType::Keyword, "Interface", "interface");
        add(TokenType::K_is, MetaType::Keyword, "Is", "is");
        add(TokenType::K_let, MetaType::Keyword, "Let", "let");
        add(TokenType::K_lib, MetaType::Keyword, "Library", "lib");
        add(TokenType::K_module, MetaType::Keyword, "Module", "module");
        add(TokenType::K_move, MetaType::Keyword, "Move", "move");
        add(TokenType::K_mut, MetaType::Keyword, "Mutable", "mut");
        add(TokenType::K_nan, MetaType::Keyword, "NotANumber", "nan");
        add(TokenType::K_otherwise, MetaType::Keyword, "Otherwise", "otherwise");
        add(TokenType::K_package, MetaType::Keyword, "Package", "package");
        add(TokenType::K_panic, MetaType::Keyword, "PanicCall", "panic");
        add(TokenType::K_postop, MetaType::Keyword, "PostOp", "postop");
        add(TokenType::K_preop, MetaType::Keyword, "PreOp", "preop");
        add(TokenType::K_ptr, MetaType::Keyword, "Pointer", "ptr");
        add(TokenType::K_pub, MetaType::Keyword, "Public", "pub");
        add(TokenType::K_ref, MetaType::Keyword, "Reference", "ref");
        add(TokenType::K_return, MetaType::Keyword, "Return", "return");
        add(TokenType::K_rune, MetaType::Keyword, "Rune", "rune");
        add(TokenType::K_self, MetaType::Keyword, "SelfVariable", "self");
        add(TokenType::K_Self, MetaType::Keyword, "SelfType", "Self");
        add(TokenType::K_static, MetaType::Keyword, "Static", "static");
        add(TokenType::K_string, MetaType::Keyword, "String", "string");
        add(TokenType::K_switch, MetaType::Keyword, "Switch", "switch");
        add(TokenType::K_typeof, MetaType::Keyword, "TypeOf", "typeof");
        add(TokenType::K_u8, MetaType::Keyword, "u8", "u8");
        add(TokenType::K_u16, MetaType::Keyword, "u16", "u16");
        add(TokenType::K_u32, MetaType::Keyword, "u32", "u32");
        add(TokenType::K_u64, MetaType::Keyword, "u64", "u64");
        add(TokenType::K_undefined, MetaType::Keyword, "Undefined", "undefined");
        add(TokenType::K_unsafe, MetaType::Keyword, "Unsafe", "unsafe");
        add(TokenType::K_use, MetaType::Keyword, "Use", "use");
        add(TokenType::K_where, MetaType::Keyword, "Where", "where");
        add(TokenType::K_while, MetaType::Keyword, "While", "while");
        add(TokenType::K_yield, MetaType::Keyword, "Yield", "yield");

        // Symbols
        add(TokenType::S_question, MetaType::Symbol, "QuestionMark", "?");
        add(TokenType::S_closeParen, MetaType::Symbol, "CloseParentheses", ")");
        add(TokenType::S_openParen, MetaType::Symbol, "OpenParentheses", "(");
        add(TokenType::S_colon, MetaType::Symbol, "Colon", ":");
        add(TokenType::S_openCurly, MetaType::Symbol, "OpenCurlyBrace", "{");
        add(TokenType::S_closeCurly, MetaType::Symbol, "CloseCurlyBrace", "}");
        add(TokenType::S_openBrace, MetaType::Symbol, "OpenSquareBrace", "[");
        add(TokenType::S_closeBrace, MetaType::Symbol, "CloseSquareBrace", "]");
        add(TokenType::S_equal, MetaType::Symbol, "Equal", "=");
        add(TokenType::S_plus, MetaType::Symbol, "Plus", "+");
        add(TokenType::S_minus, MetaType::Symbol, "Minus", "-");
        add(TokenType::S_asterisk, MetaType::Symbol, "Asterisk", "*");
        add(TokenType::S_ampersand, MetaType::Symbol, "Ampersand", "&");
        add(TokenType::S_carot, MetaType::Symbol, "UpCarot", "^");
        add(TokenType::S_percent, MetaType::Symbol, "Percent", "%");
        add(TokenType::S_dollar, MetaType::Symbol, "DollarSign", "$");
        add(TokenType::S_pound, MetaType::Symbol, "PoundSign", "#");
        add(TokenType::S_at, MetaType::Symbol, "AtSign", "@");
        add(TokenType::S_bang, MetaType::Symbol, "Bang", "!");
        add(TokenType::S_tilde, MetaType::Symbol, "Tilde", "~");
        add(TokenType::S_grave, MetaType::Symbol, "Grave", "`");
        add(TokenType::S_bar, MetaType::Symbol, "Bar", "|");
        add(TokenType::S_doubleQuote, MetaType::Symbol, "DoubleQuote", "\"");
        add(TokenType::S_singleQuote, MetaType::Symbol, "SingleQuote", "'");
        add(TokenType::S_semicolon, MetaType::Symbol, "Semicolon", ";");
        add(TokenType::S_forwardSlash, MetaType::Symbol, "ForwardSlash", "/");
        add(TokenType::S_backSlash, MetaType::Symbol, "BackSlash", "\\");
        add(TokenType::S_period, MetaType::Symbol, "Period", ".");
        add(TokenType::S_comma, MetaType::Symbol, "Comma", ",");
        add(TokenType::S_greater, MetaType::Symbol, "GreaterThan", ">");
        add(TokenType::S_less, MetaType::Symbol, "LessThan", "<");
    }
}
