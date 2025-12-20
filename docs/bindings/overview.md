# Overview

This section explains how to wrap C++ code for use in Ruby. Rice provides a simple, intuitive API that lets you expose C++ classes, methods, functions, and data to Ruby with minimal boilerplate.

## Core Concepts

Wrapping C++ code involves several key concepts:

**Classes and Modules**

Use `define_class<T>()` to wrap a C++ class as a Ruby class, or `define_module()` to create a Ruby module for organizing functions. See [Constructors](constructors.md) for details on defining classes.

**Methods and Functions**

Use `define_method()` to wrap C++ member functions as Ruby instance methods, and `define_function()` or `define_singleton_function()` for static/class methods. See [Methods](methods.md) for details.

**Attributes**

Use `define_attr()` to expose C++ member variables as Ruby attributes with getter/setter methods. See [Attributes](attributes.md) for details.

**Type Conversion**

Rice automatically converts between C++ and Ruby types. For fundamental types (int, float, string, etc.), values are copied. For wrapped C++ classes, objects are shared between the two languages. See [Type Conversion](../types/conversion.md) for details.

## Quick Reference

| Task | Rice API |
|------|----------|
| Wrap a C++ class | `define_class<MyClass>("MyClass")` |
| Add a constructor | `.define_constructor(Constructor<MyClass, Args...>())` |
| Wrap a member function | `.define_method("name", &MyClass::method)` |
| Wrap a static function | `.define_singleton_function("name", &MyClass::static_method)` |
| Expose a member variable | `.define_attr("name", &MyClass::member)` |
| Wrap an enum | `define_enum<MyEnum>("MyEnum")` |
| Add a constant | `.define_constant("NAME", value)` |

## Topics

- [Constructors](constructors.md) - Defining how Ruby creates C++ objects
- [Methods](methods.md) - Wrapping C++ functions and member functions
- [Overloaded Methods](overloaded_methods.md) - Handling C++ function overloading
- [Attributes](attributes.md) - Exposing C++ member variables
- [Enums](enums.md) - Wrapping C++ enumerations
- [Constants](constants.md) - Defining Ruby constants
- [Inheritance](inheritance.md) - Mapping C++ class hierarchies to Ruby
- [Operators](operators.md) - Wrapping C++ operators
- [Iterators](iterators.md) - Exposing C++ iterators to Ruby
- [Pointers](pointers.md) - Working with C++ pointers
- [Buffers](buffers.md) - Managing memory buffers
- [Exceptions](exceptions.md) - Exception handling between C++ and Ruby
- [Memory Management](memory_management.md) - Controlling object lifetimes
- [Callbacks](callbacks.md) - Calling Ruby from C++
- [GVL](gvl.md) - Managing Ruby's Global VM Lock
