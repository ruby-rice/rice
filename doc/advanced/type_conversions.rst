=======================
Type Conversion
=======================

The purpose of Rice is to enable native C++ code and Ruby code work together. This requires making it easy to translate types between the two languages.

There are three main use cases:

1. Converting, i.e copying, types between the two languages
2. Enable Ruby to access C++ code via a Ruby wrapper
3. Enable C++ to access Ruby code via a C++ wrapper

Type conversion works well for primitive types such as boolean and numeric types. For example, a C++ unsigned 32 bit integer is copied into a Ruby Fixnum instance (and vice versa). Conversion of primitive types is also easy to understand because its familiar to programmers. When you pass a boolean or integer into a method, you don't expect the method is going to change it - instead it just gets a copy.

However, type conversion rarely makes sense for more complex types. You likely do not want to copy simple instances of C++ structure to Ruby, and you almost never want to copy instances of C++ classes. There are a lot of reasons for this, including:

* C++ objects may contain uncopyable internal state, such as a database connection or open file handle
* C++ has complex object lifetime rules that control how objects are created, copied and destructed that do not translate to Ruby
* A C++ object may use a lot of memory, such as a million element vector, that make it untenable to copy the data to Ruby.
* Copying data, by definition, creates two separate versions making it impossible to share data between the two languages.

As a result, a more practical approach is to provide thin wrappers that allow Ruby to access C++ objects and C++ to access Ruby objects.


Builtin Types
---------------
Rice calls native types that should be converted (copied) Builtin Types. Builtin types are types that directly map from C++ to Ruby. Examples include nullptr, bool, numeric types (integer, float, double, complex), char types and strings.

Since they are copied, instances of builtin types are disconnected. Therefore, if a Ruby string is converted to a std::string then the two strings are independent and changes in one will *not* be reflected in the other. Also understand that if you allocate a new char* in C++ and pass it to Ruby, then you will get a memory leak because Ruby will copy the contents on the char* but will *not* free the original buffer. Generally you don't have to worry about builtin types because Rice supports them out of the box.

Adding a Builtin Type
---------------------
For the sake of an example, let's say you want to expose ``std::deque<int>`` to Ruby and are not using Rice's built-in standard library support. You also want to copy the data between the two languages, as opposed to providing wrappers. To do this requires requires the following steps:

1. Mark the object as a builtin type
2. Add a To_Ruby method
3. Optionally add a From_Ruby method

Step 1 - Mark as Builtin Type
-----------------------------

Since we want Rice to copy the  ``std::deque<int>`` from C++ to Ruby, we need to tell Rice that it should be treated as a builtin type. For more information about builtin types, please refer to the :ref:`Memory Management` section.

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
