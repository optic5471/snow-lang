
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#pragma once

#include <optional>
#include <variant>

#include <snowlib/file/Loc.hpp>
#include <snowlib/file/SnowFile.hpp>
#include <util/NotNull.hpp>
#include <util/String.hpp>

namespace diag {
    enum class Type : uint16_t;

    namespace internal {
        struct MakeArgs {
            std::variant<
                std::monostate,
                util::RefWrap<file::SnowFile>,
                util::RefWrap<file::Loc>> mAt;
            std::optional<file::LocRange> mRange1;
            std::optional<file::LocRange> mRange2;
            std::vector<std::string> mArgs;
            Type mType;
        };

#ifdef TEST_ENABLED
        void testOnly_beginCapture();
        std::vector<Type> testOnly_endCapture();
#endif

        void printDiag(const MakeArgs& args);

        template <size_t depth, typename tArg, typename... tArgs>
        void constructMakeArgs(MakeArgs& out, bool parseToArgs, tArg&& arg, tArgs&&... args) {
            if constexpr (depth == 0) {
                if constexpr (std::is_same_v<std::decay_t<tArg>, file::SnowFile>) {
                    out.mAt = std::forward<tArg>(arg);
                }
                else if constexpr (std::is_same_v<std::decay_t<tArg>, file::Loc>) {
                    out.mAt = std::forward<tArg>(arg);
                }
                else if constexpr (std::is_same_v<std::decay_t<tArg>, file::LocRange>) {
                    out.mRange1 = std::forward<tArg>(arg);
                }
                else {
                    parseToArgs = true;
                    out.mArgs.emplace_back(std::to_string(std::forward<tArg>(arg)));
                }
            }
            else if constexpr (depth == 1) {
                if (parseToArgs) {
                    out.mArgs.emplace_back(std::to_string(std::forward<tArg>(arg)));
                }
                else {
                    if constexpr (std::is_same_v<std::decay_t<tArg>, file::LocRange>) {
                        if (out.mRange1.has_value()) {
                            out.mRange2 = std::forward<tArg>(arg);
                        }
                        else {
                            out.mRange1 = std::forward<tArg>(arg);
                        }
                    }
                    else {
                        parseToArgs = true;
                        out.mArgs.emplace_back(std::to_string(std::forward<tArg>(arg)));
                    }
                }
            }
            else if constexpr (depth == 2) {
                if (parseToArgs) {
                    out.mArgs.emplace_back(std::to_string(std::forward<tArg>(arg)));
                }
                else {
                    if constexpr (std::is_same_v<std::decay_t<tArg>, file::LocRange>) {
                        if (out.mRange2.has_value()) {
                            DEBUG_FAIL("Too many ranges have been provided");
                        }
                        else {
                            out.mRange2 = std::forward<tArg>(arg);
                        }
                    }
                    else {
                        parseToArgs = true;
                        out.mArgs.emplace_back(std::to_string(std::forward<tArg>(arg)));
                    }
                }
            }
            else {
                parseToArgs = true;
                out.mArgs.emplace_back(std::to_string(std::forward<tArg>(arg)));
            }
            constructMakeArgs<depth + 1, tArgs...>(out, parseToArgs, std::forward<tArgs>(args)...);
        }
    }

    // returns the same string with proper highlighting
    std::string syntaxHighlight(const std::string& src);

    // Constructs and prints the diagnostic to the screen given the data passed
    // Optionally Pass first argument as the file or loc that the error happen with
    // if passed, optionally pass second argument as range the diag references
    // if passed, optionally pass third argument as second range the diag references
    // remaining arguments are used in diagnostic message formatting
    template <typename... tArgs>
    void make(Type t, tArgs&&... args) {
        internal::MakeArgs a;
        a.mType = t;
        internal::constructMakeArgs<0, tArgs...>(a, false, std::forward<tArgs>(args)...);
        internal::printDiag(a);
    }
}