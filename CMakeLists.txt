cmake_minimum_required(VERSION 3.26)

project(rice LANGUAGES CXX)

# Put actual target definitions in a separate file so the root stays tiny.
include(${CMAKE_CURRENT_SOURCE_DIR}/Rice.cmake)

# Default behavior:
# - If you're working on Rice directly (top-level), build tests by default (good for Visual Studio).
# - If Rice is consumed via FetchContent/add_subdirectory, do NOT build tests by default.
if (PROJECT_IS_TOP_LEVEL)
  option(RICE_BUILD_TESTS "Build Rice tests" ON)
else ()
  option(RICE_BUILD_TESTS "Build Rice tests" OFF)
endif ()

if (RICE_BUILD_TESTS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test/CMakeLists.txt")
  add_subdirectory(test)
endif ()
