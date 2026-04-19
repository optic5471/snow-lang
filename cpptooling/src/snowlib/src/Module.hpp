
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#pragma once

#include <optional>
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
        File,       // This is a file on the file system, the entire files is treated as a module of the file name
        Submodule,  // This is a submodule in a file, has no file system counterpart
        Folder      // This is a folder on the file system, these are treated as modules with files being their submodules
    };

    class Module {
        // Points at the first character in the file, or at the module name, or is empty if a folder
        std::optional<Loc> mLoc;
        ModuleType mType;
        std::string mName;
        std::shared_ptr<ast::BaseNode> mAst;
        std::unordered_map<util::HashedString, std::shared_ptr<Module>> mSubModules;

    public:
        Module(std::optional<Loc> loc, const std::string& name, ModuleType type);
        void setAstRoot(std::shared_ptr<ast::BaseNode> ast);
        std::shared_ptr<ast::BaseNode>& getAst();
        const std::shared_ptr<ast::BaseNode>& getAst() const;
        void addSubModule(std::shared_ptr<Module> subModule);
        const std::unordered_map<util::HashedString, std::shared_ptr<Module>>& getSubModules() const;
        const std::optional<Loc>& getLoc() const;
        const std::string& getName() const;
        ModuleType getType() const;
    };
}
