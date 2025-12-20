# CMake

For more complex C++ extensions, you may wish to use `CMake` to build your extension. `CMake` has a lot of advantages over `extconf.rb`:

* It is likely the build system the library you are wrapping uses
* Provides built-in functionality for finding installed modules (like Ruby!)
* Provides much more control over the build process
* Parallelizes builds resulting in *much* faster compilation times compared to `make`

## CMakePresets.json

Rice includes a `CMakePresets.json` file (in the `test` directory) that provides pre-configured build settings for various platforms and compilers. Using presets is highly recommended as they encode the correct compiler flags for each platform.

The available presets include:

* `linux-debug` / `linux-release` - Linux with GCC
* `macos-debug` / `macos-release` - macOS with Clang
* `msvc-debug` / `msvc-release` - Windows with MSVC
* `clang-windows-debug` / `clang-windows-release` - Windows with Clang
* `mingw-debug` / `mingw-release` - Windows with MinGW

To use a preset:

```bash
# Configure using a preset
cmake --preset macos-debug

# Build using a preset
cmake --build --preset macos-debug
```

You can copy this file to your own project and customize it as needed. Using presets ensures consistent builds and makes it easy to switch between debug and release configurations.

## Enabling

To build your extension using `CMake`, add the following to your Gemspec (assuming the extension directory is `ext`):

```ruby
Gem::Specification.new do |spec|
  spec.extensions = ["ext/CMakeLists.txt"]
end
```

RubyGems 4.0 and later include full CMake support, with improvements contributed by the Rice project.

If you are using an older version of RubyGems (< 4.0), Rice will automatically patch RubyGems to add improved CMake support. This is done via the [rubygems_plugin.rb](https://github.com/ruby-rice/rice/blob/master/lib/rubygems_plugin.rb) file which uses RubyGem's [plugin](https://guides.rubygems.org/plugins/) system. To enable this, include `Rice` as a dependency in your gemspec:

```ruby
Gem::Specification.new do |spec|
  spec.add_runtime_dependency('rice')
end
```

## Example

Below is an example `CMakeLists.txt` file to build a Ruby C++ extension:

```cmake
cmake_minimum_required(VERSION 3.26)

project(MyExtension)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Ruby REQUIRED)

# Platform-specific compiler settings
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_definitions(_CRT_SECURE_NO_DEPRECATE _CRT_NONSTDC_NO_DEPRECATE)
    add_compile_options(/bigobj /utf-8)
    # The default of /EHsc crashes Ruby when calling longjmp with optimizations on (/O2)
    string(REGEX REPLACE "/EHsc" "/EHs" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-ftemplate-backtrace-limit=0)
endif ()

add_library(MyExtension SHARED
            "MyExtension.cpp")

# Link to Ruby using the modern imported target
target_link_libraries(MyExtension PRIVATE Ruby::Module)

# Add in Rice headers
target_include_directories(MyExtension PRIVATE <path-to-rice-headers>)

# Add in the library you are wrapping (headers and libraries)
target_include_directories(MyExtension PRIVATE <path-to-library-headers>)
target_link_libraries(MyExtension PRIVATE <library-name>)
```

Currently you will need to manually specify where the Rice [header files](packaging.md#header-files) are located.

Note that using the `Ruby::Module` imported target is preferred over manually specifying include directories and libraries, as it automatically sets up all necessary include paths and compiler settings for building Ruby extensions.

## Compiler Settings

See [compiler settings](packaging.md#compiler-settings) for details about compiler settings.

## FindRuby

Notice the inclusion of the following line in the above `CMakeLists.txt` file:

```cmake
find_package("Ruby")
```

This will find a locally installed Ruby, whether it is the system Ruby or a [RVM](https://rvm.io/) or [RbENV](https://rbenv.org/) installed Ruby.

### macOS with Homebrew

CMake's built-in `FindRuby` module does not currently support finding Ruby installed via [Homebrew](https://brew.sh/) on macOS. If you are using Homebrew-installed Ruby, you will need to use Rice's [FindRuby.cmake](https://github.com/ruby-rice/rice/blob/master/FindRuby.cmake) script instead.

Rice's `FindRuby.cmake` includes support for:

* RVM virtual environments
* rbenv virtual environments
* Homebrew-installed Ruby (both Apple Silicon and Intel Macs)
* System Ruby

To use Rice's FindRuby script, copy it to your project and include it before calling `find_package`:

```cmake
# Use Rice's FindRuby which supports Homebrew
include("${CMAKE_CURRENT_SOURCE_DIR}/FindRuby.cmake")

# Now find_package will use the included module
find_package(Ruby REQUIRED)
```

Alternatively, you can add the directory containing `FindRuby.cmake` to your module path:

```cmake
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
find_package(Ruby REQUIRED)
```

Rice's `FindRuby.cmake` is also useful for older versions of CMake that lack good Ruby support.

### FindRuby Imported Targets

Rice's `FindRuby.cmake` provides modern CMake imported targets:

| Target | Description |
|--------|-------------|
| `Ruby::Interpreter` | Ruby interpreter executable |
| `Ruby::Module` | For building Ruby extension modules. Use this for gems with native extensions. On Unix, does not link to libruby (symbols resolved at load time). Includes hidden visibility settings. |
| `Ruby::Ruby` | For embedding Ruby in C/C++ applications. Links to libruby. |

For building Ruby extensions, use `Ruby::Module`:

```cmake
target_link_libraries(MyExtension PRIVATE Ruby::Module)
```

### FindRuby Variables

The following CMake variables are set by FindRuby:

| Variable | Description |
|----------|-------------|
| Ruby_VERSION_MAJOR | Ruby major version |
| Ruby_VERSION_MINOR | Ruby minor version |
| Ruby_VERSION_PATCH | Ruby patch version |
| Ruby_ARCH_DIR | Ruby arch dir |
| Ruby_HDR_DIR | Ruby header dir (1.9+) |
| Ruby_ARCHHDR_DIR | Ruby arch header dir (2.0+) |
| Ruby_RUBY_LIB_DIR | Ruby ruby-lib dir |
| Ruby_SITEARCH_DIR | Ruby site arch dir |
| Ruby_SITELIB_DIR | Ruby site lib dir |
| Ruby_EXECUTABLE | Ruby interpreter |
| Ruby_LIBRARY | Ruby shared library |
| Ruby_INCLUDE_DIR | Ruby include directory |
| Ruby_CONFIG_INCLUDE_DIR | Ruby config include directory |
| Ruby_INCLUDE_DIRS | Include and config directories |

## Rice CMake Target

Rice provides the `rice::rice` imported target for including Rice headers:

```cmake
target_link_libraries(MyExtension PRIVATE rice::rice)
```

This target:

* Adds Rice include directories
* Requires C++17 or higher

## Complete Example

For a complete, real-world example of using CMake with Rice, see [BitmapPlusPlus-ruby](https://github.com/ruby-rice/BitmapPlusPlus-ruby/blob/main/ext/CMakeLists.txt).

This example demonstrates:

* Fetching Rice via `FetchContent`
* Using `Ruby::Module` for extension modules
* Using `rice::rice` for Rice headers
* Setting the correct extension suffix
* Configuring output directories
