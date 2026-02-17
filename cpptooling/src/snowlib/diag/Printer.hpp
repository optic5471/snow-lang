
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#pragma once

#include <optional>
#include <variant>

#include <snowlib/src/Loc.hpp>
#include <snowlib/src/SourceVault.hpp>
#include <util/NotNull.hpp>
#include <util/String.hpp>

namespace diag {
    enum class Type : uint16_t;

    namespace internal {
        struct MakeArgs {
            std::variant<
                std::monostate,
                src::FileID,
                src::Loc> mAt;
            std::optional<src::LocRange> mRange1;
            std::optional<src::LocRange> mRange2;
            std::vector<std::string> mArgs;
            Type mType;
        };

#ifdef TEST_ENABLED
        void testOnly_beginCapture();
        std::vector<Type> testOnly_endCapture();
#endif

        void printDiag(const MakeArgs& args);

        // These debug utils help diagnose type issues with the constructMakeArgs function
        template <size_t depthToPrint, size_t depth, typename tArg, typename... tArgs>
        void DEBUG_print_type_at_depth(tArg&& arg, tArgs&&... args) {
            if constexpr (depth == depthToPrint) {
                // this will produce an error similar to `C2039: 'something_made_up': is not a member of 'file::Loc'
                typedef typename std::decay_t<tArg>::something_made_up X;
            }
            else {
                DEBUG_print_type_at_depth<depthToPrint, depth + 1>(std::forward<tArgs>(args)...);
            }
        }
        template <size_t depthToPrint, size_t depth>
        void DEBUG_print_type_at_depth() {
            static_assert("No type to print");
        }

        template <size_t depth, typename tArg, typename... tArgs>
        void constructMakeArgs(MakeArgs& out, bool parseToArgs, tArg&& arg, tArgs&&... args) {
            if constexpr (depth == 0) {
                if constexpr (std::is_same_v<std::decay_t<tArg>, src::FileID>) {
                    out.mAt = arg;
                }
                else if constexpr (std::is_same_v<std::decay_t<tArg>, src::Loc>) {
                    out.mAt = arg;
                }
                else if constexpr (std::is_same_v<std::decay_t<tArg>, src::LocRange>) {
                    out.mRange1 = arg;
                }
                else {
                    parseToArgs = true;
                    out.mArgs.emplace_back(util::toString(std::forward<tArg>(arg)));
                }
            }
            else if constexpr (depth == 1) {
                if constexpr (std::is_same_v<std::decay_t<tArg>, src::LocRange>) {
                    if (parseToArgs) {
                        DEBUG_FAIL("Range was provided after arguments. Please define all ranges before arguments");
                    }
                    else if (out.mRange1.has_value()) {
                        out.mRange2 = arg;
                    }
                    else {
                        out.mRange1 = arg;
                    }
                }
                else {
                    parseToArgs = true;
                    out.mArgs.emplace_back(util::toString(std::forward<tArg>(arg)));
                }
            }
            else if constexpr (depth == 2) {
                if constexpr (std::is_same_v<std::decay_t<tArg>, src::LocRange>) {
                    if (parseToArgs) {
                        DEBUG_FAIL("Range was provided after arguments. Please define all ranges before arguments");
                    }
                    else if (out.mRange2.has_value()) {
                        DEBUG_FAIL("Too many ranges have been provided");
                    }
                    else {
                        out.mRange2 = arg;
                    }
                }
                else {
                    parseToArgs = true;
                    out.mArgs.emplace_back(util::toString(std::forward<tArg>(arg)));
                }
            }
            else {
                parseToArgs = true;
                out.mArgs.emplace_back(util::toString(std::forward<tArg>(arg)));
            }
            constructMakeArgs<depth + 1, tArgs...>(out, parseToArgs, std::forward<tArgs>(args)...);
        }

        template <size_t depth>
        void constructMakeArgs(MakeArgs& out, bool parseToArgs) {}
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

    // These debug utils help diagnose type issues with the constructMakeArgs function
    template <typename... tArgs>
    void makeDEBUG(Type t, tArgs&&... args) {
        internal::DEBUG_print_type_at_depth<1, 0>(std::forward<tArgs>(args)...);
    }
}