# Rice - Ruby Interface for C++ Extensions

# Introduction

Rice is a C++ header-only library that serves dual purposes. First, it makes it much
easier to create Ruby bindings for existing C++ libraries. Second, it provides an
object oriented interface to Ruby's C API that makes it easy to embed Ruby and write
Ruby extensions in C++.

Rice is similar to Boost.Python and pybind11 in that it minimizes boilerplate code needed
to interface with C++. It does this by automatically determining type information allowing
Ruby object to be converted to C++ and vice versa.

What Rice gives you:
- A simple C++-based syntax for wrapping and defining classes
- Automatic type conversions between C++ and Ruby
- Automatic exception conversions between C++ and Ruby
- Smart pointers for handling garbage collection
- Wrappers for most builtin types to simplify calling code

# Version Differences 3.x vs 4.x and later

This documentation and the `master` branch are for Rice 4.x and later, which is the
header-only version of this library. Use the `3.x` branch for the docs and code for that
line of releases.

# Project Details

The source is hosted on GitHub: http://github.com/jasonroelofs/rice

Bug tracking: http://github.com/jasonroelofs/rice/issues

API documentation: http://jasonroelofs.github.io/rice

# Installation

```bash
  gem install rice
```

Rice is header-only library and therefore does not need to be built separately.
Instead it should be #included in your C++ project. Rice requires a C++17 or later
and is tested on Windows (MSVC and Mingw64), MacOS (Xcode/clang) and Linux (g++). 

# Tutorial

## Getting started

Writing an extension with Rice is very similar to writing an extension
with the C API.

The first step is to create an extconf.rb file:

```cpp
  require 'mkmf-rice'
  create_makefile('test')
```

Note that we use `mkmf-rice` instead of `mkmf`. This will ensure that the
extension will be linked with standard C++ library and allows access to the
Rice header files.

Next we create our extension and save it to test.cpp:

```cpp
  extern "C"
  void Init_test()
  {
  }
```

Note the extern "C" line above. This tells the compiler that the
function `Init_test` should have C linkage and calling convention. This
turns off name mangling so that the Ruby interpreter will be able to
find the function (remember that Ruby is written in C, not C++).

So far we haven't put anything into the extension, so it isn't
particularly useful. The next step is to define a class so we can add
methods to it.

## Defining classes

Defining a class in Rice is a single call:

```cpp
  #include <rice/rice.hpp>

  using namespace Rice;

  extern "C"
  void Init_test()
  {
    Class rb_cTest = define_class("Test");
  }
```

This will create a class called `Test` that inherits from `Object`. If we
wanted to inherit from a different class, we do so with the second parameter:

```cpp
  #include <rice/rice.hpp>

  using namespace Rice;

  extern "C"
  void Init_test()
  {
    Class rb_cMySocket = define_class("MySocket", rb_cIO);
  }
```

Note the prefix rb_c on the name of the class. This is a convention
that the Ruby interpreter and many extensions tend to use. It signifies
that this is a class and not some other type of object. Some other
naming conventions that are commonly used:

- rb_c variable name prefix for a Class
- rb_m variable name prefix for a Module
- rb_e variable name prefix for an Exception type
- rb_  function prefix for a function in the Ruby C API
- rb_f_ function prefix to differentiate between an API function that
takes Ruby objects as arguments and one that takes C argument types
- rb_*_s_ indicates the function is a singleton function
- *_m suffix to indicate the function takes variable number of
arguments

Also note that we don't include "ruby.h" directly. Rice has a wrapper
for ruby.h that handles some compatibility issues across platforms and
Ruby versions. Always include Rice headers before including anything
that might include "ruby.h".

## Defining methods

Now let's add a method to our class:

```cpp
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
```

Here we add a method `Test#hello` that returns the string
"Hello, World". The method takes self as an implicit parameter, but
isn't used, so we comment it out to prevent a compiler warning.

We could also add an `#initialize` method to our class:

```cpp
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
```

The `initialize` method sets an instance variable `@foo` to the value 42.
The number is automatically converted to a `Fixnum` before doing the
assignment.

Note that we're chaining calls on the `Class` object. Most member
functions in `Module` and `Class` return a reference to `self`, so we can
chain as many calls as we want to define as many methods as we want.

## Defining methods with lambda
It is also possible to define_methods using C++ lambdas. Similar
to define_method, the lambda takes self as an implicit parameter:

```cpp
  Class rb_cTest =
    define_class("Test")
    .define_method("hello", [](Object object)
    {
      return test_hello
    });
```

## Defining functions
If is also possible to add methods to a ruby class using `define_fuction`. The difference is
that no implicit self parameter is passed. Once again, you can use function pointers
or lambdas:

```cpp
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
      .define_fuction("some_function_lambda", []()
      {
        return some_function();
      });
  }
```

## Wrapping C++ Types

It's useful to be able to define Ruby classes in a C++ style rather than
using the Ruby API directly, but the real power Rice is in wrapping
already-defined C++ types.

Let's assume we have the following C++ class that we want to wrap:

```cpp
  class Test
  {
  public:
    static std::string static_hello();
  public:
    Test();
    std::string hello();
  };
```

This is a C++ version of the Ruby class we just created in the previous
section. To wrap it:

```cpp
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
```

In this example we use `Data_Type<>` instead of `Class` and the template version 
of define_class() instead of the non-template version. This creates a binding in the Rice library
between the Ruby class `Test` and the C++ class Test.

Next, we define a function `static_hello` that is implemented by a C++ static member function.
Since we use `define_function`, there is no implicit self parameter.

Last, we define a method `hello` that is implemented by a C++ member function. When Ruby calls
this function, instead of passing an implicit self parameter, Rice is smart enough to direct the
call to the correct C++ Test instance.  

## Defining attributes
C++ structures, and sometimes classes, often have public member variables that store data. Rice makes it
easy to wrap these member variables via the use of `define_attr`:

```cpp
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
```

These attributes can then be accessed in the expected way in Ruby:

```ruby
my_struct = MyStruct.new
a = my_struct.read_only
my_struct.write_only = 5
my_struct.read_write = 10
b = my_struct.read_write
```

Similarly, you can wrap static members via the use of `define_singleton_attr`:

```cpp
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
```

These attributes can then be accessed in the expected way in Ruby:

```ruby
a = MyStruct.read_only
MyStruct.write_only = 5
MyStruct.read_write = 10
b = MyStruct.read_write
```


## Type conversions

Rice is smart enough to convert between most Ruby and C++ objects. Let's look again at our example class:

```cpp
  class Test
  {
  public:
    Test();
    std::string hello();
  };
```

When we wrote our class, we never wrote a single line of code to convert
the `std::string` returned by `hello()` into a Ruby type. Nevertheless, the
conversion works, and when we write:

```cpp
  test = Test.new
  puts test.hello
```

We get the expected result.

Rice has two template conversion functions to convert between C++ and
Ruby types:

```cpp
  template<typename T>
  T from_ruby(Object x);

  template<typename T>
  Object to_ruby(T const & x);
```

Rice includes default specializations for many of the builtin
types. To define your own conversion, you need to create two 
class template specializations:

```cpp
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
```

The implementation of these functions would, of course, depend on the
implementation of `Foo`.


## Conversions for wrapped C++ types

Take another look at the wrapper we wrote for the `Test` class:

```cpp
  extern "C"
  void Init_test()
  {
    Data_Type<Test> rb_cTest =
      define_class<Test>("Test")
      .define_constructor(Constructor<Test>())
      .define_method("hello", &Test::hello);
  }
```

When we called `define_class<Test>`, it created a Class for us and
automatically registered the new Class with the type system, so that the
calls:

```cpp
  Data_Object<Foo> obj(new Foo);
  Foo * f = detail::From_Ruby<Foo *>::convert(obj);
  Foo const * f = detail::From_Ruby<Foo const *>::convert(obj);
```

works as expected.

The `Data_Object` class is a wrapper for the `TypedData_Wrap_Struct` and the
`TypedData_Get_Struct` macros in C extensions. It can be used to wrap or
unwrap any class that has been assigned to a `Data_Type`. It inherits
from `Object`, so any member functions we can call on an `Object` we can
also call on a `Data_Object`:

```cpp
  Object object_id = obj.call("object_id");
  std::cout << object_id << std::endl;
```

The `Data_Object` class can be used to wrap a newly-created object:

```cpp
  Data_Object<Foo> foo(new Foo);
```

or to unwrap an already-created object:

```cpp
  VALUE obj = ...;
  Data_Object<Foo> foo(obj);
```

A `Data_Object` functions like a smart pointer:

```cpp
  Data_Object<Foo> foo(obj);
  foo->foo();
  std::cout << *foo << std::endl;
```

Like a `VALUE` or an `Object`, data stored in a `Data_Object` will be marked
by the garbage collector as long as the `Data_Object` is on the stack.


## Exceptions

Suppose we added a member function to our example class that throws an
exception:

```cpp
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
```

If we were to wrap this function:

```cpp
  extern "C"
  void Init_test()
  {
    Data_Type<Test> rb_cTest =
      define_class<Test>("Test")
      .define_constructor(Constructor<Test>())
      .define_method("hello", &Test::hello)
      .define_method("error", &Test::error);
  }
```

and call it from inside Ruby:

```cpp
  test = Test.new
  test.error()
```

we would get an exception. Rice will automatically convert any
C++ exception it catches into a Ruby exception. But what if we wanted
to use a custom error message when we convert the exception, or what if
we wanted to convert to a different type of exception? We can write
an exception handler like so:

```cpp
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
```

The `handle_my_exception` function need only rethrow the exception as a
`Rice::Exception`:

```cpp
  void handle_my_exception(MyException const & ex)
  {
    throw Exception(rb_eRuntimeError, "Goodnight, moon");
  }
```

And what if we want to call Ruby code from C++? These exceptions are
also converted:

```cpp
  Object o;
  o.call("some_function_that_raises", 42);

  protect(rb_raise, rb_eRuntimeError, "some exception msg");
```

Internally whenever Rice catches a C++ or a Ruby exception, it converts
it to an `Exception` object. This object will later be re-raised as a
Ruby exception when control is returned to the Ruby VM.

Rice uses a similar class called `Jump_Tag` to handle symbols thrown by
Ruby's `throw`/`catch` or other non-local jumps from inside the Ruby VM.


## Overloaded functions

If you try to create a member function pointer to an overloaded
function, you will get an error. So how do we wrap classes that have
overloaded functions?

Consider a class that uses this idiom for accessors:

```cpp
  class Container
  {
    size_t capacity(); // Get the capacity
    void capacity(size_t cap); // Set the capacity
  };
```

We can wrap this class by using `typedef`s:

```cpp
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
```

# Methods
In the tutorial we touched upon how to wrap C++ functions, static member functions and
member functions. Now let's go into more depth.

## Default Arguments

Going back to our initial C++ class example, lets say that `hello()` now
takes more arguments, one of which has a default value:

```cpp
  class Test
  {
  public:
    Test();
    std::string hello(std::string first, std::string second = "world");
  };
```

As default parameter information is not available through templates,
it is necessary to define this in Rice explicitly using `Rice::Arg`:

```cpp
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
```

The syntax here is `Arg(nameOfParameter)[ = defaultValue]`. The name of the
parameter is not important here (it is for readability), but the value set via `operator=`
must match the type of the parameter. As such it may be necessary to
explicitly cast the default value.

```cpp
  .define_method("hello",
     &Test::hello,
     Arg("hello"), Arg("second") = (std::string)"world"
  );
```

These `Rice::Arg` objects must be in the correct positional order. Thus if the second argument
has a default value, then there must be two Arg objects.

Now, Ruby will now know about the default arguments, and this wrapper
can be used as expected:

```cpp
  t = Test.new
  t.hello("hello")
  t.hello("goodnight", "moon")
```

This also works with Constructors:

```cpp
  .define_constructor(Constructor<SomeClass, int, int>(),
      Arg("arg1") = 1, Arg("otherArg") = 12;
```

## Ownership
When Rice wraps a C++ object returned either by reference or pointer, it does *not* take ownership 
of that object. Instead, Rice simply keeps a copy of the reference or pointer for later use. This 
is consistent with modern C++ practices where the use of a reference or pointer does not imply a transfer
of ownership. Instead, a transfer of ownership should be indicated via the use of and the appropriate type 
of smart pointer as function parameter or return type.

Of course, many APIs exist that do not follow these rules. Therefore, Rice let's you override the ownership
rules for each method call. Let's look at an example:

```cpp
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

```

Each time Factory#create is called from Ruby, a new C++ instance of MyClass will be created. Using Rice's default rules, 
this will result in a memory leak because those instance will never be freed. 

```ruby
1_000.times do 
  my_class = Factory.create
end  
```

To fix this, you need to tell Rice that it should take ownership of the returned instance:

```cpp
   define_function("create", &Factory::create, Return().takeOwnership());
```

Notice the addition of the `Return().takeOwnership()`, which creates an instance of Return class and tells it 
to take ownership of the object returned from C++. You can mix `Arg` and `Return` objects in any order. For example:

```cpp
   define_function("create", &Factory::create, Return().takeOwnership(), Arg("arg1"), Arg("arg2"), ...);
```

Note that if the same C++ object is returned to Ruby multiple times, Rice will create a new wrapper Ruby object 
each time (perhaps a future enhancement will change this). In such cases, you definitely do *not* want Ruby to 
take ownership of the object.

## Keep Alive

Sometimes it is necessary to tie the lifetime of one Ruby object to another. This often times happens with containers. 
For example, imagine we have a ```Listener``` and a ```ListenerContainer``` class.

```cpp
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
```

Assuming these classes are wrapped with Rice, next run this Ruby code:

```ruby
  @handler = ListenerContainer.new
  @handler.add_listener(Listener.new)
  GC.start
  @handler.process !!!! crash !!!!!
```

After the call to ```add_listener```, there is no longer a reference to the Ruby Listener object. When the GC runs, it will
notice this and free the Ruby object. That it turn frees the underlying C++ Listener object resulting in a crash when
```process``` is called.

To prevent this, we want to tie the lifetime of the Ruby listener instance to the container. This is done by calling
keepAlive() in the argument list:

```ruby
  define_class<ListenerContainer>("ListenerContainer")
    .define_method("add_listener", &ListenerContainer::addListener, Arg("listener").keepAlive())
```

With this change, when a listener is added to the container the container keeps a reference to it and will
call rb_gc_mark to keep it alive. The `Listener` object will not be freed until the container itself goes out of scope.

# Memory Management

The trickiest part of wrapping a C++ API is correctly managing memory shared between C++ and Ruby. It is critical to
get this right - otherwise your program *will* crash. The key to getting this right is being crystal clear
on who owns each piece of memory.

Rice manages much of this work for you, but does requires some help in getting it right. 

# C++ to Ruby
As described in the [methods](#methods) section, use the Return class to specify whether ownership of objects
returned from C++ functions should be transferred to Ruby.

In the case where Ruby does *not* take ownership of the object, the transfer happens like this:

Method Return Type (T)   | C++ to Ruby       | Cleanup
------------            | -----------        | --------
Value (T)               | Copy constructor  |  Ruby frees the copy, C++ the original
Reference (T&)          | No copy           |  C++ frees the object
Pointer (T*)            | No copy           |  C++ frees the object

In the case where Ruby takes ownership of the object, the transfer happens like this:

Method Return Type (T)   | C++ to Ruby       | Cleanup
------------            | -----------        | --------
Value (T)               | Copy constructor  |  Ruby frees the copy
Reference (T&)          | Move constructor  |  Ruby frees the copy
Pointer (T*)            | No copy           |  Ruby frees C++ object

# Ruby to C++
For more information see the [keep alive](#keep-alive) section.

# C++ Referencing Ruby Objects
When reference Ruby objects from C++, you need to let Ruby know about them so they are not prematurely
garbage collected.

In simple cases, with Objects on the stack, the Ruby GC will automatically find them and you don't have to 
do anything. If instead, you allocate an Object on the heap or if it is a member of an object that might be
allocated on the heap, use an Rice::Address_Registration_Guard to register the object with the garbage
collector.

If you create classes or structures that reference Ruby objects, you need to implement a custom mark function:

```cpp
class MyClass
{
  VALUE value_;
}
  
namespace Rice
{  
  template<>
  ruby_mark<MyClass>(const MyClass* myClass)
  {
    rb_gc_mark(myClass->value_);
  }
}

  Data_Type<MyClass> class = define_class<MyClass>("MyClass")
             .define_constructor(Constructor<MyClass>());
```

# User-defined type conversions

Rice provides default conversions for many built-in types. Sometimes,
however, the default conversion is not what is expected. For
example, consider a function:

```cpp
  void foo(char * x);
```

Is `x` a pointer to a single character or a pointer to the first character
of a null-terminated string or a pointer to the first character of an
array of char?

Because the second case is the most common use case (a pointer to the
first character of a C string), Rice provides a default conversion that
treats a `char *` as a C string. But suppose the above function actually
expects to receive a pointer to a single char instead?

If we write this:

```cpp
  extern "C"
  void Init_test()
  {
    define_global_function("foo", &foo);
  }
```

It will likely have the wrong behavior.

To avoid this problem, it is necessary to write a wrapper function where
the extension can be more explicit about how to handle the parameters:

```cpp
  Object wrap_foo(Object o)
  {
    char c = from_ruby<char>(o);
    foo(&c);
    return to_ruby(c);
  }

  extern "C"
  void Init_test()
  {
    define_global_function("foo", &wrap_foo);
  }
```

Note that the out parameter is returned from `wrap_foo`, as Ruby does not
have pass-by-variable-reference (it uses pass-by-object-reference).

# Inheritance

## Basic

Inheritance is a tricky problem to solve in extensions. This is because
wrapper functions for base classes typically don't know how to accept
pointers to derived classes. It is possible to write this logic, but
the code is nontrivial.

Rice also provides a solution to this problem:

```cpp
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
```

The second template parameter to define_class indicates that `Derived`
inherits from `Base`.

Rice does not support multiple inheritance.

## Advanced

Polymorphism creates yet another wrinkle in building exceptions around C++ code,
because now we have to deal with cross-language polymorphism, where C++ can call
into a Ruby subclass, and a Ruby subclass can `super` back into C++ land. `super`
calls already work through define_class, but making code travel from C++ into Ruby
via polymorphism is tricker. Rice provides the `Rice::Director` class and the
`define_director` method to enable this code path.

Like `SWIG_Director`, `Rice::Director` is a class that is used to build a proxy class
to properly send execution up or down the object hierarchy for that class. Take
the following class:

```cpp
  class VirtualBase {
    public:
      VirtualBase();
      virtual int doWork();
      virtual int processWorker() = 0;
  };
```

Due to the abstract nature of this class, we cannot directly wrap it in Rice, as
any C++ compiler will complain about trying to instantiate a virtual class.
Even without the pure virtual function, any call to `VirtualBase::doWork` will stop
at the C++ level and execution will not pass down into any Ruby subclasses.

To properly wrap both of these methods, use a `Rice::Director` subclass as a proxy
and use this new proxy class as the type to wrap with `define_class`:

```cpp
  #include <rice/rice.hpp>

  class VirtualBaseProxy : public VirtualBase, public Rice::Director {
    public:
      VirtualBaseProxy(Object self) : Rice::Director(self) { }

      virtual int doWork() {
        return from_ruby<int>( getSelf().call("do_work") );
      }

      int default_doWork() {
        return VirtualBase::doWork();
      }

      virtual int processWorker() {
        return from_ruby<int>( getSelf().call("process_worker") );
      }

      int default_processWorker() {
        raisePureVirtual();
      }
  };
```

There is a lot going on here, so we'll go through each part.

```cpp
  class VirtualBaseProxy : public Virtualbase, public Rice::Director {
```

First, the class needs to subclass both the virtual class in question and `Rice::Director`.

```cpp
    public:
      VirtualBaseProxy(Object self) : Rice::Director(self) { }
```

For `Rice::Director` to work its magic, every instance of this class needs to
have a handle to its Ruby instance. The constructor
must take a `Rice::Object` as the first argument and pass it up into
`Rice::Director`. The code here is the minimum required for a `Rice::Director` proxy.

```cpp
      virtual int doWork() {
        return from_ruby<int>( getSelf().call("do_work") );
      }

      int default_doWork() {
        return VirtualBase::doWork();
      }
```

Here the proxy class implements the virtual methods and provides implementations
that delegate execution in the correct direction. The actual method calls into Ruby,
providing all necessary type conversions to and from C++ types. The other method
is how Ruby calls back into C++ and is the method that must be exposed with
`define_method`. The `default_` prefix is a naming convention to help keep straight
which methods perform which function. If Ruby should never call into C++, then the
`default_` implementation should call `raisePureVirtual()`:

```cpp
      int default_processWorker() {
        raisePureVirtual();
      }
```

The method `raisePureVirtual()` exists to allow wrapping a pure virtual method into Ruby
(and ensuring compliation is possible) but making sure any users of this extension are
informed quickly that there's nothing callable in the C++ side of the library.

Once the proxy class is built, it's time to wrap it into Ruby:

```cpp
extern "C"
void Init_virtual() {
  define_class<VirtualBase>("VirtualBase")
    .define_director<VirtualBaseProxy>()
    .define_constructor(Constructor<VirtualBaseProxy, Rice::Object>())
    .define_method("do_work", &VirtualBaseProxy::default_doWork)
    .define_method("process_worker", &VirtualBaseProxy::default_processWorker);
}
```

The wrapping is the same as is described earlier in this document. Expose the class
`VirtualBase`, and register `VirtualBaseProxy` as a director proxy of `VirtualBase` with
`Rice::Data_Type::define_director`, then `define_method`s pointing to the proxy methods as necessary.

You must use the `Rice::Director` proxy class in the Constructor line, this allows proper
object construction / destruction of the types in question.

## Implicit Casting

There are times when a library exposes classes that, while unrelated, are
built to be interchangeable across the library. One example of this is found in
the Open Source 3d rendering engine <a
href="http://www.ogre3d.org/">OGRE</a>: Ogre::Degree and Ogre::Radian.
When a given method takes a Radian, you're free to pass in a Degree, and vice versa.

Rice cannot automatically figure out if this kind of functionality is
possible in a given library but it does provide an API for defining
these relationships: `Rice::define_implicit_cast<From, To>()`.

```cpp
class Degree { ... };
class Radian { ... };

extern "C"
void Init_implicit() {
  define_class<Degree>()
    ...;
  define_class<Radian>()
    ...;

  define_implicit_cast<Degree, Radian>();
  define_implicit_cast<Radian, Degree>();
}
```

Using `Rice::define_implicit_cast` has the following requirements:

- The two types must be bound in Rice before defining the cast.
- The classes must have constructors that take the other type.
- This feature cannot be used with fundamental types.

To see a full example of this feature, please check out
test/test_Data_Type.cpp.

# Ruby C++ API
Rice provides builtin wrappers for many builtin Ruby types, including:

- Object
- Module
- Class
- String
- Array
- Hash
- Struct
- Symbol
- Exception

Rice mimics the Ruby class hierarchy as closely as it can.

For example:

```cpp
  Object object_id = obj.call("object_id");
  std::cout << object_id << std::endl;

  Class rb_cTest = define_class<Test>("Test");
  Object object_id = rb_cTest.call("object_id");
  std::cout << object_id << std::endl;
```

The `Array` and `Hash` types can even be iterated over the same way one
would iterate over an STL container:

```cpp
  Array a;
  a.push(to_ruby(42));
  a.push(to_ruby(43));
  a.push(to_ruby(44));
  Array::iterator it = a.begin();
  Array::iterator end = a.end();
  for(; it != end; ++it)
  {
    std::cout << *it << std::endl;
  }
```

STL algorithms should also work as expected on `Array` and `Hash` containers.

# Motivation

There are a number of common problems when writing C or C++ extensions
for Ruby:

- Type safety. It is easy to mix-up integral types such as ID and
VALUE. Some of the functions in the Ruby API are not consistent with
which types they take (e.g. rb_const_defined takes an ID and
rb_mod_remove_const takes a Symbol).

- DRY principle. Specifying the number of arguments that each wrapped
function takes is easy to get wrong. Adding a new argument to the
function means that the number of arguments passed to rb_define_method
must also be updated.

- Type conversion. There are many different functions to convert data
to and from ruby types. Many of them have different semantics or
different forms. For example, to convert a string, one might use the
StringValue macro, but to convert a fixnum, one might use FIX2INT.
Unwrapping previously wrapped C data uses yet another form.

- Exception safety. It is imperative that C++ exceptions never make
their way into C code, and it is also imperative that a Ruby exception
never escape while there are objects on the stack with nontrivial
destructors. Rules for when it is okay to use which exceptions are
difficult to get right, especially as code is maintained through time.

- Thread safety. Because the Ruby interpreter is not thread-safe,
the Ruby interpreter must not be run from more than one thread.
Because of tricks the GC and scheduler play with the C stack, it's not
enough to ensure that only one thread runs the interpreter at any
given time; once the interpreter has been run from one thread, it must
only ever be run from that thread in the future. Additionally,
because Ruby copies the stack when it switches threads, C++ code must
be careful not to access objects in one Ruby thread that were created
on the stack in another Ruby thread.

- C-based API. The Ruby API is not always convenient for accessing
Ruby data structurs such as Hash and Array, especially when writing C++
code, as the interface for these containers is not consistent with
standard containers.

- Calling convention. Function pointers passed into the Ruby API must
follow the C calling convention. This means that it is not possible to
pass a pointer to a template function or static member function (that
is, it will work on some platforms, but isn't portable).

- Inheritance. When wrapping C++ objects, it is easy to store a
pointer to a derived class, but then methods in the base class must have
knowledge of the derived class in order to unwrap the object. It is
possible to always store a pointer to the base class and then
dynamic_cast the pointer to the derived type when necessary, but this
can be slow and cumbersome, and it isn't likely to work with multiple
inheritance. A system that properly handles inheritance for all corner
cases is nontrivial.

- Multiple inheritance. C++ supports true multiple inheritance, but
the Ruby object model uses single inheritance with mixins. When
wrapping a library whose public interface uses multiple inheritance,
care must be taken in constructing the mapping.

- GC safety. All live Ruby objects must be marked during the garbage
collector's mark phase, otherwise they will be prematurely destroyed.
The general rule is that object references stored on the heap should be
either registered with rb_gc_register_address or marked by a data
object's mark function; object references stored on the stack will be
automatically marked, provided the Ruby interpreter was properly
initialized at startup.

- Callbacks. C implements callbacks via function pointers, while Ruby
typically implements callbacks via procs. Writing an adapter function
to call the proc is not difficult, but there is much opportunity for
error (particularly with exception-safety).

- Data serialization. By default data objects defined at the C layer
are not marshalable. The user must explicitly define functions to
marshal the data member-by-member.

Rice addresses these issues in many ways:

- Type safety. Rice provides encapsulation for all builtin types,
such as Object, Identifier, Class, Module, and String. It
automatically checks the dynamic type of an object before constructing
an instance of a wrapper.

- DRY principle. Rice uses introspection through the use of templates
and function overloading to automatically determine the number and types
of arguments to functions. Default arguments must still be handled
explicitly, however.

- Type conversions. Rice provides cast-style to_ruby<> and
from_ruby<> template functions to simplify explicit type conversions.
Automatic type conversions for parameters and return values are
generated for all wrapped functions.

- Exception safety. Rice automatically converts common exceptions and
provides a mechanism for converting user-defined exception types. Rice
also provides convenience functions for converting exceptions when
calling back into ruby code.

- Thread safety. Rice provides no mechanisms for dealing with thread
safety. Many common thread safety issues should be alleviated by YARV,
which supports POSIX threads.

- C++-based API. Rice provides an object-oriented C++-style API to
most common functions in the Ruby C API.

- Calling convention. Rice automatically uses C calling convention
for all function pointers passed into the Ruby API.

- Inheritance. Rice provides automatic conversion to the base class
type when a wrapped member function is called on the base class.

- Multiple inheritance. Rice provides no mechanism for multiple
inheritance. Multiple inheritance can be simulated via mixins, though
this is not yet as easy as it could be.

- GC safety. Rice provides a handful of convenience classes for
interacting with the garbage collector. There are still basic rules
which must be followed to ensure that objects get properly destroyed.

- Callbacks. Rice provides a handful of convenience classes for
dealing with callbacks.

- Data serialization. Rice provides no mechanism for data
serialization, but it is likely this may be added in a future release.


# What Rice is Not

There are a number projects which server similar functions to Rice. Two
such popular projects are SWIG and Boost.Python. Rice has some
distinct features which set it apart from both of these projects.

Rice is not trying to replace SWIG. Rice is not a generic wrapper
interface generator. Rice is a C++ library for creating Ruby bindings
for C++ libraries and interfacing with the Ruby C API. This provides
a very natural way for C++ programmers to wrap their C++ code, without
having to learn a new domain-specific language. However, there is no 
reason why SWIG and Rice could not work together; a SWIG module could
be written to generate Rice code. Such a module would combine the 
portability of SWIG with the maintainability of Rice (I have written 
extensions using both, and I have found Rice
extensions to be more maintainable when the interface is constantly
changing. Your mileage may vary).

Rice is also not trying to simply be a Ruby version of Boost.Python.
Rice does use some of the same template tricks that Boost.Python uses,
however there are some important distinctions. First of all,
Boost.Python attempts to create a declarative DSL in C++ using
templates. Rice is a wrapper around the Ruby C API and attempts to make
its interface look like an OO version of the API; this means that class
declarations look procedural rather than declarative. Secondly, the
Ruby object model is different from the python object model. This is
reflected in the interface to Rice; it mimics the Ruby object model at
the C++ level.


# History

Rice originated as Excruby, a project to interface with C++-based trading
software at Automated Trading Desk in Mount Pleasant, South Carolina.
The Ruby bindings for Swig were at the time less mature than they are
today, and did not suit the needs of the project.

Excruby was written not as a wrapper for the Ruby API, but rather as a
set of helper functions and classes for interfacing with the Ruby
interpreter in an exception-safe manner. Over the course of five years,
the project grew into wrappers for pieces of the API, but the original
helper functions remained as part of the public interface.

This created confusion for the users of the library, because there were
multiple ways of accomplishing most tasks -- directly through the C API,
through a low-level wrapper around the C API, and through a high-level
abstraction of the lower-level interfaces.

Rice was then born in an attempt to clean up the interface. Rice keeps
the lower-level wrappers, but as an implementation detail; the public
interface is truly a high-level abstraction around the Ruby C API.
