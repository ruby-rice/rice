# Overview

Once you have wrapped a C++ library, the next step is to package it as a [RubyGem](https://guides.rubygems.org). As part of the RubyGem, you should either include a pre-built binary or enable the extension to be built upon installation.

This is done via the `extensions` field in a Gemspec:

```ruby
Gem::Specification.new do |spec|
  spec.extensions = ["ext/CMakeLists.txt"]
end
```

The name of the file determines what extension builder `RubyGems` invokes on installation. There are two built-in choices:

* extconf.rb - uses [extconf.rb](extconf.rb.md)
* CMakeLists.txt - uses [CMake](cmake.md)

Almost all `RubyGems` that build extensions use extconf.rb, so you will find extensive documentation on how to use it. In contrast, very few, if any, `RubyGems` use CMake. Thus you will not find any documentation outside of what Rice provides.

In general, for simple extensions use `extconf.rb` but otherwise use `CMake`.

It is also possible to use a custom build system, but in that case you will need to create a RubyGem [plugin](https://guides.rubygems.org/plugins/) to tell `RubyGems` how to make the build.

## Header Files

Rice is packaged as [header-only](https://en.wikipedia.org/wiki/Header-only) library. That means you do not have to worry about linking a shared Rice library with your extension.

The two headers are:

* rice/rice.hpp
* rice/stl.hpp

There are two ways to incorporate the headers into your build.

The first option is to install them by installing Rice - thus make Rice a required dependency of your Gem like this:

```ruby
Gem::Specification.new do |spec|
  spec.add_runtime_dependency('rice')
end
```

The second option is to simply copy them from [github](https://github.com/ruby-rice/rice/tree/master/include/rice) and copy them into your repo. Remember the provided link points to the master branch - you should pick a specific release tag and use those instead.

## Compiler Settings

Rice extensions requires several compiler settings to be set. These are captured in the [CMakePresets.json](https://github.com/ruby-rice/rice/blob/master/test/CMakePresets.json) file for all supported platforms.

For Clang and GCC:

```bash
-std=c++17 -Wa,-mbig-obj -ftemplate-backtrace-limit=0
```

For MINGW:

```bash
-std=c++17, -Wa,-mbig-obj
```

For Microsoft Visual C++ and Windows Clang:

```bash
/std:c++17 /EHs /permissive- /bigobj /utf-8 -D_ALLOW_KEYWORD_MACROS -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE
```

These options are described below:

### C++17

First, C++17 must be enabled since Rice makes heavy use of C++17 template and metaprogramming features.

### Big Objects

Second, bigobject support needs to enabled. This tells the compiler to increase the number of sections allowed in a .obj file.

### UTF-8

Rice uses UTF-8 characters when [mapping](../stl/stl.md#automatically-generated-ruby-classes) instantiated STL templates to Ruby class names.

### Exception Handling Model

For Visual C++, the default exception [model](https://learn.microsoft.com/en-us/cpp/build/reference/eh-exception-handling-model?view=msvc-170) setting of `/EHsc` crashes Ruby when calling longjmp with optimizations enabled (/O2). Therefore you must `/EHs` instead.

### Template Backtrace

For g++, you must set `-ftemplate-backtrace-limit=0` to avoid compilation errors.
