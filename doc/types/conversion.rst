.. _type_conversion:

Type Conversions
================
Rice converts fundamental types to their equivalent Ruby types. Fundamental types include nullptr, bool, numeric types (integer, float, double, complex) and char types.

Type conversion is easy to understand because it is familiar to programmers. When you pass a boolean or integer into a method, you don't expect the method is going to change it - instead it just gets a copy.

Since they are copied, instances of fundamental types are disconnected. If a Ruby string is converted to a ``std::string`` then the two strings are independent and changes in one will *not* be reflected in the other.

Built In Conversions
--------------------
Rice supports all fundamental types out of the box. It also includes special handling for ``char*`` which is converted to a Ruby String. In addition, various STL classes are also converted as described in the :ref:`STL documentation <stl>`.

======================  ======================  ========================
C++ Type                Other Name              Ruby Type
======================  ======================  ========================
bool                                            TrueClass / FalseClass
int8_t / uint8_t        char                    String (binary encoding)
int8_t / uint8_t        char[]                  String (binary encoding)
int8_t / uint8_t        char*                   String (binary encoding)
int8_t                  signed char             String (binary encoding)
int8_t[]                signed char[]           String (binary encoding)
int8_t                  unsigned char           String (binary encoding)
int8_t[]                unsigned char[]         String (binary encoding)
int16_t                 short                   Integer
int16_t[]               short[]                 String (binary encoding)
uint16_t                unsigned short          Integer
uint16_t[]              unsigned short[]        String (binary encoding)
int32_t                 int                     Integer
int32_t[]               int[]                   String (binary encoding)
uint32_t                unsigned int            Integer
uint32_t[]              unsigned int[]          String (binary encoding)
int64_t                 long                    Integer
int64_t[]               long[]                  String (binary encoding)
uint64_t                unsigned long           Integer
int64_t[]               unsigned long[]         String (binary encoding)
int64_t                 long long               Integer
int64_t[]               long long[]             String (binary encoding)
uint64_t                unsigned long long      Integer
int64_t[]               unsigned long long[]    String (binary encoding)
float                                           Float
float[]                                         String (binary encoding)
double                                          Float
double[]                                        String (binary encoding)
======================  ======================  ========================

Memory Management
-----------------
Because data is copied, it is up to each language to manage its own memory. For example, the following code would cause a memory leak:

.. code-block:: cpp

  char* leakMemory()
  {
    char* data = new char[BUFFER_SIZE];
    return data;
  }

  Class rb_cTest =
    define_class("Test")
    .define_function("leak_memory", &leakMemory);

.. code-block:: ruby

  test = Test.new
  data = test.data

The leak happens because Ruby does *not* take ownership of the ``char*`` and thus it is never freed. For more information refer to the :ref:`memory_management` section.

