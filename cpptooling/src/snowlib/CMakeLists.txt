# Copyright (c) 2026 Andrew Griffin - All Rights Reserved

add_library(SnowLib)
set_target_properties(SnowLib PROPERTIES WIN32_EXECUTABLE OFF)
message(STATUS "Creating SnowLib library")

file(GLOB_RECURSE SnowLib_SourceFiles "*.cpp")
file(GLOB_RECURSE SnowLib_HeaderFiles "*.hpp")

target_sources(SnowLib
    PRIVATE ${SnowLib_SourceFiles}
    PUBLIC ${SnowLib_HeaderFiles}
)

include(${CMAKE_SOURCE_DIR}/cmake/sourceGrouping.cmake)

target_source_group(SnowLib ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(SnowLib PRIVATE CppUtils)
