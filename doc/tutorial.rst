========
Tutorial
========

Getting started
---------------

Writing an extension with Rice is very similar to writing an extension
with the C API.

The first step is to install the Rice gem:

.. code-block:: bash

  gem install rice

Next create an extconf.rb file:

.. code-block:: ruby

  require 'mkmf-rice'
  create_makefile('test')

Note that we use ``mkmf-rice`` instead of ``mkmf``. This will ensure that the
extension will be linked with standard C++ library and allows access to the
Rice header files.

.. note::
  For advanced users - instead of using mkmf-rice you can use your own build system such as CMake. In this case you may prefer to download the Rice header file, `rice.hpp <https://github.com/jasonroelofs/rice/blob/master/include/rice/rice.hpp>`_, from github and directly include it in your source tree.
  
Next we create our extension and save it to ``test.cpp``:

.. code-block:: cpp

  extern "C"
  void Init_test()
  {
  }

Note the ``extern "C"`` line above. This tells the compiler that the
function ``Init_test`` should have C linkage and calling convention. This
turns off name mangling so that the Ruby interpreter will be able to
find the function (remember that Ruby is written in C, not C++).

So far we haven't put anything into the extension, so it isn't
particularly useful. The next step is to define a class so we can add
methods to it.

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

This will create a class called ``Test`` that inherits from ``Object``. If we
wanted to inherit from a different class, we do so with the second parameter:

.. code-block:: cpp

  #include <rice/rice.hpp>

  using namespace Rice;

  extern "C"
  void Init_test()
  {
    Class rb_cMySocket = define_class("MySocket", rb_cIO);
  }

Note the prefix ``rb_c`` on the name of the class. This is a convention
that the Ruby interpreter and many extensions tend to use. It signifies
that this is a class and not some other type of object. Some other
naming conventions that are commonly used:

* ``rb_c`` variable name prefix for a Class
* ``rb_m`` variable name prefix for a Module
* ``rb_e`` variable name prefix for an Exception type
* ``rb_``  function prefix for a function in the Ruby C API
* ``rb_f_`` function prefix to differentiate between an API function that takes Ruby objects as arguments and one that takes C argument types
* ``rb_*_s_`` indicates the function is a singleton function
* ``*_m`` suffix to indicate the function takes variable number of arguments

Also note that we don't include "ruby.h" directly. Rice has a wrapper
for ruby.h that handles some compatibility issues across platforms and
Ruby versions. Always include Rice headers before including anything
that might include "ruby.h".

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

Here we add a method ``Test#hello`` that returns the string
"Hello, World". The method takes self as an implicit parameter, but
isn't used, so we comment it out to prevent a compiler warning.

We could also add an ``#initialize`` method to our class:

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

The ``initialize`` method sets an instance variable ``@foo`` to the value 42.
The number is automatically converted to a ``Fixnum`` before doing the
assignment.

Note that we're chaining calls on the ``Class`` object. Most member
functions in ``Module`` and ``Class`` return a reference to ``self``, so we can
chain as many calls as we want to define as many methods as we want.

Defining methods with lambda
----------------------------

It is also possible to define_methods using C++ lambdas. Similar
to define_method, the lambda takes self as an implicit parameter:

.. code-block:: cpp

  Class rb_cTest =
    define_class("Test")
    .define_method("hello", [](Object& object) {
      return test_hello
    });

Note that we pass self as a reference since we do not want to copy it!

Defining functions
------------------

It is also possible to add methods to a Ruby class using ``define_function``. The difference is
that no implicit self parameter is passed. Once again, you can use function pointers
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

It's useful to be able to define Ruby classes in a C++ style rather than
using the Ruby API directly, but the real power Rice is in wrapping
already-defined C++ types.

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

This is a C++ version of the Ruby class we just created in the previous
section. To wrap it:

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
      .define_function("static_hello", &Test::static_hello)
      .define_method("hello", &Test::hello);
  }

In this example we use ``Data_Type<>`` instead of ``Class`` and the template version
of ``define_class()`` instead of the non-template version. This creates a binding in the Rice library
between the Ruby class ``Test`` and the C++ class Test.

Next, we define a function ``static_hello`` that is implemented by a C++ static member function.
Since static functions are not tied to a specific object, there is no self parameter. Therefore we use ``define_function`` instead of ``define_method``.

Last, we define a method ``hello`` that is implemented by a C++ member function. When Ruby calls
this function, instead of passing an implicit self parameter, Rice is smart enough to direct the
call to the correct C++ Test instance.

Defining attributes
-------------------

C++ structures, and sometimes classes, often have public member variables that store data. Rice makes it
easy to wrap these member variables via the use of ``define_attr``:

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
    .define_attr("read_only", &MyStruct::readOnly, Rice::AttrAccess::Read)
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
conversion works, and when we write:

.. code-block:: ruby

  test = Test.new
  puts test.hello

We get the expected result.

Rice includes default specializations for many C++ types. To define your own conversion, please refer to the :doc:`Type Conversions <advanced/type_conversions>` section.

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

When we called ``define_class<Test>``, it created a Class for us and
automatically registered the new Class with the type system, so that the
calls:

.. code-block:: cpp

  Data_Object<Foo> obj(new Foo);
  Foo * f = detail::From_Ruby<Foo *>::convert(obj);
  Foo const * f = detail::From_Ruby<Foo const *>::convert(obj);

works as expected.

The ``Data_Object`` class is a wrapper for the ``TypedData_Wrap_Struct`` and the
``TypedData_Get_Struct`` macros in C extensions. It can be used to wrap or
unwrap any class that has been assigned to a ``Data_Type``. It inherits
from ``Object``, so any member functions we can call on an ``Object`` we can
also call on a ``Data_Object``:

.. code-block:: cpp

  Object object_id = obj.call("object_id");
  std::cout << object_id << std::endl;

The ``Data_Object`` class can be used to wrap a newly-created object:

.. code-block:: cpp

  Data_Object<Foo> foo(new Foo);

or to unwrap an already-created object:

.. code-block:: cpp

  VALUE obj = ...;
  Data_Object<Foo> foo(obj);

A ``Data_Object`` functions like a smart pointer:

.. code-block:: cpp

  Data_Object<Foo> foo(obj);
  foo->foo();
  std::cout << *foo << std::endl;

Like a ``VALUE`` or an ``Object``, data stored in a ``Data_Object`` will be marked
by the garbage collector as long as the ``Data_Object`` is on the stack.

Exceptions
----------

Suppose we added a member function to our example class that throws an
exception:

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

If we were to wrap this function:

.. code-block:: cpp

  extern "C"
  void Init_test()
  {
    Data_Type<Test> rb_cTest =
      define_class<Test>("Test")
      .define_constructor(Constructor<Test>())
      .define_method("hello", &Test::hello)
      .define_method("error", &Test::error);
  }

and call it from inside Ruby:

.. code-block:: ruby

  test = Test.new
  test.error()

we would get an exception. Rice will automatically convert any
C++ exception it catches into a Ruby exception. But what if we wanted
to use a custom error message when we convert the exception, or what if
we wanted to convert to a different type of exception? We can write
an exception handler like so:

.. code-block:: cpp

  extern "C"
  void Init_test()
  {
    Data_Type<Test> rb_cTest =
      define_class<Test>("Test")
      .add_handler<MyException>(handle_my_exception)
      .define_constructor(Constructor<Test>())
      .define_method("hello", &Test::hello)
      .define_method("error", &Test::error);
  }

The ``handle_my_exception`` function need only rethrow the exception as a
``Rice::Exception``:

.. code-block:: cpp

  void handle_my_exception(MyException const & ex)
  {
    throw Exception(rb_eRuntimeError, "Goodnight, moon");
  }

And what if we want to call Ruby code from C++? These exceptions are
also converted:

.. code-block:: cpp

  Object o;
  o.call("some_function_that_raises", 42);

  protect(rb_raise, rb_eRuntimeError, "some exception msg");

Internally whenever Rice catches a C++ or a Ruby exception, it converts
it to an ``Exception`` object. This object will later be re-raised as a
Ruby exception when control is returned to the Ruby VM.

Rice uses a similar class called ``Jump_Tag`` to handle symbols thrown by
Ruby's ``throw``/``catch`` or other non-local jumps from inside the Ruby VM.

Overloaded functions
--------------------

If you try to create a member function pointer to an overloaded
function, you will get an error. So how do we wrap classes that have
overloaded functions?

Consider a class that uses this idiom for accessors:

.. code-block:: cpp

  class Container
  {
    size_t capacity(); // Get the capacity
    void capacity(size_t cap); // Set the capacity
  };

We can wrap this class by using ``typedef``:

.. code-block:: cpp

  extern "C"
  void Init_Container()
  {
    typedef size_t (Container::*get_capacity)();
    typedef void (Container::*set_capacity)(size_t);

    Data_Type<Container> rb_cContainer =
      define_class<Container>("Container")
      .define_method("capacity", get_capacity(&Container::capacity))
      .define_method("capacity=", set_capacity(&Container::capacity))
  }
