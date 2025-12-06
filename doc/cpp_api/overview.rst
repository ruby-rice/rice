.. _cpp_api:

Overview
========

Rice provides C++ wrapper classes for Ruby's built-in types, offering a natural C++ interface while preserving full access to Ruby's functionality.

Class Hierarchy
---------------

Rice mimics the Ruby class hierarchy:

.. code-block:: text

   Object
   ├── Module
   │   └── Class
   │       └── Struct
   ├── Array
   ├── Hash
   ├── Identifier
   ├── String
   └── Symbol

   Identifier (not an Object - wraps Ruby's internal ID type)

Core Classes
------------

* :ref:`cpp_object` - Base class for all Ruby objects
* :ref:`cpp_module` - Wraps Ruby modules with method definition support
* :ref:`cpp_class` - Wraps Ruby classes (inherits from Module)
* :ref:`cpp_array` - Array with STL-compatible random-access iterators
* :ref:`cpp_hash` - Hash with STL-compatible iterators
* :ref:`cpp_identifier` - Lightweight ID wrapper (not a Ruby Object)
* :ref:`cpp_string` - String wrapper with C++ string interop
* :ref:`cpp_symbol` - Symbol wrapper (Ruby Object)
* :ref:`cpp_struct` - Dynamic struct class creation

Quick Examples
--------------

**Calling Ruby Methods**

.. code-block:: cpp

   Object obj(some_value);
   Object result = obj.call("method_name", arg1, arg2);

   // With keyword arguments
   Hash kwargs;
   kwargs["encoding"] = "UTF-8";
   Object result = obj.call_kw("method", kwargs);

**Working with Arrays**

.. code-block:: cpp

   Array a;
   a.push(1, false);
   a.push(2, false);
   a.push(3, false);

   // Range-based for loop (C++11)
   for (auto elem : a) {
     std::cout << elem << std::endl;
   }

   // STL algorithms work
   auto dist = std::distance(a.begin(), a.end());

**Working with Hashes**

.. code-block:: cpp

   Hash h;
   h["name"] = "Alice";
   h["age"] = 30;

   // Iterate over entries
   for (auto entry : h) {
     std::cout << entry.key << ": " << Object(entry.value) << std::endl;
   }

   // Get typed values
   std::string name = h.get<std::string>("name");
   int age = h.get<int>("age");

**Defining Modules and Classes**

.. code-block:: cpp

   Module m = define_module("MyModule")
     .define_module_function("helper", []() { return 42; })
     .define_constant("VERSION", "1.0.0");

   Class c = define_class("MyClass")
     .define_method("process", [](Object self, int x) {
       return x * 2;
     });

**Type Conversions**

All wrapper classes automatically convert between C++ and Ruby types:

.. code-block:: cpp

   // C++ to Ruby
   String s("hello");           // C string to Ruby String
   Array a({1, 2, 3});          // Initializer list to Ruby Array
   Symbol sym("my_symbol");     // C string to Ruby Symbol

   // Ruby to C++
   std::string cpp_str = s.str();
   std::vector<int> vec = a.to_vector<int>();
   std::string sym_str = sym.str();
