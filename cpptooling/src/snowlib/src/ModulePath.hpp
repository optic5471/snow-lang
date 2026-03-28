
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#pragma once

#include <vector>

#include <util/HashedString.hpp>

namespace src {
    struct ModulePath {
        std::vector<util::HashedString> mParts;

        ModulePath() = default;
        ModulePath(const std::string& path);
    };
}