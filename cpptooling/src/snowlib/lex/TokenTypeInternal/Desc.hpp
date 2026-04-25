
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#pragma once

#include <optional>
#include <string>
#include <unordered_map>

namespace lex {
    enum class MetaType : char;
    enum class TokenType : short;

    class Desc {
        static std::unordered_map<TokenType, Desc> sDescriptions;

    public:
        static void init();
        static const Desc& get(TokenType t);

        const TokenType mType;
        const MetaType mMetaType;
        const std::string_view mToString;
        const std::optional<std::string_view> mKeywordValue; // only defined for keywords
    };
}
