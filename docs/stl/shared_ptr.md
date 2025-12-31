# std::shared_ptr

Rice provides full support for `std::shared_ptr`, allowing C++ shared pointers to be used seamlessly from Ruby.

## Ruby API

When Rice wraps a `std::shared_ptr<T>`, it creates a Ruby class under the `Std` module. The class exposes the following methods:

| Method | Description |
| ------ | ----------- |
| `empty?` | Returns true if the shared pointer is null |
| `get` | Returns the managed object |
| `swap` | Swaps the managed object with another shared_ptr |
| `use_count` | Returns the number of shared_ptr instances managing the object |

For non-void types, a constructor is also defined that takes ownership of a raw pointer.

Additionally, for non-fundamental and non-void types, Rice sets up forwarding so that methods and attributes defined on `T` can be called directly on the shared pointer wrapper.

Note: Methods or attributes that have the same name on both the smart pointer wrapper and the managed type (e.g., `swap`) are not forwarded. To call the managed object's version, use `ptr.get.method_name`.

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
  std::shared_ptr<MyClass> share()
  {
    if (!instance_)
    {
      instance_ = std::make_shared<MyClass>();
    }
    return instance_;
  }

private:
  static inline std::shared_ptr<MyClass> instance_;
};

void setupRice()
{
  define_class<MyClass>("MyClass").
    define_method("flag=", &MyClass::setFlag).
    define_method("flag", &MyClass::getFlag);

  define_class<Factory>("Factory").
    define_constructor(Constructor<Factory>()).
    define_method("share", &Factory::share);
}
```

In Ruby:

```ruby
factory = Factory.new
my_instance = factory.share

# Check the shared pointer
puts my_instance.empty?     # => false
puts my_instance.use_count  # => 1

# Methods are forwarded to the managed object
my_instance.flag = 5
puts my_instance.flag       # => 5

# Can also access via get
puts my_instance.get.flag   # => 5
```

## Ownership

When a native method returns a `std::shared_ptr` by value, Rice copies it to maintain standard shared pointer semantics. This increments the reference count, and Ruby then shares ownership with any other `std::shared_ptr` instances in C++ code.

When the Ruby object is garbage collected, it releases its reference to the `std::shared_ptr`, decrementing the reference count. The underlying C++ object is freed only when all shared pointer instances are destroyed.

If a method returns a `std::shared_ptr` by reference or pointer (which would be unusual), Rice's usual reference/pointer semantics apply instead.

## Passing to C++ Methods

Rice automatically determines whether to pass the `std::shared_ptr` itself or the underlying raw pointer based on what the C++ method expects. This happens efficiently in C++ without requiring a round-trip through Ruby.

```cpp
// Method that takes a shared_ptr
int processShared(std::shared_ptr<MyClass> ptr)
{
  return ptr->getFlag();
}

// Method that takes a raw pointer
int processRaw(MyClass* ptr)
{
  return ptr->getFlag();
}

void setupRice()
{
  define_class<MyClass>("MyClass").
    define_method("flag", &MyClass::getFlag);

  define_global_function("process_shared", &processShared);
  define_global_function("process_raw", &processRaw);
}
```

In Ruby, you simply pass the shared pointer wrapper to either method:

```ruby
factory = Factory.new
my_instance = factory.share

# Rice passes the std::shared_ptr to C++
process_shared(my_instance)

# Rice extracts the raw pointer and passes it to C++
process_raw(my_instance)

# This also works but is less efficient because it goes through Ruby
process_raw(my_instance.get)
```

## Type Registration

Rice automatically registers `std::shared_ptr<T>` types when they are used. You can also explicitly register them:

```cpp
Rice::define_shared_ptr<MyClass>();
```

## See Also

To learn how to implement support for a custom smart pointer type, see [Smart Pointer Architecture](../architecture/smart_pointers.md).
