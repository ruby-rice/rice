std::vector
-----------
Along with ``std::string``, ``std::vector`` is a workhorse of many C++ code bases. Although there is a direct conceptual mapping between a ``std::vector`` and a Ruby ``Array``, Rice does not copy a vector to an Array. Instead, it wraps std::vector.

There are multiple reasons for this:

* vector can only contain one type, while Ruby Arrays can contain different types
* vectors can be quite large
* vectors commonly contain C++ classes that have complex copy semantics
* having two copies of data, once in C++ and one in Ruby, usually does not make sense.

Rice will automatically define Ruby classes for each instantation of ``std::vector`` it finds. You may manually define Ruby classes via the use of ``define_vector`` or ``define_vector_under`` methods.

Example:

.. code-block:: cpp

  std::vector<std::string> makeStringVector())
  {
     return std::vector {"one", "two", "three"};
  }

  define_vector<std::vector<std::string>("StringVector");
  define_global_function("make_string_vector", &makeStringVector);

Once you have defined this Ruby class, you can create a new instance like this:

.. code-block:: cpp

  vector = StringVector.new
  vector.push("value 1")
  vector.push("value 2")

API
----
Rice tries to make ``std::vector`` look like a Ruby Array by giving it a subset of the Array API. However, there are differences you need to keep in mind.

First, the following methods only work if the vector type is copyable (copying is done in C++):

    * Vector#copy(other)
    * Vector#resize

Second, the following methods only work if the vector type implements C++'s equal operator, ``operator==``:

If the underlying ``std::pair`` has types that are supported by C++ streams, then the following method will work, otherwise it will raise an exception:

    * Vector#delete
    * Vector#include?
    * Vector#index

Third, if the vector type supportsC++ streams, then the following method will work, otherwise it will return "Not Printable"

    * Vector#to_s

