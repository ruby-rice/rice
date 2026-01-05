cmake_minimum_required(VERSION 3.26)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MESSAGE_LOG_LEVEL DEBUG)

project(unittest LANGUAGES CXX)

# ----- Dependencies ------
#target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Ruby
#find_package(Ruby Required)
include("../FindRuby.cmake")
target_link_libraries(${PROJECT_NAME} PRIVATE Ruby::Ruby)

# FFI
if (MSVC) # This will include Clang on Windows
    find_package(unofficial-libffi CONFIG REQUIRED)
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE unofficial::libffi::libffi)
else()
    # Use pkg-config on Unix-like systems (Linux, macOS, MinGW)
    find_package(PkgConfig QUIET)
    if (PkgConfig_FOUND)
        pkg_check_modules(FFI REQUIRED IMPORTED_TARGET libffi)
        target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE PkgConfig::FFI)
    else()
        # Fallback for systems without pkg-config
        find_library(FFI_LIBRARY NAMES ffi REQUIRED)
        target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${FFI_LIBRARY})
    endif()
endif()
target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_LIBFFI)

# Rice
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

# ----- Sources ------
# Add source to this project's executable.
add_executable(${PROJECT_NAME}
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
               "test_Reference.cpp"
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
