
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#include <snowlib/src/Package.hpp>
#include <snowlib/src/Module.hpp>
#include <snowlib/src/ModulePath.hpp>
#include <util/Assertions.hpp>

namespace src {
    Package::Package(util::fs::Path rootFile)
        : mRootPath(rootFile) {
    }

    const util::HashedString& Package::name() const {
        return mPackageName;
    }

    bool Package::executePackage() {
        DEBUG_FAIL("Not implemented");
        // run tokenization to find the package statement in the file. Disregard all other tokens at this time

        // determine if the package is a statement or block
        return false;
    }

    bool Package::_parseStatement(PackageType type, const std::string& name) {
        mPackageName = name;
        mBuildData = {};
        mBuildData.mPackageType = type;
        return true;
    }

    bool Package::_parseBlock(PackageType type, const std::string& name) {
        DEBUG_FAIL("Not implemented");
        mPackageName = name;
        mBuildData = {};
        mBuildData.mPackageType = type;

        // startup interpreter with the global package object and execute the block starting at the scope

        return false;
    }

    const std::shared_ptr<Module> Package::tryResolveModule(const std::string& modulePath) const {
        return tryResolveModule(ModulePath(modulePath), 0);
    }

    const std::shared_ptr<Module> Package::tryResolveModule(const ModulePath& path, size_t pathIndex) const {
        if (path.mParts.empty()) {
            return nullptr;
        }
        else if (path.mParts.size() == pathIndex + 1) {
            return mRootModule;
        }

        const util::HashedString& name = path.mParts[pathIndex + 1];
        auto it = mModules.find(name);
        if (it == mModules.end()) {
            return nullptr;
        }

        size_t i = pathIndex + 2;
        while (i < path.mParts.size()) {
            const std::unordered_map<util::HashedString, std::shared_ptr<Module>>& modules = it->second->getSubModules();
            it = modules.find(path.mParts[i]);
            if (it == modules.end()) {
                return nullptr;
            }
            ++i;
        }

        return it->second;
    }

    void Package::addModule(std::shared_ptr<Module> module) {
        if (mModules.find(module->getName()) == mModules.end()) {
            mModules.emplace(module->getName(), module);
            return;
        }
        DEBUG_FAIL("Module already exists in package root directory.");
        // TODO: Need an error here
    }

#ifdef TEST_ENABLED
    Package::Package(const std::string& name, std::shared_ptr<Module> rootModule)
        : mRootPath("")
        , mPackageName(name)
        , mRootModule(rootModule) {
    }
#endif
}