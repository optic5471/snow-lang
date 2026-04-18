
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#pragma once

#include <memory>
#include <unordered_map>

#include <snowlib/src/Loc.hpp>
#include <util/HashedString.hpp>

namespace ast {
    class BaseNode;
}

namespace src {
    struct ModulePath;

    enum class ModuleType : char {
        FileScope,
        Submodule
    };

    class Module {
        // Points at the first character in the file, or at the module name
        Loc mLoc;
        ModuleType mType;
        std::string mName;
        std::shared_ptr<ast::BaseNode> mAst;
        std::unordered_map<util::HashedString, std::shared_ptr<Module>> mSubModules;

    public:
        Module(Loc loc, const std::string& name, ModuleType type);
        void setAstRoot(std::shared_ptr<ast::BaseNode> ast);
        std::shared_ptr<ast::BaseNode>& getAst();
        const std::shared_ptr<ast::BaseNode>& getAst() const;
        void addSubModule(std::shared_ptr<Module> subModule);
        const std::unordered_map<util::HashedString, std::shared_ptr<Module>>& getSubModules() const;
        const Loc& getLoc() const;
        const std::string& getName() const;
        ModuleType getType() const;
    };
}
