cmake_minimum_required(VERSION 3.26)

project(RiceUnitTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# For older versions of CMake can use include("./FindRuby.cmake")
find_package("Ruby")

if (MSVC)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
  add_compile_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE)
  add_compile_options(/bigobj)
  # The default of /EHsc crashes Ruby when calling longjmp with optimizations on (/O2)
  string(REGEX REPLACE "/EHsc" "/EHs" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else (GCC)
  add_compile_options(-ftemplate-backtrace-limit=0)
  # https://github.com/doxygen/doxygen/issues/9269#issuecomment-1094975328
  #add_compile_options(unittest PRIVATE -Wa,-mbig-obj)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og")
endif ()

# Include sub-directories.
add_subdirectory("ext/t1")
add_subdirectory("ext/t2")

# Add source to this project's executable.
add_executable(unittest
               "unittest.cpp"
               "embed_ruby.cpp"
               "test_Enum.cpp"
               "test_Exception.cpp"
               "test_File.cpp"
               "test_From_Ruby.cpp"
               "test_Hash.cpp"
               "test_global_functions.cpp"
               "test_Identifier.cpp"
               "test_Inheritance.cpp"
               "test_Iterator.cpp"
               "test_JumpException.cpp"
               "test_Keep_Alive.cpp"
               "test_Memory_Management.cpp"
               "test_Module.cpp"
               "test_Object.cpp"
               "test_Ownership.cpp"
               "test_Overloads.cpp"
               "test_Proc.cpp"
               "test_Self.cpp"
               "test_String.cpp"
               "test_Struct.cpp"
               "test_Symbol.cpp"
               "test_Template.cpp"
               "test_To_Ruby.cpp"
               "test_Tracking.cpp"
               "test_Type.cpp"
               "test_Stl_Exception.cpp"
               "test_Stl_Map.cpp"
               "test_Stl_Optional.cpp"
               "test_Stl_Pair.cpp"
               "test_Stl_Reference_Wrapper.cpp"
               "test_Stl_SmartPointer.cpp"
               "test_Stl_String.cpp"
               "test_Stl_String_View.cpp"
               "test_Stl_Type.cpp"
               "test_Stl_Unordered_Map.cpp"
               "test_Stl_Variant.cpp"
               "test_Stl_Vector.cpp")


target_include_directories(unittest PRIVATE ..)
target_include_directories(unittest PRIVATE ${Ruby_INCLUDE_DIR} ${Ruby_CONFIG_INCLUDE_DIR})
target_include_directories(unittest PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(unittest ${Ruby_LIBRARY})