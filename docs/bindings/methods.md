# Functions and Methods

In the [tutorial](../tutorial.md) we touched upon how to wrap C++ functions, static member functions and member functions. Now let's go into more depth.

## Default Arguments

Going back to our initial C++ class example, lets add additional arguments to the `hello()` method, one of which has a default value:

```cpp
class Test
{
public:
  Test();
  std::string hello(std::string first, std::string second = "world");
};
```

Since default parameter values are not available through templates, it is necessary to tell Rice about it using `Rice::Arg`:

```cpp
#include <rice/rice.hpp>

using namespace Rice;

extern "C"
void Init_test()
{
  Data_Type<Test> rb_cTest =
    define_class<Test>("Test").
    define_constructor(Constructor<Test>()).
    define_method("hello", &Test::hello,
       Arg("first"), Arg("second") = "world"
    );
}
```

The syntax is `Arg(nameOfParameter)[ = defaultValue]`. If using [keyword arguments](#keyword-arguments), then the name of the parameter must match the name of the Ruby parameter. Otherwise it is not important. In either case, the value set by the `operator=` must match the type of the parameter. As such it may be necessary to explicitly cast the default value.

```cpp
.define_method("hello",
   &Test::hello,
   Arg("first"), Arg("second") = (std::string)"world"
);
```

These `Rice::Arg` objects must be in the correct positional order. Thus if the second argument has a default value, then there must be two Arg objects.

Note that Rice stores a copy of default values internally, so the type must be copyable. Types with deleted or private copy constructors cannot be used as default arguments. This includes types that inherit from non-copyable base classes. If you need to wrap a function with a non-copyable default parameter, you'll need to omit the default value and require Ruby callers to always provide that argument explicitly.

Now, Ruby will now know about the default arguments, and this wrapper can be used as expected:

```ruby
t = Test.new
t.hello("hello")
t.hello("goodnight", "moon")
```

This also works with Constructors:

```cpp
.define_constructor(Constructor<SomeClass, int, int>(),
    Arg("arg1") = 1, Arg("otherArg") = 12);
```

## Output Parameters

Some C/C++ APIs use output parameter to return multiple values from a functiom. To wrap these functions you can use either [Buffers](buffers.md#out-parameters) or [Tuples](../stl/tuple.md#out-parameters).

## Return

Similarly to the `Arg` class, Rice also supports a `Return` class that let's you tell Rice how to handle returned values from C++. This is particularly important in correctly managing memory (see [C++ to Ruby](memory_management.md#c-to-ruby)).

It is also helpful in dealing with Ruby's VALUE type which represent Ruby objects. Most of the time Rice will automatically handle VALUE instances, but if a native method takes a VALUE argument or returns a VALUE instance then you have tell Rice.

This is because VALUE is a typedef for `unsigned long long` - but under the hood it is really a pointer to a Ruby object. However, to Rice it is just an integer that needs to be converted to a Ruby numeric value. As a result, if a method takes a VALUE parameter then Rice will convert it to a C++ unsigned long long value instead of  passing it through. Similarly, if a method returns a VALUE then Rice will also convert it to a numeric Ruby object as opposed to simply returning it.

To avoid this incorrect conversion, use the `setValue()` method on the `Arg` and `Return` classes. For example:

```cpp
VALUE some_function(VALUE ary)
{
  VALUE new_ary = rb_ary_dup(ary);
  rb_ary_push(new_ary, Qtrue);
  return new_ary;
}

define_global_function("some_function", &some_function, Arg("ary").setValue(), Return().setValue());
```

Note that you can mix `Arg` and `Return` objects in any order. For example this also works:

```cpp
define_global_function("some_function", &some_function, Return().setValue(), Arg("ary").setValue());
```

## Keyword Arguments

Starting with version 4.5, Rice supports using Ruby keyword parameters to call C++ functions. The names of the keyword arguments must match the names specified in the `Arg` parameters used to define the method. The actual underlying names of the C++ parameters are irrelevant because C++ templates have no access to them.

For example, reusing the example above:

```cpp
.define_method("hello",
   &Test::hello,
   Arg("hello"), Arg("second") = (std::string)"world"
);
```

The `hello` function can be called from Ruby like this:

```ruby
test = Test.new
test.hello(first: "Hello", second: "World")
test.hello(first: "Hello") # This is ok because the second parameter has a default value
```

And it can also be called in the traditional manner like this:

```ruby
test = Test.new
test.hello("Hello", "World")
test.hello("Hello")
```

The ability to call the function in two different ways (position and keyword) could cause problems in your extensions if you change its API in the future. Imagine that you decide to move some code from C++ to Ruby - you will need to chose one of the two forms. That could result in breaking code that uses your extension because some users may have used positional arguments and others keyword arguments. However, this seems quite low risk though so Rice only supports defining arguments using the `Arg` class. In the future Rice may introduce a `KeyArg` class to avoid this issue.

## Return Self

In the case of methods that return `self` - meaning they return back the same C++ object that was the receiver of the function call - Rice ensures that the same Ruby object is returned. Returning self is a common pattern in Ruby.

For example:

```ruby
a = Array.new
a << 1 << 2 << 3 << 4
```

The above code works because the `<<` method returns the Array `a`. You can mimic this behavior by the use of lambdas when wrapping C++ classes. For example, Rice wraps `std::vector` like this:

```cpp
define_vector<int32_t>().
define_method("<<", [](std::vector<int32_t>& self, int32_t value) -> std::vector<int32_t>&  // <----- DON'T MISS THIS
{
  self.push_back(value);
  return self;  // <------  Allows chaining on calls
});
```

Pay careful attention to the lambda return type of `std::vector<int32_t>&`. If the return type is *not* specified, then by default the lambda will return by value. That will invoke `std::vector`'s copy constructor, resulting in *two* `std::vector<int32_t>` instance and two Ruby objects. Not at all what you want.

## Lambda Functions

Ruby classes are expected to define a `to_s` method that provides a string representation of an object. A good way of adding addition methods to a class is to use lambda functions:

```cpp
Data_Type<Test> rb_cTest =
  define_class<Test>("Test").
  define_method("to_s", [](Test& self)
  {
     return "<Test>";
  });
```

We define the `to_s` method to take a single parameter, self, which is an C++ instance of `Test`. Note that `self` is passed by reference - we do not want to create a copy of the Test object!

The lambda function can take any number of additional parameters. It can be either a stateless or stateful lambda.

## Global VM Lock (GVL)

Refer to the [GVL](gvl.md) section to learn how to call C++ code without holding Ruby's GVL.
