.. _architecture_overview:

========
Overview
========

Rice is a header-only C++ library that bridges Ruby and C++ using template metaprogramming. This section explains the key architectural concepts that make Rice work.

Core Design Principles
======================

**Header-Only**
  Rice requires no compilation—just include the headers. All implementation is in ``.hpp`` and ``.ipp`` files.

**Template Metaprogramming**
  Rice uses C++ templates to automatically deduce types at compile time, eliminating boilerplate code.

**Type Safety**
  The C++ type system catches errors at compile time rather than runtime.

**Minimal Ruby C API Exposure**
  Rice wraps Ruby's C API (VALUE, rb_* functions) so users rarely interact with it directly.

Call Flow
=========

The diagram below shows how Rice's components interact when a Ruby method calls into C++:

::

    Ruby Code
        │
        ▼
    ┌─────────────────┐
    │   Ruby VM       │  Calls registered RUBY_METHOD_FUNC
    └────────┬────────┘
             │
             ▼
    ┌─────────────────┐
    │ Native::resolve │  Finds matching overload from NativeRegistry
    └────────┬────────┘
             │
             ▼
    ┌─────────────────┐
    │   From_Ruby<T>  │  Converts Ruby VALUEs to C++ types
    └────────┬────────┘
             │
             ▼
    ┌─────────────────┐
    │   C++ Method    │  Your actual C++ code executes
    └────────┬────────┘
             │
             ▼
    ┌─────────────────┐
    │   To_Ruby<T>    │  Converts C++ return value to Ruby VALUE
    └────────┬────────┘
             │
             ▼
    Ruby Code (receives result)

Key Components
==============

Rice is organized into several subsystems:

:ref:`architecture_type_binding`
  ``Data_Type<T>`` and ``Wrapper<T>`` - Binding C++ types to Ruby classes and managing object storage.

:ref:`architecture_method_binding`
  ``Native`` hierarchy - Binding C++ functions and methods to Ruby.

:ref:`types_overview`
  ``From_Ruby<T>`` and ``To_Ruby<T>`` - Converting values between languages.

:ref:`architecture_registries`
  Runtime tracking of types, methods, and instances.

:ref:`enumerators`
  Implementing Ruby-style iteration for C++ containers.

Thread Safety
=============

Rice itself is thread-safe for reading (method invocation). However:

- Type registration (``define_class``, ``define_method``) should happen during extension initialization
- Ruby's Global VM Lock (GVL) applies to all Ruby-touching code
- Use ``NoGVL`` to release the GVL for long-running C++ operations (see :ref:`gvl`)
