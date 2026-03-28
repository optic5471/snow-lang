
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#include <testInfra/TestInfra.hpp>

#include <snowlib/src/ModulePath.hpp>

using namespace src;

namespace test {
    TEST_CLASS(ModulePathTests) {
        TEST(ModulePath_Empty_IsEmpty) {
            ModulePath p("");
            EXPECT_TRUE(p.mParts.empty());
        }

        TEST(ModulePath_One_HasOne) {
            ModulePath p("hello");
            ASSERT_EQ(1, p.mParts.size());
            EXPECT_EQ("hello", p.mParts[0].getStr());
        }

        // TODO: This should be considered an error in the file
        TEST(ModulePath_MultiSlash_DoesNotEmplaceEmpty) {
            ModulePath p("//hello////world");
            ASSERT_EQ(2, p.mParts.size());
            EXPECT_EQ("hello", p.mParts[0].getStr());
            EXPECT_EQ("world", p.mParts[1].getStr());
        }

        TEST(ModulePath_EndsWithSlash_Works) {
            ModulePath p("hello/");
            ASSERT_EQ(1, p.mParts.size());
            EXPECT_EQ("hello", p.mParts[0].getStr());
        }

        TEST(ModulePath_GeneralUsage_Works) {
            ModulePath p("one/two/three/four/five");
            ASSERT_EQ(5, p.mParts.size());
            EXPECT_EQ("one", p.mParts[0].getStr());
            EXPECT_EQ("two", p.mParts[1].getStr());
            EXPECT_EQ("three", p.mParts[2].getStr());
            EXPECT_EQ("four", p.mParts[3].getStr());
            EXPECT_EQ("five", p.mParts[4].getStr());
        }
    };
}