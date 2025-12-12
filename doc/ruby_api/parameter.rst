.. _ruby_parameter:

=========
Parameter
=========

.. code-block:: ruby

   Rice::Parameter

``Rice::Parameter`` provides access to parameter information for C++ functions and methods. Each parameter has a type (Ruby class) and associated argument metadata.

----

Instance Methods
================

klass -> Class
--------------

Returns the Ruby class that represents the parameter's type.

**Returns:**

The Ruby Class for this parameter's type.

.. code-block:: ruby

   native = Rice::Registries.instance.natives.native_methods(MyClass).first
   native.parameters.each do |param|
     puts "Type: #{param.klass.name}"
   end

----

arg -> Arg
----------

Returns the :ref:`ruby_arg` associated with this parameter.

**Returns:**

An ``Arg`` object containing metadata like the parameter name.

.. code-block:: ruby

   native = Rice::Registries.instance.natives.native_methods(MyClass).first
   native.parameters.each do |param|
     puts "#{param.arg.name}: #{param.klass.name}"
   end

----

Example
=======

.. code-block:: ruby

   # Generate parameter documentation for all methods
   def document_parameters(klass)
     registry = Rice::Registries.instance.natives

     registry.native_methods(klass).each do |method|
       puts "#{method.name}:"

       if method.parameters.empty?
         puts "  (no parameters)"
       else
         method.parameters.each_with_index do |param, i|
           name = param.arg.name || "arg#{i}"
           type = param.klass.name
           puts "  #{name}: #{type}"
         end
       end

       puts "  returns: #{method.return_type&.name || 'void'}"
       puts
     end
   end

   document_parameters(MyClass)

----

See Also
========

* :ref:`ruby_native` - Native wrapper that contains parameters
* :ref:`ruby_arg` - Argument metadata
