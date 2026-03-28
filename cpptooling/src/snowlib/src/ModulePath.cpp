
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#include <snowlib/src/ModulePath.hpp>

namespace src {
    ModulePath::ModulePath(const std::string &path) {
        std::string part;
        for (auto c : path) {
            if (c == '/') {
                if (part.empty()) {
                    continue;
                }
                mParts.emplace_back(part);
                part.clear();
            }
            else {
                part.push_back(c);
            }
        }
        if (!part.empty()) {
            mParts.emplace_back(part);
        }
    }
}
