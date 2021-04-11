============
Ruby C++ API
============

Rice provides builtin wrappers for many builtin Ruby types, including:

- Object
- Module
- Class
- String
- Array
- Hash
- Struct
- Symbol
- Exception

Rice mimics the Ruby class hierarchy as closely as it can.

For example:

.. code-block:: cpp

  Object object_id = obj.call("object_id");
  std::cout << object_id << std::endl;

  Class rb_cTest = define_class<Test>("Test");
  Object object_id = rb_cTest.call("object_id");
  std::cout << object_id << std::endl;

The `Array` and `Hash` types can even be iterated over the same way one
would iterate over an STL container:

.. code-block:: cpp

  Array a;
  a.push(detail::to_ruby(42));
  a.push(detail::to_ruby(43));
  a.push(detail::to_ruby(44));
  Array::iterator it = a.begin();
  Array::iterator end = a.end();
  for(; it != end; ++it)
  {
    std::cout << *it << std::endl;
  }

STL algorithms should also work as expected on `Array` and `Hash` containers.

