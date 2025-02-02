.. _enums:

Enums
=====
Rice supports both C enums and newer C++ enum classes.

.. code-block:: cpp

  enum Color
  { RED,
    BLACK,
    GREEN };

  enum class Season
  { Spring,
    Summer,
    Fall,
    Winter };

To expose an enum to Ruby, use ``define_enum`` like this:

.. code-block:: cpp

  Enum<Color> colors = define_enum<Color>("Color")
    .define_value("RED", RED)
    .define_value("BLACK", BLACK)
    .define_value("GREEN", GREEN);

``define_enum<Color>("Color")`` creates a new Ruby class called ``Color``. Each call to ``define_value``  defines a new instance of Color that is stored as a constant on the Color class. Thus from the Ruby side of things, the mapping looks like:

.. code-block:: ruby

  class Color
    RED = Color.new(RED)
    BLACK = Color.new(BLACK)
    GREEN = Color.new(GREEN)
  end

.. _anonymous_enums:

Nested and Anonymous Enums
--------------------------
C/C++ supports the concept of anonymous enums. They used to be used as hack to define constants before C++ compilers supported member constants. For example:

.. code-block:: cpp

  class MyClass
  {
    public:
      static const int SOME_CONSTANT = 42;

      enum class Season
      { Spring,
        Summer,
        Fall,
        Winter };

      // Enum hack that used to be needed by compilers
      enum
      {
          HACKED_CLASS_CONSTANT_1 = 43,
          HACKED_CLASS_CONSTANT_2 = 44
      };
  };

In this case, there is one class enum and one anonymous enum. The anonymous enum should be mapped to static member constants like this:

.. code-block:: cpp

    // Define the class. Map anonymous enums to constants
    Data_Type<MyClass> rb_cMyClass = define_class<MyClass>("MyClass").
      define_constructor(Constructor<MyClass>()).
      define_constant("SOME_CONSTANT", 42).
      define_constant("HACKED_CLASS_CONSTANT_1", (int)MyClass::HACKED_CLASS_CONSTANT_1).
      define_constant("HACKED_CLASS_CONSTANT_2", (int)MyClass::HACKED_CLASS_CONSTANT_2);

   // Define the class and enum and nest it under MyClass
   Enum<Season> seasons = define_enum_under<Season>("Season", rb_cMyClass)
    .define_value("Spring", Season::Spring)
    .define_value("Summer", Season::Summer)
    .define_value("Fall", Season::Fall)
    .define_value("Winter", Season::Winter);

From the Ruby side, this creates:

.. code-block:: ruby

  class MyClass
    SOME_CONSTANT = 42
    HACKED_CLASS_CONSTANT_1 = MyClass::HACKED_CLASS_CONSTANT_1
    HACKED_CLASS_CONSTANT_2", MyClass::HACKED_CLASS_CONSTANT_2

    class Season
      Spring = Color.new(Season::Spring)
      Summer = Color.new(Season::Summer)
      Fall = Color.new(Season::Fall)
      Winter = Color.new(Season::Winter)
    end
  end

Ruby API
--------
Generated enum classes have the following Ruby API.

Enum.from_int

Enum#<=>
Enum#eql?
Enum#hash
Enum#each
Enum#inspect
Enum#to_int
Enum#to_s

Enum#&
Enum#|
Enum#^
Enum#~
Enum#<<
Enum#>>

In addition, they have the following aliases:

Enum#===  Enum#eql?
Enum#to_i Enum#to_int

And mixin the following modules:

* Comparable
* Enumerable