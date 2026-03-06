
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#pragma once

#include <cinttypes>
#include <string>

#include <util/meta/meta.hpp>

namespace parse {
    class IntegerLiteral {
        size_t mValue;
        bool mPositive;

    public:
        static const IntegerLiteral INVALID;
        IntegerLiteral(size_t value, bool positive);

        bool isU32() const;
        bool isU64() const;
        bool isI32() const;

        uint32_t asU32() const;
        uint64_t asU64() const;
        int32_t asI32() const;
    };

    class LiteralValue {
        enum class Type {
            Int,
            Bool,
            String,
            Double,
            Char
        };

        Type mType;
        char mData[util::meta::max_size_struct<IntegerLiteral, bool, std::string, double, char>::max_size];

    public:
        LiteralValue(IntegerLiteral i);
        LiteralValue(size_t intVal, bool isPositive);
        LiteralValue(bool b);
        LiteralValue(const std::string& s);
        LiteralValue(double d);
        LiteralValue(char c);

        bool isInt() const;
        bool isBool() const;
        bool isString() const;
        bool isFloat() const;
        bool isRune() const;

        char asRune() const;
        bool asBool() const;
        const std::string& asString() const;
        double asFloat() const;
        const IntegerLiteral& asInt() const;
    };
}

