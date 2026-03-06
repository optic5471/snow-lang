
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#pragma once

#include <cinttypes>
#include <string>
#include <unordered_map>

namespace diag {
    enum class Level : uint8_t;
    enum class Stage : uint8_t;
    enum class Type : uint16_t;

    // Diagnostic descriptor, used to describe the diagnostic type and how to handle and print it
    class Desc {
        static std::unordered_map<Type, Desc> sDescriptors;

    public:
        static void init();
        static const Desc& get(Type t);

        const Stage mStage;
        const Level mLevel;
        const Type mType;
        const std::string mDescription;
    };
}