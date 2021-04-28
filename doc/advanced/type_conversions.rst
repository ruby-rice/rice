.. _type_conversions:

Type Conversions
================

Rice refers to types that should be converted (copied) between Ruby and C++ as builtin types. Builtin types are types that directly map from C++ to Ruby. Examples include nullptr, bool, numeric types (integer, float, double, complex), char types and strings.

Since they are copied, instances of builtin types are disconnected. Therefore, if a Ruby string is converted to a std::string then the two strings are independent and changes in one will *not* be reflected in the other. Also understand that if you allocate a new char* in C++ and pass it to Ruby, then you will get a memory leak because Ruby will copy the contents on the char* but will *not* free the original buffer. Generally you don't have to worry about builtin types because Rice supports them out of the box.

Adding a Builtin Type
---------------------
For the sake of an example, let's say you want to expose ``std::deque<int>`` to Ruby and are not using Rice's STL (standard template library) support. You also want to copy the data between the two languages, as opposed to providing wrappers. To do this requires requires the following steps:

1. Specialize Type template
2. Specialize To_Ruby template
3. Specialize From_Ruby template

Step 1 - Specialize Type
-------------------------
First we have to tell Rice that ``std::deque<int>`` is a known type so that it passes :doc:`type verification <type_verification>`. This is done by specializing the Type template:

.. code-block:: cpp

  namespace Rice::detail
  {
    template<>
    struct Type<std::deque<int>>
    {
      static bool verify()
      {
        return true;
      }
    };
  }

The specialization *must* be in the ``Rice::detail`` namespace. If your type contains subtypes, then make sure to verify them also. For an example, here is the verify method for ``std::optional``:

.. code-block:: cpp

  namespace Rice::detail
  {
    template<typename T>
    struct Type<std::optional<T>>
    {
      static bool verify()
      {
        return Type<T>::verify();
      }
    };
  }

Notice that std::optional is only valid if the type it stores is valid.

Step 2 - Specialize To_Ruby
----------------------------
Next, we need to write C++ code that converts the ``std::deque<int>`` to a Ruby object. The most obvious Ruby object to map it to is an array.

.. code-block:: cpp

  namespace Rice::detail
  {
    template<>
    class To_Ruby<std::deque<int>>
    {
      VALUE convert(const std::deque<int>& deque)
      {
        // Notice we wrap Ruby API calls with protect in case Ruby throws an exception.
        // If you do not  use protect and Ruby throws an exception then your program 
        // *will* crash.
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

Instead of using the raw Ruby C API as above, you may prefer to use ``Rice::Array`` which provides an nice C++ wrapper for Ruby arrays.

Step 3 - Specialize From_Ruby
-----------------------------
Last, if we want to convert a Ruby array to a  ``std::deque<int>``, then we need to write C++ code for that too.

.. code-block:: cpp

  namespace Rice::detail
  {
    template<>
    class From_Ruby<std::deque<int>>
    {
      std::deque<int> convert(VALUE ary)
      {
        // Make sure array is really an array - if not this call will
        // throw a Ruby exception so we need to protect it
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

And as usual, the definition *must* be in the ``Rice::detail`` namespace.
