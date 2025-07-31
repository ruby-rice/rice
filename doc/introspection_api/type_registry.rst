.. _introspection_type_registry:

Rice::TypeRegistry
==================
The ``TypeRegistry`` class provides access to the C++ types that have been wrapped by Rice and their mappings to Ruby types.

Usage
-----

.. code-block:: ruby

  types = Registries.instance.types
  types.klasses.each do |klass|
    ...
  end

Ruby API
--------

Methods
^^^^^^^^^^
* ``klasses`` - Returns the list of Ruby classes that the extension defines
