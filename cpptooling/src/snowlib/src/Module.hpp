
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
    class Module {
        // points at the first character inside the module (maybe whitespace).
        // For a file, this is the beginning of the file
        // For a submodule in a file, this is the character after {.
        // If this points at }, then this is an empty module (not valid for file)
        Loc mLoc;
        std::string mName;
        std::shared_ptr<ast::BaseNode> mAst;
        std::unordered_map<util::HashedString, std::shared_ptr<Module>> mSubModules;

    public:
        Module(Loc loc, const std::string& name);
        void setAstRoot(std::shared_ptr<ast::BaseNode> ast);
        std::shared_ptr<ast::BaseNode>& getAst();
        const std::shared_ptr<ast::BaseNode>& getAst() const;
        void addSubModule(std::shared_ptr<Module> subModule);
        const std::unordered_map<util::HashedString, std::shared_ptr<Module>>& getSubModules() const;
        std::shared_ptr<Module> getSubModule(const util::HashedString& moduleName);
        const Loc& getLoc() const;
        const std::string& getName() const;
    };
}
