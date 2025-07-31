.. _introspection_parameter:

Rice::Parameter
===============
The ``Parameter`` class provides access to a C++ function or method's parameter list.

Usage
-----

.. code-block:: ruby

  natives = Registries.instance.natives.native_methods(klass).sort.group_by(&:name)
  natives.each do |native|
    native.parameters.map do |parameter|
      "#{parameter.arg.name}: #{parameter.klass}"
    end.join(", ")
  end

Ruby API
--------

Methods
^^^^^^^
* ``klass`` - Returns the Ruby class that represents the Parameter type
* ``arg`` - Returns the ``Arg`` associated with the ``Parameter``
