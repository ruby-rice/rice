.. _ruby_registries:

==========
Registries
==========

.. code-block:: ruby

   Rice::Registries

``Rice::Registries`` is the entry point to the introspection API. It is a singleton class that provides access to Rice's internal registries for type and native method information.

----

Class Methods
=============

instance -> Registries
----------------------

Returns the singleton instance of ``Rice::Registries``.

**Returns:**

The singleton ``Registries`` instance.

.. code-block:: ruby

   registries = Rice::Registries.instance

----

Instance Methods
================

natives -> NativeRegistry
-------------------------

Returns a reference to the native registry.

**Returns:**

A :ref:`ruby_native_registry` instance containing all wrapped C++ functions, methods, and attributes.

.. code-block:: ruby

   native_registry = Rice::Registries.instance.natives
   native_registry.native_methods(MyClass)

----

types -> TypeRegistry
---------------------

Returns a reference to the type registry.

**Returns:**

A :ref:`ruby_type_registry` instance containing all wrapped C++ types.

.. code-block:: ruby

   type_registry = Rice::Registries.instance.types
   type_registry.klasses.each { |klass| puts klass.name }

----

Example
=======

.. code-block:: ruby

   # Get the registries singleton
   registries = Rice::Registries.instance

   # List all wrapped classes
   puts "Wrapped classes:"
   registries.types.klasses.each do |klass|
     puts "  #{klass.name}"
   end

   # Get native methods for a specific class
   puts "\nMethods for MyClass:"
   registries.natives.native_methods(MyClass).each do |native|
     params = native.parameters.map { |p| "#{p.arg.name}: #{p.klass}" }.join(", ")
     puts "  #{native.name}(#{params}) -> #{native.return_type}"
   end

----

See Also
========

* :ref:`ruby_type_registry` - Registry of wrapped C++ types
* :ref:`ruby_native_registry` - Registry of wrapped C++ functions and methods
* :ref:`introspection_enable` - How to enable the introspection API
