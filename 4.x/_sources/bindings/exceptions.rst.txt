.. _exceptions:

Exceptions
===========
Rice automatically handles exceptions - making sure that C++ exceptions do not propagate to Ruby code and Ruby exceptions do not propagate to C++. If that happens then your program will crash.

It may be necessary, however, for your C++ code to call Ruby code. And that Ruby code, in turn, may call into C++ code. Rice makes it easy to handle that situation as explained below.

Exceptions Translation
----------------------
When Ruby code calls C++ functions or methods or reads or writes C++ attributes, Rice installs an exception handler that traps any raised C++ exceptions. The handler then translates the C++ exception to a Ruby exception and re-raises it so that it can be handled by the calling Ruby code.

The mapping of C++ exceptions to Ruby exceptions is summarized in the table below:

.. table::
    :widths: grid

    +--------------------------------------------+--------------------------------------+
    |  C++ Exception                             |  Ruby Exception                      |
    +============================================+======================================+
    | :class:`std::bad_alloc`                    | ``NoMemoryError``                    |
    +--------------------------------------------+--------------------------------------+
    | :class:`std::domain_error`                 | ``FloatDomainError``                 |
    +--------------------------------------------+--------------------------------------+
    | :class:`std::exception`                    | ``RuntimeError``                     |
    +--------------------------------------------+--------------------------------------+
    | :class:`std::invalid_argument`             | ``ArgumentError``                    |
    +--------------------------------------------+--------------------------------------+
    | :class:`std::filesystem::filesystem_error` | ``IOError``                          |
    +--------------------------------------------+--------------------------------------+
    | :class:`std::length_error`                 | ``RuntimeError``                     |
    +--------------------------------------------+--------------------------------------+
    | :class:`std::out_of_range`                 | ``IndexError``                       |
    +--------------------------------------------+--------------------------------------+
    | :class:`std::overflow_error`               | ``RangeError``                       |
    +--------------------------------------------+--------------------------------------+
    | :class:`std::range_error`                  | ``RangeError``                       |
    +--------------------------------------------+--------------------------------------+
    | :class:`std::regex_error`                  | ``RegexpError``                      |
    +--------------------------------------------+--------------------------------------+
    | :class:`std::system_error`                 | ``SystemCallError``                  |
    +--------------------------------------------+--------------------------------------+
    | :class:`std::underflow_error`              | ``RangeError``                       |
    +--------------------------------------------+--------------------------------------+
    | :class:`Rice::Exception`                   | ``RuntimeError``                     |
    +--------------------------------------------+--------------------------------------+
    | Any other exception                        | ``RuntimeError``                     |
    +--------------------------------------------+--------------------------------------+

Note the Rice::Exception class is a custom exception type defined by Rice and used when Rice itself needs to raise an exception.

Custom Handlers
---------------
Rice also enables you to register a custom exception handler. This can be done like this:

.. code-block:: cpp

  extern "C"
  void Init_test()
  {
    register_handler<MyException>(handle_my_exception);

    Data_Type<Test> rb_cTest =
      define_class<Test>("Test")
      .define_constructor(Constructor<Test>())
      .define_method("hello", &Test::hello)
      .define_method("error", &Test::error);
  }

The ``handle_my_exception`` can any type of exception it wants. For example, it can throw a C++ exception:

.. code-block:: cpp

  void handle_my_exception(const MyException& ex)
  {
    throw std::runtime_error(ex.what());
  }

Perhaps more useful though is to translate a C++ exception to a Ruby exception. That is done by using the ``Rice::Exception`` class like this:

.. code-block:: cpp

  void handle_my_exception(const MyException& ex)
  {
    throw Rice::Exception(rb_eRuntimeError, ex.what_without_backtrace());
  }

Handler Order
-------------
Exception handlers are applied in order in which they are registered. Thus if you register handlers A, B and C then A will be checked first, then B and then C.

Exception handlers are global, meaning they are used when Ruby calls C++ functions or reads/writes attributes. They are also applied if you use cpp_protect (see :ref:`c++_exceptions`).


Ruby Exceptions
---------------

If your C++ code calls a Ruby API it must protect the call to catch any Ruby exceptions. Rice provides a ``protect`` method do this. For example, assume you have implemented an ``each`` method to add enumerable support to a custom C++ class. The ``each`` method should yield values to a user specified block using ``rb_yield``. However if you directly call ``rb_yield`` and the Ruby code raises an exception, your program will crash. Instead, use the ``protect`` function:

.. code-block:: cpp

          .define_method("each", [](T& vector) -> Object
          {
            for (Value_T& item : vector)
            {
              VALUE element = detail::To_Ruby<Value_T>().convert(item);
              detail::protect(rb_yield, element);
            }

            return vector;

In almost all cases, the ``protect`` method will correctly maps its function parameters to the Ruby API being called. However, in rare instances you must help it deduce the correct parameter types. See the C++ Exceptions section below for an example.

Rice uses a similar class called ``Jump_Tag`` to handle symbols thrown by
Ruby's ``throw``/``catch`` or other non-local jumps from inside the Ruby VM.

.. _c++_exceptions:

C++ Exceptions
--------------
If your C++ code calls a Ruby API which then in turns calls C++ code, you will need to catch any potential C++ exceptions. This is an uncommon case, but can happen when iterating over a Ruby collection from C++. For example:

.. code-block:: cpp

      static int convertPair(VALUE key, VALUE value, VALUE user_data)
      {
        // Get the map
        std::map<T, U>* result = (std::map<T, U>*)(user_data);

        // This method is being called from Ruby so we cannot let any C++
        // exceptions propogate back to Ruby
        return cpp_protect([&]
        {
          result->operator[](From_Ruby<T>().convert(key)) = From_Ruby<U>().convert(value);
          return ST_CONTINUE;
        });
      }

      static std::map<T, U> createFromHash(VALUE value)
      {
        std::map<T, U> result;
        VALUE user_data = (VALUE)(&result);

        // MSVC needs help here, but g++ does not
        using Rb_Hash_ForEach_T = void(*)(VALUE, int(*)(VALUE, VALUE, VALUE), VALUE);
        detail::protect<Rb_Hash_ForEach_T>(rb_hash_foreach, value, convertPair, user_data);

        return result;
      }

This code creates a new :ref:`std_map` from a Ruby hash. To do this, it iterates over the hash  using ``rb_hash_foreach``. The ``rb_hash_foreach`` function takes a pointer to a C++ function, in our case called ``convertPair``. Note this is a rare case where the ``protect`` call needs help understanding the types of the method it is calling. In this case, the function signature of ``rb_hash_foreach`` is ``void(*)(VALUE, int(*)(VALUE, VALUE, VALUE), VALUE)``.

For each item in the hash, Ruby calls the ``convertPair`` function. Thus we have gone from C++ to Ruby to C++. The ``convertPair`` function must catch any raised C++ exceptions. It does that by wrapping the function's code inside a ``cpp_protect`` lambda:

.. code-block:: cpp

        // This method is being called from Ruby so we cannot let any C++
        // exceptions propagate back to Ruby
        return cpp_protect([&]
        {
          result->operator[](From_Ruby<T>().convert(key)) = From_Ruby<U>().convert(value);
          return ST_CONTINUE;
        });
