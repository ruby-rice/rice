.. _type_wrapping:

Type Wrapping
================

Type conversion usually does not make sense for C++ compound types. You likely do not want to copy instances of simple C++ structures to Ruby, and you almost never want to copy instances of C++ classes. There are a lot of reasons for this, including:

* C++ objects may contain uncopyable internal state, such as a database connection or an open file handle
* C++ has complex object lifetime rules that control how objects are created, copied and destructed that do not translate to Ruby
* A C++ object may use a lot of memory, such as a million element vector, that makes it untenable to copy it to Ruby.
* Copying data, by definition, creates two separate versions making it impossible to share data between the two languages.

As a result, a more practical approach is to create Ruby wrapper classes that allow Ruby to access C++ objects and C++ to access Ruby objects. Ruby wrappers are created via ``define_enum``, ``define_class``, etc. as described in other parts of the documentation.

The way wrapped C++ objects are stored internally depends on how they were returned to Rice. Returned references are stored as references, pointers and pointers and values are either move constructed or copy constructed.

On the flip side, from C++ you may wish to manipuate Ruby objects. This can be done by using Rice's object-oriented :ref:`cpp_api` that provides C++ wrapper classes for Ruby objects. This allows C++ code to manipulate Ruby objects.

Conversions for wrapped C++ types
---------------------------------

Take another look at the wrapper we wrote for the ``Test`` class:

.. code-block:: cpp

  extern "C"
  void Init_test()
  {
    Data_Type<Test> rb_cTest =
      define_class<Test>("Test")
      .define_constructor(Constructor<Test>())
      .define_method("hello", &Test::hello);
  }

When we called ``define_class<Test>``, it created a Class for us and automatically registered the new Class with the type system.

The ``Data_Object`` class can be used to wrap a C++ object in a Ruby object:

.. code-block:: cpp

  Data_Object<Foo> obj(new Foo);

We can then convert it back to C++:

.. code-block:: cpp

  Foo *f = detail::From_Ruby<Foo *>().convert(obj);

The ``Data_Object`` class is a wrapper for the ``TypedData_Wrap_Struct`` and the ``TypedData_Get_Struct`` macros in C extensions. It can be used to wrap or unwrap any class that has been previously defined using a ``Data_Type``.

A ``Data_Object`` functions like a smart pointer:

.. code-block:: cpp

  Data_Object<Foo> foo(obj);
  foo->foo();
  std::cout << *foo << std::endl;

Like a ``VALUE`` or an ``Object``, data stored in a ``Data_Object`` will be marked by the garbage collector as long as the ``Data_Object`` is on the stack.

Last, ``Data_Object`` inherits from ``Object``, so any Object member functions work with ``Data_Object``:

.. code-block:: cpp

  Object object_id = obj.call("object_id");
  std::cout << object_id << std::endl;