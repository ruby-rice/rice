.. _constructors:

Constructors
============

In the :ref:`tutorial` we touched upon how to wrap C++ constructors. Now let's go into more depth.

Unlike Ruby, C++ supports several types of constructors, including:

* Default constructors
* Custom constructors
* Copy constructors
* Move constructors

In addition, a C++ class may have multiple constructors.

.. _default_constructor:

Default Constructor
-------------------
Most C++ classes include a default constructor that takes no arguments. These are mapped to Rice like this:

.. code-block:: cpp

    define_constructor(Constructor<Mat>())

This defines a constructor that takes no arguments. It can be invoked from Ruby using the following code:

.. code-block:: ruby

    Mat.new

Under the hood, the ``define_constructor`` call creates a new ``initialize`` method on the Ruby ``Mat`` class. The ``initialize`` method is responsible for creating a new C++ ``Mat`` instance and associating it with the wrapper Ruby object. Thus if you override the ``initialize`` method you MUST call ``super``:

.. code-block:: ruby

    class Mat
      def initialize
        super  # <--- You MUST call super first
        ...your code..
      end
    end

.. _custom_constructors:

Custom Constructors
-------------------
The ``Mat`` class defines two custom constructors:

.. code-block:: cpp

  define_constructor(Constructor<Mat, int, int, int>())
  define_constructor(Constructor<Mat, const std::vector<int>&, int>())

These are invoked from Ruby like this:

.. code-block:: ruby

    Mat.new(1, 2, 3)

    vec = Std::Vector<int>.new
    Mat.new(vec, 4)

Similarly to default constructors, calling ``define_constructor`` will creates a new ``initialize`` method on the corresponding Ruby class.

.. _copy_constructors:

Copy Constructors
-------------------
Most C++ classes include a copy constructor that takes one argument. These are mapped to Rice like this:

.. code-block:: cpp

  define_constructor(Constructor<const Mat&>())

Rice maps copy constructors to Ruby's ``clone`` and ``dup`` methods:

.. code-block:: ruby

    mat1 = Mat.new(1, 2, 3)
    mat2 = mat1.dup
    mat3 = mat1.clone

Under the hood, the ``define_constructor`` call creates a new ``initialize_copy`` method on the Ruby Mat class. The ``initialize_copy`` method is responsible for calling the C++ copy constructor and assigning the new C++ instance to the wrapper Ruby object. Thus if you override the ``initialize_copy`` method you MUST call ``super``:

.. code-block:: ruby

    class Mat
      def initialize_copy
        super  # <--- You MUST call super first
        ...your code..
      end
    end

.. _move_constructors:

Move Constructors
-------------------
Rice does not support move constructors because there is no clean mapping of them to Ruby.

.. _multiple_constructors:

Multiple Constructors
---------------------
As seen in the examples above, a C++ clas can have multiple constructors. For example, consider this simplified version of `OpenCV's Mat <https://docs.opencv.org/4.x/d3/d63/classcv_1_1Mat.html>`_ class:

.. code-block:: cpp

    class Mat
    {
    public:
        Mat();
        Mat(int rows, int cols, int type);
        Mat(const std::vector<int>& sizes, int type);
        Mat(const Mat& m);
        Mat(Mat&& m);
    }

The Rice binding for the above example is:

.. code-block:: cpp

  #include <rice/rice.hpp>
  #include <rice/stl.hpp>

  using namespace Rice;

  extern "C"
  void Init_Mat()
  {
    Data_Type<Test> rb_cMat = define_class<Mat>("Mat")
      .define_constructor(Constructor<Mat>())
      .define_constructor(Constructor<Mat, int, int, int>())
      .define_constructor(Constructor<Mat, const std::vector<int>&, int>())
      .define_constructor(Constructor<const Mat&>())
      .define_constructor(Constructor<Mat&&>());
  }

Notice that the Constructor template requires the full list of argument types needed by the constructor.

For more information on how Rice decides which constructor to call, please see the :ref:`overloaded methods <overloaded_methods>` section.
