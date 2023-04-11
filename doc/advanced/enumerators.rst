.. _enumerators:

Enumerators
===========

Although not used that frequently, Ruby supports enumerators that enable both internal and external iteration. The easiest way to create an enumerator is to not pass a block to an enumerable method. For example:

.. code-block:: ruby

    a = [1, 2, 3, 4, 5]

    # Common way to iterate
    a.each do |i|
      puts i
    end

    # Get an enumerator instead
    enumerator = a.each

    # Now  use it
    enumerator.map |i|
      i * 2
    end

Rice has built-in support to returning enumerators for STL containers such as :ref:`std_vector`, :ref:`std_map` and :ref:`std_unordered_map`.

Implementing enumerators is tricky - and in fact requires a number of Rice features. So let's take a look at how enumerator support is implemented for std::vector.

Implementation
--------------
Let's start with looking at the code:

.. code-block:: cpp

  define_method("each", [](T& vector) -> const std::variant<std::reference_wrapper<T>, Object>
  {
    if (!rb_block_given_p())
    {
      auto rb_size_function = [](VALUE recv, VALUE argv, VALUE eobj) -> VALUE
      {
        // Since we can't capture the vector from above (because then we can't send
        // this lambda to rb_enumeratorize_with_size), extract it from recv
        T* receiver = Data_Object<T>::from_ruby(recv);
        return detail::To_Ruby<size_t>().convert(receiver->size());
      };

      return rb_enumeratorize_with_size(detail::selfThread, Identifier("each").to_sym(), 0, nullptr, rb_size_function);
    }

    for (Value_T& item : vector)
    {
      VALUE element = detail::To_Ruby<Value_T>().convert(item);
      detail::protect(rb_yield, element);
    }

    return std::ref(vector);
  });

We will go through each section in more detail below.

Method Signature
----------------

First, Rice defines an ``each`` method to support Ruby's enumerable module. Its signature looks like this:

.. code-block:: cpp

  define_method("each", [](T& vector) -> const std::variant<std::reference_wrapper<T>, Object>

Since std::vector doesn't have an ``each`` method, Rice creates a lambda function instead that interacts with the vector. The vector is passed in by reference ``T&`` to avoid a copy.

Even more interestingly, notice the return type is a ``std::variant``. This is needed because the method can either return a Ruby enumerator or the vector.

In the first case, returning the vector is the same as returning ``this`` from a C++ member function or ``self`` from a Ruby function. This allows methods to be chained together - for example  ``vector.a.b``.

We have to return a reference to the vector and not a copy. Besides being potentially wasteful, a copy would result in creating a new Ruby object. Thus ``self`` would no longer be self - which would be quite unexpected. However, ``std::variants`` cannot container references and thus what we need to return is a ``std::reference_wrapper<T>``.

In the second case, we want to return a new Ruby enumerator which has a type of VALUE. However, we can't directly return a VALUE because Rice will interpret it as an unsigned long long (which in fact is what it is). Instead, we return a ``Rice::Object.`` For more information refer to :ref:`return_values`.

Creating an Enumerator
----------------------
Next, let's look at the code that returns an enumerator:

.. code-block:: cpp

    if (!rb_block_given_p())
    {
      auto rb_size_function = [](VALUE recv, VALUE argv, VALUE eobj) -> VALUE
      {
        // Since we can't capture the vector from above (because then we can't send
        // this lambda to rb_enumeratorize_with_size), extract it from recv
        T* receiver = Data_Object<T>::from_ruby(recv);
        return detail::To_Ruby<size_t>().convert(receiver->size());
      };

      return rb_enumeratorize_with_size(detail::selfThread, Identifier("each").to_sym(), 0, nullptr, rb_size_function);
    }

If a block is not provided by the user, then the method should return an enumerator. The enumerator is created like this:

.. code-block:: cpp

      return rb_enumeratorize_with_size(detail::selfThread, Identifier("each").to_sym(), 0, nullptr, rb_size_function);

Notice the first parameter of ``rb_enumeratorize_with_size`` requires a Ruby instance and not a C++ instance. The correct Ruby instance is the one that is wrapping the C++ instance which is stored in a thread-local variable called selfThread.

Supporting Enumerator Size
--------------------------

The ``rb_enumeratorize_with_size`` call includes an optional pointer to a function that can return the size of the enumerated object, in this case the vector. That is implemented as another lambda function:

.. code-block:: cpp

      auto rb_size_function = [](VALUE recv, VALUE argv, VALUE eobj) -> VALUE
      {
        // Since we can't capture the vector from above (because then we can't send
        // this lambda to rb_enumeratorize_with_size), extract it from recv
        T* receiver = Data_Object<T>::from_ruby(recv);
        return detail::To_Ruby<size_t>().convert(receiver->size());
      };

Since this lambda is being sent to C code, it cannot capture any local variables. Thus it does not have direct access to the ``T& vector`` parameter. Instead, it needs to extract the vector from the Ruby object wrapping the vector:

.. code-block:: cpp

        T* receiver = Data_Object<T>::from_ruby(recv);

It then needs to determine the vector size and return it back as a Ruby object:

.. code-block:: cpp

        return detail::To_Ruby<size_t>().convert(receiver->size());

Yielding to a Block
-------------------
Finally we get to the most common use case by far - yielding values to a passed in block:

.. code-block:: cpp

    for (Value_T& item : vector)
    {
      VALUE element = detail::To_Ruby<Value_T>().convert(item);
      detail::protect(rb_yield, element);
    }

The code is fairly simple. Iterate over each item in the vector by reference (no copies!), wrap it in a Ruby object, and return it to the block. Note the call to rb_yield is done via detail::protect in case Ruby raises an exception.

Returning Self
--------------
Last we return ``self`` is a common practice in Ruby to enable method chaining. Self in this case is the Ruby object wrapping the vector. By returning a reference to the vector, Rice is smart enough to map it back to the original Ruby object.

.. code-block:: cpp

    return std::ref(vector);

As explained above, we need to put the vector inside a ``std::reference_wrapper`` to include it in the returned variant.
