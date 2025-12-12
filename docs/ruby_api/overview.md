# Overview

Rice exposes several Ruby classes for working with C++ data and introspecting extension bindings.

## Data Classes

These classes help Ruby code work with C++ memory:

* [Buffer](buffer.md) - Manages contiguous blocks of C++ memory
* [Pointer](pointer.md) - Lightweight wrapper around C++ pointers

## Introspection Classes

These classes provide access to Rice's internal registries, enabling tools like `rice_rbs` to generate RBS type definitions:

* [Registries](registries.md) - Entry point singleton for accessing registries
* [TypeRegistry](type_registry.md) - Registry of C++ types mapped to Ruby classes
* [NativeRegistry](native_registry.md) - Registry of wrapped C++ functions and methods
* [Native](native.md) - Wrapper around a C++ function, method, or attribute
* [Parameter](parameter.md) - Parameter information for functions and methods
* [Arg](arg.md) - Argument metadata from Rice bindings

## Enabling the Introspection API

The introspection API is opt-in. To enable it, include the `<rice/api.hpp>` header and call `Init_Rice_Api`:

```cpp
#include <rice/api.hpp>

extern "C"
void Init_my_extension()
{
  // Enable Introspection API
  Init_Rice_Api();

  // Add Rice binding code here
}
```

## Quick Example

**Working with Buffers and Pointers**

```ruby
# Create a buffer from Ruby data
buffer = Rice::Buffer<int>.new([1, 2, 3, 4, 5])

# Access elements
buffer[0]      #=> 1
buffer.size    #=> 5
buffer.to_ary  #=> [1, 2, 3, 4, 5]

# Get a pointer to pass to C++ APIs
pointer = buffer.data
some_cpp_method(pointer)
```

**Introspecting an Extension**

```ruby
# Get the registries singleton
registries = Rice::Registries.instance

# List all wrapped classes
registries.types.klasses.each do |klass|
  puts klass.name
end

# Get methods for a class
methods = registries.natives.native_methods(MyClass)
methods.each do |native|
  puts "#{native.name}: #{native.return_type}"
end
```
