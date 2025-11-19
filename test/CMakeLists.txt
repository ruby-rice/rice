cmake_minimum_required(VERSION 3.26)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MESSAGE_LOG_LEVEL DEBUG)

project(unittest)

# Add source to this project's executable.
add_executable(unittest
               "unittest.cpp"
               "embed_ruby.cpp"
               "test_Address_Registration_Guard.cpp"
               "test_Array.cpp"
               "test_Attribute.cpp"
               "test_Buffer.cpp"
               "test_Builtin_Object.cpp"
               "test_Callback.cpp"
               "test_Class.cpp"
               "test_Constructor.cpp"
               "test_Data_Object.cpp"
               "test_Data_Type.cpp"
               "test_Director.cpp"
               "test_Enum.cpp"
               "test_Exception.cpp"
               "test_File.cpp"
               "test_From_Ruby.cpp"
               "test_GVL.cpp"
               "test_Hash.cpp"
               "test_global_functions.cpp"
               "test_Identifier.cpp"
               "test_Inheritance.cpp"
               "test_Iterator.cpp"
               "test_Jump_Exception.cpp"
               "test_Keep_Alive.cpp"
               "test_Keep_Alive_No_Wrapper.cpp"
               "test_Memory_Management.cpp"
               "test_Module.cpp"
               "test_Native_Registry.cpp"
               "test_Object.cpp"
               "test_Overloads.cpp"
               "test_Ownership.cpp"
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
               "test_Stl_Multimap.cpp"
               "test_Stl_Optional.cpp"
               "test_Stl_Pair.cpp"
               "test_Stl_Reference_Wrapper.cpp"
               "test_Stl_Set.cpp"
               "test_Stl_SharedPtr.cpp"
               "test_Stl_String.cpp"
               "test_Stl_String_View.cpp"
               "test_Stl_Tuple.cpp"
               "test_Stl_Type_Info.cpp"
               "test_Stl_UniquePtr.cpp"
               "test_Stl_Unordered_Map.cpp"
               "test_Stl_Variant.cpp"
               "test_Stl_Vector.cpp")

#find_package("Ruby")
include("../FindRuby.cmake")

target_include_directories(unittest PRIVATE ${Ruby_INCLUDE_DIR} ${Ruby_CONFIG_INCLUDE_DIR})
target_include_directories(unittest PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(unittest ${Ruby_LIBRARY} ffi)

target_include_directories(unittest PRIVATE ..)
target_compile_definitions(unittest PRIVATE -DHAVE_LIBFFI)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
  add_compile_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE)
  add_compile_options(/W4 /bigobj /utf-8)
  # The default of /EHsc crashes Ruby when calling longjmp with optimizations on (/O2)
  string(REGEX REPLACE "/EHsc" "/EHs" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  add_compile_definitions(-D_CRT_SECURE_NO_WARNINGS)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options(-Wall -ftemplate-backtrace-limit=0)
  # https://github.com/doxygen/doxygen/issues/9269#issuecomment-1094975328
  #add_compile_options(unittest PRIVATE -Wa,-mbig-obj)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og")
endif ()
