.. _cpp_identifier:

==========
Identifier
==========

.. code-block:: cpp

   #include <rice/rice.hpp>

``Rice::Identifier`` is a wrapper for Ruby's internal ``ID`` type. An ID is Ruby's internal representation of a Symbol - it's an integer that uniquely identifies a symbol name. Unlike :ref:`cpp_symbol`, an Identifier is not a Ruby Object; it's a lightweight handle used internally by Ruby.

Identifiers are commonly used for:

* Method names when calling methods
* Instance variable names (``@foo``)
* Constant names
* Hash keys in internal operations

----

Constructors
============

Identifier(ID id)
-----------------

Construct an Identifier from a Ruby ID.

**Parameters:**

* ``id`` - A Ruby ID value.

.. code-block:: cpp

   ID ruby_id = rb_intern("my_method");
   Identifier id(ruby_id);

----

Identifier(Symbol const& symbol)
--------------------------------

Construct an Identifier from a Symbol.

**Parameters:**

* ``symbol`` - A Symbol to convert.

.. code-block:: cpp

   Symbol sym("my_method");
   Identifier id(sym);

----

Identifier(char const* s, Encoding encoding = Encoding::utf8())
---------------------------------------------------------------

Construct an Identifier from a C string.

**Parameters:**

* ``s`` - A null-terminated C string.
* ``encoding`` - The string encoding (default: UTF-8).

.. code-block:: cpp

   Identifier id("my_method");
   Identifier id2("to_s");
   Identifier ivar("@name");

----

Identifier(std::string const& string, Encoding encoding = Encoding::utf8())
---------------------------------------------------------------------------

Construct an Identifier from a C++ string.

**Parameters:**

* ``string`` - A C++ std::string.
* ``encoding`` - The string encoding (default: UTF-8).

.. code-block:: cpp

   std::string method_name = "calculate";
   Identifier id(method_name);

----

Public Methods
==============

c_str() const -> char const*
----------------------------

Get a pointer to the identifier's string representation.

**Returns:**

A pointer to the null-terminated string.

.. code-block:: cpp

   Identifier id("my_method");
   const char* name = id.c_str();
   printf("Method: %s\n", name);  // "Method: my_method"

----

str() const -> std::string
--------------------------

Get the identifier as a C++ string.

**Returns:**

A std::string copy of the identifier name.

.. code-block:: cpp

   Identifier id("calculate");
   std::string name = id.str();  // "calculate"

----

id() const -> ID
----------------

Get the underlying Ruby ID value.

**Returns:**

The raw ID.

.. code-block:: cpp

   Identifier id("my_method");
   ID raw_id = id.id();

----

operator ID() const
-------------------

Implicit conversion to Ruby ID.

**Returns:**

The raw ID.

.. code-block:: cpp

   Identifier id("my_method");
   ID raw_id = id;  // Implicit conversion

   // Can use directly with Ruby C API
   rb_funcall(obj, id, 0);

----

to_sym() const -> VALUE
-----------------------

Convert the Identifier to a Ruby Symbol VALUE.

**Returns:**

A Ruby Symbol VALUE.

.. code-block:: cpp

   Identifier id("my_method");
   VALUE sym = id.to_sym();  // :my_method as VALUE

----

Example
=======

.. code-block:: cpp

   #include <rice/rice.hpp>
   #include <iostream>

   using namespace Rice;

   void example()
   {
     // Create identifiers
     Identifier method_id("to_s");
     Identifier ivar_id("@value");
     Identifier const_id("VERSION");

     // Get string representations
     std::cout << method_id.str() << std::endl;  // "to_s"
     std::cout << ivar_id.c_str() << std::endl;  // "@value"

     // Use with Object::call()
     Object str(rb_str_new_cstr("hello"));
     Object result = str.call(method_id);  // Calls to_s

     // Use with instance variables
     Object obj(/* some object */);
     obj.iv_set(ivar_id, 42);
     Object val = obj.iv_get(ivar_id);

     // Convert to Symbol
     VALUE sym = method_id.to_sym();
     Symbol s(sym);

     // Use directly with Ruby C API (implicit conversion)
     ID raw = method_id;  // Implicit conversion to ID
     VALUE result2 = rb_funcall(str.value(), raw, 0);
   }

   // Common use case: defining methods
   extern "C"
   void Init_my_extension()
   {
     define_class("MyClass")
       .define_method(Identifier("calculate"), [](Object self, int x) {
         return x * 2;
       })
       .define_method("process", [](Object self) {
         // Use identifier for method calls
         return self.call(Identifier("calculate"), 21);
       });
   }

----

Identifier vs Symbol
====================

While ``Identifier`` and ``Symbol`` both represent the same underlying concept (an interned string), they serve different purposes:

+----------------+---------------------------+---------------------------+
| Feature        | Identifier                | Symbol                    |
+================+===========================+===========================+
| Ruby Type      | ID (C integer)            | VALUE (Ruby Object)       |
+----------------+---------------------------+---------------------------+
| Inherits from  | Nothing                   | Object                    |
+----------------+---------------------------+---------------------------+
| Memory         | Lightweight (just an int) | Full Ruby object          |
+----------------+---------------------------+---------------------------+
| Use case       | Internal operations       | Ruby-visible values       |
+----------------+---------------------------+---------------------------+
| GC tracked     | No                        | Yes                       |
+----------------+---------------------------+---------------------------+

.. code-block:: cpp

   // Use Identifier for internal operations
   Identifier method_name("calculate");
   obj.call(method_name, args);

   // Use Symbol when you need a Ruby Object
   Symbol sym("key");
   hash[sym] = value;

----

See Also
========

* :ref:`cpp_symbol` - Ruby Symbol wrapper (a Ruby Object)
* :ref:`cpp_object` - For using identifiers in method calls
