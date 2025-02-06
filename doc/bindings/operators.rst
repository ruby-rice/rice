.. _operators:

C++ Operators
=============
C++ supports a lot of operators! These include operators that you likely think of, such as arithmetic and logical operators, as well as `conversion <https://en.cppreference.com/w/cpp/language/cast_operator>`_ operators.

Both C++ and Ruby support operator overriding, although C++ supports more of them. The sections below describe how to map C++ operators to Ruby operators.

Arithmetic Operators
--------------------
Most overridden C++ arithmetic operators can be overridden in Ruby.

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
|   ++    |  Not overridable  |
+---------+-------------------+
|   --    |  Not overridable  |
+---------+-------------------+

Assignment Operators
--------------------
Ruby does not support overriding assignment operators.

+---------+-------------------+
| C++     |     Ruby          |
+=========+===================+
|  =      | assign            |
+---------+-------------------+
| +=      | Not overridable   |
+---------+-------------------+
| -=      | Not overridable   |
+---------+-------------------+
| \*=     | Not overridable   |
+---------+-------------------+
| /=      | Not overridable   |
+---------+-------------------+
| %=      | Not overridable   |
+---------+-------------------+

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

+---------+-------------------+
| C++     |        Ruby       |
+=========+===================+
|  &&     |  Not overridable  |
+---------+-------------------+
| \|\|    |  Not overridable  |
+---------+-------------------+
|  !      |        !          |
+---------+-------------------+

Other Operators
-------------------
C++ and Ruby support overriding an additional set of operators.

+---------+----------------------+
| C++     |     Ruby             |
+=========+======================+
| []      | []                   |
+         +----------------------+
|         | []= (if reference)   |
+---------+----------------------+
| ()      |   call               |
+---------+----------------------+
|  ,      |  Not overridable     |
+---------+----------------------+
| <<      |   <<                 |
+---------+----------------------+
| >>      |   >>                 |
+---------+----------------------+

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





