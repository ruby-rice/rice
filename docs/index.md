# Introduction

Rice is a C++ 17 header-only library that serves dual purposes. First, it makes it much easier to create Ruby bindings for existing C++ libraries. Second, it provides an object oriented interface to Ruby's C API that makes it easy to embed Ruby and write Ruby extensions in C++.

Rice is similar to [Boost.Python](https://github.com/boostorg/python) and [pybind11](https://github.com/pybind/pybind11) in that it minimizes boilerplate code needed to interface with C++. It does this by automatically determining type information allowing Ruby objects to be converted to C++ and vice versa.

Rice provides:

* A C++ based syntax for wrapping and defining classes
* Automatic type conversions between C++ and Ruby
* Automatic exception handling between C++ and Ruby
* Smart pointers for handling garbage collection
* A C++ API to wrap Ruby's C API

## Project Details

Source code is hosted on GitHub: <https://github.com/ruby-rice/rice>

Bug tracking is also hosted on GitHub: <https://github.com/ruby-rice/rice/issues>

API documentation: <https://ruby-rice.github.io/4.x>

## Next Steps

- [Installation](installation.md) - How to install Rice
- [Tutorial](tutorial.md) - Get started with Rice
