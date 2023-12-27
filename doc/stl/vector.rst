.. _std_vector:

std::vector
-----------
Along with ``std::string``, ``std::vector`` is a workhorse of many C++ code bases. Although there is a direct conceptual mapping between a ``std::vector`` and a Ruby ``Array``, Rice does not copy a vector to an Array. Instead, it wraps ``std::vector``.

There are multiple reasons for this:

* ``std::vector`` instantiations can only contain one type, while Ruby Arrays can contain different types
* ``std::vector`` instances can be quite large
* ``std::vector`` instances commonly contain C++ classes that have complex copy or move semantics
* having two disconnected copies of data, one in C++ and one in Ruby, is usually undesirable

Rice will automatically define Ruby classes for each instantiation of ``std::vector`` it finds. You may also manually define Ruby classes via the use of ``define_vector`` or ``define_vector_under`` methods. But make sure to define them *before* Rice automatically creates them.

Example:

.. code-block:: cpp

  std::vector<std::string> makeStringVector()
  {
     return std::vector {"one", "two", "three"};
  }

  define_vector<std::vector<std::string>>("StringVector");
  define_global_function("make_string_vector", &makeStringVector);

Once you have defined this Ruby class, you can create a new instance like this:

.. code-block:: ruby

  vector = StringVector.new
  vector.push("value 1")
  vector.push("value 2")

Array to Vector
^^^^^^^^^^^^^^^
For C++ methods that take vector arguments, you can instantiate a new vector from Ruby (see :ref:`stl_class_names`).

For example, assume this C++ code:

.. code-block:: cpp

  void passVector(std::vector<int> ints)
  {
  }

  define_vector<std::vector<std::int>("IntVector");
  define_global_function("pass_vector", &passVector);

One way to call it from Ruby is like this:

.. code-block:: ruby

  vector = IntVector.new
  vector.push(37)
  pass_vector(vector)

In this case, Ruby is wrapping a C++ vector. Therefore any changes made to the vector in C++ will be visible to Ruby.

However, it is often more convenient to pass a Ruby array instead. This is especially true if you are using Rice's :ref:`automatic <stl_class_names>` stl classes. 

Therefore Rice also supports this usage:

.. code-block:: ruby

  array = [3, 5, 9]
  pass_vector(array)

In this case, Rice will *copy* the Ruby array instead of wrapping it. Thus any modifications made in C++ will not be visible to Ruby.  

Ruby API
^^^^^^^^
Rice tries to make ``std::vector`` look like a Ruby Array by giving it an API that is a subset of ``Array``. However, there are differences you need to keep in mind.

First, the following methods only work if the vector type is copyable (copying is done in C++):

* Vector#copy(other)
* Vector#resize

Second, the following methods only work if the vector type implements C++'s equal operator, ``operator==``:

* Vector#delete
* Vector#include?
* Vector#index

Third, if the vector type supports C++ streams, then the following method will work, otherwise it will return "Not Printable"

* Vector#to_s

