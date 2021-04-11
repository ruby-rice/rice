=======================
Adding Type Conversions
=======================

Rice provides default conversions for native C++ types as well as types you define via  ``define_class``. As a result, you generally should not have to add your own custom type conversions.

However, for the sake of an example, let's say you want to expose a  ``std::deque<int>`` to Ruby and are not using Rice's built-in standard library support.

One approach, as described throughout this document, is to use  ``define_class`` and then  ``define_method`` to setup its API. However,  ``std::deque`` has a large API and you may only want to copy the data to Ruby and do not need to modify it from Ruby. Thus making a copy, instead of a wrapper, is perfectly fine.

To do this requires requires the following steps:

1. Mark the object as a builtin type
2. Add a To_Ruby method
3. Optionally add a From_Ruby method

Step 1 - Mark as Builtin Type
-----------------------------

Since we want Rice to copy the  ``std::deque<int>`` from C++ to Ruby, we need to tell Rice that it should be treated as a builtin type. For more information about builtin types, please refer to the [memory management](#memory_management) section.

Marking a type as builtin is simple:

.. code-block:: cpp

  namespace Rice::detail
  {
    template <>
    struct is_builtin<std::deque<int>> : public std::true_type {};
  }

Note the definition *must* be in the  ``Rice::detail`` namespace.

Step 2 - To_Ruby
----------------

Next, we need to write C++ code that converts the  ``std::deque<int>`` to a Ruby object. The most obvious Ruby object to map it to is an array.

.. code-block:: cpp

  namespace Rice::detail
  {
    template<>
    struct To_Ruby<std::deque<int>>
    {
      static VALUE convert(const std::deque<int>& deque, bool takeOwnership = true)
      {
        // Notice we wrap Ruby API calls with protect in case Ruby throws an exception. If you do not
        // use protect and Ruby throws an exception then your program *will* crash.
        VALUE result = protect(rb_ary_new2, deque.size());

        for (int element : deque)
        {
          // Convert the C++ int to a Ruby integer
          VALUE value = To_Ruby<int>::convert(element, takeOwnership);
          // Now add it to the Ruby array
          detail::protect(rb_ary_push, result, value));
        }
        return result;
      }
    };
  }

Once again, the definition *must* be in the  ``Rice::detail`` namespace.

Note that instead of using the raw Ruby C API as above, you may prefer to use  ``Rice::Array`` which provides an nice C++ wrapper for Ruby arrays.

Step 3 - From_Ruby
------------------

Last, if we want to convert a Ruby array to a  ``std::deque<int>``, then we need to write C++ code for that too.

.. code-block:: cpp

  namespace Rice::detail
  {
    template<>
    struct From_Ruby<std::deque<int>>
    {
      static std::deque<int> convert(VALUE ary)
      {
        // Make sure array is really an array - if not this call will throw a Ruby exception so we need to protect it
        detail::protect(rb_check_type, array, (int)T_ARRAY);

        long size = protect(rb_array_len, ary);
        std::deque<int> result(size);

        for (long i=0; i<size; i++)
        {
          // Get the array element
          VALUE value = protect(rb_ary_entry, ary, i);

          // Convert the Ruby int to a C++ int
          int element = From_Ruby<int>::convert(value);

          // Add it to our deque
          result[i] = element;
        }

        return result;
      }
    };
  }

And as usual, the definition *must* be in the  ``Rice::detail`` namespace.
