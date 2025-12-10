.. _ruby_native:

======
Native
======

.. code-block:: ruby

   Rice::Native

``Rice::Native`` wraps C++ functions, methods, attributes, and procedures. Each wrapped C++ callable has a corresponding Native instance that provides introspection capabilities.

----

NativeKind Enum
===============

``Rice::NativeKind`` distinguishes between different types of natives:

* ``Function`` - A class/module-level function (``define_function``, ``define_singleton_function``)
* ``Method`` - An instance method (``define_method``)
* ``AttributeReader`` - A getter attribute (``define_attr``)
* ``AttributeWriter`` - A setter attribute (``define_attr``)
* ``Proc`` - A Ruby proc or lambda

----

Instance Methods
================

name -> String
--------------

Returns the Ruby name of the wrapped function, method, attribute, or proc.

**Returns:**

The method name as a String.

.. code-block:: ruby

   native = Rice::Registries.instance.natives.native_methods(MyClass).first
   puts native.name  #=> "calculate"

----

kind -> NativeKind
------------------

Returns the kind of the Native object.

**Returns:**

A ``Rice::NativeKind`` enum value.

.. code-block:: ruby

   native = Rice::Registries.instance.natives.lookup(MyClass).first

   case native.kind
   when Rice::NativeKind::Function
     puts "#{native.name} is a class/module function"
   when Rice::NativeKind::Method
     puts "#{native.name} is an instance method"
   when Rice::NativeKind::AttributeReader
     puts "#{native.name} is a getter"
   when Rice::NativeKind::AttributeWriter
     puts "#{native.name} is a setter"
   end

----

return_type -> Class
--------------------

Returns the Ruby class representing the return type.

**Returns:**

The Ruby Class that the method returns, or ``nil`` for void methods.

.. code-block:: ruby

   native = Rice::Registries.instance.natives.native_methods(MyClass).first
   puts "#{native.name} returns #{native.return_type}"  #=> "calculate returns Integer"

----

parameters -> Array
-------------------

Returns the parameters for the function or method.

**Returns:**

An Array of :ref:`ruby_parameter` objects.

.. code-block:: ruby

   native = Rice::Registries.instance.natives.native_methods(MyClass).first
   native.parameters.each do |param|
     puts "  #{param.arg.name}: #{param.klass}"
   end

----

to_s -> String
--------------

Returns a human-readable description of the Native instance.

**Returns:**

A String describing the native.

.. code-block:: ruby

   native = Rice::Registries.instance.natives.native_methods(MyClass).first
   puts native.to_s

----

Example
=======

.. code-block:: ruby

   # Generate method signatures for a class
   def print_method_signatures(klass)
     registry = Rice::Registries.instance.natives

     puts "#{klass.name}"
     puts "=" * klass.name.length

     # Instance methods
     methods = registry.native_methods(klass).group_by(&:name)
     methods.each do |name, overloads|
       overloads.each do |method|
         params = method.parameters.map do |p|
           "#{p.arg.name}: #{p.klass.name}"
         end.join(", ")
         ret = method.return_type&.name || "void"
         puts "  def #{name}(#{params}) -> #{ret}"
       end
     end

     # Class functions
     functions = registry.native_functions(klass).group_by(&:name)
     functions.each do |name, overloads|
       overloads.each do |func|
         params = func.parameters.map do |p|
           "#{p.arg.name}: #{p.klass.name}"
         end.join(", ")
         ret = func.return_type&.name || "void"
         puts "  def self.#{name}(#{params}) -> #{ret}"
       end
     end

     # Attributes
     attrs = registry.native_attributes(klass)
     readers = attrs.select { |a| a.kind == Rice::NativeKind::AttributeReader }
     readers.each do |attr|
       puts "  attr_reader :#{attr.name}  # -> #{attr.return_type&.name}"
     end
   end

   print_method_signatures(MyClass)

----

See Also
========

* :ref:`ruby_native_registry` - Registry that provides Native objects
* :ref:`ruby_parameter` - Parameter information
* :ref:`ruby_arg` - Argument metadata
