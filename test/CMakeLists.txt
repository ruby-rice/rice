cmake_minimum_required(VERSION 3.26)

project(RiceUnitTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# For older versions of CMake can use include("./FindRuby.cmake")
find_package("Ruby")

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

# Include sub-directories.
#add_subdirectory("ext/t1")
#add_subdirectory("ext/t2")

# Add source to this project's executable.
add_executable(unittest
               "unittest.cpp"
               "embed_ruby.cpp"
               "test_Template.cpp"
               "test_Stl_Map.cpp"
)

target_include_directories(unittest PRIVATE ..)
#target_compile_definitions(unittest PRIVATE -DHAVE_LIBFFI)
target_include_directories(unittest PRIVATE ${Ruby_INCLUDE_DIR} ${Ruby_CONFIG_INCLUDE_DIR})
target_include_directories(unittest PRIVATE ${PROJECT_SOURCE_DIR})
target_link_directories(unittest PRIVATE c:/msys64/ucrt64/lib)
target_link_libraries(unittest ${Ruby_LIBRARY} ffi)