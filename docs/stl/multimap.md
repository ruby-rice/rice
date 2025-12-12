# std::multimap

Ruby does not natively provide MultiMaps. Thus Rice wraps `std::multimap`.

Rice will automatically define Ruby classes for each instantiation of `std::multimap` it finds. You may also manually define Ruby classes via the use of the `define_multimap` method. Multimap classes are added to the `Std` module.

## Usage

For C++ methods that take multimap arguments, you can instantiate a new multimap from Ruby (see [STL Class Names](stl.md#automatically-generated-ruby-classes)).

For example, assume this C++ code:

```cpp
void passMultiMap(std::multimap<std::string, int> stringMultimap)
{
}

define_global_function("pass_multimap", &passMultiMap);
```

One way to call it from Ruby is like this:

```ruby
multimap = Std::Map≺string‚ int≻.new
multimap["thrity seven"] = "thirty seven"
pass_multimap(multimap)
```

In this case, Ruby is wrapping a C++ multimap. Therefore any changes made to the multimap in C++ will be visible to Ruby.

A second alternative is to pass it a Ruby hash instead:

```ruby
hash = {"three" => 3, "five" => 5, "nine" => 9}
pass_multimap(hash)
```

In this case, Rice will *copy* the Ruby hash instead of wrapping it. Thus any modifications made in C++ will not be visible to Ruby.

## Ruby API

Rice tries to make `std::multimap` look like a Ruby Hash by giving it an API that is a subset of `Hash`. However, there are differences you need to keep in mind.

First, the following methods only work if the multimap type is copyable (copying is done in C++):

* multimap#copy(other)

Second, the following methods only work if the multimap type implements C++'s equal operator, `operator==`:

* multimap#value?

Third, if the multimap type supports C++ streams, then the following method will work, otherwise it will return "Not Printable"

* multimap#to_s
