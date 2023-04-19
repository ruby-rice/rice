.. _overloaded_methods:

Overloaded Methods
====================

C++ supports overloaded methods and functions. When you try to wrap an overloaded function the C++ compiler will throw an error message that says something like "no matching overloaded function found."

For example, consider this C++ class:

.. code-block:: cpp

  class Container
  {
  public:
    size_t capacity()
    {
      return this->capacity_;
    }

    void capacity(size_t value)
    {
      this->capacity_ = value;
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

Instead, you need to tell the C++ compiler which overloaded method to use. There are several ways you can do this as explained below.

Template Parameter
------------------
``define_method`` is a template function, therefore one solution is to specify which method you are trying to call like this:

.. code-block:: cpp

   Class c = define_class<Container>("Container")
    .define_constructor(Constructor<Container>())
    .define_method<size_t(Container::*)()>("capacity", &Container::capacity)
    .define_method<void(Container::*)(size_t)>("capacity=", &Container::capacity);

``size_t(Container::*)()`` and ``void(Container::*)(size_t)`` are C++ pointers to member functions.

Using
-----
Another solution is via C++ ``using`` functionality, like this:

.. code-block:: cpp

  using Getter_T = size_t(Container::*)();
  using Setter_T = void(Container::*)(size_t);

  Class c = define_class<Container>("Container")
    .define_constructor(Constructor<Container>())
    .define_method<Getter_T>("capacity", &Container::capacity)
    .define_method<Setter_T>("capacity=", &Container::capacity);

Or even like this:

.. code-block:: cpp

  using Getter_T = size_t(Container::*)();
  using Setter_T = void(Container::*)(size_t);

  Class c = define_class<Container>("Container")
    .define_constructor(Constructor<Container>())
    .define_method("capacity", (Getter_T)&Container::capacity)
    .define_method("capacity=", (Setter_T)&Container::capacity);

Typedef
-------
If you are old school, and like obtuse syntax, you can also use a ``typdef`` like this:

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

Ruby Example
------------
Once you have wrapped this class, it is easy to use in Ruby:

.. code-block:: ruby

  container = Container.new
  container.capacity = 6
  puts container.capacity

The printed result will be 7.

