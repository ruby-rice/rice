# std::unique_ptr

Rice provides full support for `std::unique_ptr`, allowing C++ unique pointers to be used seamlessly from Ruby.

## Ruby API

When Rice wraps a `std::unique_ptr<T>`, it creates a Ruby class under the `Std` module. The class exposes the following methods:

| Method | Description |
| ------ | ----------- |
| `empty?` | Returns true if the unique pointer is null |
| `get` | Returns the managed object |
| `release` | Releases ownership and returns the managed object |
| `reset` | Replaces the managed object |
| `swap` | Swaps the managed object with another unique_ptr |

Additionally, for non-fundamental and non-void types, Rice sets up forwarding so that methods and attributes defined on `T` can be called directly on the unique pointer wrapper.

Note: Methods or attributes that have the same name on both the smart pointer wrapper and the managed type (e.g., `swap`, `reset`) are not forwarded. To call the managed object's version, use `ptr.get.method_name`.

## Example

```cpp
class MyClass
{
public:
  int flag = 0;

  void setFlag(int value)
  {
    this->flag = value;
  }

  int getFlag()
  {
    return this->flag;
  }
};

class Factory
{
public:
  std::unique_ptr<MyClass> transfer()
  {
    return std::make_unique<MyClass>();
  }
};

void setupRice()
{
  define_class<MyClass>("MyClass").
    define_method("flag=", &MyClass::setFlag).
    define_method("flag", &MyClass::getFlag);

  define_class<Factory>("Factory").
    define_constructor(Constructor<Factory>()).
    define_method("transfer", &Factory::transfer);
}
```

In Ruby:

```ruby
factory = Factory.new
my_instance = factory.transfer

# Check the unique pointer
puts my_instance.empty?  # => false

# Methods are forwarded to the managed object
my_instance.flag = 5
puts my_instance.flag    # => 5

# Can also access via get
puts my_instance.get.flag  # => 5

# Release ownership (returns the raw pointer, unique_ptr becomes empty)
raw = my_instance.release
puts my_instance.empty?  # => true
```

## Ownership

When a native method returns a `std::unique_ptr` by value, Rice moves it into Ruby's control. Ruby then owns the unique pointer and its underlying object.

When the Ruby object is garbage collected, it destroys the `std::unique_ptr`, which in turn frees the managed C++ object.

If a method returns a `std::unique_ptr` by reference or pointer (which would be unusual), Rice's usual reference/pointer semantics apply instead.

Rice supports transferring ownership of a `std::unique_ptr` back to C++ by passing it by value to a C++ method:

```cpp
class Sink
{
public:
  int takeOwnership(std::unique_ptr<MyClass> ptr)
  {
    return ptr->flag;
  }
};

void setupRice()
{
  define_class<Sink>("Sink").
    define_constructor(Constructor<Sink>()).
    define_method("take_ownership", &Sink::takeOwnership);
}
```

In Ruby:

```ruby
factory = Factory.new
my_class = factory.transfer
my_class.flag = 5

sink = Sink.new
sink.take_ownership(my_class)  # Ownership transferred to C++

puts my_class.empty?  # => true (unique_ptr is now empty)
```

You can also pass a reference to allow C++ code to access the managed object without transferring ownership:

```cpp
int extractFlag(std::unique_ptr<MyClass>& myClass)
{
  return myClass->flag;
}
```

Passing a reference does *not* transfer ownership.

## Type Registration

Rice automatically registers `std::unique_ptr<T>` types when they are used. You can also explicitly register them:

```cpp
Rice::define_unique_ptr<MyClass>();
```

## See Also

To learn how to implement support for a custom smart pointer type, see [Smart Pointer Architecture](../architecture/smart_pointers.md).
