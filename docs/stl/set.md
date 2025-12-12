# std::set

Although there is a direct conceptual mapping between a `std::set` and a Ruby `Set`, Rice does not copy a C++ set to an Ruby set. Instead, it wraps `std::set`.

There are multiple reasons for this:

* `std::set` instantiations can only contain one type, while Ruby Sets can contain different types
* `std::set` instances can be quite large
* `std::set` instances commonly contain C++ classes that have complex copy or move semantics
* having two disconnected copies of data, one in C++ and one in Ruby, is usually undesirable

Rice will automatically define Ruby classes for each instantiation of `std::set` it finds. You may also manually define Ruby classes via the use of the `define_set` method. Set classes are added to the `Std` module.

## Usage

For C++ methods that take set arguments, you can instantiate a new set from Ruby (see [STL Class Names](stl.md#automatically-generated-ruby-classes)).

For example, assume this C++ code:

```cpp
void passSet(std::set<int> ints)
{
}

define_global_function("pass_set", &passSet);
```

One way to call it from Ruby is like this:

```ruby
set = Std::Set≺int≻.new
set.push(37)
pass_set(set)
```

In this case, Ruby is wrapping a C++ set. Therefore any changes made to the set in C++ will be visible to Ruby.

A second way is to pass a Ruby Set like this:

```ruby
set = Set.new([3, 5, 9])
pass_set(set)
```

In this case, Rice will *copy* the Ruby array instead of wrapping it. Thus any modifications made in C++ will not be visible to Ruby.

## Ruby API

Rice tries to make a `std::set` look like a Ruby Set by defining an API that is a subset of `Set`. However, there are differences you need to keep in mind.

If the set type supports C++ streams, then the following method will work, otherwise it will return "Not Printable"

* set#to_s
