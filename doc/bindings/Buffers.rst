.. _arrays:

Arrays and Buffers
==================
A number of C and C++ APIs take pointers to a buffer or an array. For example, from OpenCV a ``Mat`` can be constructed like this:

.. code-block: cpp

    Matrix(int rows, int columns, void uint8_t* data)

``data`` is a pointer to a buffer consisting of rows x columns uint_8 values.

This API can be called in two ways from Ruby. The easiest way is to create a Ruby array and pass it to the constructor like this:

.. code-block: ruby
   data = [1, 2, 3, 4]
   mat = Matrix.new(2, 2, data)

Rice will automatically convert the Ruby array to a C++ buffer using ``Array#pack``. The created buffer will be valid for the entirety of the call to C++. However, after that the buffer will be automatically deleted. If instead C++ should take ownership of the buffer, then use ``takeOwnership`` as explained in the :ref:`Memory Management` section:

.. code-block: cpp

  rb_cMatrix = define_class<Matrix>("Matrix").
    define_constructor(Constructor<Matrix, int, int, uint8_t*>(),
      Arg("rows"), Arg("columns"), Arg("data").takeOwnership();

You can also create the buffer yourself and manage its lifetime as is appropriate:

.. code-block: ruby

  buffer = [1, 2, 3, 4]to_a.pack("i*")
  mat = Cv::Mat.new(2, 2, CV_32SC1, buffer)

To to this manually you will need to specify the correct ``pack`` `format <https://docs.ruby-lang.org/en/3.4/packed_data_rdoc.html>`_ string.

Using these techniques, Ruby can call almost any C/C++ API that takes a buffer made up of fundamental types (uint8_t, int8_t, uint16_t, int16_t, .... float, double..., etc).

In the case of a parameter that is ``void*``, you will have to use the buffer approach instead of the Array approach. That is because Rice will not be able to determine what ``pack`` format string to use.

Array of Objects
----------------
C++ APIs can also take a pointer to an C array of objects. For example:

.. code-block: cpp

  Matrix Matrix::operator()(const Range* ranges) const

This code creates a view onto an existing Matrix based on an array of ``Range`` objects. The length of the array is equal to the Matrix's number of dimensions.

To call this from Ruby:

.. code-block: ruby

  matrix1 = Matrix.new(10, 10, buffer)

  range1 = Range.new(4, 5)
  range2 = Range.new(7, 8)
  ranges = [range1, range2]
  matrix2 = matrix1[ranges]

Array of Pointers
-----------------
The above example also works with an array of pointers. In that case:

.. code-block: cpp

  Matrix Matrix::operator()(const Range** ranges) const

Notice the ``ranges`` parameter is now a ``**`` - or an array of pointers. You would call this method from Ruby in the exact same way as the example above.
