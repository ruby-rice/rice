.. _cmake:

CMake
=====
Since Rice is header library, it is easy to use alternative build systems instead of Ruby and mkmf-rice. Rice itself uses CMake for building its test suite.

Getting started with CMake is easy. An example CMakeLists.txt file is shown below:

.. code-block:: cmake

    cmake_minimum_required (VERSION 3.26)

    project(MyExtensionLibrary)

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
        add_compile_options(unittest PRIVATE -Wa,-mbig-obj)
    endif ()

    add_library (MyExtension SHARED
                 "MyExtension.cpp")

    target_include_directories(sample_callbacks PRIVATE ${Ruby_INCLUDE_DIR} ${Ruby_CONFIG_INCLUDE_DIR})
    target_include_directories(sample_callbacks PRIVATE ${PROJECT_SOURCE_DIR})
    target_link_libraries(sample_callbacks ${Ruby_LIBRARY})

    target_include_directories(sample_callbacks PRIVATE rice)

Note that Rice requires C++17. Then it search for Ruby, sets various compiler settings and finally defines a new library called MyExtension which is a Ruby extension.

FindRuby
--------
After creating a CMake project, you will want to locate Ruby. This is done like this:

.. code-block:: cmake

    # For older versions of CMake can use include("./FindRuby.cmake")
    find_package("Ruby")

This sets the following CMake CACHE variables:

========================== ================
Variable                   Description
========================== ================
Ruby_VERSION_MAJOR         Ruby major version
Ruby_VERSION_MINOR         Ruby minor version
Ruby_VERSION_PATCH         Ruby patch version
Ruby_ARCH_DIR              Ruby arch dir
Ruby_HDR_DIR               Ruby header dir (1.9+)
Ruby_ARCHHDR_DIR           Ruby arch header dir (2.0+)
Ruby_RUBY_LIB_DIR          Ruby ruby-lib dir
Ruby_SITEARCH_DIR          Ruby site arch dir
Ruby_SITELIB_DIR           Ruby site lib dir
Ruby_EXECUTABLE            Ruby interpreter
Ruby_LIBRARY               Ruby shared library
Ruby_INCLUDE_DIR           Ruby include directory
Ruby_CONFIG_INCLUDE_DIR    Ruby config include directory
Ruby_INCLUDE_DIRS          Include and config directories
========================== ================

Compiler Settings
-----------------
On Windows, for both GCC (MinGW) and MSVC you have to turn on big object support. For MSVC, you will want to turn on UTF-8 since there are UTF-8 characters in the comments of the source code. See :ref:`stl_class_names`.

