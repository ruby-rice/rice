.. _methods:

Functions and Methods
=====================

In the tutorial we touched upon how to wrap C++ functions, static member functions and member functions. Now let's go into more depth.

.. _default_arguments:

Default Arguments
-----------------

Going back to our initial C++ class example, lets say that ``hello()`` now takes more arguments, one of which has a default value:

.. code-block:: cpp

  class Test
  {
  public:
    Test();
    std::string hello(std::string first, std::string second = "world");
  };

As default parameter information is not available through templates, it is necessary to define this in Rice explicitly using ``Rice::Arg``:

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

The syntax is ``Arg(nameOfParameter)[ = defaultValue]``. The name of the parameter is not important here (it is for readability), but the value set via ``operator=`` must match the type of the parameter. As such it may be necessary to explicitly cast the default value.

.. code-block:: cpp

  .define_method("hello",
     &Test::hello,
     Arg("hello"), Arg("second") = (std::string)"world"
  );

These ``Rice::Arg`` objects must be in the correct positional order. Thus if the second argument has a default value, then there must be two Arg objects.

Now, Ruby will now know about the default arguments, and this wrapper can be used as expected:

.. code-block:: ruby

  t = Test.new
  t.hello("hello")
  t.hello("goodnight", "moon")

This also works with Constructors:

.. code-block:: cpp

  .define_constructor(Constructor<SomeClass, int, int>(),
      Arg("arg1") = 1, Arg("otherArg") = 12);

.. _return:

Return
-------
Similarly to the ``Arg`` class, Rice also supports a ``Return`` class that let's you tell Rice how to handle returned values from C++. This is particularly important in correctly managing memory (see :ref:`cpp_to_ruby`).

It is also helpful in dealing with Ruby's VALUE type which represent Ruby objects. Most of the time Rice will automatically handle VALUE instances, but if a native method takes a VALUE argument or returns a VALUE instance then you have tell Rice about it.

This is because VALUE is a typedef for ``unsigned long long`` - under the hood it is really a pointer to the heap. However, to Rice it is just a big integer that needs to be converted to a Ruby numeric value. As a result, if a method takes a VALUE parameter then Rice will convert it to a C++ long long value instead of  passing it through. Similarly, if a method returns a VALUE then Rice will also convert it to a numeric Ruby object as opposed to simply returning it.

To avoid this incorrect conversion, use the ``setValue()`` method on the ``Arg`` and ``Return`` classes. For example:

.. code-block:: cpp

  VALUE some_function(VALUE ary)
  {
    VALUE new_ary = rb_ary_dup(ary);
    rb_ary_push(new_ary, Qtrue);
    return new_ary;
  }

  define_global_function("some_function", &some_function, Arg("ary").setValue(), Return.setValue());

Note that oyu can mix ``Arg`` and ``Return`` objects in any order. For example this also works:

.. code-block:: cpp

  define_global_function("some_function", &some_function, Return.setValue(), Arg("ary").setValue());

.. _return_self:

Return Self
-----------
In the case of methods that return ``self`` - meaning they return back the same C++ object that was the receiver of the function call - Rice does ensure that the same Ruby object is returned. Returning self is a common pattern in Ruby.

For example:

.. code-block:: ruby

  a = Array.new
  a << 1 << 2 << 3 << 4

The above code works because the ``<<`` method returns the Array ``a``. You can mimic this behavior by the use of lambdas when wrapping C++ classes. For example, Rice wraps ``std::vector`` like this:

.. code-block:: cpp

  define_vector<std::vector<int32_t>>().
  define_method("<<", [](std::vector<int32_t>& self, int32_t value) -> std::vector<int32_t>&  // <----- DONT MISS THIS
  {
    self.push_back(value);
    return self;  // <------  Allows chaining on calls
  });

Pay careful attention to the lambda return type of ``std::vector<int32_t>&``. If the return type is *not* specified, then by default the lambda will return by value. That will invoke ``std::vector``'s copy constructor, resulting in *two* ``std::vector<int32_t>`` instance and two Ruby objects. Not at all what you want.
