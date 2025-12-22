# Build Settings

## Compiler Settings

Rice extensions requires several compiler settings to be set. These are captured in the [CMakePresets.json](https://github.com/ruby-rice/rice/blob/master/CMakePresets.json) file for all supported platforms.

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

## Linker Settings

Ruby extensions are shared libraries that the Ruby interpreter loads at runtime using `dlopen`. These extensions reference symbols from the Ruby C API (such as `rb_define_class`, `rb_funcall`, etc.) that are provided by the Ruby interpreter itself. Since these symbols are not available at link time, the linker must be told to allow unresolved symbols.

### macOS

On macOS, use the following linker flag:

```bash
-Wl,-undefined,dynamic_lookup
```

This tells the linker to allow undefined symbols and resolve them dynamically at load time. When Ruby loads the extension, the symbols are resolved against the Ruby interpreter's symbol table.

### Linux

On Linux, use the following linker flag:

```bash
-Wl,--unresolved-symbols=ignore-all
```

This instructs the linker to ignore all unresolved symbols during linking. The symbols will be resolved when the Ruby interpreter loads the shared library.

### Windows

On Windows, no special linker flags are required. The extension links directly against the Ruby library (e.g., `x64-vcruntime140-ruby320.lib`), which provides the necessary symbols at link time.
