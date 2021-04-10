========
Tutorial
========

.. toctree::
  :maxdepth: 2

  tutorial/memory_management
  tutorial/type_conversions
  tutorial/inheritance
  tutorial/cpp_api

Getting started
---------------

Writing an extension with Rice is very similar to writing an extension
with the C API.

The first step is to create an extconf.rb file:

.. code-block:: ruby

  require 'mkmf-rice'
  create_makefile('test')

Note that we use `mkmf-rice` instead of `mkmf`. This will ensure that the
extension will be linked with standard C++ library and allows access to the
Rice header files.

Next we create our extension and save it to test.cpp:

.. code-block:: cpp

  extern "C"
  void Init_test()
  {
  }

Note the extern "C" line above. This tells the compiler that the
function `Init_test` should have C linkage and calling convention. This
turns off name mangling so that the Ruby interpreter will be able to
find the function (remember that Ruby is written in C, not C++).

So far we haven't put anything into the extension, so it isn't
particularly useful. The next step is to define a class so we can add
methods to it.

Defining classes
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

This will create a class called `Test` that inherits from `Object`. If we
wanted to inherit from a different class, we do so with the second parameter:

.. code-block:: cpp

  #include <rice/rice.hpp>

  using namespace Rice;

  extern "C"
  void Init_test()
  {
    Class rb_cMySocket = define_class("MySocket", rb_cIO);
  }

Note the prefix rb_c on the name of the class. This is a convention
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

Here we add a method `Test#hello` that returns the string
"Hello, World". The method takes self as an implicit parameter, but
isn't used, so we comment it out to prevent a compiler warning.

We could also add an `#initialize` method to our class:

.. code-block:: cpp

  #include <rice/rice.hpp>

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

The `initialize` method sets an instance variable `@foo` to the value 42.
The number is automatically converted to a `Fixnum` before doing the
assignment.

Note that we're chaining calls on the `Class` object. Most member
functions in `Module` and `Class` return a reference to `self`, so we can
chain as many calls as we want to define as many methods as we want.

Defining methods with lambda
----------------------------

It is also possible to define_methods using C++ lambdas. Similar
to define_method, the lambda takes self as an implicit parameter:

.. code-block:: cpp

  Class rb_cTest =
    define_class("Test")
    .define_method("hello", [](Object object) {
      return test_hello
    });

Defining functions
------------------

If is also possible to add methods to a ruby class using `define_fuction`. The difference is
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
      .define_fuction("some_function_lambda", []() {
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

In this example we use `Data_Type<>` instead of `Class` and the template version
of define_class() instead of the non-template version. This creates a binding in the Rice library
between the Ruby class `Test` and the C++ class Test.

Next, we define a function `static_hello` that is implemented by a C++ static member function.
Since we use `define_function`, there is no implicit self parameter.

Last, we define a method `hello` that is implemented by a C++ member function. When Ruby calls
this function, instead of passing an implicit self parameter, Rice is smart enough to direct the
call to the correct C++ Test instance.

Defining attributes
-------------------

C++ structures, and sometimes classes, often have public member variables that store data. Rice makes it
easy to wrap these member variables via the use of `define_attr`:

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

These attributes can then be accessed in the expected way in Ruby:

.. code-block:: ruby

  my_struct = MyStruct.new
  a = my_struct.read_only
  my_struct.write_only = 5
  my_struct.read_write = 10
  b = my_struct.read_write

Similarly, you can wrap static members via the use of `define_singleton_attr`:

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
the `std::string` returned by `hello()` into a Ruby type. Nevertheless, the
conversion works, and when we write:

.. code-block:: ruby

  test = Test.new
  puts test.hello

We get the expected result.

Rice has two template conversion functions to convert between C++ and
Ruby types:

.. code-block:: cpp

  template<typename T>
  T from_ruby(Object x);

  template<typename T>
  Object to_ruby(T const & x);

Rice includes default specializations for many of the builtin
types. To define your own conversion, you need to create two
class template specializations:

.. code-block:: cpp

  namespace Rice::detail
  {
    template<>
    struct From_Ruby<Foo>
    {
      static Foo convert(Object x)
      {
        // ...
      }
    };

    template<>
    struct To_Ruby<Foo>
    {
      static Object convert(Foo const & x)
      {
        // ...
      }
    };
  }

The implementation of these functions would, of course, depend on the
implementation of `Foo`.

Conversions for wrapped C++ types
---------------------------------

Take another look at the wrapper we wrote for the `Test` class:

.. code-block:: cpp

  extern "C"
  void Init_test()
  {
    Data_Type<Test> rb_cTest =
      define_class<Test>("Test")
      .define_constructor(Constructor<Test>())
      .define_method("hello", &Test::hello);
  }

When we called `define_class<Test>`, it created a Class for us and
automatically registered the new Class with the type system, so that the
calls:

.. code-block:: cpp

  Data_Object<Foo> obj(new Foo);
  Foo * f = detail::From_Ruby<Foo *>::convert(obj);
  Foo const * f = detail::From_Ruby<Foo const *>::convert(obj);

works as expected.

The `Data_Object` class is a wrapper for the `TypedData_Wrap_Struct` and the
`TypedData_Get_Struct` macros in C extensions. It can be used to wrap or
unwrap any class that has been assigned to a `Data_Type`. It inherits
from `Object`, so any member functions we can call on an `Object` we can
also call on a `Data_Object`:

.. code-block:: cpp

  Object object_id = obj.call("object_id");
  std::cout << object_id << std::endl;

The `Data_Object` class can be used to wrap a newly-created object:

.. code-block:: cpp

  Data_Object<Foo> foo(new Foo);

or to unwrap an already-created object:

.. code-block:: cpp

  VALUE obj = ...;
  Data_Object<Foo> foo(obj);

A `Data_Object` functions like a smart pointer:

.. code-block:: cpp

  Data_Object<Foo> foo(obj);
  foo->foo();
  std::cout << *foo << std::endl;

Like a `VALUE` or an `Object`, data stored in a `Data_Object` will be marked
by the garbage collector as long as the `Data_Object` is on the stack.

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

The `handle_my_exception` function need only rethrow the exception as a
`Rice::Exception`:

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
it to an `Exception` object. This object will later be re-raised as a
Ruby exception when control is returned to the Ruby VM.

Rice uses a similar class called `Jump_Tag` to handle symbols thrown by
Ruby's `throw`/`catch` or other non-local jumps from inside the Ruby VM.

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

We can wrap this class by using `typedef`:

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

Methods
-------

In the tutorial we touched upon how to wrap C++ functions, static member functions and
member functions. Now let's go into more depth.

Default Arguments
-----------------

Going back to our initial C++ class example, lets say that `hello()` now
takes more arguments, one of which has a default value:

.. code-block:: cpp

  class Test
  {
  public:
    Test();
    std::string hello(std::string first, std::string second = "world");
  };

As default parameter information is not available through templates,
it is necessary to define this in Rice explicitly using `Rice::Arg`:

.. code-block:: cpp

  #include <rice/rice.hpp>

  using namespace Rice;

  extern "C"
  void Init_test()
  {
    Data_Type<Test> rb_cTest =
      define_class<Test>("Test")
      .define_constructor(Constructor<Test>())
      .define_method("hello",
         &Test::hello,
         Arg("hello"), Arg("second") = "world"
      );
  }

The syntax here is `Arg(nameOfParameter)[ = defaultValue]`. The name of the
parameter is not important here (it is for readability), but the value set via `operator=`
must match the type of the parameter. As such it may be necessary to
explicitly cast the default value.

.. code-block:: cpp

  .define_method("hello",
     &Test::hello,
     Arg("hello"), Arg("second") = (std::string)"world"
  );

These `Rice::Arg` objects must be in the correct positional order. Thus if the second argument
has a default value, then there must be two Arg objects.

Now, Ruby will now know about the default arguments, and this wrapper
can be used as expected:

.. code-block:: ruby

  t = Test.new
  t.hello("hello")
  t.hello("goodnight", "moon")

This also works with Constructors:

.. code-block:: cpp

  .define_constructor(Constructor<SomeClass, int, int>(),
      Arg("arg1") = 1, Arg("otherArg") = 12);

Return Values
-------------

Every C++ object returned from a function, except for self, is wrapped in a new Ruby object.
Therefore if you make multiple calls to a C++ method that returns the same C++ object each time via a reference
or pointer, multiple wrapping Ruby objects will be created. It would be possible for Rice to track this
and return the same Ruby object each time, but at potentially significant runtime cost especially in multi-threaded
programs. As a result, Rice does not do this. By default having multiple Ruby objects wrap a C++ object is
fine since the Ruby objects do not own the C++ object. For more information please carefully read
the [Ownership](#ownership) section.

In the case of methods that return self - meaning they return back the same C++ object that was the receiver of
the function call - Rice does ensure that the same Ruby object is returned. Returning self is a common pattern in Ruby.
For example:

.. code-block:: ruby

  a = Array.new
  a << 1 << 2 << 3 << 4
  The above code works because the `<<` method returns the Array `a`. You can mimic this behavior by the use of lambdas
  when wrapping C++ classes. For example, Rice wraps std::vectors like this:

.. code-block:: cpp

  define_vector<std::vector<int32_t>>().
  define_method("<<", [](std::vector<int32_t>& self, int32_t value) -> std::vector<int32_t>&  // <----- DONT MISS THIS
  {
    self.push_back(value);
    return self;  // <------  Allows chaining on calls
  });

Pay careful attention to the lambda return type of `std::vector<int32_t>&`. If the return type is *not* specified,
then by default the lambda will return by value. That will invoke std::vector's copy consructor, resulting in
*two* std::vector<int32_t> instance and two Ruby objects. Not at all what you want.

Ownership
---------

When Rice wraps a C++ object returned either by reference or pointer, it does *not* take ownership
of that object. Instead, Rice simply keeps a copy of the reference or pointer for later use. This
is consistent with modern C++ practices where the use of a reference or pointer does not imply a transfer
of ownership. Instead, a transfer of ownership should be indicated via the use of and the appropriate type
of smart pointer as function parameter or return type.

Of course, many APIs exist that do not follow these rules. Therefore, Rice let's you override the ownership
rules for each method call. Let's look at an example:

.. code-block:: cpp

  class MyClass
  {
  }

  class Factory
  {
  public:
    static MyClass* create()
    {
      return new MyClass();;
    }
  }

  extern "C"
  void Init_test()
  {
    Data_Type<MyClass> rb_cMyClass = define_class<MyClass>("MyClass");

    Data_Type<Factory> rb_cFactory = define_class<Factory>("Factory")
        .define_function("create", &Factory::create); <--- WRONG, results in memory leak
  }


Each time Factory#create is called from Ruby, a new C++ instance of MyClass will be created. Using Rice's default rules,
this will result in a memory leak because those instance will never be freed.

.. code-block:: ruby

  1_000.times do
    my_class = Factory.create
  end

To fix this, you need to tell Rice that it should take ownership of the returned instance:

.. code-block:: cpp

  define_function("create", &Factory::create, Return().takeOwnership());

Notice the addition of the `Return().takeOwnership()`, which creates an instance of Return class and tells it
to take ownership of the object returned from C++. You can mix `Arg` and `Return` objects in any order. For example:

.. code-block:: cpp

  define_function("create", &Factory::create, Return().takeOwnership(), Arg("arg1"), Arg("arg2"), ...);

Keep Alive
----------

Sometimes it is necessary to tie the lifetime of one Ruby object to another. This often times happens with containers.
For example, imagine we have a `Listener` and a `ListenerContainer` class.


.. code-block:: cpp

  class Listener {
  };

  class ListenerContainer
  {
    public:
      void addListener(Listener* listener)
      {
        mListeners.push_back(listener);
      }

      int process()
      {
        for(const Listener& listener : mListeners)
        {
        }
      }

    private:
      std::vector<Listener*> mListeners;
  };

Assuming these classes are wrapped with Rice, next run this Ruby code:

.. code-block:: ruby

  @handler = ListenerContainer.new
  @handler.add_listener(Listener.new)
  GC.start
  @handler.process !!!! crash !!!!!

notice this and free the Ruby object. That it turn frees the underlying C++ Listener object resulting in a crash when
`process` is called.

To prevent this, we want to tie the lifetime of the Ruby listener instance to the container. This is done by calling
keepAlive() in the argument list:

.. code-block:: ruby

  define_class<ListenerContainer>("ListenerContainer")
    .define_method("add_listener", &ListenerContainer::addListener, Arg("listener").keepAlive())

With this change, when a listener is added to the container the container keeps a reference to it and will
call rb_gc_mark to keep it alive. The `Listener` object will not be freed until the container itself goes out of scope.
