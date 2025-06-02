.. _std_map:

std::map
========
Outside of ``std::vector``, ``std::map`` is perhaps the most commonly used container in C+. Although there is a direct conceptual mapping between a ``std::map`` and a Ruby ``Hash``, Rice does not copy a map to an Hash. Instead, it wraps ``std::map``.

There are multiple reasons for this:

* ``std::map`` instantiations can only contain one type of key and value, while Ruby Hashes can contain different types of keys and values
* ``std::map`` instances can be quite large
* ``std::map`` instances commonly contain C++ classes that have complex copy or move semantics
* having two disconnected copies of data, one in C++ and one in Ruby, is usually undesirable

Rice will automatically define Ruby classes for each instantiation of ``std::map`` it finds. You may also manually define Ruby classes via the use of the ``define_map`` method. Map classes are added to the ``Std`` module.

Usage
^^^^^
For C++ methods that take map arguments, you can instantiate a new map from Ruby (see :ref:`stl_class_names`).

For example, assume this C++ code:

.. code-block:: cpp

  void passMap(std::map<std::string, int> map)
  {
  }

  define_global_function("pass_map", &passMap);

One way to call it from Ruby is like this:

.. code-block:: ruby

  map = Std::Map≺string‚ int≻.new
  map["thirty seven"] = 37
  pass_map(map)

In this case, Ruby is wrapping a C++ map. Therefore any changes made to the map in C++ will be visible to Ruby.

A second way to call the method is to pass a Ruby Hash:

.. code-block:: ruby

  hash = {"three" => 3, "five" => 5, "nine" => 9}
  pass_map(hash)

In this case, Rice will *copy* the Ruby hash instead of wrapping it. Thus any modifications made in C++ will not be visible to Ruby.  

Ruby API
^^^^^^^^
Rice tries to make ``std::map`` look like a Ruby Hash by giving it an API that is a subset of ``Hash``. However, there are differences you need to keep in mind.

First, the following methods only work if the map type is copyable (copying is done in C++):

* Map#copy(other)

Second, the following methods only work if the map type implements C++'s equal operator, ``operator==``:

* Map#value?

Third, if the map type supports C++ streams, then the following method will work, otherwise it will return "Not Printable"

* Map#to_s

