.. _constants:

Constants
=========
C++ allows constants to be defined globally, on namespaces and classes/structs.

.. code-block:: cpp

    constexpr int CONSTANT_1 = 1;

    namespace SomeNamespace
    {
      constexpr int CONSTANT_2 = 2;
    }

    class MyClass
    {
      static constexpr int CONSTANT_3 = 3;
    }

These constants can be wrapped like this:

.. code-block:: cpp

  Object(rb_cObject).define_constant("CONSTANT_1", CONSTANT_1);

  Module rb_mSomeNamespace = define_module("SomeNamespace");
  rb_mSomeNamespace.define_constant("CONSTANT_2", CONSTANT_2);

  Data_Type<MyClass> rb_cMyClass = define_class<MyClass>("MyClass").
    define_constant("CONSTANT_3", CONSTANT_3);

Enums as Constants
------------------
Older C++ code sometimes uses anonymous C style enums as a hack for defining class constants. For more information see :ref:`anonymous_enums`.
