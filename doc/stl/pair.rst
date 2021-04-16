std::pair
-----------
``std::pair`` is a simple container that provides C++ code a way of associating two values. ``std::map`` and ``std::unordered_map`` use ``std::pair`` to hold keys and their associated values.

Ruby does not have a concept of a pair. Therefore, Rice wraps ``std::pair`` which means that data is not copied between C++ and Ruby.

Since ``std::pair`` is a template of two types, each ``std::pair`` instantiation is its own unique C++ class, and thus its own unique Ruby class. You may manually define pair classes or let Rice do it for you. To manually define a Ruby class, use either the ``define_pair`` or ``define_pair_under`` methods.

Example:

.. code-block:: cpp

  std::pair<std::string, uint32_t> makeStringIntPair(std::string key, uint32_t value)
  {
     return std::make_pair(key, value);
  }

  define_pair<std::pair<std::string, uint32_t>("StringIntPair");
  define_global_function("make_string_int_pair", &makeStringIntPair);

Once you have defined this Ruby class, you can create a new instance like this:

.. code-block:: cpp

  pair = StringIntPair.new("key 2", 33)


API
----
The Ruby API exposed for ``std::pair`` should be fairly self-explanatory and consists of the following methods (assume we have created a Ruby class called Pair):

    * Pair#new(value1, value2)
    * Pair#first
    * Pair#first=(value)
    * Pair#second
    * Pair#second=(value)

If the underlying ``std::pair`` has copyable types (remember copying is done in C++), then the following method will work, otherwise it will raise an exception:

    * PairClass#copy(other)

If the underlying ``std::pair`` has types that are supported by C++ streams, then the following method will work, otherwise it will return "Not Printable"

    * PairClass#to_s

