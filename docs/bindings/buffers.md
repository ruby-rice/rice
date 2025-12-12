# Buffers

Buffers are often used in C and C++ APIs. Examples include:

* Buffers of fundamental types
* Pointers to arrays of fundamental type or C++ objects
* Pointer to a fundamental type (often used to support function parameters used to return values)

Rice supports these uses cases via its `Buffer<T>` Ruby class. For example, a Buffer to an int, `Buffer<T>`, will be wrapped by a Ruby class named `Rice::Buffer≺int≻`.

The [pointers](pointers.md) documentation describes how to create `Buffer<T>` classes.

## Fundamental Types

It is common for C and C++ APIs to take pointers to blocks of memory consisting of fundamental types like unsigned chars. For example, the OpenCV library includes an API to create a `Mat` instance like this:

```cpp
Matrix(int rows, int columns, void uint8_t* data)
```

`data` is a pointer to a buffer consisting of uint_8 values.

To call this API from Ruby, first create a `Buffer` from a Ruby array like this:

```ruby
data = [1, 2, 3, 4]
buffer = Rice::Buffer≺unsigned char≻.new(data)
mat = Matrix.new(2, 2, buffer.data)
```

It is your responsibility to manage the memory of the buffer. When the `buffer` variable goes out of scope, Ruby will garbage collect it which will free the underlying buffer. In most cases, the target API will immediately use the buffer so this is not a problem. In cases where a C++ API takes ownership of the buffer then make sure to call `release` on the buffer.

```cpp
mat = Matrix.new(2, 2, buffer.release)
```

## Array of Objects

C++ APIs can also take a pointer to an array of objects. For example:

```cpp
Matrix Matrix::operator()(const Range* ranges) const
```

This code creates a view onto an existing Matrix based on an array of `Range` objects. The length of the array is equal to the Matrix's number of dimensions.

To call this from Ruby:

```ruby
data = [Cv::Range.new(0, 10), Cv::Range(30, 40)]
buffer = Rice::Buffer≺Cv::Range≻.new(data)

matrix = Matrix.new(100, 100)
matrix[buffer.data]
```

## Array of Pointers

The above example also works with an array of pointers. In that case:

```cpp
Matrix Matrix::operator()(const Range** ranges) const
```

Notice the `ranges` parameter is now a `**` - or an array of pointers. You would call this method from Ruby in the exact same way as the example above.

## Out Parameters

C and C++ APIs sometimes return values via function parameters. For example, from OpenCV:

```cpp
void cv::minMaxLoc(cv::InputArray src,
    double*         minVal,
    double*         maxVal = 0,
    Point*          minLoc = 0,
    Point*          maxLoc = 0,
    cv::InputArray  mask = cv::noArray())
```

All of `minVal`, `maxVal`, `minLoc` and `maxLoc` are out parameters designed to return values.

One way to wrap this code is to return a tuple as explained in [out parameters with tuples](../stl/tuple.md#out-parameters). An alternative is to use `Buffers` like this:

```ruby
min_val = Rice::Buffer≺double≻.new()
max_val = Rice::Buffer≺double≻.new()
min_loc = Cv::Point.new
max_loc = Cv::Point.new
CV::min_max_loc(min_val.data, max_val.data, min_loc, max_loc)

# Read the min_val
puts min_val[0]
```

## Ruby API

Buffer's have the following Ruby API:

* new(value) - Create a new buffer from a Ruby object such as an Array
* new(value, size) - Create a new buffer from a Ruby object with a specified size
* size - The size of the buffer in bytes
* to_s - A user friendly representation of the buffer
* bytes - A Ruby string with a binary encoding. Buffer#size must be set
* bytes(count) - A Ruby string with a binary encoding of the specified length
* to_ary - A Ruby array. Buffer#size must be set
* to_ary(count) - A Ruby array of the specified length
* [](index) - Get the item at the specified index
* []=(index) - Update the item at the specified index
* data - Get a `Pointer<T>` object to the Buffer's managed memory that can be passed to C++ APIs
* release - Same as #data but tells the buffer to release ownership of its memory
