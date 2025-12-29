# References

C++ uses references (`T&`) extensively. References allow functions to modify the caller's data. Rice handles references to objects automatically, but references to fundamental types require special handling.

## Object Types

For object types (classes wrapped with `define_class`), Rice automatically handles references. When a C++ method takes a reference to an object:

```cpp
void process(MyClass& obj);
```

Rice will automatically unwrap the Ruby object and pass the underlying C++ object by reference. No special handling is required.

## Fundamental Types

References to fundamental types (`int&`, `double&`, `bool&`, etc.) require the `Reference<T>` Ruby class. This is because Ruby's fundamental types (Integer, Float, etc.) are immutable and cannot be passed by reference directly.

Consider a C++ API that modifies a value through a reference:

```cpp
void increment(int& value);
```

To call this from Ruby, you must use a `Reference<T>` object:

```ruby
ref = Rice::Reference≺int≻.new(5)
increment(ref)
ref.value  #=> 6
```

Rice automatically detects methods that take references to fundamental types and [generates](../types/naming.md) the corresponding `Reference<T>` Ruby classes.

## Disambiguating Overloads

References are particularly useful for disambiguating overloaded C++ methods. When C++ has both pointer and reference overloads:

```cpp
void process(int* ptr);   // pointer overload
void process(int& ref);   // reference overload
```

Ruby code can explicitly choose which overload to call:

```ruby
# Call the pointer overload (int*)
buffer = Rice::Buffer≺int≻.new([42])
obj.process(buffer.ptr)

# Call the reference overload (int&)
ref = Rice::Reference≺int≻.new(42)
obj.process(ref)
```

Without `Reference<T>`, Rice would not be able to distinguish between these overloads.

## Return Values

When a C++ method returns a reference to a fundamental type, Rice always converts it to the corresponding Ruby type. For example:

```cpp
int& get_value();
```

This will return a Ruby `Integer`, not a `Reference<int>`. This is because Ruby's fundamental types are immutable, so there is no way to maintain a reference back to the C++ value.

## Supported Types

Rice supports references to all fundamental types:

* `int&`, `unsigned int&`
* `long&`, `unsigned long&`
* `long long&`, `unsigned long long&`
* `short&`, `unsigned short&`
* `char&`, `unsigned char&`, `signed char&`
* `float&`, `double&`
* `bool&`

Rice also supports references to `std::string`.

## See Also

* [Pointers](pointers.md) - For working with `T*` parameters
* [Buffers](buffers.md) - For working with arrays and memory blocks
* [Overloaded Methods](overloaded_methods.md) - For more on overload resolution
