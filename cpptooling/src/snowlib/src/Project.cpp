
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#include <snowlib/src/Package.hpp>
#include <snowlib/src/Project.hpp>
#include <util/Assertions.hpp>

namespace src {
    void Project::setInvokedPackage(std::shared_ptr<Package> invokedPackage) {
        DEBUG_ASSERT(!mInvokedPackage, "Invoked package has already been set, dont initialize the project more than once");
        mInvokedPackage = invokedPackage;
        mPackages.emplace(invokedPackage->name(), invokedPackage);
    }

    std::shared_ptr<Package> Project::getInvokedPackage() {
        return mInvokedPackage;
    }

    std::shared_ptr<Package> Project::getPackage(const util::HashedString& name) {
        if (auto it = mPackages.find(name); it != mPackages.end()) {
            return it->second;
        }
        return nullptr;
    }

    const std::unordered_map<util::HashedString, std::shared_ptr<Package>>& Project::getPackages() const {
        return mPackages;
    }

    void Project::addPackage(std::shared_ptr<Package> package) {
        mPackages.emplace(package->name(), package);
    }
}
