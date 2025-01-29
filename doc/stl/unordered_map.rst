.. _std_unordered_map:

std::unordered_map
==================
``std::unordered_map`` is the Hash table of C++. However, Rice does not copy unordered_maps to Hash tables and instead wraps them.

There are multiple reasons for this:

* ``std::unordered_map`` instantiations can only contain one type of key and value, while Ruby Hashes can contain different types of keys and values
* ``std::unordered_map`` instances can be quite large
* ``std::unordered_map`` instances commonly contain C++ classes that have complex copy or move semantics
* having two disconnected copies of data, one in C++ and one in Ruby, is usually undesirable

Rice will automatically define Ruby classes for each instantation of ``std::unordered_map`` it finds. You may also manually define Ruby classes via the use of ``define_unordered_map`` or ``define_unordered_map_under`` methods. But make sure to define them *before* Rice automatically creates them.

Example:

.. code-block:: cpp

  std::unordered_map<std::string, int> makeStringIntMap()
  {
     return std::unordered_map {{"one", 1}, {"two", 2}, {"three", 3}};
  }

  define_unordered_map<std::unordered_map<std::string>>("StringIntMap");
  define_global_function("make_string_int_map", &makeStringIntMap);

Once you have defined this Ruby class, you can create a new instance like this:

.. code-block:: ruby

  map = StringMap.new
  map["value 1"] = 1
  map["value 2"] = 2

Hash to Map
^^^^^^^^^^^^^^^
For C++ methods that take map arguments, you can instantiate a new map from Ruby (see :ref:`stl_class_names`).

For example, assume this C++ code:

.. code-block:: cpp

  void passMap(std::unordered_map<std::string, int> stringIntMap)
  {
  }

  define_unordered_map<std::unordered_map<std::string, std::int>>("StringIntMap");
  define_global_function("pass_map", &passMap);

One way to call it from Ruby is like this:

.. code-block:: ruby

  map = StringIntMap.new
  map["thrity seven"] = 37
  pass_map(map)

In this case, Ruby is wrapping a C++ map. Therefore any changes made to the map in C++ will be visible to Ruby.

However, it is often more convenient to pass a Ruby hash instead. This is especially true if you are using Rice's :ref:`automatic <stl_class_names>` stl classes. 

Therefore Rice also supports this usage:

.. code-block:: ruby

  hash = {"three" => 3, "five" => 5, "nine" => 9}
  pass_map(hash)

In this case, Rice will *copy* the Ruby hash instead of wrapping it. Thus any modifications made in C++ will not be visible to Ruby.  

Ruby API
^^^^^^^^
Rice tries to make ``std::unordered_map`` look like a Ruby Hash by giving it an API that is a subset of ``Hash``. However, there are differences you need to keep in mind.

First, the following methods only work if the map type is copyable (copying is done in C++):

* Map#copy(other)

Second, the following methods only work if the map type implements C++'s equal operator, ``operator==``:

* Map#value?

Third, if the map type supports C++ streams, then the following method will work, otherwise it will return "Not Printable"

* Map#to_s

