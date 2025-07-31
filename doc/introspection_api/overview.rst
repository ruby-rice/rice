.. _introspection_api:

Ruby Introspection API
======================
Rice exposes its internals to Ruby via an introspection API. This API is used by the ``rice_rbs`` script to generate :ref:`rbs_files` for a Rice extension.

.. _introspection_enable:

Enabling
--------
The introspection API is an opt-in feature and therefore must be enabled. To do this requires adding the ``<rice/ruby_api.hpp>`` header file to your extension and then calling ``Init_Ruby_Api``:

.. code-block:: cpp

  #include <rice/ruby_api.hpp>

  extern "C"
  void Init_My_Exgtension()
  {
    // Enable Introspection API
    Init_Ruby_Api();

    <Add Rice Binding Code>
  }

Classes
-------
The Introspection API includes the following classes:

- :ref:`introspection_registries`
- :ref:`introspection_type_registry`
- :ref:`introspection_native_registry`
- :ref:`introspection_native`
- :ref:`introspection_arg`
- :ref:`introspection_parameter`
