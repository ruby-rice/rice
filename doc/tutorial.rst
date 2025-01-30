.. _tutorial:

========
Tutorial
========

Getting started
---------------

First :ref:`install <installation>` Rice.

Next, create an extconf.rb file:

.. code-block:: ruby

  require 'mkmf-rice'
  create_makefile('test')

Note that we use ``mkmf-rice`` instead of ``mkmf``. This ensures that the extension will be linked with the standard C++ library and allows access to the Rice header files.

.. note::
  For advanced users - instead of using mkmf-rice you can use your own build system such as CMake. In this case you may prefer to download the Rice header file, `rice.hpp <https://github.com/ruby-rice/rice/blob/master/include/rice/rice.hpp>`_, from github and directly include it in your source tree.
  
Next we create our extension and save it to ``test.cpp``:

.. code-block:: cpp

  extern "C"
  void Init_test()
  {
  }

Note the ``extern "C"`` line above. This tells the compiler that the function ``Init_test`` should have C linkage and calling convention. This turns off name mangling so that the Ruby interpreter will be able to find the function (remember that Ruby is written in C, not C++).

So far we haven't put anything into the extension, so it isn't particularly useful. The next step is to define a class so we can add methods to it.

Defining Classes
----------------

Defining a class in Rice is a single call:

.. code-block:: cpp

  #include <rice/rice.hpp>

  using namespace Rice;

  extern "C"
  void Init_test()
  {
    Class rb_cTest = define_class("Test");
  }

This will create a class called ``Test`` that inherits from ``Object``. If we wanted to inherit from a different class, we can do so by adding a second parameter:

.. code-block:: cpp

  #include <rice/rice.hpp>

  using namespace Rice;

  extern "C"
  void Init_test()
  {
    Class rb_cMySocket = define_class("MySocket", rb_cIO);
  }

Note the prefix ``rb_c`` on the name of the class. This is a convention that the Ruby interpreter and many extensions use. It signifies that this is a class and not another type of object. Common naming conventions include:

* ``rb_c`` variable name prefix for a Class
* ``rb_m`` variable name prefix for a Module
* ``rb_e`` variable name prefix for an Exception type
* ``rb_``  function prefix for a function in the Ruby C API
* ``rb_f_`` function prefix to differentiate between an API function that takes Ruby objects as arguments and one that takes C argument types
* ``rb_*_s_`` indicates the function is a singleton function
* ``*_m`` suffix to indicate the function takes variable number of arguments

Also note that we don't include "ruby.h" directly. Rice has a wrapper for ruby.h that handles some compatibility issues across platforms and Ruby versions. Always include Rice headers before including anything that might include "ruby.h".

.. _Defining Methods:

Defining methods
----------------

Now let's add a method to our class:

.. code-block:: cpp

  #include <rice/rice.hpp>

  using namespace Rice;

  Object test_hello(Object /* self */)
  {
    String str("hello, world");
    return str;
  }

  extern "C"
  void Init_test()
  {
    Class rb_cTest =
      define_class("Test")
      .define_method("hello", &test_hello);
  }

Here we add a method ``Test#hello`` that returns the string "Hello, World". The method takes self as an implicit parameter, but isn't used, so we comment it out to prevent a compiler warning.

We can also add an ``#initialize`` method to our class:

.. code-block:: cpp

  #include <rice/rice.hpp>
  #include <rice/stl.hpp>

  using namespace Rice;

  Object test_initialize(Object self)
  {
    self.iv_set("@foo", 42);
  }

  Object test_hello(Object /* self */)
  {
    String str("hello, world");
    return str;
  }

  extern "C"
  void Init_test()
  {
    Class rb_cTest =
      define_class("Test")
      .define_method("initialize", &test_initialize)
      .define_method("hello", &test_hello);
  }

The ``initialize`` method sets an instance variable ``@foo`` to the value 42. The number is automatically converted to a ``Fixnum`` before doing the assignment.

Note that we're chaining calls on the ``Class`` object. Most member functions in ``Module`` and ``Class`` return a reference to ``self``, so we can chain as many calls as we want to define as many methods as we want.

.. note::
   If your compiler complains about "no matching overloaded function found" followed by "could not deduce template argument for 'Function_T" then that means you are working with an overloaded C++ function or method. As a result, you'll need to give Rice some help as explained in the :ref:`overloaded_methods` section.

Defining methods with lambdas
-----------------------------

It is also possible to define_methods using C++ lambdas. Similar to define_method, the lambda takes self as an implicit parameter:

.. code-block:: cpp

  Class rb_cTest =
    define_class("Test")
    .define_method("hello", [](Object& object) {
      return test_hello
    });

Note that we pass self as a reference since we do not want to copy it!

Defining functions
------------------

It is also possible to add methods to a Ruby class using ``define_function``. The difference is that no implicit self parameter is passed. Once again, you can use function pointers
or lambdas:

.. code-block:: cpp

  void some_function()
  {
    // do something
  }

  extern "C"
  void Init_test()
  {
    Class rb_cTest =
      define_class("Test")
      .define_function("some_function", &some_function);
      .define_function("some_function_lambda", []() {
        return some_function();
      });
  }

Wrapping C++ Types
------------------

It's useful to be able to define Ruby classes in a C++ style rather than using the Ruby API directly, but the real power Rice is in wrapping already-defined C++ types.

Let's assume we have the following C++ class that we want to wrap:

.. code-block:: cpp

  class Test
  {
  public:
    static std::string static_hello();
  public:
    Test();
    std::string hello();
  };

This is a C++ version of the Ruby class we just created in the previous section. To wrap it:

.. code-block:: cpp

  #include <rice/rice.hpp>
  #include <rice/stl.hpp>

  using namespace Rice;

  extern "C"
  void Init_test()
  {
    Data_Type<Test> rb_cTest =
      define_class<Test>("Test")
      .define_constructor(Constructor<Test>())
      .define_singleton_function("static_hello", &Test::static_hello)
      .define_method("hello", &Test::hello);
  }

In this example we use ``Data_Type<>`` instead of ``Class`` and the template version of ``define_class()`` instead of the non-template version. This creates a binding in the Rice library between the Ruby class ``Test`` and the C++ class Test.

Next, we define a function ``static_hello`` that is implemented by a C++ static member function. Since static functions are not tied to a specific object, there is no self parameter. Therefore we use ``define_singleton_function`` instead of ``define_method``.

Last, we define a method ``hello`` that is implemented by a C++ member function. When Ruby calls this function, instead of passing an implicit self parameter, Rice is smart enough to direct the call to the correct C++ Test instance.

Defining attributes
-------------------
C++ structures, and sometimes classes, often have public member variables that store data. Rice makes it easy to wrap these member variables via the use of ``define_attr``:

.. code-block:: cpp

  struct MyStruct
  {
    int readOnly = 0;
    int writeOnly = 0;
    int readWrite = 0;
  };

  Data_Type<MyStruct> rb_cMyStrut =
    define_class<MyStruct>("MyStruct")
    .define_constructor(Constructor<MyStruct>())
    .define_attr("read_only",  &MyStruct::readOnly, Rice::AttrAccess::Read)
    .define_attr("write_only", &MyStruct::writeOnly, Rice::AttrAccess::Write)
    .define_attr("read_write", &MyStruct::readWrite);
  }

Notice the use of ``Rice::AttrAccess::Read`` to define read-only attributes and ``Rice::AttrAccess::Write`` for write-only attributes. If you do not specify an AttrAccess value then Rice make the attribute readable and writable.

These attributes can then be accessed in the expected way in Ruby:

.. code-block:: ruby

  my_struct = MyStruct.new
  a = my_struct.read_only
  my_struct.write_only = 5
  my_struct.read_write = 10
  b = my_struct.read_write

Similarly, you can wrap static members via the use of ``define_singleton_attr``:

.. code-block:: cpp

  struct MyStruct
  {
    static int readOnly = 0;
    static int writeOnly = 0;
    static int readWrite = 0;
  };

  Data_Type<MyStruct> rb_cMyStrut =
    define_class<MyStruct>("MyStruct")
    .define_constructor(Constructor<MyStruct>())
    .define_singleton_attr("read_only", &MyStruct::readOnly, Rice::AttrAccess::Read)
    .define_singleton_attr("write_only", &MyStruct::writeOnly, Rice::AttrAccess::Write)
    .define_singleton_attr("read_write", &MyStruct::readWrite);
  }

These attributes can then be accessed in the expected way in Ruby:

.. code-block:: ruby

  a = MyStruct.read_only
  MyStruct.write_only = 5
  MyStruct.read_write = 10
  b = MyStruct.read_write


Type conversions
----------------

Rice is smart enough to convert between most Ruby and C++ objects. Let's look again at our example class:

.. code-block:: cpp

  class Test
  {
  public:
    Test();
    std::string hello();
  };

When we wrote our class, we never wrote a single line of code to convert
the ``std::string`` returned by ``hello()`` into a Ruby type. Nevertheless, the
conversion works. When we write:

.. code-block:: ruby

  test = Test.new
  puts test.hello

We get the expected result.

Rice includes default specializations for many C++ types. To define your own conversion, please refer to the :doc:`Type Conversions <bindings/type_conversions>` section.

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
  Foo *f = detail::From_Ruby<Foo *>::convert(obj);

The ``Data_Object`` class is a wrapper for the ``TypedData_Wrap_Struct`` and the ``TypedData_Get_Struct`` macros in C extensions. It can be used to wrap or unwrap any class that has been previously defined using a ``Data_Type``.

A ``Data_Object`` functions like a smart pointer:

.. code-block:: cpp

  Data_Object<Foo> foo(obj);
  foo->foo();
  std::cout << *foo << std::endl;

Like a ``VALUE`` or an ``Object``, data stored in a ``Data_Object`` will be marked by the garbage collector as long as the ``Data_Object`` is on the stack.

Last, ``Data_Object`` inherites from ``Object``, so any Object member functions work with ``Data_Object``:

.. code-block:: cpp

  Object object_id = obj.call("object_id");
  std::cout << object_id << std::endl;


Exceptions
----------
Rice automatically handles exceptions. For example, suppose a member function of our example class can throw an exception:

.. code-block:: cpp

  class MyException
    : public std::exception
  {
  };

  class Test
  {
  public:
    Test();
    std::string hello();
    void error();
  };

  extern "C"
  void Init_test()
  {
    Data_Type<Test> rb_cTest =
      define_class<Test>("Test")
      .define_constructor(Constructor<Test>())
      .define_method("hello", &Test::hello)
      .define_method("error", &Test::error);
  }

When we call the ``error`` function from Ruby, C++ will raise an exception. Rice will catch the exception and convert it into a Ruby exception:

.. code-block:: ruby

  test = Test.new
  begin
    test.error()
  rescue => e
    ..
  end

For more information about exceptions please refer to the :ref:`Exceptions` section.
