
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#include <testInfra/TestInfra.hpp>

#include <snowlib/src/Module.hpp>
#include <snowlib/src/Package.hpp>

namespace test {
    TEST_CLASS(PackageModuleNameResolutionTests, "src") {
        TEST(ModuleNameRes_EmptyName_Nullptr) {
            src::Package package("package",
                std::make_shared<src::Module>(
                    src::Loc(0,0),
                    "package",
                    src::ModuleType::FileScope
                ));
            EXPECT_NULL(package.tryResolveModule(""));
        }

        TEST(ModuleNameRes_PackageName_RootModule) {
            src::Package package("package",
                std::make_shared<src::Module>(
                    src::Loc(0,0),
                    "package",
                    src::ModuleType::FileScope
                ));

            auto mod = package.tryResolveModule("package");
            ASSERT_NOTNULL(mod);
            EXPECT_EQ("package", mod->getName());
        }

        TEST(ModuleNameRes_ModuleName_GetsModule) {
            src::Package package("package",
                std::make_shared<src::Module>(
                    src::Loc(0,0),
                    "package",
                    src::ModuleType::FileScope
                )
            );
            package.addModule(std::make_shared<src::Module>(
                src::Loc(0,0),
                "mod",
                src::ModuleType::FileScope
            ));

            auto mod = package.tryResolveModule("package/mod");
            ASSERT_NOTNULL(mod);
            EXPECT_EQ("mod", mod->getName());
        }

        TEST(ModuleNameRes_SubModuleName_GetsSubModule) {
            src::Package package("package",
                std::make_shared<src::Module>(
                    src::Loc(0,0),
                    "package",
                    src::ModuleType::FileScope
                )
            );

            auto module = std::make_shared<src::Module>(
                src::Loc(0,0),
                "mod",
                src::ModuleType::FileScope
            );
            package.addModule(module);

            module->addSubModule(std::make_shared<src::Module>(
                src::Loc(0,0),
                "sub",
                src::ModuleType::Submodule
            ));

            auto mod = package.tryResolveModule("package/mod/sub");
            ASSERT_NOTNULL(mod);
            EXPECT_EQ("sub", mod->getName());
        }

        TEST(ModuleNameRes_StressTest_GetsModule) {
            src::Package package("a",
                std::make_shared<src::Module>(
                    src::Loc(0,0),
                    "a",
                    src::ModuleType::FileScope
                )
            );

            auto b = std::make_shared<src::Module>(
                src::Loc(0,0),
                "b",
                src::ModuleType::FileScope
            );
            package.addModule(b);

            auto c = std::make_shared<src::Module>(src::Loc(0, 0), "c", src::ModuleType::Submodule);
            auto d = std::make_shared<src::Module>(src::Loc(0, 0), "d", src::ModuleType::Submodule);
            auto e = std::make_shared<src::Module>(src::Loc(0, 0), "e", src::ModuleType::Submodule);
            auto f = std::make_shared<src::Module>(src::Loc(0, 0), "f", src::ModuleType::Submodule);
            auto g = std::make_shared<src::Module>(src::Loc(0, 0), "g", src::ModuleType::Submodule);
            auto h = std::make_shared<src::Module>(src::Loc(0, 0), "h", src::ModuleType::Submodule);
            auto i = std::make_shared<src::Module>(src::Loc(0, 0), "i", src::ModuleType::Submodule);
            auto j = std::make_shared<src::Module>(src::Loc(0, 0), "j", src::ModuleType::Submodule);
            auto k = std::make_shared<src::Module>(src::Loc(0, 0), "k", src::ModuleType::Submodule);
            auto l = std::make_shared<src::Module>(src::Loc(0, 0), "l", src::ModuleType::Submodule);
            b->addSubModule(c);
            c->addSubModule(d);
            d->addSubModule(e);
            e->addSubModule(f);
            f->addSubModule(g);
            g->addSubModule(h);
            h->addSubModule(i);
            i->addSubModule(j);
            j->addSubModule(k);
            k->addSubModule(l);

            auto mod = package.tryResolveModule("a/b/c/d/e/f/g/h/i/j/k/l");
            ASSERT_NOTNULL(mod);
            EXPECT_EQ("l", mod->getName());
        }

        TEST(ModuleNameRes_NotModuleName_Nullptr) {
            src::Package package("a",
                std::make_shared<src::Module>(
                    src::Loc(0,0),
                    "a",
                    src::ModuleType::FileScope
                )
            );

            EXPECT_NULL(package.tryResolveModule("a/b"));
        }

        TEST(ModuleNameRes_NotSubModuleName_Nullptr) {
            src::Package package("package",
                std::make_shared<src::Module>(
                    src::Loc(0,0),
                    "package",
                    src::ModuleType::FileScope
                )
            );
            package.addModule(std::make_shared<src::Module>(
                src::Loc(0,0),
                "mod",
                src::ModuleType::FileScope
            ));

            EXPECT_NULL(package.tryResolveModule("package/mod/sub"));
        }

        TEST(ModuleNameRes_StressTestNotName_Nullptr) {
            src::Package package("a",
                std::make_shared<src::Module>(
                    src::Loc(0,0),
                    "a",
                    src::ModuleType::FileScope
                )
            );

            auto b = std::make_shared<src::Module>(
                src::Loc(0,0),
                "b",
                src::ModuleType::FileScope
            );
            package.addModule(b);

            auto c = std::make_shared<src::Module>(src::Loc(0, 0), "c", src::ModuleType::Submodule);
            auto d = std::make_shared<src::Module>(src::Loc(0, 0), "d", src::ModuleType::Submodule);
            auto e = std::make_shared<src::Module>(src::Loc(0, 0), "e", src::ModuleType::Submodule);
            auto f = std::make_shared<src::Module>(src::Loc(0, 0), "f", src::ModuleType::Submodule);
            auto g = std::make_shared<src::Module>(src::Loc(0, 0), "g", src::ModuleType::Submodule);
            auto h = std::make_shared<src::Module>(src::Loc(0, 0), "h", src::ModuleType::Submodule);
            auto i = std::make_shared<src::Module>(src::Loc(0, 0), "i", src::ModuleType::Submodule);
            auto j = std::make_shared<src::Module>(src::Loc(0, 0), "j", src::ModuleType::Submodule);
            auto k = std::make_shared<src::Module>(src::Loc(0, 0), "k", src::ModuleType::Submodule);
            auto l = std::make_shared<src::Module>(src::Loc(0, 0), "l", src::ModuleType::Submodule);
            b->addSubModule(c);
            c->addSubModule(d);
            d->addSubModule(e);
            e->addSubModule(f);
            f->addSubModule(g);
            g->addSubModule(h);
            h->addSubModule(i);
            i->addSubModule(j);
            j->addSubModule(k);
            k->addSubModule(l);

            EXPECT_NULL(package.tryResolveModule("a/b/c/d/e/f/g/h/i/j/k/l/m"));
        }
    };
}
