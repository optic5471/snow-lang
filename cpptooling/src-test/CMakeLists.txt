# Copyright (c) 2025 Andrew Griffin - All Rights Reserved

add_executable(UnitTest)
set_target_properties(UnitTest PROPERTIES WIN32_EXECUTABLE OFF)
message(STATUS "Creating UnitTest executable")

file(GLOB_RECURSE UnitTest_SourceFiles "*.cpp")
file(GLOB_RECURSE UnitTest_HeaderFiles "*.hpp")
target_sources(UnitTest
    PRIVATE ${UnitTest_SourceFiles}
    PUBLIC ${UnitTest_HeaderFiles}
)

include(${CMAKE_SOURCE_DIR}/cmake/SourceGrouping.cmake)

target_source_group(UnitTest ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(UnitTest PRIVATE CppUtils)
target_link_libraries(UnitTest PRIVATE CppUtils_UnitTest)
target_link_libraries(UnitTest PRIVATE SnowLib)
