.. _type_conversions:

Type Conversions
================

Rice refers to types that should be converted (copied) between Ruby and C++ as builtin types. Builtin types are types that directly map from C++ to Ruby. Examples include nullptr, bool, numeric types (integer, float, double, complex), char types and strings.

Since they are copied, instances of builtin types are disconnected. Therefore, if a Ruby string is converted to a ``std::string`` then the two strings are independent and changes in one will *not* be reflected in the other. Also understand that if you allocate a new ``char*`` in C++ and pass it to Ruby, then you will get a memory leak because Ruby will copy the contents on the ``char*`` but will *not* free the original buffer. 

Rice supports all common builtin types out of the box. In general, to add new C++ types to Ruby you should wrap them by using ``define_class``, ``define_enum``, etc.  It should be quite rare to add new builtin types.

Adding a Builtin Type
---------------------
For the sake of an example, let's say you want to expose ``std::deque<int>`` to Ruby and are not using Rice's STL (standard template library) support. You also want to copy the data between the two languages, as opposed to providing wrappers. To do this requires requires the following steps:

1. Specialize Type template
2. Specialize To_Ruby template
3. Specialize From_Ruby template

.. _type_specialiazation:

Step 1 - Specialize Type
^^^^^^^^^^^^^^^^^^^^^^^^
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

.. _to_ruby_specialiazation:

Step 2 - Specialize To_Ruby
^^^^^^^^^^^^^^^^^^^^^^^^^^^
Next, we need to write C++ code that converts the ``std::deque<int>`` to a Ruby object. The most obvious Ruby object to map it to is an array.

.. code-block:: cpp

  namespace Rice::detail
  {
    template<>
    class To_Ruby<std::deque<int>>
    {
    public:
      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const std::deque<int>& deque)
      {
        Array result;

        for (int element : deque)
        {
          result.push(element, true);
        }
        return result;
      }
    private:
      Arg* arg_ = nullptr;
    };
  }

Once again, the definition *must* be in the  ``Rice::detail`` namespace.

Instead of using the raw Ruby C API as above, you may prefer to use ``Rice::Array`` which provides an nice C++ wrapper for Ruby arrays.

.. _from_ruby_specialization:

Step 3 - Specialize From_Ruby
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Last, if we want to convert a Ruby array to a  ``std::deque<int>``, then we need to write C++ code for that too.

.. code-block:: cpp

  namespace Rice::detail
  {
    template<>
    class From_Ruby<std::deque<int>>
    {
    public:
      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      Convertible is_convertible(VALUE value)
      {
        switch (rb_type(value))
        {
          case RUBY_T_ARRAY:
            return Convertible::Cast;
            break;
          default:
            return Convertible::None;
        }
      }

      std::deque<int> convert(VALUE value)
      {
        Array array(value);
        std::deque<int> result(array.size());

        for (long i=0; i<size; i++)
        {
          // Convert the Ruby int to a C++ int
          int element = From_Ruby<int>::convert(array[i]);

          // Add it to our deque
          result[i] = element;
        }

        return result;
      }
    private:
      Arg* arg_ = nullptr;
    };
  }

And as usual, the definition *must* be in the ``Rice::detail`` namespace.

Supporting Default Arguments
----------------------------
Rice supports C++ :ref:`default_arguments`. To enable this support for your custom type requires making the following changes to the ``From_Ruby`` specialization:

*  Add an additional constructor that takes a ``detail::Arg`` pointer and store it in a member variable
*  Add back in the default constructor.
*  In the ``convert`` method, if the Ruby value is ``nil`` (ie, ``Qnil``) and arg is set then return the default value.

Expanding on our example above:

.. code-block:: cpp

    namespace Rice::detail
    {
      template<>
      class From_Ruby<std::deque<int>>
      {
      public:
        explicit From_Ruby(Arg* arg) : arg_(arg)
        {
        }

        std::deque<int> convert(VALUE ary)
        {
          if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
          {
            return this->arg_->defaultValue<std::deque<int>>();
          }
          else
          {
            // .... Same as code from example above
          }
        }

      private:
        Arg* arg_ = nullptr;
      };
    }
