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

## Link Time Optimization (LTO)

Link Time Optimization is **highly recommended** for Rice extensions, especially for release builds. Rice makes extensive use of C++ templates, which can result in significant code duplication across translation units. LTO allows the linker to deduplicate template instantiations and perform whole-program optimizations.

### Why LTO Matters for Rice

Rice's template-heavy design means that each `.cpp` file that uses Rice generates its own copies of template instantiations. Without LTO, these duplicate instantiations remain in the final binary, dramatically increasing its size. LTO enables the linker to:

- Deduplicate identical template instantiations across object files
- Inline functions across translation unit boundaries
- Remove dead code more effectively

### Build Size Comparison

The following table shows the impact of LTO on a real-world Rice extension (opencv-ruby bindings):

| Platform | Debug  | Release (with LTO) | Size Reduction |
|----------|--------|--------------------|----------------|
| MSVC     | 140 MB | 70 MB              | 50%            |
| macOS    | 324 MB | 164 MB             | 50%            |
| MinGW    | 1.4 GB | 200 MB             | 86%            |

As shown, LTO provides substantial size reductions across all platforms, with MinGW benefiting the most dramatically.

Rice's `CMakePreset.json` automatically enables LTO by setting `CMAKE_INTERPROCEDURAL_OPTIMIZATION` to `ON`.

If you are using `extconf.rb (Mkmf)` then:

```ruby
$CXXFLAGS << " -flto"
$LDFLAGS << " -flto"
```

For MSVC:

```ruby
$CXXFLAGS << " /GL"
$LDFLAGS << " /LTCG"
```

### Debug Symbol Splitting (GCC/Clang)

For debug builds with GCC or Clang, consider using `-gsplit-dwarf` to separate debug information into `.dwo` files. This keeps the main binary smaller while preserving full debug capability:

```cmake
set(CMAKE_CXX_FLAGS_DEBUG "-g -gsplit-dwarf")
```

This is particularly useful for g++ where debug builds can exceed 1 GB without it.
