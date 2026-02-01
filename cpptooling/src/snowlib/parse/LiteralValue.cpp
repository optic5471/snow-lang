
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#include <snowlib/parse/LiteralValue.hpp>

#include <util/Assertions.hpp>

namespace parse {
    const IntegerLiteral IntegerLiteral::INVALID = IntegerLiteral(0, false);

    IntegerLiteral::IntegerLiteral(size_t value, bool positive)
        : mValue(value)
        , mPositive(positive) {
        if (isU64() && !mPositive) {
            DEBUG_FAIL("An impossible number has been saved, too large for 64 bits");
        }
    }

    bool IntegerLiteral::isU32() const {
        return mValue > std::numeric_limits<int32_t>::max() &&
            mValue <= std::numeric_limits<uint32_t>::max();
    }

    bool IntegerLiteral::isU64() const {
        return mValue > std::numeric_limits<uint32_t>::max();
    }

    bool IntegerLiteral::isI32() const {
        return mValue <= std::numeric_limits<int32_t>::max();
    }

    uint32_t IntegerLiteral::asU32() const {
        return static_cast<uint32_t>(mValue);
    }

    uint64_t IntegerLiteral::asU64() const {
        return static_cast<uint64_t>(mValue);
    }

    int32_t IntegerLiteral::asI32() const {
        int32_t v = static_cast<int32_t>(mValue);
        if (mPositive) {
            return v;
        }
        return -v;
    }

    LiteralValue::LiteralValue(IntegerLiteral i)
        : mData(i) {
    }

    LiteralValue::LiteralValue(size_t intVal, bool isPositive)
        : mData(IntegerLiteral(intVal, isPositive)) {
    }

    LiteralValue::LiteralValue(bool b)
        : mData(b) {
    }

    LiteralValue::LiteralValue(const std::string& s)
        : mData(s) {
    }

    LiteralValue::LiteralValue(double d)
        : mData(d) {
    }

    LiteralValue::LiteralValue(char c)
        : mData(c) {
    }

    bool LiteralValue::isInt() const {
        return !!std::get_if<IntegerLiteral>(&mData);
    }

    bool LiteralValue::isBool() const {
        return !!std::get_if<bool>(&mData);
    }

    bool LiteralValue::isString() const {
        return !!std::get_if<std::string>(&mData);
    }

    bool LiteralValue::isFloat() const {
        return !!std::get_if<double>(&mData);
    }

    bool LiteralValue::isRune() const {
        return !!std::get_if<char>(&mData);
    }

    char LiteralValue::asRune() const {
        if (const char* r = std::get_if<char>(&mData)) {
            return *r;
        }
        DEBUG_FAIL("Not storing a rune");
        return '\0';
    }

    bool LiteralValue::asBool() const {
        if (const bool* r = std::get_if<bool>(&mData)) {
            return *r;
        }
        DEBUG_FAIL("Not storing a boolean");
        return false;
    }

    const std::string& LiteralValue::asString() const {
        if (const std::string* r = std::get_if<std::string>(&mData)) {
            return *r;
        }
        DEBUG_FAIL("Not storing a string");
        return util::EMPTY_STRING;
    }

    const IntegerLiteral& LiteralValue::asInt() const {
        if (const IntegerLiteral* r = std::get_if<IntegerLiteral>(&mData)) {
            return *r;
        }
        DEBUG_FAIL("Not storing an int");
        return IntegerLiteral::INVALID;
    }

    double LiteralValue::asFloat() const {
        if (const double* r = std::get_if<double>(&mData)) {
            return *r;
        }
        DEBUG_FAIL("Not storing a double");
        return 0.0;
    }
}
