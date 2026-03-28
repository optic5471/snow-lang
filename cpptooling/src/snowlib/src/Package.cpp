
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#include <snowlib/src/Package.hpp>
#include <snowlib/src/Module.hpp>
#include <snowlib/src/ModulePath.hpp>

namespace src {
    Package::Package(util::fs::Path rootFile)
        : mRootPath(rootFile) {
    }

    const std::string& Package::name() const {
        return mPackageName;
    }

    const std::shared_ptr<Module> Package::tryResolveModule(const std::string& modulePath) const {
        return tryResolveModule(ModulePath(modulePath));
    }

    const std::shared_ptr<Module> Package::tryResolveModule(const ModulePath&) const {
        return nullptr;
    }
}