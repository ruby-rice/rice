.. _buffers:

Buffers
=======
Buffers are often used in C and C++ APIs. Examples include:

* Buffers of fundamental types
* Pointers to arrays of fundamental type or C++ objects
* Pointer to a fundamental type (often used to support function parameters used to return values)

Rice supports these uses cases via its ``Buffer`` class. The following buffer types are provided out of the box:

.. code-block:: cpp

    Buffer≺bool≻
    Buffer≺unsigned char≻
    Buffer≺unsigned char*≻
    Buffer≺signed char≻
    Buffer≺signed char*≻
    Buffer≺char≻
    Buffer≺char*≻
    Buffer≺unsigned short≻
    Buffer≺unsigned short*≻
    Buffer≺short≻
    Buffer≺short*≻
    Buffer≺unsigned int≻
    Buffer≺unsigned int*≻
    Buffer≺int≻
    Buffer≺int*≻
    Buffer≺float≻
    Buffer≺float*≻
    Buffer≺double≻
    Buffer≺double*≻
    Buffer≺unsigned long≻
    Buffer≺unsigned long*≻
    Buffer≺long≻
    Buffer≺long*≻
    Buffer≺unsigned long long≻
    Buffer≺unsigned long long*≻
    Buffer≺long long≻
    Buffer≺long long*≻
    Buffer≺void≻

However, to use them you need to register them. To do that add the following code to your bindings:

.. code-block:: cpp

   Rice::init();

Fundamental Types
-----------------
It is common for C and C++ APIs to take pointers to blocks of memory consisting of fundamental types like unsigned chars. For example, the OpenCV library includes an API to create a ``Mat`` instance like this:

.. code-block:: cpp

    Matrix(int rows, int columns, void uint8_t* data)

``data`` is a pointer to a buffer consisting of uint_8 values.

To call this API from Ruby, first create a ``Buffer`` from a Ruby array like this:

.. code-block:: ruby

   data = [1, 2, 3, 4]
   buffer = Rice::Buffer≺unsigned char≻.new(data)
   mat = Matrix.new(2, 2, buffer)

It is your responsibility to manage the memory of the buffer. When the ``buffer`` variable goes out of scope, Ruby will garbage collect it which will free the underlying buffer. In most cases, the target API will immediately use the buffer so this is not a problem. In cases where API takes ownership of the buffer then make sure to use ``takeOwnership`` as explained in the :ref:`Memory Management` section:

.. code-block:: cpp

  rb_cMatrix = define_class<Matrix>("Matrix").
    define_constructor(Constructor<Matrix, int, int, uint8_t*>(),
      Arg("rows"), Arg("columns"), Arg("data").takeOwnership();

Array of Objects
----------------
C++ APIs can also take a pointer to an array of objects. For example:

.. code-block:: cpp

  Matrix Matrix::operator()(const Range* ranges) const

This code creates a view onto an existing Matrix based on an array of ``Range`` objects. The length of the array is equal to the Matrix's number of dimensions.

To call this from Ruby:

.. code-block:: ruby

   data = [Cv::Range.new(0, 10), Cv::Range(30, 40)]
   buffer = Rice::Buffer≺Cv::Range≻.new(data)

   matrix = Matrix.new(100, 100)
   matrix[buffer]

Array of Pointers
-----------------
The above example also works with an array of pointers. In that case:

.. code-block:: cpp

  Matrix Matrix::operator()(const Range** ranges) const

Notice the ``ranges`` parameter is now a ``**`` - or an array of pointers. You would call this method from Ruby in the exact same way as the example above.

.. _out_parameters:

Out Parameters
--------------
C and C++ APIs sometimes return values via function parameters. For example, from OpenCV:

.. code-block:: cpp

  void cv::minMaxLoc(cv::InputArray src,
		double*         minVal,
		double*         maxVal = 0,
		Point*          minLoc = 0,
		Point*          maxLoc = 0,
		cv::InputArray  mask = cv::noArray())

All of ``minVal``, ``maxVal``, ``minLoc`` and ``maxLoc`` are out parameters designed to return values.

One way to wrap this code is to return a tuple as explained in :ref:`out_parameters_tuple`. An alternative is to use ``Buffers`` like this:

.. code-block:: ruby

   min_val = Rice::Buffer≺double≻.new()
   max_val = Rice::Buffer≺double≻.new()
   min_loc = Cv::Point.new
   max_loc = Cv::Point.new
   CV::min_max_loc(min_val, max_val, min_loc, max_loc)

   # Read the min_val
   puts min_val[0]
