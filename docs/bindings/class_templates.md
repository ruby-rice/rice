# Class Templates

Some C++ libraries make heavy use of class templates. A class template is not a type, nor an object, but instead is blueprint used to create classes. Examples include STL containers such as `std::vector` or `std::map`. In addition, many libraries define their own custom class templates.

Since class templates are not types and cannot be instantiated, they cannot be wrapped by Rice. Instead class templates must be turned into classes so that they can be wrapped. For example, `std::vector<int>` is a class type that Rice can wrap.

## Wrapping Class Templates

Redefining the same methods over and over for every class that a class template generates would be tedious.

Lets look at an example, the [Mat_](https://docs.opencv.org/3.4/df/dfc/classcv_1_1Mat__.html) class template from OpenCV:

```cpp
template<typename _Tp>
class Mat_ : public Mat
{
public:
    Mat_();
    Mat_(int _rows, int _cols);

    template <std::size_t _Nm>
    explicit Mat_(const std::array<_Tp, _Nm>& arr, bool copyData=false);

    iterator begin();
    iterator end();

    _Tp& operator ()(int row, int col);
};
```

After defining the class template, OpenCV then defines a number of concrete classes. The list below is just a subset of them:

```cpp
typedef Mat_<uchar> Mat1b;
typedef Mat_<Vec2b> Mat2b;
typedef Mat_<Vec3b> Mat3b;
typedef Mat_<Vec4b> Mat4b;

typedef Mat_<int>   Mat1i;
typedef Mat_<Vec2i> Mat2i;
typedef Mat_<Vec3i> Mat3i;
typedef Mat_<Vec4i> Mat4i;

...
```

A naive approach to wrapping these classes is to define each one separately. Don't do that!

Instead, write a function template that creates and returns the wrapper:

```cpp
template<typename _Tp>
inline Data_Type<cv::Mat_<_Tp>> Mat__instantiate(VALUE module, const char* name)
{
  return define_class_under<cv::Mat_<_Tp>, cv::Mat>(module, name)
    .define_constructor(Constructor<cv::Mat_::Mat_<_Tp>>())
    .define_constructor(Constructor<cv::Mat_::Mat_<_Tp>, int, int>(), Arg("_rows"), Arg("_cols"))

    .template define_iterator<typename cv::Mat_<_Tp>::iterator(cv::Mat_<_Tp>::*)()>(&cv::Mat_<_Tp>::begin, &cv::Mat_<_Tp>::end, "each")
    .template define_method<_Tp&(cv::Mat_<_Tp>::*)(int, int)>("[]", &cv::Mat_<_Tp>::operator(), Arg("row"), Arg("col"))

    .define_method("[]=", [](cv::Mat_<_Tp>& self, int row, int column, _Tp& value)
    {
      self(row, column) = value;
    });
}
```

Then call this function to instantiate each concrete class:

```cpp
VALUE rb_cMat1b = Mat__instantiate<unsigned char>(rb_mCv, "Mat1b");

VALUE rb_cMat2b = Mat__instantiate<cv::Vec<unsigned char, 2>>(rb_mCv, "Mat2b");

...
```

There are few things to notice about the above code.

First, by convention, the function is named `"#{template_name}_instantiate"`. So in this case `Mat__instantiate`. You may of course name the function anything you want.

Second, the `template` keyword needs to be used in front of methods:

```cpp
.template define_iterator<typename cv::Mat_<_Tp>::iterator(cv::Mat_<_Tp>::*)()>(&cv::Mat_<_Tp>::begin, &cv::Mat_<_Tp>::end, "each")

.template define_method<_Tp&(cv::Mat_<_Tp>::*)(int, int)>("[]", &cv::Mat_<_Tp>::operator(), Arg("row"), Arg("col"))
```

Third, the array constructor cannot be wrapped because it uses a template parameter that is not defined:

```cpp
template <std::size_t _Nm>
explicit Mat_(const std::array<_Tp, _Nm>& arr, bool copyData=false);
```

Fourth, the `operator()` is mapped to two Ruby methods, `[]` and `[]=`.

```cpp
.template define_method<_Tp&(cv::Mat_<_Tp>::*)(int, int)>("[]", &cv::Mat_<_Tp>::operator(), Arg("row"), Arg("col"))

.define_method("[]=", [](cv::Mat_<_Tp>& self, int row, int column, _Tp& value)
{
  self(row, column) = value;
});
```

Once you have created an instantiation function it is easy to create new C++ classes from class templates and wrap them in Ruby.
