# CMake

For more complex C++ extensions, you will likely want to use `CMake` to build your extension. `CMake` has a number of advantages over `extconf.rb`:

* It is probably the build system the library you are wrapping uses
* Provides built-in functionality for finding installed modules (like Ruby!)
* Provides much more control over the build process
* Parallelizes builds resulting in *much* faster compilation times compared to `make`

Getting up to speed with CMake can be daunting, so the [bitmap-plus-plus](https://github.com/ruby-rice/BitmapPlusPlus-ruby) gem includes a fully documented CMake based build system as well as GitHub actions to run tests.  The example shows how to:

* Fetch Rice headers via `FetchContent`
* Using `Ruby::Module` for extension modules
* Using `Rice::Rice` for Rice headers
* Setting the correct extension suffix
* Configuring output directories

## CMakeLists.txt

The first step in using CMake is to create a [CMakeLists.txt](https://github.com/ruby-rice/BitmapPlusPlus-ruby/blob/main/ext/CMakeLists.txt) file. Start by copying the one from the `bitmap-plus-plus` gem. It automatically downloads the latest version of `FindRuby.cmake` as well as the Rice header files (make sure to set the git tag setting to the version of Rice you want to use!).

Next, tell Ruby gems to use CMake to build your gem by adding the following to your Gemspec (assuming the extension directory is `ext`):

```ruby
Gem::Specification.new do |spec|
  spec.extensions = ["ext/CMakeLists.txt"]
end
```

RubyGems 4.0 and later include much improved CMake support, with improvements contributed by the Rice project.

If you are using an older version of RubyGems (< 4.0), Rice will automatically patch RubyGems to add improved CMake support. This is done via the [rubygems_plugin.rb](https://github.com/ruby-rice/rice/blob/master/lib/rubygems_plugin.rb) file which uses RubyGem's [plugin](https://guides.rubygems.org/plugins/) system. To enable this, you must install the Rice gem. That is easy to accomplish by incluing it as a dependency in your gemspec:

```ruby
Gem::Specification.new do |spec|
  spec.add_runtime_dependency('rice')
end
```

## CMakePresets.json

Next, it is recommended to add a [CMakePresets.json](https://github.com/ruby-rice/BitmapPlusPlus-ruby/blob/main/ext/CMakePresets.json) to your gem. A `CMakePresets.json` makes it easier to define all the [compiler settings](build_settings.md#compiler-settings) your gem will need to build with commonly used operating systems and compilers.

Once again, the [bitmap-plus-plus](https://github.com/ruby-rice/BitmapPlusPlus-ruby) gem includes an [example](https://github.com/ruby-rice/BitmapPlusPlus-ruby/blob/main/ext/CMakePresets.json) that you can copy. It includes the following presets:

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

## Installation

When users install your gem, make sure to document how to select a preset. For example, for bitmap-plus-plus

```bash
gem install bitmap-plus-plus --preset macos-release|linux-release|mingw-release
```

## FindRuby

`FindRuby.cmake` is what enables CMake to work with Ruby. Its job is to find all locally installed versions of Ruby, including:

* [RVM](https://rvm.io/) virtual environments
* [RbENV](https://rbenv.org/) virtual environments
* [Homewbrew](https://https://formulae.brew.sh/formula/ruby) installed Ruby (both Apple Silicon and Intel Macs)
* System Ruby

It is invoked by adding the following code in your `CMakeLists.txt` file:

```cmake
find_package("Ruby")
```

CMake comes bundled with `FindRuby`. However, older versions do not include support for RbEnv, Homebrew or CMake targets (all of which have been contributed upstream to CMake by the Rice project).

Therefore, the example `CMakeLists.txt` file automatically downloads the latest version of `FindRuby.cmake` from the Rice github repository. In addition, it also downloads the latest version of `Rice.make` which makes it easy to include the Rice header files in your project.


### FindRuby Imported Targets

Rice's `FindRuby.cmake` provides modern CMake targets, including:

| Target              | Description                               |
|---------------------|-------------------------------------------|
| `Ruby::Interpreter` | Ruby interpreter executable               |
| `Ruby::Module`      | For building Ruby extensions              |
| `Ruby::Ruby`        | For embedding Ruby in C/C++ applications. |

For building Ruby extensions, use `Ruby::Module`:

```cmake
target_link_libraries(MyExtension PRIVATE Ruby::Module)
```
Once again, refer to the example [CMakeLists.txt](https://github.com/ruby-rice/BitmapPlusPlus-ruby/blob/main/ext/CMakeLists.txt) for an example.

### FindRuby Variables

The following CMake variables are set by FindRuby:

| Variable | Description                                   |
|----------|-----------------------------------------------|
| Ruby_VERSION_MAJOR | Ruby major version                  |
| Ruby_VERSION_MINOR | Ruby minor version                  |
| Ruby_VERSION_PATCH | Ruby patch version                  |
| Ruby_ARCH_DIR | Ruby arch dir                            |
| Ruby_HDR_DIR | Ruby header dir (1.9+)                    |
| Ruby_ARCHHDR_DIR | Ruby arch header dir (2.0+)           |
| Ruby_RUBY_LIB_DIR | Ruby ruby-lib dir                    |
| Ruby_SITEARCH_DIR | Ruby site arch dir                   |
| Ruby_SITELIB_DIR | Ruby site lib dir                     |
| Ruby_EXECUTABLE | Ruby interpreter                       |
| Ruby_LIBRARY | Ruby shared library                       |
| Ruby_INCLUDE_DIR | Ruby include directory                |
| Ruby_CONFIG_INCLUDE_DIR | Ruby config include directory  |
| Ruby_INCLUDE_DIRS | Include and config directories       |

## Rice.cmake

Rice provides the `Rice::Rice` imported target for including Rice headers:

```cmake
target_link_libraries(MyExtension PRIVATE Rice::Rice)
```

This target:

* Adds Rice include directories
* Requires C++17 or higher
