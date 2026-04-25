
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#include <snowlib/parse/LiteralValue.hpp>

#include <util/Assertions.hpp>

namespace parse {
    const IntegerLiteral IntegerLiteral::INVALID = IntegerLiteral(0, false);

    IntegerLiteral::IntegerLiteral(size_t value, bool positive)
        : mValue(value)
        , mPositive(positive) {
        if ((isU32() || isU64()) && !mPositive) {
            DEBUG_FAIL("Unsigned values cannot be negative, or a u64 being negative is more than 64 bits");
        }
    }

    bool IntegerLiteral::isU32() const {
        return mValue > 2147483647 && mValue <= 4294967295 && mPositive;
    }

    bool IntegerLiteral::isU64() const {
        return mValue > 9223372036854775807U && mPositive;
    }

    bool IntegerLiteral::isI32() const {
        if (mPositive) {
            return mValue <= 2147483647;
        }
        else {
            return mValue <= 2147483648;
        }
    }

    bool IntegerLiteral::isI64() const {
        if (mPositive) {
            return mValue <= 9223372036854775807U;
        }
        else {
            return mValue <= 9223372036854775808U;
        }
    }

    uint32_t IntegerLiteral::asU32() const {
        return static_cast<uint32_t>(mValue);
    }

    uint64_t IntegerLiteral::asU64() const {
        return mValue;
    }

    int64_t IntegerLiteral::asI64() const {
        int64_t v = static_cast<int64_t>(mValue);
        if (mPositive) {
            return v;
        }
        return -v;
    }

    int32_t IntegerLiteral::asI32() const {
        int32_t v = static_cast<int32_t>(mValue);
        if (mPositive) {
            return v;
        }
        return -v;
    }

    LiteralValue::LiteralValue(IntegerLiteral i) {
        mType = Type::Int;
        IntegerLiteral* data = reinterpret_cast<IntegerLiteral*>(&mData);
        *data = i;
    }

    LiteralValue::LiteralValue(size_t intVal, bool isPositive) {
        mType = Type::Int;
        IntegerLiteral* data = reinterpret_cast<IntegerLiteral*>(&mData);
        *data = IntegerLiteral(intVal, isPositive);
    }

    LiteralValue::LiteralValue(bool b) {
        mType = Type::Bool;
        bool* data = reinterpret_cast<bool*>(&mData);
        *data = b;
    }

    LiteralValue::LiteralValue(const std::string& s) {
        mType = Type::String;
        std::string* data = reinterpret_cast<std::string*>(&mData);
        *data = s;
    }

    LiteralValue::LiteralValue(double d) {
        mType = Type::Double;
        double* data = reinterpret_cast<double*>(&mData);
        *data = d;
    }

    LiteralValue::LiteralValue(char c) {
        mType = Type::Char;
        char* data = reinterpret_cast<char*>(&mData);
        *data = c;
    }

    bool LiteralValue::isInt() const {
        return mType == Type::Int;
    }

    bool LiteralValue::isBool() const {
        return mType == Type::Bool;
    }

    bool LiteralValue::isString() const {
        return mType == Type::String;
    }

    bool LiteralValue::isFloat() const {
        return mType == Type::Double;
    }

    bool LiteralValue::isRune() const {
        return mType == Type::Char;
    }

    char LiteralValue::asRune() const {
        if (mType == Type::Char) {
            return *reinterpret_cast<const char*>(&mData);
        }
        DEBUG_FAIL("Not storing a rune");
        return '\0';
    }

    bool LiteralValue::asBool() const {
        if (mType == Type::Bool) {
            return *reinterpret_cast<const bool*>(&mData);
        }
        DEBUG_FAIL("Not storing a boolean");
        return false;
    }

    const std::string& LiteralValue::asString() const {
        if (mType == Type::String) {
            return *reinterpret_cast<const std::string*>(&mData);
        }
        DEBUG_FAIL("Not storing a string");
        return util::EMPTY_STRING;
    }

    const IntegerLiteral& LiteralValue::asInt() const {
        if (mType == Type::Int) {
            return *reinterpret_cast<const IntegerLiteral*>(&mData);
        }
        DEBUG_FAIL("Not storing an int");
        return IntegerLiteral::INVALID;
    }

    double LiteralValue::asFloat() const {
        if (mType == Type::Double) {
            return *reinterpret_cast<const double*>(&mData);
        }
        DEBUG_FAIL("Not storing a double");
        return 0.0;
    }
}
