.. _Migrating:

=====================
Migrating from 3 to 4
=====================

Prior versions of Rice required compiling some initial code on ``gem install``. This has made Rice and libraries that use Rice to be difficult to use on some platforms, such as Heroku or Github Actions, where
where the appropriate shared libraries and/or build systems are not allowed or available.

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

In addition, they work with Ruby's VALUE type as opposed to Rice's Object type. This switch was made to avoid making extra copies of objects when translating between C++ and Ruby. For more information, please refer to the :doc:`type converstion <advanced/type_conversions>` section.


Functions vs Methods
--------------------

Rice now has different ``define_`` methods depending on if you are defining a method on an object or
a normal function. If you need ``self``, use ``define_method`` or ``define_singleton_method``. Otherwise
you should use ``define_function`` and ``define_singleton_function``. You can read more in :ref:`the tutorial <Defining Methods>`.

Memory Management
-----------------

Rice 4 also requires and provides more tools for explicitly defining which objects should and should not be managed by the Ruby garbage collector. Read more in :ref:`Memory Management`.
