============
Introduction
============

Rice is a C++ header-only library that serves dual purposes. First, it makes it much
easier to create Ruby bindings for existing C++ libraries. Second, it provides an
object oriented interface to Ruby's C API that makes it easy to embed Ruby and write
Ruby extensions in C++.

Rice is similar to Boost.Python and pybind11 in that it minimizes boilerplate code needed
to interface with C++. It does this by automatically determining type information allowing
Ruby object to be converted to C++ and vice versa.

What Rice gives you:

* A simple C++-based syntax for wrapping and defining classes
* Automatic type conversions between C++ and Ruby
* Automatic exception conversions between C++ and Ruby
* Smart pointers for handling garbage collection
* Wrappers for most builtin types to simplify calling code

Version Differences 3.x vs 4.x and later
----------------------------------------

This documentation and the ``master`` branch are for Rice 4.x and later, which is the
header-only version of this library. Use the ``3.x`` branch for the docs and code for that
line of releases.

The docs for the 3.x line of Rice is viewable at https://jasonroelofs.com/rice/3.x.

Project Details
---------------

The source is hosted on GitHub: http://github.com/jasonroelofs/rice

Bug tracking: http://github.com/jasonroelofs/rice/issues

API documentation: http://jasonroelofs.com/rice

Installation
------------

.. code-block:: shell

  gem install rice

Rice is header-only library and therefore does not need to be built separately.
Instead it should be #included in your C++ project. Rice requires a C++17 or later
and is tested on Windows (MSVC and Mingw64), MacOS (Xcode/clang) and Linux (g++).

