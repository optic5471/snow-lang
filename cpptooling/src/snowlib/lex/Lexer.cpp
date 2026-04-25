
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#include <snowlib/lex/Lexer.hpp>

#include <snowlib/lex/Context.hpp>
#include <snowlib/lex/Token.hpp>
#include <snowlib/lex/TokenType.hpp>
#include <snowlib/src/SourceVault.hpp>
#include <util/ServiceLocator.hpp>

namespace lex {
    std::optional<uint64_t> Lexer::_fastAtoiBase2(src::LocRange loc) {
        uint64_t ret = 0;
        char i = 0;
        const std::string_view str = ServiceLocator<src::SourceVault>::fetch().getLexeme(loc);
        for (char c : str) {
            if (c == '_' || c == '`') {
                continue;
            }

            if (i++ >= 64) {
                return {}; // overflow, u64 base 2 max is 1111111111111111111111111111111111111111111111111111111111111111
            }

            ret = (ret << 1) + (c - '0');
        }
        return ret;
    }

    std::optional<uint64_t> Lexer::_fastAtoiBase10(src::LocRange loc) {
        uint64_t ret = 0;
        bool firstNonZero = false;
        const std::string_view str = ServiceLocator<src::SourceVault>::fetch().getLexeme(loc);
        for (char c : str) {
            if (c == '_' || c == '`') {
                continue;
            }
            if (c == '0' && !firstNonZero) {
                continue;
            }
            firstNonZero = true;

            // multiplcation overflow check
            if (ret > (std::numeric_limits<uint64_t>::max() / 10)) {
                return {}; // next digit causes overflow, u64 base 10 max is 18446744073709551615
            }

            ret *= 10;
            char v = c - '0';
            if (ret > (std::numeric_limits<uint64_t>::max() - v)) {
                return {}; // exceeds max for this digit, overflow, u64 base 10 max is 18446744073709551615
            }

            ret += v;
        }
        return ret;
    }

    std::optional<uint64_t> Lexer::_fastAtoiBase16(src::LocRange loc) {
        uint64_t ret = 0;
        char i = 0;
        const std::string_view str = ServiceLocator<src::SourceVault>::fetch().getLexeme(loc);
        for (char c : str) {
            if (c == '_' || c == '`') {
                continue;
            }

            if (i++ >= 16) {
                return {}; // overflow, u64 base 16 max is FFFFFFFFFFFFFFFF
            }

            if (c >= 'a' && c <= 'f') {
                ret = (ret * 16) + ((c - 'a') + 10);
            }
            else if (c >= 'A' && c <= 'F') {
                ret = (ret * 16) + ((c - 'A') + 10);
            }
            else if (c >= '0' && c <= '9') {
                ret = (ret * 16) + (c - '0');
            }
        }
        return ret;
    }

    std::optional<double> Lexer::_fastAtod(src::LocRange loc) {
        std::string toParse;
        const std::string_view str = ServiceLocator<src::SourceVault>::fetch().getLexeme(loc);
        for (char c : str) {
            if (c == '_' || c == '`') {
                continue;
            }
            toParse.push_back(c);
        }

        if (toParse == "0.0" || toParse == "0.0e0" ||
            toParse == "0.0e+0" || toParse == "0.0e-0" ||
            toParse == "0.0E0" || toParse == "0.0E+0" ||
            toParse == "0.0E-0") {
            return 0.0;
            }

        char* noConversion = nullptr;
        double d = std::strtod(toParse.c_str(), &noConversion);
        if (d == HUGE_VAL) {
            return {}; // overflow
        }
        if (d == 0.0 && noConversion != nullptr) {
            DEBUG_FAIL("Unable to read double, was there a problem with the parse function?");
            return {};
        }
        return d;
    }

    Token Lexer::_startsWithInteger(Context& ctx) {
        
    }
}
