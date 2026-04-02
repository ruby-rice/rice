# Pin

```cpp
#include <rice/rice.hpp>
```

`Rice::Pin` is a GC-safety helper for Ruby `VALUE`s that are owned from C++.

Use `Pin` when C++ code needs to hold on to a Ruby object, but that C++ object is not itself wrapped by Ruby and therefore cannot participate in Ruby's mark phase. `Pin` keeps the Ruby object alive by storing it in an internal GC anchor.

Typical uses include:

* long-lived C++ objects that cache Ruby objects
* callback adapters that capture a Ruby proc
* C++ containers that hold Ruby `VALUE`s outside the Ruby heap

## Interface

```cpp
class Pin
{
public:
  Pin(VALUE value);

  Pin(const Pin&) = default;
  Pin& operator=(const Pin&) = default;

  Pin(Pin&&) noexcept = default;
  Pin& operator=(Pin&&) noexcept = default;

  VALUE value() const;
};
```

## Basic Example

```cpp
class Container
{
public:
  explicit Container(VALUE value) : pin_(value) {}

  VALUE value() const
  {
    return pin_.value();
  }

private:
  Rice::Pin pin_;
};
```

## Copy and Move Semantics

`Pin` uses shared ownership internally. Copying a `Pin` shares the same underlying GC anchor, so multiple C++ objects can safely refer to the same pinned Ruby object. Moving a `Pin` is cheap.

This is especially useful for copied callables such as `std::function` objects that capture Ruby procs: every copy keeps the proc alive for as long as any copy still exists.

## Pin Versus ruby_mark

Use `Pin` when the C++ object is not Ruby-managed.

Use `ruby_mark` when the C++ object is wrapped by Ruby with `Data_Type` and Ruby will call the mark function during GC.

See also: [Memory Management](../bindings/memory_management.md)
