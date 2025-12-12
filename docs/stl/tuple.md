# std::tuple

`std::tuple` is a fixed size container that may contain heterogeneous values.

Ruby does not have the concept of a tuple, but it can be mapped to a fixed size array. Thus Rice unwraps `std::tuple` instances into an array that has the same size as the tuple.

Rice also support passing a Ruby array to C++ api that takes tuples.

## Out Parameters

Prior to the introduction of tuples, C++ did not have a good way of returning multiple values from a function. One workaround was to return values via function parameters that are pointers. These are known as `out` parameters.

For example, the [minMaxLoc](https://docs.opencv.org/4.x/d2/de8/group__core__array.html#gab473bf2eb6d14ff97e89b355dac20707) function in OpenCV is defined as:

```cpp
void cv::minMaxLoc(cv::InputArray src,
    double *         minVal,
    double *         maxVal = 0,
    Point *         minLoc = 0,
    Point *         maxLoc = 0,
    cv::InputArray     mask = cv::noArray() )
```

All of `minVal`, `maxVal`, `minLoc` and `maxLoc` are out parameters designed to return values.

One way to wrap this function is to use [Buffers](../bindings/buffers.md#out-parameters).

An alternative approach is to use `std::tuple`.

```cpp
define_module_function("min_max_loc", [](cv::InputArray src, cv::InputArray mask = cv::noArray()) -> std::tuple<double, double, cv::Point, cv::Point>
{
  double minVal = 0;
  double maxVal = 0;
  cv::Point minLoc = 0;
  cv::Point maxLoc = 0;

  cv::minMaxLoc(src, &minVal, &maxVal, &minLoc, &maxLoc, mask);
  return std::forward_as_tuple(minVal, maxVal, minLoc, maxLoc);
},
Arg("src"), Arg("mask") = static_cast<cv::InputArray>(cv::noArray()));
```

Then to call the method from Ruby:

```ruby
mat = Cv::Mat.new(2, 2, CV_8UC4, cv::Scalar.new(10, 20, 30, 40))
min_val = 10
min_val, max_val, min_loc, max_loc = min_max_loc(mat.input_array, min_val)
```
