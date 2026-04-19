
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#include <snowlib/src/Module.hpp>
#include <snowlib/src/ModulePath.hpp>
#include <util/Assertions.hpp>

namespace src {
    Module::Module(std::optional<Loc> loc, const std::string& name, ModuleType type)
        : mLoc(loc)
        , mType(type)
        , mName(name) {
#ifdef OP_DEBUG
        if (mType == ModuleType::Folder) {
            DEBUG_ASSERT(!mLoc.has_value(), "Folder modules should not have locs");
        }
#endif
    }

    void Module::setAstRoot(std::shared_ptr<ast::BaseNode> ast) {
#ifdef OP_DEBUG
        DEBUG_ASSERT(ast, "Dont add null ast");
        if (mType == ModuleType::Folder) {
            DEBUG_FAIL("Folder modules have no code, dont set an AST here");
        }
#endif
        mAst = ast;
    }

    std::shared_ptr<ast::BaseNode>& Module::getAst() {
        return mAst;
    }

    const std::shared_ptr<ast::BaseNode>& Module::getAst() const {
        return mAst;
    }

    void Module::addSubModule(std::shared_ptr<Module> subModule) {
#ifdef OP_DEBUG
        DEBUG_ASSERT(subModule, "Dont add null submodules");

        if (mType == ModuleType::File || mType == ModuleType::Submodule) {
            DEBUG_ASSERT(subModule->getType() == ModuleType::Submodule, "File and submodules can only accept other submodules as a submodule");
        }
        else if (mType == ModuleType::Folder) {
            DEBUG_ASSERT(subModule->getType() == ModuleType::File, "Folders can only accept File submodules");
        }
#endif

        mSubModules.emplace(subModule->getName(), subModule);
    }

    const std::unordered_map<util::HashedString, std::shared_ptr<Module> > &Module::getSubModules() const {
        return mSubModules;
    }

    const std::optional<Loc> &Module::getLoc() const {
        return mLoc;
    }

    const std::string &Module::getName() const {
        return mName;
    }

    ModuleType Module::getType() const {
        return mType;
    }
}
