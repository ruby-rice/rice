cmake_minimum_required (VERSION 3.26)

project(Rice)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# For older versions of CMake can use include("./FindRuby.cmake")
find_package("Ruby")

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
    add_compile_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE)
    add_compile_options(/bigobj /utf-8)
    # The default of /EHsc crashes Ruby when calling longjmp with optimizations on (/O2)
    string(REGEX REPLACE "/EHsc" "/EHs" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_definitions(-D_CRT_SECURE_NO_WARNINGS)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-ftemplate-backtrace-limit=0)
    # https://github.com/doxygen/doxygen/issues/9269#issuecomment-1094975328
    #add_compile_options(unittest PRIVATE -Wa,-mbig-obj)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og")
endif ()

# Include sub-projects.
add_subdirectory ("test")
#add_subdirectory ("sample/callbacks")
#add_subdirectory ("sample/enum")
#add_subdirectory ("sample/inheritance")
#add_subdirectory ("sample/map")
