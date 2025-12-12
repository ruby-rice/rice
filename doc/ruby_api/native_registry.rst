.. _ruby_native_registry:

==============
NativeRegistry
==============

.. code-block:: ruby

   Rice::NativeRegistry

``Rice::NativeRegistry`` provides access to all C++ functions, methods, procs, and attributes wrapped by Ruby classes. It is accessed through :ref:`ruby_registries`.

----

Instance Methods
================

lookup(klass) -> Array
----------------------

Returns an array of all natives for a class.

**Parameters:**

* ``klass`` - The Ruby class to look up.

**Returns:**

An Array of :ref:`ruby_native` objects representing wrapped C++ functions, methods, attributes, and procs.

**Description:**

This is a low-level API implemented in C++. For most use cases, prefer the convenience methods below.

.. code-block:: ruby

   natives = Rice::Registries.instance.natives.lookup(MyClass)
   natives.each { |native| puts native.name }

----

native_methods(klass) -> Array
------------------------------

Returns a list of native methods for a class.

**Parameters:**

* ``klass`` - The Ruby class to look up.

**Returns:**

An Array of :ref:`ruby_native` objects where ``kind`` is ``Method``.

.. code-block:: ruby

   methods = Rice::Registries.instance.natives.native_methods(MyClass)
   methods.each do |method|
     puts "#{method.name} -> #{method.return_type}"
   end

----

native_functions(klass) -> Array
--------------------------------

Returns a list of native functions (class/module methods) for a class.

**Parameters:**

* ``klass`` - The Ruby class or module to look up.

**Returns:**

An Array of :ref:`ruby_native` objects where ``kind`` is ``Function``.

.. code-block:: ruby

   functions = Rice::Registries.instance.natives.native_functions(MyModule)
   functions.each do |func|
     puts "#{func.name} -> #{func.return_type}"
   end

----

native_attributes(klass) -> Array
---------------------------------

Returns a list of native attributes for a class.

**Parameters:**

* ``klass`` - The Ruby class to look up.

**Returns:**

An Array of :ref:`ruby_native` objects where ``kind`` is ``AttributeReader`` or ``AttributeWriter``.

.. code-block:: ruby

   attrs = Rice::Registries.instance.natives.native_attributes(MyClass)
   attrs.each do |attr|
     puts "#{attr.name} (#{attr.kind})"
   end

----

Example
=======

.. code-block:: ruby

   registry = Rice::Registries.instance.natives

   # Get all methods for a class, grouped by name
   methods = registry.native_methods(MyClass).group_by(&:name)

   methods.each do |name, overloads|
     puts "#{name}:"
     overloads.each do |method|
       params = method.parameters.map { |p| p.klass.name }.join(", ")
       puts "  (#{params}) -> #{method.return_type}"
     end
   end

   # Filter by kind manually
   def native_by_kind(klass, kinds)
     Rice::Registries.instance.natives.lookup(klass).find_all do |native|
       kinds.include?(native.kind)
     end
   end

   procs = native_by_kind(MyClass, [Rice::NativeKind::Proc])

----

See Also
========

* :ref:`ruby_registries` - Entry point for accessing registries
* :ref:`ruby_native` - Individual native wrapper objects
* :ref:`ruby_type_registry` - Registry of wrapped C++ types
