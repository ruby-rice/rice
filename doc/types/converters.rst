.. _type_converters:

Type Converters
===============
Rice includes type converters for all fundamental C++ types, most STL classes and for user defined C++ classes. Therefore, it is unlikely you will need to create your own converters. However, you can add new converters to Rice if needed.

For the sake of an example, let's say you want to convert ``std::deque<int>`` to Ruby and are not using Rice's STL (standard template library) support. Let's also assume you want to copy the data between the two languages, as opposed to providing wrappers.

To do this requires requires the following steps:

1. Specialize Type template
2. Specialize To_Ruby template
3. Specialize From_Ruby template

.. _type_specialization:

Step 1 - Specialize Type
------------------------
First we have to tell Rice that ``std::deque<int>`` is a known type so that it passes :doc:`type verification <verification>`. This is done by specializing the Type template:

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

.. _to_ruby_specialization:

Step 2 - Specialize To_Ruby
---------------------------
Next, we need to write C++ code that converts the ``std::deque<int>`` to a Ruby object. The most obvious Ruby object to map it to is an array.

.. code-block:: cpp

  namespace Rice::detail
  {
    template<>
    class To_Ruby<std::deque<int>>
    {
    public:
      To_Ruby() = default;

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

The ```arg``` parameter includes information about the passed in argument, including:

* Whether Ruby should take ownership of the object
* Whether the argument is a VALUE
* Whether the argument is a C style Array

Your code will need to take this information into account when converting C++ objects to Ruby.

.. _from_ruby_specialization:

Step 3 - Specialize From_Ruby
-----------------------------
Last, we need to write C++ code that converts a Ruby Array to ``std::deque<int>``.

.. code-block:: cpp

  namespace Rice::detail
  {
    template<>
    class From_Ruby<std::deque<int>>
    {
    public:
      From_Ruby() = default;

      explicit From_Ruby(Arg* arg) : arg_(arg)
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

The ```arg``` parameter includes information about the passed in argument, including:

* Whether Ruby should take ownership of the object
* Whether the argument is a VALUE
* Whether the argument is a C style Array
* Whether the argument has a default value

And as usual, the definition *must* be in the ``Rice::detail`` namespace.

Supporting Default Arguments
----------------------------
Rice supports C++ :ref:`default_arguments`. To enable this support for your custom type requires making updating the ``convert`` method to check if the passed in Ruby value is ``nil`` (ie, ``Qnil``).

Expanding on our example above:

.. code-block:: cpp

    namespace Rice::detail
    {
      template<>
      class From_Ruby<std::deque<int>>
      {
      public:
        From_Ruby() = default;

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
