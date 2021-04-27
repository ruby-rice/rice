.. _types_overview:

Types Overview
================

The purpose of Rice is to enable native C++ code and Ruby code work together. This requires making it easy to translate types between the two languages.

There are three main use cases:

1. Converting, i.e copying, types between the two languages (:doc:`Type conversion <type_conversions>`)
2. Enable Ruby to access C++ code via a Ruby wrapper (define_class, define_enum, etc)
3. Enable C++ to access Ruby code via a C++ wrapper (:doc:`C++ interface <cpp_api>`)

:doc:`Type conversion <type_conversions>` works well for primitive types such as boolean and numeric types. For example, a C++ unsigned 32 bit integer is copied into a Ruby Fixnum instance (and vice versa). Conversion of primitive types is also easy to understand because its familiar to programmers. When you pass a boolean or integer into a method, you don't expect the method is going to change it - instead it just gets a copy.

However, type conversion usually does not make sense for more complex types. You likely do not want to copy instances of simple C++ structures to Ruby, and you almost never want to copy instances of C++ classes. There are a lot of reasons for this, including:

* C++ objects may contain uncopyable internal state, such as a database connection or an open file handle
* C++ has complex object lifetime rules that control how objects are created, copied and destructed that do not translate to Ruby
* A C++ object may use a lot of memory, such as a million element vector, that makes it untenable to copy it to Ruby.
* Copying data, by definition, creates two separate versions making it impossible to share data between the two languages.

As a result, a more practical approach is to provide thin wrappers that allow Ruby to access C++ objects and C++ to access Ruby objects. Ruby wrappers are created via define_enum, define_class, etc. as described in other parts of the documentation.

Last, you may wish to manipulate Ruby objects from C++ using Rice's object-oriented :doc:`C++ interface <cpp_api>` versus Ruby's C api. 