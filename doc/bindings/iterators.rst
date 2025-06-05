.. _iterators:

Iterators
=========

C++ iterators are used to traverse through elements stored in a container. C++ iterators are external iterators that work in pairs, with a beginning iterator and an ending iterator. For example, ``std::vector`` has begin/end, cbegin/cend, rbegin/rend, etc.

Enumerable Support (Internal Iterators)
---------------------------------------
Rice makes it easy to add `Enumerable <https://ruby-doc.org/3.2.2/Enumerable.html>`_ support to C++ classes. The Enumerable module adds internal iterator support to a Ruby class as long as it defines an ``each`` instance method.

Rice makes this easy via the ``define_iterator`` method. ``define_iterator`` creates an ``each`` method and also mixes in the ``Enumerable`` module.

For example let's create a simple wrapper around std::vector (for full support please see :ref:`std_vector`).

.. code-block:: cpp

  #include <vector>
  #include <rice/rice.hpp>

  using namespace Rice;

  extern "C"
  void Init_IntVector()
  {
    using IntVector = std::vector<int>;
    define_class<IntVector>("IntVector")
      .define_constructor(Constructor<IntVector>())
      .define_method<void(IntVector::*)(const IntVector::value_type&)>("push_back", &IntVector::push_back)
      .define_iterator<IntVector::iterator(IntVector::*)()>(&IntVector::begin, &IntVector::end);
  }

Notice that we have to tell Rice which overloaded version of ``push_back``, ``begin`` and ``end`` we want to expose For more information please see :ref:`overloaded_methods`.

Once the iterator is defined you can write standard Ruby code such as:

.. code-block:: ruby

  intVector = IntVector.new
  intVector.push_back(1)
  intVector.push_back(2)
  intVector.push_back(3)

  result = intVector.map do |value|
    value * 2
  end

Where the result will be ``[2, 4, 6]``.

Let's say you also want to expose std::vector's reverse iterator to Ruby using the method name ``reach``. This is done by adding a third parameter to the ``define_iterator`` call, in this case it is set to ``"reach"``:

.. code-block:: cpp

  extern "C"
  void Init_IntVector()
  {
    define_class<IntVector>("IntVector")
      .define_iterator<IntVector::reverse_iterator(IntVector::*)()>(&IntVector::rbegin, &IntVector::rend, "reach");
  }

Example Ruby code is then:

.. code-block:: ruby

  intVector = IntVector.new
  intVector.push_back(1)
  intVector.push_back(2)
  intVector.push_back(3)

  result = intVector.reach do |value|
    result << value * 2
  end

Where the result will be ``[6, 4, 2]``.

Enumerator Support (External Iterators)
---------------------------------------
Ruby supports external iterators via the `Enumerator <https://ruby-doc.org/3.2.2/Enumerator.html>`_ class. The ``define_iterator`` method automatically adds support for Enumerators.

Enumerators can be created by calling an iterator method without a block, in the same way you can call ``Array#each`` or other methods without a block. For example:

.. code-block:: ruby

    intVector = IntVector.new
    intVector.push_back(1)
    intVector.push_back(2)
    intVector.push_back(3)

    # Get an enumerator
    enumerator = intVector.each

    # Now  use it
    enumerator.map |i|
      i * 2
    end
