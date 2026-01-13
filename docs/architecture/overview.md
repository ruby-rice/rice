# Overview

Rice is a header-only C++ library that bridges Ruby and C++ using template metaprogramming. This section explains the key architectural concepts that make Rice work.

## Core Design Principles

**Header-Only**
  Rice requires no compilation—just include the headers. All implementation is in `.hpp` and `.ipp` files.

**Template Metaprogramming**
  Rice uses C++ templates to automatically deduce types at compile time, eliminating boilerplate code.

**Type Safety**
  The C++ type system catches errors at compile time rather than runtime.

**Minimal Ruby C API Exposure**
  Rice wraps Ruby's C API (VALUE, rb_* functions) so users rarely interact with it directly.

## Call Flow

The diagram below shows how Rice's components interact when a Ruby method calls into C++:

```
Ruby Code
    |
    v
+------------------+
|   Ruby VM        |  Calls registered RUBY_METHOD_FUNC
+--------+---------+
         |
         v
+------------------+
| Native::resolve  |  Finds matching overload from NativeRegistry
+--------+---------+
         |
         v
+------------------+
|   From_Ruby<T>   |  Converts Ruby VALUEs to C++ types
+--------+---------+
         |
         v
+------------------+
|   C++ Method     |  Your actual C++ code executes
+--------+---------+
         |
         v
+------------------+
|   To_Ruby<T>     |  Converts C++ return value to Ruby VALUE
+--------+---------+
         |
         v
Ruby Code (receives result)
```

## Key Components

Rice is organized into several subsystems:

[Type Binding](type_binding.md)
  `Data_Type<T>` and `Wrapper<T>` - Binding C++ types to Ruby classes and managing object storage.

[Method Binding](method_binding.md)
  `Native` hierarchy - Binding C++ functions and methods to Ruby.

[Overload Resolution](overload_resolution.md)
  How Rice selects the correct C++ overload based on Ruby arguments using precision-based scoring.

[Types Overview](../types/overview.md)
  `From_Ruby<T>` and `To_Ruby<T>` - Converting values between languages.

[Registries](registries.md)
  Runtime tracking of types, methods, and instances.

[Enumerators](enumerators.md)
  Implementing Ruby-style iteration for C++ containers.

[Procs and Blocks](procs_and_blocks.md)
  Bridging Ruby procs/blocks and C++ function pointers.

## Thread Safety

Rice itself is thread-safe for reading (method invocation). However:

- Type registration (`define_class`, `define_method`) should happen during extension initialization
- Ruby's Global VM Lock (GVL) applies to all Ruby-touching code
- Use `NoGVL` to release the GVL for long-running C++ operations (see [GVL](../bindings/gvl.md))
