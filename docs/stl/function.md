# std::function

Rice supports `std::function`, allowing C++ code to accept and return callable objects that can interoperate with Ruby.

## Automatic Registration

Rice automatically defines Ruby wrapper classes for `std::function` types when it encounters them in a C++ API. You do not need to manually register them.

For example, if you have a C++ function:

```cpp
std::function<int(int, int)> getAdder()
{
  return [](int a, int b) { return a + b; };
}
```

And bind it with Rice:

```cpp
define_module("MyModule")
  .define_module_function("get_adder", &getAdder);
```

Rice will automatically create the `Std::Function≺int❨int‚ int❩≻` class and wrap the returned function.

## Using std::function in Ruby

### Calling a Function Returned from C++

When C++ returns a `std::function`, you can call it directly in Ruby:

```ruby
func = MyModule.get_adder
result = func.call(7, 6)  # => 13
```

### Creating a std::function from Ruby

You can create a `std::function` wrapper from a Ruby proc, lambda, or block:

```ruby
# Using a proc
proc = Proc.new { |a, b| a * b }
func = Std::FunctionInt.new(proc)
func.call(5, 4)  # => 20

# Using a lambda
lamb = ->(a, b) { a + b }
func = Std::FunctionInt.new(lamb)
func.call(3, 8)  # => 11

# Using a block
func = Std::FunctionInt.new { |a, b| a - b }
func.call(10, 3)  # => 7
```

### Passing std::function to C++

When a C++ function accepts a `std::function` parameter, you must first wrap your Ruby callable in an `std::function` object:

```cpp
// C++ side
int invokeFunction(std::function<int(int, int)> func, int a, int b)
{
  return func(a, b);
}
```

```ruby
# Ruby side - correct approach
func = Std::FunctionInt.new { |a, b| a * b }
result = invoke_function(func, 5, 3)  # => 15
```

**Important:** You cannot pass a raw Ruby proc, lambda, or block directly to a C++ function that expects a `std::function`. This design simplifies memory management by ensuring the Ruby callable's lifetime is properly tracked.

```ruby
# This will NOT work
proc = Proc.new { |a, b| a * b }
invoke_function(proc, 5, 3)  # Raises an exception
```

## Manual Registration

If you prefer a custom class name, use `define_stl_function`:

```cpp
define_stl_function<int(int, int)>("FunctionInt");
```

This creates `Std::FunctionInt` instead of the auto-generated name `Std::Function≺int❨int‚ int❩≻`.

## Methods

The wrapped `std::function` class provides:

| Method | Description |
|--------|-------------|
| `new(callable)` | Creates a new std::function wrapping a Ruby proc, lambda, or block |
| `call(...)` | Invokes the function with the given arguments |
| `callable?` | Returns `true` if the function contains a valid callable target |

## Void Return Type

Functions with void return type are supported. The `call` method returns `nil` in Ruby:

```cpp
define_stl_function<void(double)>();
```

```ruby
func = Std::Function≺void❨double❩≻.new { |x| puts x }
func.call(3.14)  # prints 3.14, returns nil
```

## Exception Handling

Exceptions are properly propagated in both directions:

- C++ exceptions thrown during function creation or invocation are translated to Ruby exceptions
- Ruby exceptions raised inside the callable are propagated back through the C++ call stack
