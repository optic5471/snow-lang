# Copyright (c) 2026 Andrew Griffin - All Rights Reserved

cmake_minimum_required(VERSION 4.1.0)
include(${CMAKE_SOURCE_DIR}/cmake/log.cmake)

message(STATUS "Main Cmake Running")
message(STATUS "Logging to " ${LOG_FILE_PATH})

if (TEST_ENABLED)
    message(STATUS ${clitext_Green} "Tests are enabled" ${clitext_Reset})
    add_compile_definitions(
        TEST_ENABLED=true
    )
else()
    message(STATUS ${clitext_Yellow} "Tests are disabled" ${clitext_Reset})
endif()

project(SnowCompilerCppTooling CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (TEST_ENABLED)
    include_directories(src-test)
endif()
include_directories(src)
include_directories(src-external/cpp-utils/src/src-utils)

add_subdirectory(src)
add_subdirectory(src-external)

if (TEST_ENABLED)
    add_subdirectory(src-test)
endif()

if (TEST_ENABLED)
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT UnitTest)
else()
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT snowc)
endif()