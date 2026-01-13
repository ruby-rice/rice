# Installation

Rice is a header-only library and therefore does not need to be built separately. Instead it should be `#include`d in your C++ project. Rice requires C++17 or later and is tested on Windows (MSVC, Clang and Mingw64), MacOS (Xcode/Clang) and Linux (g++, Clang).

## Option 1: Install the Gem

The easiest way to use Rice is to install it as a gem:

```shell
gem install rice
```

This installs the Rice header files and provides the `mkmf-rice` helper for building extensions with `extconf.rb`. Note this works for simple projects, but will not scale to large projects. In that case, you will want to use [Cmake](packaging/cmake.md) as described in detail in the packaging section.

## Option 2: Copy Header Files

Alternatively, you can copy the Rice header files directly into your project. This is useful if you don't want a gem dependency or need to bundle Rice with your source code.

The header files are located in the [`include/rice`](https://github.com/ruby-rice/rice/tree/master/include/rice) folder on GitHub. Copy this folder into your project and add the include path to your build system.

The three headers are:

* `rice/rice.hpp` - Core Rice functionality
* `rice/stl.hpp` - STL container support (optional)
* `rice/api.hpp` - Enables automatic generation of documentation and RBS files (optional)

When copying headers, use a specific release tag rather than the master branch to ensure stability.

## Compiler Requirements

Rice requires a C++17 compliant compiler and RTTI to be enabled. See [Compiler Settings](packaging/build_settings.md#compiler-settings) for the specific flags needed for each compiler.
