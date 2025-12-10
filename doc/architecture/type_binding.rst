.. _architecture_type_binding:

============
Type Binding
============

Type binding connects C++ types to Ruby classes. This is handled by two key classes: ``Data_Type<T>`` for registration and ``Wrapper<T>`` for object storage.

Data_Type<T>
============

``Data_Type<T>`` is the central class that binds a C++ type to a Ruby class. When you write:

.. code-block:: cpp

   define_class<MyClass>("MyClass")

Rice creates a Ruby class and registers the binding in the ``TypeRegistry``. This mapping persists for the lifetime of the Ruby process.

**Key responsibilities:**

- Creates Ruby class with ``rb_define_class``
- Sets up ``rb_data_type_t`` for Ruby's garbage collector
- Provides fluent interface for defining methods, constructors, attributes
- Tracks inheritance relationships between C++ types

Class Hierarchy
---------------

``Data_Type<T>`` inherits from ``Class``, which inherits from ``Module``:

::

    Module
      │
      └── Class
            │
            └── Data_Type<T>

This means ``Data_Type<T>`` has access to all module/class methods like ``define_method``, ``define_singleton_method``, ``include_module``, etc.

One-Time Binding
----------------

Each C++ type can only be bound once. Attempting to bind the same type twice raises an error:

.. code-block:: cpp

   define_class<Foo>("Foo");   // OK
   define_class<Foo>("Bar");   // Error! Foo already bound

This is enforced by static class members that track binding state.

Wrapper<T>
==========

When a C++ object is returned to Ruby, it must be stored in a way Ruby's garbage collector can manage. ``Wrapper<T>`` handles this.

Template Specializations
------------------------

Different specializations handle different storage needs:

**Value types** (``Wrapper<T>``)

.. code-block:: cpp

   template <typename T>
   class Wrapper : public WrapperBase
   {
     T data_;  // Stores object directly (copy or move)
   };

**Pointer types** (``Wrapper<T*>``)

.. code-block:: cpp

   template <typename T>
   class Wrapper<T*> : public WrapperBase
   {
     T* data_;      // Stores pointer
     bool isOwner_; // Tracks ownership for deletion
   };

**Reference types** (``Wrapper<T&>``)

.. code-block:: cpp

   template <typename T>
   class Wrapper<T&> : public WrapperBase
   {
     T& data_;  // Stores reference (never owns)
   };

Ownership
---------

The ``isOwner_`` flag determines whether the wrapper deletes the C++ object when Ruby garbage collects it:

- **Owner**: Wrapper deletes the object in its destructor
- **Non-owner**: Wrapper leaves the object alone

Ownership rules:

- Objects created via Ruby constructors are owned
- Objects returned by value are owned (moved into wrapper)
- Objects returned by pointer: controlled by ``Return().takeOwnership()``
- Objects returned by reference are never owned

Ruby Integration
================

rb_data_type_t
--------------

Rice uses Ruby's typed data API (``rb_data_type_t``) to integrate with the garbage collector:

.. code-block:: cpp

   rb_data_type_t rb_data_type = {
     .wrap_struct_name = "MyClass",
     .function = {
       .dmark = mark_callback,   // Mark referenced Ruby objects
       .dfree = free_callback,   // Delete C++ object
       .dsize = size_callback    // Report memory usage
     }
   };

**mark callback**: Called during GC marking phase. Rice marks any Ruby objects stored in ``keepAlive_`` to prevent premature collection.

**free callback**: Called when Ruby collects the object. Deletes the C++ object if the wrapper is the owner.

**size callback**: Reports memory usage for Ruby's memory profiling.

Keep Alive
----------

Sometimes a C++ object holds references to Ruby objects that must not be collected. The ``keepAlive_`` mechanism handles this:

.. code-block:: cpp

   class WrapperBase
   {
     std::vector<VALUE> keepAlive_;  // Ruby objects to mark

     void addKeepAlive(VALUE value);
     void ruby_mark();  // Marks all keepAlive_ values
   };

This is used by the ``Return().keepAlive()`` feature to prevent Ruby arguments from being collected while the C++ object references them.

See Also
========

- :ref:`types_overview` - How types are converted
- :ref:`cpp_to_ruby` - Ownership when returning objects
- :ref:`ruby_to_cpp` - Ownership when receiving objects
