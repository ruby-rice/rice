cmake_minimum_required(VERSION 3.26)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MESSAGE_LOG_LEVEL DEBUG)

project(unittest)

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

# ----- Dependencies ------
#target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Ruby
#find_package("Ruby")
include("../FindRuby.cmake")
target_include_directories(${PROJECT_NAME} PRIVATE ${Ruby_INCLUDE_DIR} ${Ruby_CONFIG_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${Ruby_LIBRARY})

# FFI
if (MSVC) # This will include Clang on Windows
    find_package(unofficial-libffi CONFIG REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE unofficial::libffi::libffi)
    target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_LIBFFI)
else()
    find_library(FFI_LIBRARY NAMES ffi REQUIRED)
    target_link_libraries(${PROJECT_NAMEPROJECT_NAME} PRIVATE ${FFI_LIBRARY})
    target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_LIBFFI)
endif()

# Rice
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

# ---- Compiler Settings ---------
if (MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
            _CRT_SECURE_NO_DEPRECATE
            _CRT_NONSTDC_NO_DEPRECATE)

     target_compile_options(${PROJECT_NAME} PRIVATE 
            /W4        # Warning level 4
            /bigobj    # Increase number of sections in .obj file
            /utf-8)    # Set source and execution character sets to UTF-8
    
    # Edit and Continue for Debug builds
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT 
        "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
    
    # Fix exception handling for Ruby's longjmp
    # The default of /EHsc crashes Ruby when calling longjmp with optimizations on (/O2)
    # So replace /EHsc with /EHs just for this target
    get_target_property(_unittest_cxx_opts ${PROJECT_NAME} COMPILE_OPTIONS)
    if(_unittest_cxx_opts)
        list(TRANSFORM _unittest_cxx_opts REPLACE "/EHsc" "/EHs")
        set_target_properties(${PROJECT_NAME}  PROPERTIES COMPILE_OPTIONS "${_unittest_cxx_opts}")
    endif()
    #string(REGEX REPLACE "/EHsc" "/EHs" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else ()   
    target_compile_options(${PROJECT_NAME} PRIVATE
            -Wall
            -Wno-unused-private-field
            -ftemplate-backtrace-limit=0
            -Wa,-mbig-obj # https://github.com/doxygen/doxygen/issues/9269#issuecomment-1094975328
            $<$<CONFIG:Debug>:-Og>)
endif ()
