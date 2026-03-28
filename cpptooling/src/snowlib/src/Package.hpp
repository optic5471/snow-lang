
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
        std::string mPackageName;
        std::unordered_map<util::HashedString, std::shared_ptr<Module>> mModules;

        bool _parseStatement();
        bool _parseBlock();

    public:
        Package(util::fs::Path rootFile);
        const std::string& name() const;

        // opens the root file, finds the package statement, run the block or setup the statement
        bool executePackage();

        // attempts to find a module in this package given the path
        const std::shared_ptr<Module> tryResolveModule(const std::string& modulePath) const;
        const std::shared_ptr<Module> tryResolveModule(const ModulePath& modulePath) const;

        // modules contain other modules, this is for the root package modules of which there are very few (maybe just one)
        void addRootModule(std::shared_ptr<Module> module);
    };
}
