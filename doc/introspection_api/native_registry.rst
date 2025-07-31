.. _introspection_native_registry:

Rice::NativeRegistry
====================
The ``NativeRegistry`` class provides access to all of the C++ functions, methods, procs and attributes wrapped by Ruby classes.

Usage
-----

.. code-block:: ruby

  def native_by_kind(klass, kinds)
    Rice::Registries.instance.natives.lookup(klass).find_all do |native_method|
      kinds.include?(native_method.kind)
    end
  end

  def native_functions(klass)
    self.native_by_kind(klass, [Rice::NativeKind::Function])
  end

  native_functions = Registries.instance.natives.native_functions(klass).sort.group_by(&:name)


Ruby API
--------

Methods
^^^^^^^^^^
* ``lookup`` - Returns an array of natives for a class where a native is a wrapper around a C++ function, method, attribute or proc. This is a low-level API implemented in C++. You probably want to use one of the convenience methods below.

* ``native_attributes`` - Returns a list of native attributes for a class. This is implemented in Ruby.

* ``native_methods`` - Returns a list of native methods for a class. This is implemented in Ruby.

* ``native_functions`` - Returns a list of native functions for a class. This is implemented in Ruby.

