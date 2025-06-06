.. _migration:

=====================
Migration
=====================

Version 4.5 to 4.6
------------------
Verison 4.6 has a couple of breaking changes.

First, all C++ STL classes are now defined in the ``Std`` module. Therefore the following methods wree removed - ``define_vector_under``, ``define_map_under`` and ``define_unordered_map_under``.

Second, Rice used to automatically convert Ruby Arrays of fundamental types to std::vectors (and similarly Hashes to std::map). Now you must use :ref:Buffers<buffer>` instead.

Version 4.* to 4.5
------------------
Version 4.5 is significantly more powerful that previous Rice versions. For the most part it is backwards compatible. The exception is if you defined a custom ``From_Ruby`` instantiation. In that case, you will need to add a ``convertible`` method. See the :ref:`from_ruby_specialization` documentation for an example.

Version 3 to 4
--------------
Version 3 and earlier required compiling code on ``gem install``. This made it difficult to use Rice on some platforms, such as Heroku or Github Actions, where the appropriate shared libraries and/or build systems are not allowed or available.

Rice 4 transitions to being a header only library, making it much easier for libraries using Rice to provide binary builds. However, this work was substantial and required making a few backwards-incompatible changes This page documents the major changes that any one using Rice 3 will need to apply to their libraries to work with Rice 4 or newer.

#include <rice/rice.hpp>
------------------------

The first change is that Rice is now published as a single, combined header file, so all includes can be changed to just this one. There is one other header file that contains STL wrapper definitions, you can get that with ``#include <rice/stl.hpp>``. For more information see the :doc:`stl/stl` section.

to_ruby / from_ruby
-------------------

The most notable backwards-incompatible change is how you define Ruby <=> C++ type conversions.

In Rice 3, you defined the methods ``to_ruby`` and ``from_ruby`` like this:

.. code-block:: cpp

  template<>
  Foo from_ruby<Foo>(Rice::Object x)
  {
    // ...
  }

  template<>
  Rice::Object to_ruby<Foo>(Foo const & x)
  {
    // ...
  }

In Rice 4, the structure of these functions has changed significantly. You now define these methods
as a ``convert`` function in a struct, and they need to live in the ``Rice::detail`` namespace:

.. code-block:: cpp

  namespace Rice::detail
  {
    template<>
    class From_Ruby<Foo>
    {
      Foo convert(VALUE x)
      {
        // ...
      }
    };

    template<>
    class To_Ruby<Foo>
    {
      VALUE convert(Foo const & x)
      {
        // ...
      }
    };
  }

In addition, they work with Ruby's VALUE type as opposed to Rice's Object type. This switch was made to avoid making extra copies of objects when translating between C++ and Ruby. For more information, please refer to the :doc:`type conversion <bindings/type_conversions>` section.


Functions vs Methods
--------------------

Rice now has different ``define_`` methods depending on if you are defining a method on an object or
a normal function. If you need ``self``, use ``define_method`` or ``define_singleton_method``. Otherwise
you should use ``define_function`` and ``define_singleton_function``. You can read more in :ref:`the tutorial <Defining Methods>`.

Default Arguments
-----------------
They way Rice defines C++ :ref:`default_arguments` has subtly changed - this is an easy one to miss and can introduce bugs (default values will not be set correctly). Previously Rice relied on C++'s comma operator to combine arguments together:

.. code-block:: cpp

  define_method("hello", &Test::hello, (Arg("hello"), Arg("second") = "world"));

Notice that the two ``Args`` are surrounded by parentheses. Rice *no* longer support this style. Instead, just pass the ``Args`` in a more natural way without the parentheses:

.. code-block:: cpp

  define_method("hello", &Test::hello, Arg("hello"), Arg("second") = "world");

Memory Management
-----------------

Rice 4 also requires and provides more tools for explicitly defining which objects should and should not be managed by the Ruby garbage collector. Read more in :ref:`Memory Management`.
