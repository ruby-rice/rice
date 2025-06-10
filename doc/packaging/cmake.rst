.. _cmake:

CMake
=====
For more complex C++ extensions, you may wish to use ``CMake`` to build your extension. ``CMake`` has a lot of advantages over ``extconf.rb``:

* Is is likely the build system the library you are wrapping uses
* Provides built-in functionality for finding installed modules (like Ruby!)
* Provides much more control over the build process
* Parallelizes builds resulting in *much* faster compilation times compared to ``make``

Enabling
--------
To build your extension using ``CMake``, add the following to your Gemspec (assuming the extension directory is ``ext``):

.. code-block:: ruby

    Gem::Specification.new do |spec|
      spec.extensions = ["ext/CMakeLists.txt"]
    end

Unfortunately, ``RubyGems`` support for CMake is fairly poor. Therefore, Rice includes an improved RubyGem `CMakeBuilder <https://github.com/ruby-rice/rice/blob/master/lib/rubygems/cmake_builder.rb>`_. This updated code has been submitted `upstream <https://github.com/rubygems/rubygems/pull/8753>`_.

Rice uses RubyGem's `plugin <https://guides.rubygems.org/plugins/>`_ system to install the updated ``CMakeBuilder`` code. To use the improved builder include ``Rice`` in your gemspec like this:

.. code-block:: ruby

    Gem::Specification.new do |spec|
      spec.add_runtime_dependency('rice')
    end

Example
--------
Below is an example ``CMakeLists.txt`` file to build a Ruby C++ extension:

.. code-block:: cmake

    cmake_minimum_required (VERSION 3.26)

    project(MyExtensionLibrary)

    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

    target_include_directories(MyExtensionLibrary PRIVATE ${Ruby_INCLUDE_DIR} ${Ruby_CONFIG_INCLUDE_DIR})
    target_include_directories(MyExtensionLibrary PRIVATE ${PROJECT_SOURCE_DIR})
    target_link_libraries(MyExtensionLibrary ${Ruby_LIBRARY})

    # Add in Rice headers
    target_include_directories(MyExtensionLibrary PRIVATE <path>)

    # Add in the library you are wrapping headers and libraries
    target_include_directories(MyExtensionLibrary PRIVATE <path>)
    target_link_libraries(MyExtensionLibrary <path>})

Currently you will need to manually specify where the Rice :ref:`header_files` header files are located.

Compiler Settings
-----------------
See :ref:`compiler_settings` for details about compiler settings.

FindRuby
--------
Notice the inclusion of the following line in the above ``CMakeLists.txt`` file:

.. code-block:: cmake

    find_package("Ruby")

This will find a locally installed Ruby, whether it is the system Ruby or a `RVM <https://rvm.io/>`_ or `RbENV <https://rbenv.org/>`_ installed Ruby.

If you are using an older version of CMake, you can use the `FindRuby.cmake <https://github.com/ruby-rice/rice/blob/master/FindRuby.cmake>`_) script included in Rice. In that case, the syntax would be:

.. code-block:: cmake

     include("./FindRuby.cmake")

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



