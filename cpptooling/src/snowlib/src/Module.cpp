
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#include <snowlib/src/Module.hpp>

namespace src {
    Module::Module(Loc loc, const std::string& name)
        : mLoc(loc)
        , mName(name) {
    }

    void Module::setAstRoot(std::shared_ptr<ast::BaseNode> ast) {
        mAst = ast;
    }

    std::shared_ptr<ast::BaseNode>& Module::getAst() {
        return mAst;
    }

    const std::shared_ptr<ast::BaseNode>& Module::getAst() const {
        return mAst;
    }

    void Module::addSubModule(std::shared_ptr<Module> subModule) {
        mSubModules.emplace(subModule->getName(), subModule);
    }

    const std::unordered_map<util::HashedString, std::shared_ptr<Module> > &Module::getSubModules() const {
        return mSubModules;
    }

    std::shared_ptr<Module> Module::getSubModule(const util::HashedString &moduleName) {
        if (auto it = mSubModules.find(moduleName); it != mSubModules.end()) {
            return it->second;
        }
        return nullptr;
    }

    const Loc &Module::getLoc() const {
        return mLoc;
    }

    const std::string &Module::getName() const {
        return mName;
    }
}
