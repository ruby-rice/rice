.. _operators:

C++ Operators
=============
C++ supports a lot of operators! These include operators that you likely think of, such as arithmetic and logical operators, as well as `conversion <https://en.cppreference.com/w/cpp/language/cast_operator>`_ operators.

Both C++ and Ruby support operator overriding, although C++ supports more of them. The sections below describe how to map C++ operators to Ruby operators.

C++ operators that are not supported by Ruby can be mapped to Ruby methods instead. By convention these methods are named based on the C++ operator name. Thus the C++ assign operator, `=`, is mapped to a Ruby method called assign.

.. code-block:: ruby

  my_object1 = SomeClass.new
  my_object2 = SomeClass.new
  my_object1.assign(my_object2)

Arithmetic Operators
--------------------
C++ and Ruby support overriding the same arithmetic operators.

+---------+-------------------+
|   C++   |       Ruby        |
+=========+===================+
|   \+    |        \+         |
+---------+-------------------+
|   \-    |        \-         |
+---------+-------------------+
|   \*    |        \*         |
+---------+-------------------+
|    /    |         /         |
+---------+-------------------+
|    %    |         %         |
+---------+-------------------+

Assignment Operators
--------------------
C++ supports overriding assignment operators while Ruby does not. Thus these operators must be mapped to Ruby methods.

+---------+------------------+------------------+
| C++     |  Ruby            |   Ruby Method    |
+=========+==================+==================+
|  =      | Not overridable  |  assign          |
+---------+------------------+------------------+
|  +=     | Not overridable  |  assign_plus     |
+---------+------------------+------------------+
|  -=     | Not overridable  |  assign_minus    |
+---------+------------------+------------------+
|  *=     | Not overridable  |  assign_multiply |
+---------+------------------+------------------+
|  /=     | Not overridable  |  assign_divide   |
+---------+------------------+------------------+
|  %=     | Not overridable  |  assign_plus     |
+---------+------------------+------------------+

Bitwise Operators
-----------------
C++ and Ruby support overriding the same bitwise operators.

+---------+-------------------+
| C++     | Ruby              |
+=========+===================+
|  &      |  &                |
+---------+-------------------+
| \|      |  \|               |
+---------+-------------------+
| \~      |  \~               |
+---------+-------------------+
| <<      |  <<               |
+---------+-------------------+
| >>      |  >>               |
+---------+-------------------+

Comparison (Relational) Operators
---------------------------------
C++ and Ruby support overriding the same comparison operators.

+---------+-------------------+
| C++     | Ruby              |
+=========+===================+
| ==      | ==                |
+---------+-------------------+
| !=      |  !=               |
+---------+-------------------+
|  >      |  <                |
+---------+-------------------+
|  <      |  >                |
+---------+-------------------+
| >=      |  >=               |
+---------+-------------------+
| <=      |  <=               |
+---------+-------------------+

Logical Operators
-----------------
Ruby allows the ``!`` operator to be overridden but not ``&&`` or ``||``.

+---------+------------------+------------- +
| C++     |        Ruby      | Ruby Method  |
+=========+==================+==============+
|  &&     |  Not overridable | logical_and  |
+---------+------------------+------------- +
| \|\|    |  Not overridable | logical_or   |
+---------+------------------+------------- +
|  !      |        !         |              |
+---------+------------------+------------- +

Increment / Decrement Operators
-------------------------------
C++ supports increment and decrement operators while Ruby does not. Thus these operators must be mapped to Ruby methods.

+---------+------------------+----------------+
| C++     |        Ruby      | Ruby Method    |
+=========+==================+================+
|  ++a    |  Not overridable | increment_pre  |
+---------+------------------+----------------+
|  a++    |  Not overridable | increment      |
+---------+------------------+----------------+
| --a     |  Not overridable | decrement_pre  |
+---------+------------------+----------------+
|  a--    |  Not overridable | decrement      |
+---------+------------------+----------------+

Other Operators
-------------------
C++ and Ruby support overriding an additional set of operators. The comma operator is not overridable in Ruby nor does it make sense to map it to a Ruby method.

+---------+----------------------+--------------+
| C++     |     Ruby             |  Ruby Method |
+=========+======================+==============+
| []      | []                   |              |
+         +----------------------+--------------+
|         | []= (if reference)   |              |
+---------+----------------------+--------------+
| ()      | Not Overridable      |  call        |
+---------+----------------------+--------------+
| *       | Not Overridable      |  dereference |
+---------+----------------------+--------------+
| <<      |   <<                 |              |
+---------+----------------------+--------------+
| >>      |   >>                 |              |
+---------+----------------------+--------------+
|  ,      |  Not overridable     |              |
+---------+----------------------+--------------+

If a C++ class defines an ``[]`` operator that returns a reference, then in it should be mapped to two Ruby operators: ``[]`` and ``[]=``.

C++ classes that support the ``()`` operator are known as functors. Ruby supports overriding the ``.()`` operator by defining a ``call`` function. Note this isn't quite the same as C++ because it is invoked via ``.()`` and not ``()`` -- notice the ``.`` before the ``()``.

Conversion Operators
--------------------
C++ allows users to define explicit and implicit conversion operators or functions. These are used to convert a class to a different types. For example:

.. code-block:: cpp

    class Money
    {
    public:
        Money(float amount);
        operator float() const;
    };

The ``operator double() const;`` is a user defined conversion function that converts a ``Money`` instance to a double. Following Ruby conventions, these conversion functions should be exposed as ``to_`` method, in this case ``to_f``.





