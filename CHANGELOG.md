# Changelog

## 4.9.1 (unreleased)
This release focuses on improving memory management for STL containers and attribute setters.

Enhancements:
* Support `takeOwnership` and `keepAlive` when setting attributes via `Arg("value").takeOwnership()` and `Arg("value").keepAlive()`
* Add `Arg` parameter names to all STL container methods for keyword argument support
* Add `keepAlive` support for STL container operations (vector push/insert, set insert, map/multimap store)
* Add `keepAlive` for map/unordered_map/multimap keys to prevent GC of pointer-type keys

Bug Fixes:
* Fix error when multiple overloaded methods take different types of vectors
* Fix type unknown errors when using `std::shared_ptr` with g++
* Fix CMake `_Ruby_DLEXT` variable type (string, not path)
* Fix crash caused by static variable in keepAlive implementation
* Fix incorrect attribute overloading behavior

## 4.9.0 (2026-01-01)
This release revamps smart pointer support for `std::shared_ptr` and `std::unique_ptr`.

Rice now always creates wrapper classes for smart pointers under the `Std` module (e.g., `Std::SharedPtr≺MyClass≻`, `Std::UniquePtr≺MyClass≻`). These wrapper classes expose methods like `empty?`, `get`, `swap`, and for shared_ptr, `use_count`. Methods defined on the managed type are automatically forwarded to the wrapper class using Ruby's `Forwardable` module.

This change is backwards compatible for Ruby code but not C++ code. If you have implemented your own Smart Pointer wrapper then please read the Smart Pointer documentation for more information on how to update it.

## 4.8.0 (2025-12-29)
This release focuses on making Rice easier to use:

* Compilation times are approximately 2x faster than version 4.7
* Compiled library sizes are about 30% smaller
* Improved keyword argument support
* Improved method override support
* Improved CMake support
* Improved rbs generation support
* Improved api documentation generation support
* Expanded and improved documentation
* Enum values can now be compared against their underlying type (e.g., `Season::Red == 0`)
* STL containers now support `==` and `eql?` for equality comparison
* Automatic creation of `std::shared_ptr<T>` Ruby classes
* Support for C++ references in callbacks
* Array class now supports random iterators
* Fixed Ruby detection for Homebrew installations on macOS
* Added support for references to fundamental types

However, these changes did require some breaking changes, which include:

* `Return().isBuffer()` is replaced by `ReturnBuffer()`
* `Arg("").isBuffer()` is replaced by `ArgBuffer("")`
* `Function().noGVL()` is replaced by `NoGvL()`
* `is_convertible` methods must now return a `double` instead of a `Convertible` enum
* All function/method parameter default values are verified. You may see errors like "ArgumentError: Type is not registered with Rice" or "Invalid AnyCast". In either case, make sure to check that specified default values are correct.

## 4.7.1 (2025-10-28)
Updates:
* Update overload resolution to take into account function arguments that are tagged as buffers via Arg("").setBuffer().
* Make second parameter optional for Array#push and update docs
* Remove ostruct runtime dependency

## 4.7.0 (2025-10-22)
Updates:
* Refactor Native wrappers - functions, methods, attributes and procs - to enable introspection API
* Introduce Pointer<T> class to wrap pointers to fundamental types and arrays.
* Add new methods Arg#setBuffer and Return#setBuffer to indicate that a C++ pointer references an array of objects versus a single object
* Add a new Introspection API that exposes Rice internals to Ruby
* Using the Introspection API, add support for generating RBS files for extensions. See bin/rice-rbs.rb
* Using the Introspection API, add support for generating Markdown documentation for extensions. See bin/rice-doc.rb
* Don't create attribute writers for const attributes
* Support attribute setters for Enums
* Support wrapping std::vector<std::unique_ptr<T>>
* Update Array#push to not always copy C++ instances. This lays the foundation for extracting references and pointers from tuples, variants and optionals.
* Add very basic support for creating std::filesystem::path instances
* Remove toy samples and test libraries. These will be replaced by a new gem that wraps the BitMapPlusPlus library (https://github.com/baderouaich/BitmapPlusPlus)
* Add support for std::runtime_error since some libraries use that as a base exception class (thus when Rice wraps custom exceptions it also needs to wrap the base class)
* Improve std::vector indexing to more closely match Ruby for negative index values
* Correctly encode UTF8 Ruby class names in exception messages
* Add support for disabling Ruby's global interpreter lock (GIL) when calling native functions

Breaking Changes:
* Custom implementations of From_Ruby must include a custom constructor:
    ```
    explicit From_Ruby(Arg* arg)
   ```
* Custom implementations of To_Ruby must include a custom constructor:
    ```
    explicit To_Ruby(Return* returnInfo)
   ```
* You can no longer pass a Buffer<T> to an API that takes a pointer. Instead use Buffer<T>#data or Buffer<T>::release
* The Rice_Init method has been removed.
* Array#push requires a second argument.

## 4.6.1 (2025-06-25)
* Improve attribute handling. Correctly deal with non-copyable/assignable attributes and return references instead of copies of objects
* Improve Buffer implementation to deal with 4 cases:
	- array of fundamental types (int*)
	- array of pointers fundamental types (char**)
	- array of objects (someInstance*)
	- array of pointers of objects (someInstance**)
* Implement Buffer#to_s
* Fix header check on Ubuntu 22.04

## 4.6.0 (2025-06-09)
Rice 4.6 is a major release that adds significant new functionality based on wrapping the OpenCV library, including:

* Add a new Buffer class to provide Ruby API support for pointers sent to or returned by C++
* Support C style out parameters
* Support C style arrays
* Rewrite keyword arguments
* Rewrite default value handling allowing the removal of a lot of boilerplate
* Add support for std::multimap
* Add support for std::set
* Add support for std::tuple
* Add support for smart pointers (shared_ptr, unique_ptr) to fundamental types (void, int, etc)
* Improve std::variant support
* Update is_convertible documentation
* Fix missing version.rb file in gemspec
* Add C++ preprocessor defines for Rice version
* Include Rice::VERSION in published gem
* Moved auto-generated C++ STL classes (std::vector, std::map, etc.) from the module Rice::Std module to Std module
* Make Rice const aware to improve overload method support
* Improve error messages when Rice cannot determine what overloaded method to call
* Improve handling of unbound Data_Type instances to avoid crashes when initializing global static variables of type Data_Type<T>
* Make Enums more useful by adding coerce method to enable stringing together bitwise operators - for example Season::Winter | Season::Spring | Season::Summer.

## 4.5 (2025-02-09)
Rice 4.5 is a major release that adds significant new functionality, including:

* Support method overloading
* Support constructor overloading
* Support rvalues
* Support using keyword arguments in Ruby to call C++ methods
* Support C style callbacks, including adding a new define_callback method
* Support wrapping C/C++ functions as Ruby procs 
* Support calling methods that take pointers
* Add Data_Type#define method to more easily support C++ template classes
* Adds #define_constant method
* Be more flexible on type verification by not throwing errors until all classes/methods have been defined and also allow a error message to be printed instead of thrown
* Add ability to transfer ownership of Ruby created objects to C++ (useful for sending wrapped pointers to APIs that take smart pointers)
* Add support for *char
* Add support for **char
* Improve C++ exception handling when called from Ruby
* Improve handling of Ruby exceptions
* Update std::variant to support references
* Split NativeAttribute support to NativeAttributeGet and NativeAttributeSet
* Create base class for Native classes (NativeFunction, NativeIterator, NativeAttributeGet, NativeAttributeSet - make the type registry easier to deal with (thus using type erasure like the Wrapper classes do).
* Support Ruby 3.4
* Lots of documentation additions and updates
* Updated FindRuby to support rbenv (this change is also merged upstream to CMake)

This release also improves STL support by adding:

* std::exception
* std::exception_ptr
* std::monostage
* std::shared_ptr<void>
* std::type_index
* std::type_info
* std::vector<bool>

Please see the migration guide for updating your bindings for version 4.5

## 4.3.3 (2024-10-19)

* Fix complication issue on Ubuntu 20.04 and GCC 9.

## 4.3.2 (2024-10-18)

* Improve NativeRegistry to reduce possible hash collisions and weird "bad any cast" errors.

## 4.3.1 (2024-3-16)

* Update links and related references to the new repo and docs location: ruby-rice.github.io.

## 4.3 (2024-2-25)

* Add support for STL containers that contain pointers
* Add support for std::string_view
* Fix handling of std::shared_ptr that resulted in moving them instead of copying them
* Fix container iteration so elements are passed by reference and not copied
* Avoid unnecessary copies when creating Rice::Identifiers and Rice::Symbols

## 4.2.1 (2024-1-20)

* Support systems who use `#include <experimental/filesystem>` over `#include<filesystem>`. See [#197](https://github.com/jasonroelofs/rice/issues/197) and [#201](https://github.com/jasonroelofs/rice/pull/201)

## 4.2 (2024-1-10)

* Support Ruby 3.3.0.
* Split Object.call to an explicit Object.call_kw for calling methods expecting keyword arguments.
* Previously, if a wrapper used `keepAlive` on an argument or return value that was itself a Rice type, calling said method would segfault. We've now added an explicit exception to be thrown in this case, prevending the segfault and providing guidance on what was wrong and how to fix it. See [#193](https://github.com/jasonroelofs/rice/pull/193) and [#194](https://github.com/jasonroelofs/rice/pull/194)
* Fix wrapping of std::shared_ptr to properly take default arguments into account.

## 4.1 (2023-4-21)

Rice 4.1 builds on the 4.0 release and has a number of improvements that both polish Rice and extend its functionality. However, there are three incompatibilities to know about:

* Exception handlers are now registered globally versus per module. This requires updating code that calls Class#add_handler to use register_handler instead.
* Rename Arg#isValue to Arg#setValue and then Arg#getIsValue to Arg#isValue
* Rename Return#isValue to Return#setValue and Return#getIsValue to Return#isValue 

New or improved functionality includes:

* Add support for std::map, std::unordered_map, std::variant, std::monostate and std::reference_wrapper
* Enable calling of C++ member functions that are defined in ancestor classes
* Make it easy to wrap C++ iterators like std::vector begin and end
* Enable creating enumerators for C++ collections like std::vector and std::map
* Enable calling more Ruby API methods including those with a variable number of parameters such as rb_yield_values
* Add additional C++ to Ruby exception mappings (for example, std::system_error to SystemCallError)
* Updated documentation, including new pages for instance tracking, iterators, exceptions and newly supported STL classes
* Add support for calling Ruby methods with keywords from Rice::Object and its descendants
* Automatically translate C++ character arrays that start with colons to symbols (ie, ":mysymbol") when sending them to Ruby
* Add a constructor for Rice::Module that takes a name, to enable code like Module("Kernel")
* Fix comparison methods in Rice::Object, such as Object#is_equal, to return the correct result
* Fix various compiler warnings
* Remove deprecated APIs
* Remove support for Ruby 2.5 and 2.6 which are officially out of support
* Add support for building tests with CMake
* And lots of other fixes and code improvements

Rice also includes experimental support for instance tracking so that Rice maps the same C++ instance to the same Ruby instance each time it is passed to Ruby. See the documentation for more information.


## 4.0 (2021-4-8)

Rice 4.0 is a significant change from 3.0 and has multiple backwards-incompatible
changes. Rice 4.0 no longer requires pre-compilation and is now a header-only library,
delivered as a combined header file.

For migrating from 3 to 4, see [the migration guide](https://ruby-rice.github.io/4.x/migration.html).

There are a ton of changes, but some of the most important ones:

* Header only! `#include <rice/rice.hpp>`
* Requires C++17 or later
* Brand new, expanded documentation
* [Built-in STL support](https://ruby-rice.github.io/4.x/stl/stl.html)
* And so much more. See the documentation for more details.

## 3.0 (2021-1-8)

* Now requires a compiler supporting for C++14 or later
* Drop support for Ruby 2.4. Supported versions are now 2.5 through 3.0.
* Fix build issue on macOS Big Sur
* Fix a data corruption issue with `Rice::Exception::what`.
* Move CI from Travis to GitHub Actions. Now also able to verify Windows builds!

## 2.2.0 (2020-1-10)

* Deprecate support for Rubies older than 2.4
* Provide a few more built-in to_ruby/from_ruby conversions
* Fix compilation error when building under Ruby 2.7.0

## 2.1.3 (2019-2-28)

* Don't lock down HAVE_CXX11 on the Rice build itself.

## 2.1.2 (2017-11-20)

* Fix defining custom `begin` and `end` methods on an `Iterator`

## 2.1.1 (2020-1-10)

* Support Ruby 2.4
* Re-enable Rice::Enum#hash to support putting Enums in Hashes

## 2.1.0 (2016-1-1)

* Fix compliation issues related to g++ and Ruby 2.3.0
  To do this, I had to remove Array::to_c_array which was exposing the internals of a
	Ruby RArray type to the system. This is not something that we should support going forward
	as these internals are going to change.

## 2.0.0 (2015-11-27)

* Deprecated all versions of Ruby < 2.0
* Removed Rice::VM.
  Unsure if this class is even used anywhere and it felt strange to be
  able to load up a Ruby interpreter inside of Ruby. If you need it, it's
  two files that I can easily make available in a gist.
* Improve build process across architectures and future changes.
  Included some extra warnings for XCode updates on Mac OS X.
* Confirmed that Rice definitely does not work on static Ruby builds,
  but that seems to be more because newer Ruby versions don't have good static builds.
  Thanks to @Kagetsuki for his help tracking down what's going on here.

## 1.7.0 (2015-1-6)

* Ruby 2.2 support
  Potential breaking changes. Ruby 2.2 removed RHash as a public accessible struct
  and as such I changed all of the Builtin_Objects to work directly off of RObject
  instead of the specifics (RArray, RStruct, RString, etc). If you've been using these
  objects directly I recommend using either the Rice API or Ruby's CAPI instead for
  future compatibility.

## 1.6.3 (2014-12-18)

* Fix complication issue on some 64-bit *nix systems

## 1.6.2 (2014-5-5)

* Oops! Missed new file in the gemspec

## 1.6.1 (2014-5-5)

* Support C++x11 uniqe_ptr over auto_ptr
* Fix some warnings

## 1.6.0 (2014-2-3)

* Ruby 2.1 support -- Thanks Chai Zhenhua
* Methods and Constructors have correct method access specifiers [#57]
* Clean up some 64-bit compiler warnings

## 1.5.3 (2013-10-14)

* Fix signed / unsigned compiler warning with Hash#each
* Fix compilation on clang 5 (Xcode 5)

## 1.5.2 (2013-10-5)

* Update build system to remove deprecation warnings and allow easier building
* Fix String to work as a parameter in a wrapped method (#59)
* Update documentation a bit

## 1.5.1 (2013-5-2)

* Doc string fix

## 1.5.0 (2013-5-1)

* Ruby 2.0 compatability
* Bug fixes

## 1.4.3 (2011-10-9)

* Various build configuration fixes

## 1.4.0 (2010-8-30)

* Fully compatible with Ruby 1.9.2
* Constructor supports default arguments
* Ability to define implicit casting through define_implicit_cast
* Fixed a few memory-related issues
