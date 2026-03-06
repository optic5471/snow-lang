# Copyright (c) 2026 Andrew Griffin - All Rights Reserved

add_executable(snowc)
set_target_properties(snowc PROPERTIES WIN32_EXECUTABLE OFF)
message(STATUS "Creating snowc executable")

file(GLOB_RECURSE snowc_SourceFiles "*.cpp")
file(GLOB_RECURSE snowc_HeaderFiles "*.hpp")
target_sources(snowc
    PRIVATE ${snowc_SourceFiles}
    PUBLIC ${snowc_HeaderFiles}
)

include(${CMAKE_SOURCE_DIR}/cmake/sourceGrouping.cmake)

target_source_group(snowc ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(snowc PRIVATE CppUtils)
target_link_libraries(snowc PRIVATE SnowLib)
