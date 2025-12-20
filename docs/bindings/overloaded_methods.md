# Overloaded Methods

C++ supports overloading constructors, methods and functions. Starting with version 4.5, Rice supports C++ method overloading.

When you try to wrap an overloaded function the C++ compiler will throw an error message that says something like "no matching overloaded function found."

## Getters and Setters

For example, consider this C++ class with a getter and setter that have the same name:

```cpp
class Container
{
public:
  size_t capacity()
  {
    return this->capacity_;
  }

  void capacity(size_t value)
  {
    this->capacity_ = value;
  }

private:
  size_t capacity_;
};
```

If you try and wrap the class like this you will get a compiler error:

```cpp
 Class c = define_class<Container>("Container")
  .define_constructor(Constructor<Container>())
  .define_method("capacity", &Container::capacity)
  .define_method("capacity=", &Container::capacity);
```

You need to tell the C++ compiler which overloaded method to use. There are several ways to do this as explained below.

## Template Parameter

`define_method` is a template function, therefore one solution is to specify which method you are trying to call like this:

```cpp
 Class c = define_class<Container>("Container")
  .define_constructor(Constructor<Container>())
  .define_method<size_t(Container::*)()>("capacity", &Container::capacity)
  .define_method<void(Container::*)(size_t)>("capacity=", &Container::capacity);
```

Notice the addition of the template specializations in side the `<` and '>' brackets. In this case,
`<size_t(Container::*)()>` and `<void(Container::*)(size_t)>` are C++ pointers to member functions.

## Using

Another solution is provided by the C++ keyword `using`, like this:

```cpp
using Getter_T = size_t(Container::*)();
using Setter_T = void(Container::*)(size_t);

Class c = define_class<Container>("Container")
  .define_constructor(Constructor<Container>())
  .define_method<Getter_T>("capacity", &Container::capacity)
  .define_method<Setter_T>("capacity=", &Container::capacity);
```

Or even like this:

```cpp
using Getter_T = size_t(Container::*)();
using Setter_T = void(Container::*)(size_t);

Class c = define_class<Container>("Container")
  .define_constructor(Constructor<Container>())
  .define_method("capacity", (Getter_T)&Container::capacity)
  .define_method("capacity=", (Setter_T)&Container::capacity);
```

## Typedef

If you like old school, obtuse C syntax, then use `typedef` like this:

```cpp
extern "C"
void Init_Container()
{
    typedef size_t(Container::* Getter_T)();
    typedef void (Container::* Setter_T)(size_t);

    Class c = define_class<Container>("Container")
      .define_constructor(Constructor<Container>())
      .define_method("capacity", (Getter_T)&Container::capacity)
      .define_method("capacity=", (Setter_T)&Container::capacity);
}
```

## Method Resolution

Ruby does not natively support method overloading. Thus Rice implements overloading support itself. It does this by maintaining a global registry (see [NativeRegistry](https://github.com/ruby-rice/rice/blob/master/rice/detail/NativeRegistry.hpp)) of methods keyed on class and method name. For the example above, the key would be `Container::capacity` and the value is an array of two [NativeFunction](https://github.com/ruby-rice/rice/blob/master/rice/detail/NativeFunction.hpp) instances, where each `NativeFunction` instance maps to one C++ member function.

At runtime, Rice evaluates the method parameters sent from Ruby and determines which overloaded C++ method is the best match. It does this by looping over the native `NativeFunction` instances and calls their `matches` method. The matches method, in turn, loops over the passed-in parameters and evaluates each one (for more information see the [type conversion](../types/conversion.md) section).

Matches are scored on a scale of 0.0 to 1.0:

* **1.0 (Exact)** - The types match exactly
* **0.99 (ConstMismatch)** - Passing a non-const value to a const parameter
* **0.9 (IntToFloat)** - Domain change penalty when converting integer to float
* **0.5 (SignedToUnsigned)** - Penalty when converting signed Ruby Integer to unsigned C++ type
* **0.5 (FloatToInt)** - Domain change penalty when converting float to integer (lossy)
* **0.0 (None)** - The types do not match and cannot be converted

For numeric types, Rice uses precision-based scoring. The score is calculated as `targetBits / sourceBits` when narrowing (e.g., Ruby Integer with 63 bits to C++ `int` with 31 bits scores 31/63 ≈ 0.49). Cross-domain conversions (int↔float) multiply the precision score by the domain penalty.

The final score for each overload is: `min(all parameter scores) × 0.99^(number of defaults used)`.

Based on these scores, each overloaded C++ method is sorted from best match to worst match. The best matching function is then called.

For more in-depth information about the resolution algorithm, see the [Overload Resolution Architecture](../architecture/overload_resolution.md) documentation.

## Type Mapping

The following table shows how Ruby types are mapped to C++ types. Ruby Integer has 63 bits precision (like `long long`), and Ruby Float has 53 bits (like `double` mantissa).

| C++ Type           | True | False | Nil | String | Integer | Float |
|--------------------|:----:|:-----:|:---:|:------:|:-------:|:-----:|
| bool               | 1.0  |  1.0  | 1.0 |        |         |       |
| char               |      |       |     |  1.0   |  0.11   | 0.07  |
| signed char        |      |       |     |  1.0   |  0.11   | 0.07  |
| unsigned char      |      |       |     |  1.0   |  0.06   | 0.08  |
| short              |      |       |     |        |  0.24   | 0.14  |
| unsigned short     |      |       |     |        |  0.13   | 0.15  |
| int                |      |       |     |        |  0.49   | 0.29  |
| unsigned int       |      |       |     |        |  0.25   | 0.30  |
| long*              |      |       |     |        |  0.49   | 0.29  |
| unsigned long*     |      |       |     |        |  0.25   | 0.30  |
| long long          |      |       |     |        |  1.0    | 0.50  |
| unsigned long long |      |       |     |        |  0.50   | 0.50  |
| float              |      |       |     |        |  0.34   | 0.45  |
| double             |      |       |     |        |  0.76   | 1.0   |

\* `long` is platform-dependent. On 64-bit systems: `long` = 63 bits (score 1.0), `unsigned long` = 64 bits (score 0.50).

See the [Type Mapping Reference](../architecture/overload_resolution.md#type-mapping-reference) for the underlying calculations.

If multiple overloads have equal scores, the first one defined wins.
