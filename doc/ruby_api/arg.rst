.. _ruby_arg:

===
Arg
===

.. code-block:: ruby

   Rice::Arg

``Rice::Arg`` provides access to argument metadata that was passed to Rice's ``define_function`` and ``define_method`` calls. This includes the parameter name specified in the C++ binding code.

----

Instance Methods
================

name -> String
--------------

Returns the name of the argument.

**Returns:**

The argument name as a String, or ``nil`` if no name was specified.

**Description:**

The name comes from the ``Arg`` objects passed to Rice's method definition functions:

.. code-block:: cpp

   // C++ binding code
   define_method("calculate", &MyClass::calculate,
     Arg("x"), Arg("y"));  // Names "x" and "y" are accessible via introspection

.. code-block:: ruby

   # Ruby introspection
   native = Rice::Registries.instance.natives.native_methods(MyClass).first
   native.parameters.each do |param|
     puts param.arg.name  #=> "x", "y"
   end

----

Example
=======

.. code-block:: ruby

   # Generate method signatures with named parameters
   def method_signature(native)
     params = native.parameters.map do |param|
       name = param.arg.name || "_"
       type = param.klass.name
       "#{name}: #{type}"
     end.join(", ")

     ret = native.return_type&.name || "void"
     "def #{native.name}(#{params}) -> #{ret}"
   end

   registry = Rice::Registries.instance.natives
   registry.native_methods(MyClass).each do |method|
     puts method_signature(method)
   end

   # Output:
   #   def calculate(x: Integer, y: Integer) -> Integer
   #   def process(data: String) -> Array

----

See Also
========

* :ref:`ruby_parameter` - Parameter that contains the Arg
* :ref:`ruby_native` - Native wrapper for methods and functions
