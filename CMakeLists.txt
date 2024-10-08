﻿cmake_minimum_required (VERSION 3.26)

project(Rice)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include("./FindRuby.cmake")

if (MSVC)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
    add_compile_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE)
    add_compile_options(/bigobj)
else (GCC)
    add_compile_options(-ftemplate-backtrace-limit=0)
    # https://github.com/doxygen/doxygen/issues/9269#issuecomment-1094975328
    #add_compile_options(unittest PRIVATE -Wa,-mbig-obj)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og")
endif ()

# Include sub-projects.
add_subdirectory ("test")
add_subdirectory ("sample/callbacks")
add_subdirectory ("sample/enum")
add_subdirectory ("sample/inheritance")
add_subdirectory ("sample/map")
