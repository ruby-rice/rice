.. _Inheritance:

Inheritance
===========

Basic
-----

Inheritance is a tricky problem to solve in extensions. This is because
wrapper functions for base classes typically don't know how to accept
pointers to derived classes. It is possible to write this logic, but
the code is nontrivial.

Rice also provides a solution to this problem:

.. code-block:: cpp

  class Base
  {
  public:
    virtual void foo();
  };

  class Derived
    : public Base
  {
  };

  extern "C"
  void Init_test()
  {
    Data_Type<Base> rb_cBase =
      define_class<Base>("Base")
      .define_method("foo", &Base::foo);
    Data_Type<Derived> rb_cDerived =
      define_class<Derived, Base>("Derived");
  }

The second template parameter to define_class indicates that ``Derived``
inherits from ``Base``.

Rice does not support multiple inheritance.

Advanced
--------

Polymorphism creates yet another wrinkle in building exceptions around C++ code,
because now we have to deal with cross-language polymorphism, where C++ can call
into a Ruby subclass, and a Ruby subclass can ``super`` back into C++ land. ``super``
calls already work through ``define_class``, but making code travel from C++ into Ruby
via polymorphism is tricker. Rice provides the ``Rice::Director`` class and the
``define_director`` method to enable this code path.

Like ``SWIG_Director``, ``Rice::Director`` is a class that is used to build a proxy class
to properly send execution up or down the object hierarchy for that class. Take
the following class:

.. code-block:: cpp

  class VirtualBase {
    public:
      VirtualBase();
      virtual int doWork();
      virtual int processWorker() = 0;
  };

Due to the abstract nature of this class, we cannot directly wrap it in Rice, as
any C++ compiler will complain about trying to instantiate a virtual class.
Even without the pure virtual function, any call to ``VirtualBase::doWork`` will stop
at the C++ level and execution will not pass down into any Ruby subclasses.

To properly wrap both of these methods, use a ``Rice::Director`` subclass as a proxy
and use this new proxy class as the type to wrap with ``define_class``:

.. code-block:: cpp

  #include <rice/rice.hpp>

  class VirtualBaseProxy : public VirtualBase, public Rice::Director {
    public:
      VirtualBaseProxy(Object self) : Rice::Director(self) { }

      virtual int doWork() {
        return detail::From_Ruby<int>().convert( getSelf().call("do_work") );
      }

      int default_doWork() {
        return VirtualBase::doWork();
      }

      virtual int processWorker() {
        return detail::From_Ruby<int>().convert( getSelf().call("process_worker") );
      }

      int default_processWorker() {
        raisePureVirtual();
      }
  };

There is a lot going on here, so we'll go through each part.

.. code-block:: cpp

  class VirtualBaseProxy : public Virtualbase, public Rice::Director { }

First, the class needs to subclass both the virtual class in question and ``Rice::Director``.

.. code-block:: cpp

    public:
      VirtualBaseProxy(Object self) : Rice::Director(self) { }

For ``Rice::Director`` to work its magic, every instance of this class needs to
have a handle to its Ruby instance. The constructor
must take a ``Rice::Object`` as the first argument and pass it up into
``Rice::Director``. The code here is the minimum required for a ``Rice::Director`` proxy.

.. code-block:: cpp

      virtual int doWork() {
        return detail::From_Ruby<int>().convert( getSelf().call("do_work") );
      }

      int default_doWork() {
        return VirtualBase::doWork();
      }

Here the proxy class implements the virtual methods and provides implementations
that delegate execution in the correct direction. The actual method calls into Ruby,
providing all necessary type conversions to and from C++ types. The other method
is how Ruby calls back into C++ and is the method that must be exposed with
``define_method``. The ``default_`` prefix is a naming convention to help keep straight
which methods perform which function. If Ruby should never call into C++, then the
``default_`` implementation should call ``raisePureVirtual()``:

.. code-block:: cpp

  int default_processWorker() {
    raisePureVirtual();
  }

The method ``raisePureVirtual()`` exists to allow wrapping a pure virtual method into Ruby
(and ensuring compliation is possible) but making sure any users of this extension are
informed quickly that there's nothing callable in the C++ side of the library.

Once the proxy class is built, it's time to wrap it into Ruby:

.. code-block:: cpp

  extern "C"
  void Init_virtual() {
    define_class<VirtualBase>("VirtualBase")
      .define_director<VirtualBaseProxy>()
      .define_constructor(Constructor<VirtualBaseProxy, Rice::Object>())
      .define_method("do_work", &VirtualBaseProxy::default_doWork)
      .define_method("process_worker", &VirtualBaseProxy::default_processWorker);
  }

The wrapping is the same as is described earlier in this document. Expose the class
``VirtualBase``, and register ``VirtualBaseProxy`` as a director proxy of ``VirtualBase`` with
``Rice::Data_Type::define_director``, then ``define_method`` pointing to the proxy methods as necessary.

You must use the ``Rice::Director`` proxy class in the Constructor line, this allows proper
object construction / destruction of the types in question.

