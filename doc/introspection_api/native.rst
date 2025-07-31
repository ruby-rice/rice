.. _introspection_native:

Rice::Native
============
The ``Native`` class wraps C++ functions, methods, attributes and procedures. There is one Native instance per wrapped native type.

Usage
-----

.. code-block:: ruby

  native_functions = Registries.instance.natives.native_functions(klass).sort.group_by(&:name)
  native_functions.each do |native_function|
    native_function.parameters.map do |parameter|
      puts native.name
    end.join(", ")
  end

Ruby API
--------

Enums
^^^^^
The ``NativeKind`` enum enables the ability to distinguish between different Native types.

It has the following values:

* Function
* Method
* AttributeReader
* AttributeWriter
* Proc

Methods
^^^^^^^^^^
* ``name`` - Returns the Ruby name of the wrapped function, method, attribute or proc
* ``kind`` - Returns the kind of the Native object. See the NativeKind Enum above
* ``return_type`` - Returns the Ruby return type of a function or method or attributes
* ``parameters`` - Returns the parameters for a function or method
* ``to_s`` - Returns a human readable description of a Native instance
