
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#include <snowlib/diag/Printer.hpp>

#include <snowlib/diag/Desc.hpp>
#include <snowlib/diag/DiagLogger.hpp>
#include <snowlib/diag/StageLevel.hpp>
#include <snowlib/diag/Type.hpp>
#include <util/cmd/manip.hpp>
#include <util/ServiceLocator.hpp>

namespace diag {
    std::string syntaxHighlight(const std::string& src) {
        // TODO: Lexical Analysis, based on tokens assign colors and output string
        return src;

        /*

        parse::LexOptions opt;
        opt.suppressAllDiags = true;
        opt.stripComments = false;
        opt.stripDocComments = false;
        opt.stripWhitespace = false;;

        file::SnowFile f = file::SnowFile::LoadString(src);
        parse::LexContext ctx(f, std::move(opt));

        util::cmd::manip::Builder ret;
        std::optional<token::Token> t = parse::lex::Lexer::lexToken(ctx);
        while(t.has_value()) {
            const token::TokenMeta& it = token::TypeMeta::Get(t->type());
            if (it.hasFlag(token::TypeMeta::Flags::Keyword)) {
                ret.textColorBlue(std::string(t->lexeme()));
            }
            else if (it.hasFlag(token::TypeMeta::Flags::Comment)) {
                ret.textColorGreen(std::string(t->lexeme()));
            }
            else if (it.type() == token::Type::Identifier) {
                ret.textColorMagenta(std::string(t->lexeme()));
            }
            else {
                ret.textColorLightGrey(std::string(t->lexeme()));
            }
            t = parse::lex::Lexer::lexToken(ctx);
        }
        ret.textColorDefault();
        return ret.str();

        */
    }

    namespace internal {
        std::string _numToDigitString(size_t digit, bool zeroes) {
            if (digit < 10) {
                return util::format("${}${}", zeroes ? "000" : "   ", digit);
            }
            else if (digit < 100) {
                return util::format("${}${}", zeroes ? "00" : "  ", digit);
            }
            else if (digit < 1000) {
                return util::format("${}${}", zeroes ? "0" : " ", digit);
            }
            return util::format("${}", digit);
        }

        std::tuple<std::string, size_t> _stripAndCountBeginningWhitespace(const std::string& str) {
            std::string ret;
            ret.reserve(str.size());
            bool firstHit = false;
            size_t count = 0;
            for (const char& c : str) {
                if (!firstHit) {
                    if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
                        count++;
                        continue;
                    }
                    firstHit = true;
                }
                ret.push_back(c);
            }
            return { ret, count };
        }

        static std::optional<std::vector<Type>> gTestOnlyCaptureEndPoint = {};

        // Filename (row|col): diagStageChar-diagTypeNumber: diagDesc
        // row(5 char width) | syntaxHighlightLine
        //                   | ~~~~^~~~~
        // The location of the error can be:
        // - empty: dont print filename, row, or column. Print "Compiler: "
        // - SnowFile: print the filename, no row or col
        // - Loc: print the entire thing
        // - if empty, check if Range1 is given, if so, use that as the location and print as if Loc with ~~~~ after for the range
        // No dash ('-') between stage and type number
        // row on second line is a 5 character width line number in grey text
        // syntax highlighted line, no width restrictions
        // third line is the ranges (~~~~) and the location (^) of the error
        // Max 2 ranges (including when location is a range) allowed
        // Additional ranges should be supplemental messages to the original
        void printDiag(const MakeArgs& args) {
            const Desc& diagDesc = Desc::get(args.mType);
            std::string diagMsg = util::format_with(diagDesc.mDescription, args.mArgs);

            struct LocationData {
                size_t col;
                std::optional<std::pair<size_t, size_t>> range1; // col, len
                std::optional<std::pair<size_t, size_t>> range2; // col, len
                size_t lineNum;
                std::string src;
                size_t whitespaceCountAtBeginning;
                std::optional<src::FileID> file;
            };

            std::optional<LocationData> locationData;
            std::string locationName;
            auto locLocationParser = [&locationData, &locationName](src::Loc loc) {
                auto [line, col] = src::SourceVault::fetch().getLineCol(loc);
                const src::FileEntry* fileData = src::SourceVault::fetch().tryGetFileData(loc.mFileID);
                if (!fileData || col == 0) {
                    return;
                }

                LocationData data;
                data.lineNum = line;
                data.col = col;
                data.file = loc.mFileID;

                std::string fullLine = std::string(src::SourceVault::fetch().getLineAt(loc));
                auto [strippedLine, wsCount] = _stripAndCountBeginningWhitespace(fullLine);
                data.whitespaceCountAtBeginning = wsCount;
                data.src = syntaxHighlight(util::trimWhitespaceEnd(strippedLine));

                locationData = std::move(data);
                locationName = util::format("${} (${}|${})", fileData->mPath.str(), line, col);
            };

            if (const src::Loc* locData = std::get_if<src::Loc>(&args.mAt)) {
                locLocationParser(*locData);

                if (locationData.has_value()) {
                    auto checkRange = [&](src::LocRange r) -> bool {
                        auto [rline, rcol] = src::SourceVault::fetch().getLineCol(r);
                        if (locationData->lineNum != rline) {
                            FAIL_IN_PUBLISH("A diag provided with ranges must come from the same line, use supplemental diags for multi-line");
                            return false;
                        }
                        else if (locData->mFileID != r.mFileID) {
                            FAIL_IN_PUBLISH("A diag provided with ranges, must have ranges and loc come from the same file");
                            return false;
                        }
                        return true;
                    };

                    if (args.mRange1.has_value()) {
                        if (!checkRange(*args.mRange1)) {
                            return;
                        }
                    }
                    if (args.mRange2.has_value()) {
                        if (!checkRange(*args.mRange2)) {
                            return;
                        }
                    }
                }
            }
            else if (const src::FileID* fileData = std::get_if<src::FileID>(&args.mAt)) {
                const src::FileEntry* file = src::SourceVault::fetch().tryGetFileData(*fileData);
                if (!file) {
                    DEBUG_FAIL("Trying to diag against an empty file");
                    ServiceLocator<DiagLogger>::fetch().writeError("Attempting to create a diag from an unknown file");
                    return;
                }

                locationName = file->mPath.str();
                if (args.mRange1.has_value()) {
                    FAIL_IN_PUBLISH("A diag provided with only a file cannot use ranges");
                    return;
                }
            }
            else if (args.mRange1.has_value()) {
                locLocationParser(args.mRange1.value());
            }

            if (locationData.has_value()) {
                if (args.mRange1.has_value()) {
                    auto [line1, col1] = src::SourceVault::fetch().getLineCol(args.mRange1.value());
                    locationData->range1 = { col1, args.mRange1->mLength };
                    if (args.mRange2.has_value()) {
                        auto [line2, col2] = src::SourceVault::fetch().getLineCol(args.mRange2.value());
                        locationData->range2 = { col2, args.mRange2->mLength };

                        if (args.mRange1->mFileID != args.mRange2->mFileID) {
                            FAIL_IN_PUBLISH("A diag provided with ranges, must have both ranges come from the same file");
                            return;
                        }
                        else if (line1 != line2) {
                            FAIL_IN_PUBLISH("A diag provided with ranges must come from the same line, use supplemental diags for multi-line");
                            return;
                        }
                    }
                }
            }

            util::cmd::manip::TextColor msgColor;
            switch (diagDesc.mLevel) {
                case Level::Error:
                case Level::Fatal: {
                    msgColor = util::cmd::manip::TextColor::BrightRed;
                    break;
                }
                case Level::Supplemental:
                case Level::Info: {
                    msgColor = util::cmd::manip::TextColor::BrightCyan;
                    break;
                }
                case Level::Warn:
                case Level::Suggest: {
                    msgColor = util::cmd::manip::TextColor::BrightYellow;
                    break;
                }
                default: {
                    DEBUG_FAIL("Unknown diag level");
                    return;
                }
            }

            std::string diagMessageLine = util::format("${}${}${}: ${}${}${}${}: ${}",
                util::cmd::manip::textColorBrightBlue(),
                locationName.empty() ? "Internal" : locationName,
                util::cmd::manip::textColorDefault(),

                util::cmd::manip::textColor(msgColor),
                StageShortName(diagDesc.mStage),
                _numToDigitString(static_cast<size_t>(diagDesc.mType), true),
                util::cmd::manip::textColorDefault(),

                diagMsg
            );

            std::string finalMsg;
            if (locationData.has_value()) {
                std::string srcMessageLine = util::format("${}${} |${} ${}",
                    util::cmd::manip::textColorDarkGrey(),
                    _numToDigitString(locationData->lineNum, false),
                    util::cmd::manip::textColorDefault(),
                    locationData->src
                );

                std::string positionData;
                positionData.reserve(locationData->src.size());
                for (size_t i = 0; i < locationData->src.size(); ++i) {
                    bool inRange = false;
                    size_t pos = (i + locationData->whitespaceCountAtBeginning);

                    bool range1Invalid = false;
                    if (locationData->range1.has_value()) {
                        size_t start = locationData->range1->first - 1;
                        size_t end = start + locationData->range1->second;
                        if (pos > end) {
                            range1Invalid = true;
                        }
                        else if (pos >= start && pos < end) {
                            inRange = true;
                        }
                    }
                    else {
                        range1Invalid = true;
                    }

                    bool range2Invalid = false;
                    if (locationData->range2.has_value()) {
                        size_t start = locationData->range2->first - 1;
                        size_t end = start + locationData->range2->second;
                        if (pos > end) {
                            range2Invalid = true;
                        }
                        else if (pos >= start && pos < end) {
                            inRange = true;
                        }
                    }
                    else {
                        range2Invalid = true;
                    }

                    bool posInvalid = false;
                    if (pos > (locationData->col - 1)) {
                        posInvalid = true;
                    }

                    if (posInvalid && range1Invalid && range2Invalid) {
                        break;
                    }

                    if (pos == (locationData->col - 1)) {
                        positionData += util::format("${}^${}",
                            util::cmd::manip::textColorBrightGreen(),
                            util::cmd::manip::textColorDefault());
                    }
                    else if (inRange) {
                        positionData += util::format("${}~${}",
                            util::cmd::manip::textColorBrightMagenta(),
                            util::cmd::manip::textColorDefault());
                    }
                    else {
                        positionData.push_back(' ');
                    }
                }

                std::string caretLine = util::format("${}     | ${}${}",
                    util::cmd::manip::textColorDarkGrey(),
                    util::cmd::manip::textColorDefault(),
                    positionData);
                finalMsg = util::format("${}\n${}\n${}\n", diagMessageLine, srcMessageLine, caretLine);
            }
            else {
                finalMsg = diagMessageLine;
            }

            // finally log
            switch (diagDesc.mLevel) {
                case Level::Error: {
                    ServiceLocator<DiagLogger>::fetch().writeError(finalMsg);
                    break;
                }
                case Level::Fatal: {
                    ServiceLocator<DiagLogger>::fetch().writeError(finalMsg);
                    abort(); // Fatal errors should kill the app, including tests
                    break;
                }
                case Level::Supplemental:
                case Level::Info:
                case Level::Warn:
                case Level::Suggest: {
                    ServiceLocator<DiagLogger>::fetch().writeInfo(finalMsg);
                    break;
                }
                default: {
                    DEBUG_FAIL("Unknown diag level");
                    return;
                }
            }

#ifdef TEST_ENABLED
            if (gTestOnlyCaptureEndPoint.has_value()) {
                gTestOnlyCaptureEndPoint.value().emplace_back(diagDesc.mType);
            }
#endif
        }

#ifdef TEST_ENABLED
        void testOnly_beginCapture() {
            gTestOnlyCaptureEndPoint.emplace(std::vector<Type>{});
        }

        std::vector<Type> testOnly_endCapture() {
            std::vector<Type> ret = std::move(gTestOnlyCaptureEndPoint.value());
            gTestOnlyCaptureEndPoint.reset();
            return ret;
        }
#endif
    }
}
