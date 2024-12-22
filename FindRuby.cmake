# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#[=======================================================================[.rst:
FindRuby
--------

This module determines if Ruby is installed and finds the locations of its
include files and libraries. Ruby 1.8 through 3.4 are supported.

The minimum required version of Ruby can be specified using the
standard syntax, e.g.

.. code-block:: cmake

  find_package(Ruby 3.2.6 EXACT REQUIRED)
  # OR
  find_package(Ruby 3.2)

Virtual environments, such as RVM or RBENV, are supported.

Result Variables
^^^^^^^^^^^^^^^^

This module will set the following variables in your project:

``Ruby_FOUND``
  set to true if ruby was found successfully
``Ruby_EXECUTABLE``
  full path to the ruby binary
``Ruby_INCLUDE_DIRS``
  include dirs to be used when using the ruby library
``Ruby_LIBRARIES``
  .. versionadded:: 3.18
    libraries needed to use ruby from C.
``Ruby_VERSION``
  the version of ruby which was found, e.g. "3.2.6"
``Ruby_VERSION_MAJOR``
  Ruby major version.
``Ruby_VERSION_MINOR``
  Ruby minor version.
``Ruby_VERSION_PATCH``
  Ruby patch version.

.. versionchanged:: 3.18
  Previous versions of CMake used the ``RUBY_`` prefix for all variables.
  The following variables are provided for compatibility reasons,
  don't use them in new code:

``RUBY_EXECUTABLE``
  same as Ruby_EXECUTABLE.
``RUBY_INCLUDE_DIRS``
  same as Ruby_INCLUDE_DIRS.
``RUBY_INCLUDE_PATH``
  same as Ruby_INCLUDE_DIRS.
``RUBY_LIBRARY``
  same as Ruby_LIBRARY.
``RUBY_VERSION``
  same as Ruby_VERSION.
``RUBY_FOUND``
  same as Ruby_FOUND.

Hints
^^^^^

.. versionadded:: 3.18

``Ruby_FIND_VIRTUALENV``
  This variable defines the handling of virtual environments. Currently ``rvm` and
  ``rbenv`` are supported. ``rvm`` support requires that the ``MY_RUBY_HOME`` environment
  variable is set while ``rbenv`` support requires that rbenv is installed at
  "~/.rbenv/bin" or the ``RBENV_ROOT`` environment variable is set.

  It can be left empty or be set to one of the following values:

  * ``FIRST``: Virtual Ruby environments are searched for first, then the system Ruby installation.
               This is the default.
  * ``ONLY``: Only virtual environments are searched
  * ``STANDARD``: Only the system Ruby installation is searched.

#]=======================================================================]

# Backwards compatibility
# Define camel case versions of input variables
foreach(UPPER
    RUBY_EXECUTABLE
    RUBY_LIBRARY
    RUBY_INCLUDE_DIR
    RUBY_CONFIG_INCLUDE_DIR
    )
    if (DEFINED ${UPPER})
      string(REPLACE "RUBY_" "Ruby_" Camel ${UPPER})
        if (NOT DEFINED ${Camel})
            set(${Camel} ${${UPPER}})
        endif()
    endif()
endforeach()

#   Ruby_ARCHDIR=`$RUBY -r rbconfig -e 'printf("%s",Config::CONFIG@<:@"archdir"@:>@)'`
#   Ruby_SITEARCHDIR=`$RUBY -r rbconfig -e 'printf("%s",Config::CONFIG@<:@"sitearchdir"@:>@)'`
#   Ruby_SITEDIR=`$RUBY -r rbconfig -e 'printf("%s",Config::CONFIG@<:@"sitelibdir"@:>@)'`
#   Ruby_LIBDIR=`$RUBY -r rbconfig -e 'printf("%s",Config::CONFIG@<:@"libdir"@:>@)'`
#   Ruby_LIBRUBYARG=`$RUBY -r rbconfig -e 'printf("%s",Config::CONFIG@<:@"LIBRUBYARG_SHARED"@:>@)'`

# uncomment the following line to get debug output for this file
# set(_Ruby_DEBUG_OUTPUT TRUE)

# Determine the list of possible names of the ruby executable depending
# on which version of ruby is required
set(_Ruby_POSSIBLE_EXECUTABLE_NAMES ruby)

# Set name of possible executables, ignoring the minor
# Eg:
# 3.2.6 => from ruby34 to ruby32 included
# 3.2   => from ruby34 to ruby32 included
# 3     => from ruby34 to ruby30 included
# empty => from ruby34 to ruby18 included
if(NOT Ruby_FIND_VERSION_EXACT)
  foreach(_ruby_version RANGE 34 18 -1)
    string(SUBSTRING "${_ruby_version}" 0 1 _ruby_major_version)
    string(SUBSTRING "${_ruby_version}" 1 1 _ruby_minor_version)
    # Append both rubyX.Y and rubyXY (eg: ruby2.7 ruby27)
    list(APPEND _Ruby_POSSIBLE_EXECUTABLE_NAMES ruby${_ruby_major_version}.${_ruby_minor_version} ruby${_ruby_major_version}${_ruby_minor_version})
  endforeach()
endif()

# Virtual environment handling
if (DEFINED Ruby_FIND_VIRTUALENV AND NOT Ruby_FIND_VIRTUALENV MATCHES "^(FIRST|ONLY|STANDARD)$")
  message (AUTHOR_WARNING "FindRuby: ${Ruby_FIND_VIRTUALENV}: invalid value for 'Ruby_FIND_VIRTUALENV'. 'FIRST', 'ONLY' or 'STANDARD' expected. 'FIRST' will be used instead.")
  set (Ruby_FIND_VIRTUALENV "FIRST")
elseif (NOT DEFINED Ruby_FIND_VIRTUALENV)
  # Default is to search for virtual environments first
  set (Ruby_FIND_VIRTUALENV "FIRST")
endif()

function (_RUBY_VALIDATE_INTERPRETER path)
  if (NOT path)
    message(DEBUG "Ruby path not specified")
    return()
  endif()

  if (NOT EXISTS "${path}")
    message(DEBUG "Ruby not found. Path: \"${path}\"")
    return()
  endif()

  # Check the version it returns
  # executable found must have a specific version
  execute_process (COMMAND "${path}" -e "puts RUBY_VERSION"
                   RESULT_VARIABLE result
                   OUTPUT_VARIABLE version
                   ERROR_QUIET
                   OUTPUT_STRIP_TRAILING_WHITESPACE)

  if (result)
    set (_Ruby_Interpreter_REASON_FAILURE "Cannot use the interpreter \"${path}\"")
    return()
  endif()

  if (Ruby_FIND_VERSION)
    if (Ruby_FIND_VERSION_EXACT AND NOT version VERSION_EQUAL Ruby_FIND_VERSION)
      message(DEBUG "Incorrect Ruby found. Requested: ${Ruby_FIND_VERSION}. Found: ${version}. Path: \"${path}\"")
      return()
    elseif (version VERSION_LESS Ruby_FIND_VERSION)
      message(DEBUG "Ruby version is too old. Minimum: ${Ruby_FIND_VERSION}. Found: ${version}. Path: \"${path}\"")
      return()
    endif()
  endif()

  unset (_Ruby_Interpreter_REASON_FAILURE)
  set(Ruby_EXECUTABLE "${path}" CACHE INTERNAL "Ruby Executable")
endfunction()

function(_RUBY_CONFIG_VAR RBVAR OUTVAR)
  execute_process(COMMAND ${Ruby_EXECUTABLE} -r rbconfig -e "print RbConfig::CONFIG['${RBVAR}']"
    RESULT_VARIABLE _Ruby_SUCCESS
    OUTPUT_VARIABLE _Ruby_OUTPUT
    ERROR_QUIET)
  if(_Ruby_SUCCESS OR _Ruby_OUTPUT STREQUAL "")
    execute_process(COMMAND ${Ruby_EXECUTABLE} -r rbconfig -e "print Config::CONFIG['${RBVAR}']"
      RESULT_VARIABLE _Ruby_SUCCESS
      OUTPUT_VARIABLE _Ruby_OUTPUT
      ERROR_QUIET)
  endif()
  set(${OUTVAR} "${_Ruby_OUTPUT}" PARENT_SCOPE)
endfunction()

function(_RUBY_CLEAR_VARS)
  unset(Ruby_EXECUTABLE CACHE)
  unset(Ruby_LIBRARY CACHE)
  unset(Ruby_VERSION PARENT_SCOPE)
  unset(Ruby_ENV CACHE)

  unset(Ruby_VERSION_MAJOR CACHE)
  unset(Ruby_VERSION_MINOR CACHE)
  unset(Ruby_VERSION_PATCH CACHE)
  unset(Ruby_ARCH_DIR CACHE)
  unset(Ruby_HDR_DIR CACHE)
  unset(Ruby_ARCHHDR_DIR CACHE)
  unset(Ruby_INCLUDE_DIR CACHE)
  unset(Ruby_CONFIG_INCLUDE_DIR CACHE)
  unset(_Ruby_POSSIBLE_LIB_DIR CACHE)
  unset(Ruby_RUBY_LIB_DIR CACHE)
  unset(Ruby_SITEARCH_DIR CACHE)
  unset(Ruby_SITELIB_DIR CACHE)
  unset(Ruby_HAS_VENDOR_RUBY CACHE)
  unset(Ruby_VENDORARCH_DIR CACHE)
  unset(Ruby_VENDORLIB_DIR CACHE)
endfunction()

####  Check RVM virtual environment ###
function (_RUBY_CHECK_RVM)
  if (NOT DEFINED ENV{MY_RUBY_HOME})
    return()
  endif()

  find_program (path
                NAMES ${_Ruby_POSSIBLE_EXECUTABLE_NAMES}
                NAMES_PER_DIR
                PATHS ENV MY_RUBY_HOME
                PATH_SUFFIXES bin Scripts
                NO_CACHE
                NO_CMAKE_PATH
                NO_CMAKE_ENVIRONMENT_PATH
                NO_SYSTEM_ENVIRONMENT_PATH
                NO_CMAKE_SYSTEM_PATH)

  _RUBY_VALIDATE_INTERPRETER("${path}")

  if(Ruby_EXECUTABLE)
    set(Ruby_ENV "RVM" CACHE INTERNAL "Ruby environment")
  endif()
endfunction()

####  Check RBENV virtual environment ###
function (_RUBY_CHECK_RBENV)
  if (DEFINED ENV{RBENV_ROOT})
    execute_process (COMMAND "$ENV{RBENV_ROOT}/bin/rbenv" "which" "ruby"
                     RESULT_VARIABLE result
                     OUTPUT_VARIABLE path
                     ERROR_QUIET
                     OUTPUT_STRIP_TRAILING_WHITESPACE)
  else()
    # Check user home directory - the default installation location
    execute_process (COMMAND "$ENV{HOME}/.rbenv/bin/rbenv" "which" "ruby"
                     RESULT_VARIABLE result
                     OUTPUT_VARIABLE path
                     ERROR_QUIET
                     OUTPUT_STRIP_TRAILING_WHITESPACE)
  endif()

  _RUBY_VALIDATE_INTERPRETER("${path}")

  if(Ruby_EXECUTABLE)
    set(Ruby_ENV "RBENV" CACHE INTERNAL "Ruby environment")
  endif()
endfunction()

####  Check system installed Ruby ###
function (_RUBY_CHECK_SYSTEM)
  find_program (path
                NAMES ${_Ruby_POSSIBLE_EXECUTABLE_NAMES}
                NAMES_PER_DIR
                NO_CACHE)

  _RUBY_VALIDATE_INTERPRETER("${path}")

  if(Ruby_EXECUTABLE)
    set(Ruby_ENV "Standard" CACHE INTERNAL "Ruby environment")
  endif()
endfunction()

# Find Ruby! First clear out any cached values
_RUBY_CLEAR_VARS()
if(Ruby_FIND_VIRTUALENV MATCHES "^(FIRST|ONLY)$")
  # First check for RVM virtual environments
  _RUBY_CHECK_RVM()

  # Second check for RBENV virtual environments
  if(NOT Ruby_EXECUTABLE)
    _RUBY_CLEAR_VARS()
    _RUBY_CHECK_RBENV()
  endif()
endif()

# Third check for system installed Ruby
if(NOT ${Ruby_FIND_VIRTUALENV} STREQUAL "ONLY" AND NOT Ruby_EXECUTABLE)
  _RUBY_CLEAR_VARS()
  _RUBY_CHECK_SYSTEM()
endif()

if(Ruby_EXECUTABLE)
  # query the ruby version
  _RUBY_CONFIG_VAR("MAJOR" Ruby_VERSION_MAJOR)
  _RUBY_CONFIG_VAR("MINOR" Ruby_VERSION_MINOR)
  _RUBY_CONFIG_VAR("TEENY" Ruby_VERSION_PATCH)

  # query the different directories
  _RUBY_CONFIG_VAR("archdir" Ruby_ARCH_DIR)
  _RUBY_CONFIG_VAR("arch" Ruby_ARCH)
  _RUBY_CONFIG_VAR("rubyhdrdir" Ruby_HDR_DIR)
  _RUBY_CONFIG_VAR("rubyarchhdrdir" Ruby_ARCHHDR_DIR)
  _RUBY_CONFIG_VAR("libdir" _Ruby_POSSIBLE_LIB_DIR)
  _RUBY_CONFIG_VAR("rubylibdir" Ruby_RUBY_LIB_DIR)

  # site_ruby
  _RUBY_CONFIG_VAR("sitearchdir" Ruby_SITEARCH_DIR)
  _RUBY_CONFIG_VAR("sitelibdir" Ruby_SITELIB_DIR)

  # vendor_ruby available ?
  execute_process(COMMAND ${Ruby_EXECUTABLE} -r vendor-specific -e "print 'true'"
    OUTPUT_VARIABLE Ruby_HAS_VENDOR_RUBY  ERROR_QUIET)

  if(Ruby_HAS_VENDOR_RUBY)
    _RUBY_CONFIG_VAR("vendorlibdir" Ruby_VENDORLIB_DIR)
    _RUBY_CONFIG_VAR("vendorarchdir" Ruby_VENDORARCH_DIR)
  endif()

  set(Ruby_VERSION_MAJOR    ${Ruby_VERSION_MAJOR}    CACHE PATH "The Ruby major version" FORCE)
  set(Ruby_VERSION_MINOR    ${Ruby_VERSION_MINOR}    CACHE PATH "The Ruby minor version" FORCE)
  set(Ruby_VERSION_PATCH    ${Ruby_VERSION_PATCH}    CACHE PATH "The Ruby patch version" FORCE)
  set(Ruby_ARCH_DIR         ${Ruby_ARCH_DIR}         CACHE PATH "The Ruby arch dir" FORCE)
  set(Ruby_HDR_DIR          ${Ruby_HDR_DIR}          CACHE PATH "The Ruby header dir (1.9+)" FORCE)
  set(Ruby_ARCHHDR_DIR      ${Ruby_ARCHHDR_DIR}      CACHE PATH "The Ruby arch header dir (2.0+)" FORCE)
  set(_Ruby_POSSIBLE_LIB_DIR ${_Ruby_POSSIBLE_LIB_DIR} CACHE PATH "The Ruby lib dir" FORCE)
  set(Ruby_RUBY_LIB_DIR     ${Ruby_RUBY_LIB_DIR}     CACHE PATH "The Ruby ruby-lib dir" FORCE)
  set(Ruby_SITEARCH_DIR     ${Ruby_SITEARCH_DIR}     CACHE PATH "The Ruby site arch dir" FORCE)
  set(Ruby_SITELIB_DIR      ${Ruby_SITELIB_DIR}      CACHE PATH "The Ruby site lib dir" FORCE)
  set(Ruby_HAS_VENDOR_RUBY  ${Ruby_HAS_VENDOR_RUBY}  CACHE BOOL "Vendor Ruby is available" FORCE)
  set(Ruby_VENDORARCH_DIR   ${Ruby_VENDORARCH_DIR}   CACHE PATH "The Ruby vendor arch dir" FORCE)
  set(Ruby_VENDORLIB_DIR    ${Ruby_VENDORLIB_DIR}    CACHE PATH "The Ruby vendor lib dir" FORCE)

  mark_as_advanced(
    Ruby_ARCH_DIR
    Ruby_ARCH
    Ruby_HDR_DIR
    Ruby_ARCHHDR_DIR
    _Ruby_POSSIBLE_LIB_DIR
    Ruby_RUBY_LIB_DIR
    Ruby_SITEARCH_DIR
    Ruby_SITELIB_DIR
    Ruby_HAS_VENDOR_RUBY
    Ruby_VENDORARCH_DIR
    Ruby_VENDORLIB_DIR
    Ruby_VERSION_MAJOR
    Ruby_VERSION_MINOR
    Ruby_VERSION_PATCH
    )
endif()

# In case Ruby_EXECUTABLE could not be executed (e.g. cross compiling)
# try to detect which version we found. This is not too good.
if(Ruby_EXECUTABLE AND NOT Ruby_VERSION_MAJOR)
  # by default assume 1.8.0
  set(Ruby_VERSION_MAJOR 1)
  set(Ruby_VERSION_MINOR 8)
  set(Ruby_VERSION_PATCH 0)
  # check whether we found 1.9.x
  if(${Ruby_EXECUTABLE} MATCHES "ruby1\\.?9")
    set(Ruby_VERSION_MAJOR 1)
    set(Ruby_VERSION_MINOR 9)
  endif()
  # check whether we found 2.[0-7].x
  if(${Ruby_EXECUTABLE} MATCHES "ruby2")
    set(Ruby_VERSION_MAJOR 2)
    string(REGEX_REPLACE ${Ruby_EXECUTABLE} "ruby2\\.?([0-7])" "\\1" Ruby_VERSION_MINOR)
  endif()
  # check whether we found 3.[0-1].x
  if(${Ruby_EXECUTABLE} MATCHES "ruby3")
    set(Ruby_VERSION_MAJOR 3)
    string(REGEX_REPLACE ${Ruby_EXECUTABLE} "ruby3\\.?([0-1])" "\\1" Ruby_VERSION_MINOR)
  endif()
endif()

if(NOT Ruby_EXECUTABLE)
  set(Ruby_EXECUTABLE "Ruby_EXECUTABLE-NOTFOUND" CACHE INTERNAL "Ruby Executable")
endif()

if(Ruby_VERSION_MAJOR)
  set(Ruby_VERSION "${Ruby_VERSION_MAJOR}.${Ruby_VERSION_MINOR}.${Ruby_VERSION_PATCH}")
  set(_Ruby_VERSION_SHORT "${Ruby_VERSION_MAJOR}.${Ruby_VERSION_MINOR}")
  set(_Ruby_VERSION_SHORT_NODOT "${Ruby_VERSION_MAJOR}${Ruby_VERSION_MINOR}")
  set(_Ruby_NODOT_VERSION "${Ruby_VERSION_MAJOR}${Ruby_VERSION_MINOR}${Ruby_VERSION_PATCH}")
  set(_Ruby_NODOT_VERSION_ZERO_PATCH "${Ruby_VERSION_MAJOR}${Ruby_VERSION_MINOR}0")
endif()

# FIXME: Currently we require both the interpreter and development components to be found
# in order to use either.  See issue #20474.
find_path(Ruby_INCLUDE_DIR
  NAMES ruby.h
  HINTS
    ${Ruby_HDR_DIR}
    ${Ruby_ARCH_DIR}
    /usr/lib/ruby/${_Ruby_VERSION_SHORT}/i586-linux-gnu/
)

set(Ruby_INCLUDE_DIRS ${Ruby_INCLUDE_DIR})

# if ruby > 1.8 is required or if ruby > 1.8 was found, search for the config.h dir
if( Ruby_FIND_VERSION VERSION_GREATER_EQUAL "1.9"  OR  Ruby_VERSION VERSION_GREATER_EQUAL "1.9"  OR  Ruby_HDR_DIR)
  find_path(Ruby_CONFIG_INCLUDE_DIR
    NAMES ruby/config.h  config.h
    HINTS
      ${Ruby_HDR_DIR}/${Ruby_ARCH}
      ${Ruby_ARCH_DIR}
      ${Ruby_ARCHHDR_DIR}
  )

  set(Ruby_INCLUDE_DIRS ${Ruby_INCLUDE_DIRS} ${Ruby_CONFIG_INCLUDE_DIR} )
endif()

# Determine the list of possible names for the ruby library
set(_Ruby_POSSIBLE_LIB_NAMES ruby ruby-static ruby${_Ruby_VERSION_SHORT} ruby${_Ruby_VERSION_SHORT_NODOT} ruby${_Ruby_NODOT_VERSION} ruby-${_Ruby_VERSION_SHORT} ruby-${Ruby_VERSION})

if(WIN32)
  set(_Ruby_POSSIBLE_MSVC_RUNTIMES "ucrt;msvcrt;vcruntime140;vcruntime140_1")
  if(MSVC_TOOLSET_VERSION)
    list(APPEND _Ruby_POSSIBLE_MSVC_RUNTIMES "msvcr${MSVC_TOOLSET_VERSION}")
  else()
    list(APPEND _Ruby_POSSIBLE_MSVC_RUNTIMES "msvcr")
  endif()

  set(_Ruby_POSSIBLE_VERSION_SUFFICES "${_Ruby_NODOT_VERSION};${_Ruby_NODOT_VERSION_ZERO_PATCH}")

  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(_Ruby_POSSIBLE_ARCH_PREFIXS "libx64-;x64-")
  else()
    set(_Ruby_POSSIBLE_ARCH_PREFIXS "lib")
  endif()

  foreach(_Ruby_MSVC_RUNTIME ${_Ruby_POSSIBLE_MSVC_RUNTIMES})
    foreach(_Ruby_VERSION_SUFFIX ${_Ruby_POSSIBLE_VERSION_SUFFICES})
      foreach(_Ruby_ARCH_PREFIX ${_Ruby_POSSIBLE_ARCH_PREFIXS})
        list(APPEND _Ruby_POSSIBLE_LIB_NAMES
                   "${_Ruby_ARCH_PREFIX}${_Ruby_MSVC_RUNTIME}-ruby${_Ruby_VERSION_SUFFIX}"
                   "${_Ruby_ARCH_PREFIX}${_Ruby_MSVC_RUNTIME}-ruby${_Ruby_VERSION_SUFFIX}-static")
      endforeach()
    endforeach()
  endforeach()
endif()

find_library(Ruby_LIBRARY NAMES ${_Ruby_POSSIBLE_LIB_NAMES} HINTS ${_Ruby_POSSIBLE_LIB_DIR} )

set(_Ruby_REQUIRED_VARS Ruby_EXECUTABLE Ruby_INCLUDE_DIR Ruby_LIBRARY)
if(_Ruby_VERSION_SHORT_NODOT GREATER 18)
  list(APPEND _Ruby_REQUIRED_VARS Ruby_CONFIG_INCLUDE_DIR)
endif()

if(_Ruby_DEBUG_OUTPUT)
  message(STATUS "--------FindRuby.cmake debug------------")
  message(STATUS "_Ruby_POSSIBLE_EXECUTABLE_NAMES: ${_Ruby_POSSIBLE_EXECUTABLE_NAMES}")
  message(STATUS "_Ruby_POSSIBLE_LIB_DIR: ${_Ruby_POSSIBLE_LIB_DIR}")
  message(STATUS "_Ruby_POSSIBLE_LIB_NAMES: ${_Ruby_POSSIBLE_LIB_NAMES}")
  message(STATUS "Ruby_FIND_VIRTUALENV=${Ruby_FIND_VIRTUALENV}")
  message(STATUS "Ruby_ENV: ${Ruby_ENV}")
  message(STATUS "Found Ruby_VERSION: \"${Ruby_VERSION}\"")
  message(STATUS "Ruby_EXECUTABLE: ${Ruby_EXECUTABLE}")
  message(STATUS "Ruby_LIBRARY: ${Ruby_LIBRARY}")
  message(STATUS "Ruby_INCLUDE_DIR: ${Ruby_INCLUDE_DIR}")
  message(STATUS "Ruby_CONFIG_INCLUDE_DIR: ${Ruby_CONFIG_INCLUDE_DIR}")
  message(STATUS "Ruby_HDR_DIR: ${Ruby_HDR_DIR}")
  message(STATUS "Ruby_ARCH_DIR: ${Ruby_ARCH_DIR}")
  message(STATUS "--------------------")
endif()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Ruby  REQUIRED_VARS  ${_Ruby_REQUIRED_VARS}
                                        VERSION_VAR Ruby_VERSION )

if(Ruby_FOUND)
  set(Ruby_LIBRARIES ${Ruby_LIBRARY})
endif()

mark_as_advanced(
  Ruby_EXECUTABLE
  Ruby_LIBRARY
  Ruby_INCLUDE_DIR
  Ruby_CONFIG_INCLUDE_DIR
  )

# Set some variables for compatibility with previous version of this file (no need to provide a CamelCase version of that...)
set(RUBY_POSSIBLE_LIB_PATH ${_Ruby_POSSIBLE_LIB_DIR})
set(RUBY_RUBY_LIB_PATH ${Ruby_RUBY_LIB_DIR})
set(RUBY_INCLUDE_PATH ${Ruby_INCLUDE_DIRS})

# Backwards compatibility
# Define upper case versions of output variables
foreach(Camel
    Ruby_EXECUTABLE
    Ruby_INCLUDE_DIRS
    Ruby_LIBRARY
    Ruby_VERSION
    Ruby_VERSION_MAJOR
    Ruby_VERSION_MINOR
    Ruby_VERSION_PATCH

    Ruby_ARCH_DIR
    Ruby_ARCH
    Ruby_HDR_DIR
    Ruby_ARCHHDR_DIR
    Ruby_RUBY_LIB_DIR
    Ruby_SITEARCH_DIR
    Ruby_SITELIB_DIR
    Ruby_HAS_VENDOR_RUBY
    Ruby_VENDORARCH_DIR
    Ruby_VENDORLIB_DIR

    )
    string(TOUPPER ${Camel} UPPER)
    set(${UPPER} ${${Camel}})
endforeach()
