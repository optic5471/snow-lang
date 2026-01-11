# Copyright (c) 2025 Andrew Griffin - All Rights Reserved

add_library(CppUtils)
set_target_properties(CppUtils PROPERTIES WIN32_EXECUTABLE OFF)
message(STATUS "Creating CppUtils library")

file(GLOB_RECURSE CppUtils_SourceFiles "cpp-utils/src/src-utils/*.cpp")
file(GLOB_RECURSE CppUtils_HeaderFiles "cpp-utils/src/src-utils/*.hpp")

if (NOT TEST_ENABLED)
    file(GLOB_RECURSE CppUtils_TestSourceFiles "cpp-utils/src/src-utils/test/*.cpp")
    file(GLOB_RECURSE CppUtils_TestHeaderFiles "cpp-utils/src/src-utils/test/*.hpp")
    list(REMOVE_ITEM CppUtils_SourceFiles ${CppUtils_TestSourceFiles})
    list(REMOVE_ITEM CppUtils_HeaderFiles ${CppUtils_TestHeaderFiles})
endif()

target_sources(CppUtils
    PRIVATE ${CppUtils_SourceFiles}
    PUBLIC ${CppUtils_HeaderFiles}
)

include(${CMAKE_SOURCE_DIR}/cmake/SourceGrouping.cmake)

target_source_group(CppUtils ${CMAKE_CURRENT_SOURCE_DIR}/cpp-utils/src/src-utils)


# ================================================================================
if (TEST_ENABLED)
    add_library(CppUtils_UnitTest)
    set_target_properties(CppUtils_UnitTest PROPERTIES WIN32_EXECUTABLE OFF)
    message(STATUS "Creating CppUtils_UnitTest library")

    file(GLOB_RECURSE CppUtilsUnitTest_SourceFiles "cpp-utils/src/src-test/tests/util/*.cpp")
    file(GLOB_RECURSE CppUtilsUnitTest_HeaderFiles "cpp-utils/src/src-test/tests/util/*.hpp")

    target_sources(CppUtils_UnitTest
        PRIVATE ${CppUtilsUnitTest_SourceFiles}
        PUBLIC ${CppUtilsUnitTest_HeaderFiles}
    )

    include(${CMAKE_SOURCE_DIR}/cmake/SourceGrouping.cmake)

    target_source_group(CppUtils_UnitTest ${CMAKE_CURRENT_SOURCE_DIR}/cpp-utils/src/src-test/tests/util)

    target_link_libraries(CppUtils_UnitTest PRIVATE CppUtils)
endif()


# ================================================================================
