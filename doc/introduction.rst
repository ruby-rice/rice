.. _introduction:

============
Introduction
============

Rice is a C++ 17 header-only library that serves dual purposes. First, it makes it much easier to create Ruby bindings for existing C++ libraries. Second, it provides an object oriented interface to Ruby's C API that makes it easy to embed Ruby and write Ruby extensions in C++.

Rice is similar to `Boost.Python <https://github.com/boostorg/python>`_ and `pybind11 <https://github.com/pybind/pybind11>`_ in that it minimizes boilerplate code needed to interface with C++. It does this by automatically determining type information allowing Ruby objects to be converted to C++ and vice versa.

Rice provides:

* A simple C++-based syntax for wrapping and defining classes
* Automatic type conversions between C++ and Ruby
* Automatic exception handling between C++ and Ruby
* Smart pointers for handling garbage collection
* Support for most builtin types

Version Differences 3.x vs 4.x and later
----------------------------------------

This documentation and the ``master`` branch are for Rice 4.x and later, which is the header-only version of the library. Use the ``3.x`` branch for the docs and code for that line of releases.

To upgrade a library from Rice 3 to 4, see the :ref:`migration` guide.

The documentation for the 3.x line of Rice is viewable at https://ruby-rice.github.io/3.x.

Project Details
---------------

The source is hosted on GitHub: http://github.com/ruby-rice/rice

Bug tracking is also hosted on github: http://github.com/ruby-rice/rice/issues

API documentation: http://ruby-rice.github.io/4.x

.. _installation:

Installation
------------

.. code-block:: shell

  gem install rice

Rice is header-only library and therefore does not need to be built separately. Instead it should be #included in your C++ project. Rice requires C++17 or later and is tested on Windows (MSVC and Mingw64), MacOS (Xcode/clang) and Linux (g++).

