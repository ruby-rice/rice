.. _functions:

Functions and Methods
=====================

In the tutorial we touched upon how to wrap C++ functions, static member functions and
member functions. Now let's go into more depth.

Default Arguments
-----------------

Going back to our initial C++ class example, lets say that ``hello()`` now
takes more arguments, one of which has a default value:

.. code-block:: cpp

  class Test
  {
  public:
    Test();
    std::string hello(std::string first, std::string second = "world");
  };

As default parameter information is not available through templates,
it is necessary to define this in Rice explicitly using ``Rice::Arg``:

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

The syntax here is ``Arg(nameOfParameter)[ = defaultValue]``. The name of the
parameter is not important here (it is for readability), but the value set via ``operator=``
must match the type of the parameter. As such it may be necessary to
explicitly cast the default value.

.. code-block:: cpp

  .define_method("hello",
     &Test::hello,
     Arg("hello"), Arg("second") = (std::string)"world"
  );

These ``Rice::Arg`` objects must be in the correct positional order. Thus if the second argument
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

VALUE arguments
---------------

The Ruby API uses a type called VALUE to represent Ruby objects. Most of the time you will not have to deal with VALUEs since Rice does it for you.

However, if a native method takes or returns a VALUE then you have to tell Rice about it. That is because VALUE is a typedef for long long and thus Rice cannot distinguish them because they are the same type. As a result, if a method takes a VALUE parameter then Rice will convert it to a C++ long long value instead of  passing it through. Similarly, if a method returns a VALUE then Rice will also convert it to a numeric Ruby object as opposed to simply returning it.

To avoid this incorrect conversion, use the ``isValue()`` method on the ``Arg`` and ``Return`` classes. For example:

.. code-block:: cpp

  VALUE some_function(VALUE ary)
  {
    VALUE new_ary = rb_ary_dup(ary);
    rb_ary_push(new_ary, Qtrue);
    return new_ary;
  }

  define_global_function("some_function", &some_function, Arg("ary").isValue(), Return.isValue());

Return Values
-------------

Every C++ object returned from a function, except for ``self``, is wrapped in a new Ruby object.
Therefore if you make multiple calls to a C++ method that returns the same C++ object each time via a reference
or pointer, multiple wrapping Ruby objects will be created. It would be possible for Rice to track this
and return the same Ruby object each time, but at potentially significant runtime cost especially in multi-threaded
programs. As a result, Rice does not do this. By default having multiple Ruby objects wrap a C++ object is
fine since the Ruby objects do not own the C++ object. For more information please carefully read
the :ref:`Ownership` section below.

In the case of methods that return ``self`` - meaning they return back the same C++ object that was the receiver of
the function call - Rice does ensure that the same Ruby object is returned. Returning self is a common pattern in Ruby.
For example:

.. code-block:: ruby

  a = Array.new
  a << 1 << 2 << 3 << 4

The above code works because the ``<<`` method returns the Array ``a``. You can mimic this behavior by the use of lambdas
when wrapping C++ classes. For example, Rice wraps ``std::vector`` like this:

.. code-block:: cpp

  define_vector<std::vector<int32_t>>().
  define_method("<<", [](std::vector<int32_t>& self, int32_t value) -> std::vector<int32_t>&  // <----- DONT MISS THIS
  {
    self.push_back(value);
    return self;  // <------  Allows chaining on calls
  });

Pay careful attention to the lambda return type of ``std::vector<int32_t>&``. If the return type is *not* specified,
then by default the lambda will return by value. That will invoke ``std::vector``'s copy constructor, resulting in
*two* ``std::vector<int32_t>`` instance and two Ruby objects. Not at all what you want.

.. _Ownership:

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

Notice the addition of the ``Return().takeOwnership()``, which creates an instance of Return class and tells it
to take ownership of the object returned from C++. You can mix ``Arg`` and ``Return`` objects in any order. For example:

.. code-block:: cpp

  define_function("create", &Factory::create, Return().takeOwnership(), Arg("arg1"), Arg("arg2"), ...);

.. _Keep Alive:

Keep Alive
----------

Sometimes it is necessary to tie the lifetime of one Ruby object to another. This often times happens with containers.
For example, imagine we have a ``Listener`` and a ``ListenerContainer`` class.


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

Assuming these classes are wrapped with Rice, when the following Ruby code runs:

.. code-block:: ruby

  @handler = ListenerContainer.new
  @handler.add_listener(Listener.new)
  GC.start
  @handler.process !!!! crash !!!!!

Ruby will notice that the ``Listener.new`` object is orphaned and will free it.
That it turn frees the underlying C++ Listener object resulting in a crash when ``process`` is called.

To prevent this, we want to tie the lifetime of the Ruby listener instance to the container. This is done by calling
``keepAlive()`` in the argument list:

.. code-block:: ruby

  define_class<ListenerContainer>("ListenerContainer")
    .define_method("add_listener", &ListenerContainer::addListener, Arg("listener").keepAlive())

With this change, when a listener is added to the container the container keeps a reference to it and will
call ``rb_gc_mark`` to keep it alive. The ``Listener`` object will not be freed until the container itself goes out of scope.
