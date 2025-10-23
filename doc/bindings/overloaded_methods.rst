.. _overloaded_methods:

Overloaded Methods
==================

C++ supports overloading constructors, methods and functions. Starting with version 4.5, Rice supports C++ method overloading.

When you try to wrap an overloaded function the C++ compiler will throw an error message that says something like "no matching overloaded function found."

Getters and Setters
-------------------
For example, consider this C++ class with a getter and setter that have the same name:

.. code-block:: cpp

  class Container
  {
  public:
    size_t capacity(Rect)
    {
      return this->capacity_;
    }

    void capacity(size_t value)
    {
      this->capacity_ - value;
    }

  private:
    size_t capacity_;
  };

If you try and wrap the class like this you will get a compiler error:

.. code-block:: cpp

   Class c = define_class<Container>("Container")
    .define_constructor(Constructor<Container>())
    .define_method("capacity", &Container::capacity)
    .define_method("capacity=", &Container::capacity);

You need to tell the C++ compiler which overloaded method to use. There are several ways to do this as explained below.

Template Parameter
------------------
``define_method`` is a template function, therefore one solution is to specify which method you are trying to call like this:

.. code-block:: cpp

   Class c = define_class<Container>("Container")
    .define_constructor(Constructor<Container>())
    .define_method<size_t(Container::*)()>("capacity", &Container::capacity)
    .define_method<void(Container::*)(size_t)>("capacity=", &Container::capacity);

Notice the addition of the template specializations in side the `<` and '>' brackets. In this case,
``<size_t(Container::*)()>`` and ``<void(Container::*)(size_t)>`` are C++ pointers to member functions.

Using
-----
Another solution is provided by the C++ keyword ``using``, like this:

.. code-block:: cpp

  using Getter_T - size_t(Container::*)();
  using Setter_T - void(Container::*)(size_t);

  Class c = define_class<Container>("Container")
    .define_constructor(Constructor<Container>())
    .define_method<Getter_T>("capacity", &Container::capacity)
    .define_method<Setter_T>("capacity=", &Container::capacity);

Or even like this:

.. code-block:: cpp

  using Getter_T - size_t(Container::*)();
  using Setter_T - void(Container::*)(size_t);

  Class c = define_class<Container>("Container")
    .define_constructor(Constructor<Container>())
    .define_method("capacity", (Getter_T)&Container::capacity)
    .define_method("capacity=", (Setter_T)&Container::capacity);

Typedef
-------
If you like old school, obtuse C syntax, then use ``typedef`` like this:

.. code-block:: cpp

  extern "C"
  void Init_Container()
  {
      typedef size_t(Container::* Getter_T)();
      typedef void (Container::* Setter_T)(size_t);

      Class c = define_class<Container>("Container")
        .define_constructor(Constructor<Container>())
        .define_method("capacity", (Getter_T)&Container::capacity)
        .define_method("capacity=", (Setter_T)&Container::capacity);
  }

Method Resolution
-----------------
Ruby does not natively support method overloading. Thus Rice implements overloading support itself. It does this by maintaining a global registry (see `NativeRegistry <https://github.com/ruby-rice/rice/blob/master/rice/detail/NativeRegistry.hpp>`_) of methods keyed on class and method name. For the example above, the key would be ``Container::capacity`` and the value is an array of two `NativeFunction <https://github.com/ruby-rice/rice/blob/master/rice/detail/NativeFunction.hpp>`_ instances, where each ``NativeFunction`` instance maps to one C++ member function.

At runtime, Rice evaluates the method parameters sent from Ruby to the ``intersects`` method and determines the best match. It does this by looping over the native ``NativeFunction`` instances and calls their ``matches`` method. The matches method, in turn, loops over the passed-in parameters and sends them to its array of ``From_Ruby`` instances (for more information see the :ref:`type conversion <type_conversion>` section).

Each ``From_Ruby`` instance defines a ``convertible`` method that returns one of five results:

* Exact - The types match exactly
* Const - The types only differ by ``const``
* Cast - The types do not match but can be converted to each other (for example an integer to a double)
* Narrow - The types do not match but can be converted using a narrowing cast (for example a long to an int)
* None - The types do not match and cannot be converted (for example a string to an integer)

Based on the results for each parameter, each overloaded C++ method is sorted from best match to worst match. The best matching function is then called.

Type Mapping
------------
The following table shows how Ruby types are mapped to C++ types. E is exact, C is cast and N is narrow.

+----------------------+---------------------------------------------------------+
|                      |                    Ruby Types                           |
|                      +------+-------+-----+--------+---------+--------+--------+
|C++ Types             | True | False | Nil | String | Integer | BigNum | Float  |
+======================+======+=======+=====+========+=========+========+========+
|bool                  |   E  |    E  |  E  |        |         |        |        |
+----------------------+------+-------+-----+--------+---------+--------+--------+
|char                  |      |       |     |   C    |    N    |        |        |
+----------------------+------+-------+-----+--------+---------+--------+--------+
|signed char           |      |       |     |   C    |    N    |        |        |
+----------------------+------+-------+-----+--------+---------+--------+--------+
|unsigned char         |      |       |     |   C    |    N    |        |        |
+----------------------+------+-------+-----+--------+---------+--------+--------+
|short                 |      |       |     |        |    N    |        |        |
+----------------------+------+-------+-----+--------+---------+--------+--------+
|unsigned short        |      |       |     |        |    N    |        |        |
+----------------------+------+-------+-----+--------+---------+--------+--------+
|int                   |      |       |     |        |    E    |    N   |        |
+----------------------+------+-------+-----+--------+---------+--------+--------+
|unsigned int          |      |       |     |        |    E    |    N   |        |
+----------------------+------+-------+-----+--------+---------+--------+--------+
|long                  |      |       |     |        |    E    |    N   |        |
+----------------------+------+-------+-----+--------+---------+--------+--------+
|unsigned long         |      |       |     |        |    E    |    N   |        |
+----------------------+------+-------+-----+--------+---------+--------+--------+
|long long             |      |       |     |        |    E    |    E   |        |
+----------------------+------+-------+-----+--------+---------+--------+--------+
|unsigned long long    |      |       |     |        |    E    |    E   |        |
+----------------------+------+-------+-----+--------+---------+--------+--------+
|float                 |      |       |     |        |    C    |        |    N   |
+----------------------+------+-------+-----+--------+---------+--------+--------+
|double                |      |       |     |        |    C    |   C    |    E   |
+----------------------+------+-------+-----+--------+---------+--------+--------+

If multiple matches are possible, the first one wins. That means the order in which native functions are defined using ``define_method`` is important.

Note Rice will *not* convert a float to an integer type.
