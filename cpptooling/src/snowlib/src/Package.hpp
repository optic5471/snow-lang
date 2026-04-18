
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#pragma once

#include <unordered_map>

#include <util/FileSystem.hpp>
#include <util/HashedString.hpp>

namespace src {
    class Module;
    struct ModulePath;

    enum class PackageType {
        Exe,
        Lib,
        Dynlib
    };

    class Package {
        struct BuildData {
            std::string mStartingFunctionName = "main";
            PackageType mPackageType = PackageType::Exe;
        };

        util::fs::Path mRootPath;
        util::HashedString mPackageName;
        std::unordered_map<util::HashedString, std::shared_ptr<Module>> mModules;
        std::shared_ptr<Module> mRootModule;
        BuildData mBuildData;

        bool _parseStatement(PackageType type, const std::string& name);
        bool _parseBlock(PackageType type, const std::string& name);

    public:
        Package(util::fs::Path rootFile);
        const util::HashedString& name() const;

        // opens the root file, finds the package statement, run the block or setup the statement
        bool executePackage();

        // attempts to find a module in this package given the path
        // In a package, you dont need the package name (well its optional)
        // In the case that is is not used, it should be automatically added path by the CALLEE
        const std::shared_ptr<Module> tryResolveModule(const std::string& modulePath) const;
        const std::shared_ptr<Module> tryResolveModule(const ModulePath& modulePath, size_t pathIndex) const;

        void addModule(std::shared_ptr<Module> module);

#ifdef TEST_ENABLED
        Package(const std::string& name, std::shared_ptr<Module> rootModule);
#endif
    };
}
