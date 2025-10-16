.. _type_verification:

Type Verification
==================

Rice is very picky about types - it will throw an exception on startup if it comes across a type that has not been defined. Hopefully this will not come as a surprise to most C++ developers.

For example, let's look at the following contrived example:

.. code-block:: cpp

  class MyClass
  {
  };

  using Fancy_T = std::vector<std::unique_ptr<std::pair<std::string, MyClass>>>

  class Factory
  {
    Fancy_T& make_fancy_type();
  };

  void setupRice()
  {
    define_class<Factory>("Factory").
      define_method("make_fancy_type", &Factory::make_fancy_type);
  }

When Ruby loads the Rice extension above it will throw an exception. The reason is that ``MyClass`` has not yet been defined to Rice.

When ``define_method`` is called, Rice will see that the return type for ``&Factory::make_fancy_type`` is ``Fancy_T``. It will then drill down through the vector to the unique_ptr to the pair to ``MyClass``. When it encounters ``MyClass`` it will check its internal ``TypeRegistry`` and realize it has not yet been defined and throw a ``std::runtime_exception``.

To fix this requires first defining ``MyClass`` like this:

.. code-block:: cpp

  void setupRice()
  {
    define_class<MyClass>("MyClass");

    define_class<Factory>("Factory").
      define_method("make_fancy_type", &Factory::make_fancy_type);
  }
