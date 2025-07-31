.. _introspection_registries:

Rice::Registries
================
The ``Registries`` class is the entry point to the :ref:`introspection_api`. It is a singleton class that provides access to the following Rice registries:

- :ref:`introspection_type_registry`
- :ref:`introspection_native_registry`

Usage
-----

.. code-block:: ruby

  type_registry = Registries.instance.types

Ruby API
--------

Attributes
^^^^^^^^^^
* ``natives`` - Returns a reference to ``Rice::NativeRegistry``
* ``types`` - Returns a reference to ``Rice::TypeRegistry``

Methods
^^^^^^^^^^
* ``instance`` - Returns the singleton instance of ``Rice::Registries``
