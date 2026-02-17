
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#pragma once

#include <optional>

#include <snowlib/file/Loc.hpp>
#include <snowlib/parse/LiteralValue.hpp>

namespace parse::lex {
    enum class Type : uint8_t;

    class Token {
        file::LocRange mLoc;
        Type mType = static_cast<Type>(0);

    public:
        static const Token INVALID;

        std::optional<LiteralValue> mLiteralValue;

        Token() = default;
        Token(file::LocRange&& loc, Type type);
        Token(const file::Loc& loc, size_t len, Type type);
        Token(file::LocRange&& loc, Type type, LiteralValue value);
        Token(const file::Loc& loc, size_t len, Type type, LiteralValue value);

        Type type() const;
        const file::LocRange& loc() const;
        std::string_view lexeme() const;
        bool valid() const;

        template <typename... tArgs>
        bool isType(Type type, tArgs&&... args) const {
            if (mType == type) {
                return true;
            }
            return isType<tArgs...>(std::forward<tArgs>(args)...);
        }
        template <>
        bool isType(Type type) const {
            return mType == type;
        }
    };
}
