
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#include <snowlib/diag/Desc.hpp>

#include <snowlib/diag/StageLevel.hpp>
#include <snowlib/diag/Type.hpp>
#include <util/Assertions.hpp>
#include <util/cmd/manip.hpp>

namespace diag {
    std::unordered_map<Type, Desc> Desc::sDescriptors = {};

    const Desc& Desc::get(Type t) {
        auto it = sDescriptors.find(t);
        if (it == sDescriptors.end()) {
            FAIL_IN_PUBLISH("Invalid descriptor type, was a new one added?");
        }
        return it->second;
    }

    void Desc::init() {
        const std::string red = util::cmd::manip::textColorBrightRed();
        const std::string cyan = util::cmd::manip::textColorBrightCyan();
        const std::string bold = util::cmd::manip::textBoldOn();
        const std::string boldOff = util::cmd::manip::textBoldOff();
        const std::string italics = util::cmd::manip::textItalicOn();
        const std::string italicsOff = util::cmd::manip::textItalicOff();
        const std::string underline = util::cmd::manip::textUnderlineOn();
        const std::string underlineOff = util::cmd::manip::textUnderlineOff();
        const std::string reset = util::cmd::manip::reset();

        auto add = [&](Type t, Stage s, Level l, const std::string& desc) {
            sDescriptors.emplace(t, Desc{ s, l, t, util::format<'#'>("#{}#{}", desc, reset) });
        };

        add(Type::Unknown, Stage::Unknown, Level::Fatal, util::format<'#'>(
            "#{}An unknown error has occurred, please report as this hasnt likely happen before to anyone",
            red));

#ifdef TEST_ENABLED
        add(Type::TEST_Warn, Stage::Tokenization, Level::Warn, "Test warning");
        add(Type::TEST_UnaryError, Stage::SyntaxAnalysis, Level::Error, util::format<'#'>(
            "Test error with '#{}${}#{}' value",
            cyan, reset));
        add(Type::TEST_BinaryInfo, Stage::Unknown, Level::Info, util::format<'#'>(
            "Test binary error of '#{}${}#{}' and '#{}${}#{}'",
            cyan, reset, cyan, reset));
        add(Type::TEST_Suggestion, Stage::Unknown, Level::Suggest, "Suggest doing X");
#endif

        add(Type::InternalFatal, Stage::Unknown, Level::Fatal, util::format(
            "${}Internal compiler failure", red));


        // -------------------------------
        // 1000-1999: Tokenization
        add(Type::Tok_Unknown, Stage::Tokenization, Level::Fatal, util::format<'#'>(
            "#{}An unknown tokenization error has occurred, please report as this hasnt likely happen before to anyone",
            red));


        // -------------------------------
        // 2000-2999: Syntax Analysis
        add(Type::SA_Unknown, Stage::SyntaxAnalysis, Level::Fatal, util::format<'#'>(
            "#{}An unknown syntax analysis error has occurred, please report as this hasnt likely happen before to anyone",
            red));
    }
}