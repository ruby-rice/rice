# Migration

## Version 4.10 to 4.11

Version 4.11 introduces several breaking changes in instance tracking and the C++ API wrappers.

### InstanceRegistry setting changed from boolean to mode enum

`InstanceRegistry.isEnabled` no longer takes/returns a boolean-style state. It now uses a mode enum:
`Off`, `Owned`, `All`.

Before:

```ruby
Rice::InstanceRegistry.isEnabled = true
Rice::InstanceRegistry.isEnabled = false
```

After:

```ruby
Rice::InstanceRegistry.isEnabled = Rice::InstanceRegistry::Owned
Rice::InstanceRegistry.isEnabled = Rice::InstanceRegistry::Off
Rice::InstanceRegistry.isEnabled = Rice::InstanceRegistry::All
```

Also note the default changed from disabled to `Owned`. If your code depended on registry-off behavior, set it explicitly to `Off`.

### `Object()` now defaults to `Qnil` (not `rb_cObject`)

Default-constructed wrappers are now nil wrappers.

Before:

```cpp
Object obj;
obj.call("some_method"); // could accidentally target rb_cObject
```

After:

```cpp
Object obj;
if (!obj.is_nil())
{
  obj.call("some_method");
}
```

Or initialize explicitly:

```cpp
Object obj(rb_cObject);
obj.call("some_method");
```

### `Object::test()` removed

Replace calls to `test()` with either `operator bool()` or `is_nil()`, depending on intent.

Before:

```cpp
if (obj.test())
{
  // truthy
}
```

After:

```cpp
if (obj)
{
  // truthy
}
```

Or, if you specifically need nil checks:

```cpp
if (obj.is_nil())
{
  // nil
}
```

### Global `Object` constants removed

The convenience globals were removed:
`Rice::Nil`, `Rice::True`, `Rice::False`, `Rice::Undef`.

Before:

```cpp
Object value = Rice::Nil;
```

After:

```cpp
Object value(Qnil);
Object truthy(Qtrue);
Object falsy(Qfalse);
Object undef_value(Qundef);
```

### C++ API wrappers now use `Pin` internally

Wrapper classes (such as `Object`) now pin their wrapped Ruby value internally for GC safety.

Most users do not need code changes for this update. The primary behavior change is improved safety for long-lived wrappers, including wrappers stored in containers:

```cpp
std::vector<Object> values;
values.push_back(Object(rb_str_new_cstr("hello")));
```

If you previously added ad-hoc GC guards only to protect wrapper objects, those guards may no longer be necessary for the wrappers themselves.

## Version 4.7 to 4.10

Versions 4.8, 4.9, and 4.10 introduced several incompatible C++ API changes.

### Buffer/GVL API renames (4.8)

Before:

```cpp
define_method("read", &MyClass::read, Arg("buffer").isBuffer());
define_method("write", &MyClass::write, Return().isBuffer());
define_method("compute", &MyClass::compute, Function().noGVL());
```

After:

```cpp
define_method("read", &MyClass::read, ArgBuffer("buffer"));
define_method("write", &MyClass::write, ReturnBuffer());
define_method("compute", &MyClass::compute, NoGvL());
```

### `is_convertible` return type changed (4.8)

`From_Ruby<T>::is_convertible` must return `double` instead of `Convertible`.

Before:

```cpp
Convertible is_convertible(VALUE value)
{
  return Convertible::Exact;
}
```

After:

```cpp
double is_convertible(VALUE value)
{
  return Convertible::Exact;
}
```

### Method/default argument verification is stricter (4.8)

Rice now validates default arguments and type registrations more aggressively. Code that previously compiled but had mismatched/default argument types may now raise errors during binding setup.

Migration step:
1. Ensure every default argument value matches the bound C++ parameter type.
2. Ensure all custom/opaque types are registered before using them in defaults.

### Smart pointer wrapper internals changed (4.9)

If you implemented custom Rice-side smart pointer wrappers, update them to the current `Std::SharedPtr<T>` / `Std::UniquePtr<T>` model and forwarding behavior.

Most users who only consume smart pointers through `define_method`/`define_constructor` do not need code changes.

### `Address_Registration_Guard` replaced by `Pin` (4.10)

Before:

```cpp
VALUE value_;
Address_Registration_Guard guard_;

MyClass()
  : value_(rb_str_new2("test")), guard_(&value_)
{
}
```

After:

```cpp
Pin pin_;

MyClass()
  : pin_(rb_str_new2("test"))
{
}

VALUE value() const
{
  return pin_.value();
}
```

### Blocks are converted to Procs in C++ bindings (4.10)

If your C++ method expects a Ruby block, explicitly receive it as a `VALUE`/`Object` parameter and mark it as a value arg.

Before:

```cpp
define_method("run_with_block", [](VALUE self)
{
  // expected implicit block handling
});
```

After:

```cpp
define_method("run_with_block", [](VALUE self, VALUE proc)
{
  // proc is the Ruby block converted to Proc
}, Arg("proc").setValue());
```

### `Data_Type<T>::define()` removed (4.10)

Use class template binding helpers / explicit factory functions instead of `Data_Type<T>::define()`.

Migration step:
1. Remove `Data_Type<T>::define()` calls.
2. Replace with `define_class`/`define_class_under` flows documented in [Class Templates](bindings/class_templates.md).

## Version 4.6 to 4.7

Version 4.7 has a couple of breaking changes.

The `Rice_Init` method has been removed.

`Buffer<T>` classes can no longer be passed to C++ apis that take references or pointers. Instead use `Buffer<T>::data` or `Buffer<T>::release`.

## Version 4.5 to 4.6

Version 4.6 has a couple of breaking changes.

First, all C++ STL classes are now defined in the `Std` module. Therefore the following methods were removed - `define_vector_under`, `define_map_under` and `define_unordered_map_under`.

Second, Rice used to automatically convert Ruby Arrays of fundamental types to std::vectors (and similarly Hashes to std::map). Now you must use [Buffers](bindings/buffers.md) instead.

## Version 4.* to 4.5

Version 4.5 is significantly more powerful that previous Rice versions. For the most part it is backwards compatible. The exception is if you defined a custom `From_Ruby` instantiation. In that case, you will need to add a `convertible` method. See the [From_Ruby specialization](types/converters.md#step-3-specialize-from_ruby) documentation for an example.

## Version 3 to 4

Version 3 and earlier required compiling code on `gem install`. This made it difficult to use Rice on some platforms, such as Heroku or Github Actions, where the appropriate shared libraries and/or build systems are not allowed or available.

Rice 4 transitions to being a header only library, making it much easier for libraries using Rice to provide binary builds. However, this work was substantial and required making a few backwards-incompatible changes This page documents the major changes that any one using Rice 3 will need to apply to their libraries to work with Rice 4 or newer.

### #include <rice/rice.hpp>

The first change is that Rice is now published as a single, combined header file, so all includes can be changed to just this one. There is one other header file that contains STL wrapper definitions, you can get that with `#include <rice/stl.hpp>`. For more information see the [STL](stl/stl.md) section.

### to_ruby / from_ruby

The most notable backwards-incompatible change is how you define Ruby <=> C++ type conversions.

In Rice 3, you defined the methods `to_ruby` and `from_ruby` like this:

```cpp
template<>
Foo from_ruby<Foo>(Rice::Object x)
{
  // ...
}

template<>
Rice::Object to_ruby<Foo>(Foo const & x)
{
  // ...
}
```

In Rice 4, the structure of these functions has changed significantly. You now define these methods
as a `convert` function in a struct, and they need to live in the `Rice::detail` namespace:

```cpp
namespace Rice::detail
{
  template<>
  class From_Ruby<Foo>
  {
    Foo convert(VALUE x)
    {
      // ...
    }
  };

  template<>
  class To_Ruby<Foo>
  {
    VALUE convert(Foo const & x)
    {
      // ...
    }
  };
}
```

In addition, they work with Ruby's VALUE type as opposed to Rice's Object type. This switch was made to avoid making extra copies of objects when translating between C++ and Ruby. For more information, please refer to the [type conversion](types/conversion.md) section.


### Functions vs Methods

Rice now has different `define_` methods depending on if you are defining a method on an object or
a normal function. If you need `self`, use `define_method` or `define_singleton_method`. Otherwise
you should use `define_function` and `define_singleton_function`. You can read more in [the tutorial](tutorial.md#defining-methods).

### Default Arguments

They way Rice defines C++ [default arguments](bindings/methods.md#default-arguments) has subtly changed - this is an easy one to miss and can introduce bugs (default values will not be set correctly). Previously Rice relied on C++'s comma operator to combine arguments together:

```cpp
define_method("hello", &Test::hello, (Arg("hello"), Arg("second") = "world"));
```

Notice that the two `Args` are surrounded by parentheses. Rice *no* longer support this style. Instead, just pass the `Args` in a more natural way without the parentheses:

```cpp
define_method("hello", &Test::hello, Arg("hello"), Arg("second") = "world");
```

### Memory Management

Rice 4 also requires and provides more tools for explicitly defining which objects should and should not be managed by the Ruby garbage collector. Read more in [memory management](bindings/memory_management.md).
