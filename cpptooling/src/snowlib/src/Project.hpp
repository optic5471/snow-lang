
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#pragma once

#include <memory>
#include <unordered_map>

#include <util/HashedString.hpp>

namespace src {
    class Package;

    class Project {
        std::unordered_map<util::HashedString, std::shared_ptr<Package>> mPackages;
        std::shared_ptr<Package> mInvokedPackage;

    public:
        Project();
        void setInvokedPackage(std::shared_ptr<Package> package);
        std::shared_ptr<Package> getInvokedPackage();
        const std::unordered_map<util::HashedString, std::shared_ptr<Package>>& getPackages() const;
        void addPackage(std::shared_ptr<Package> package);
        std::shared_ptr<Package> getPackage(const util::HashedString& name);
    };
}
