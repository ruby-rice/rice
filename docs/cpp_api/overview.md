# Overview

Rice provides C++ wrapper classes for Ruby's built-in types, offering a natural C++ interface while preserving full access to Ruby's functionality.

## Class Hierarchy

Rice mimics the Ruby class hierarchy:

```text
Object
├── Module
│   └── Class
│       └── Struct
├── Array
├── Hash
├── Identifier
├── String
└── Symbol

Identifier (not an Object - wraps Ruby's internal ID type)
```

## Core Classes

* [Object](object.md) - Base class for all Ruby objects
* [Module](module.md) - Wraps Ruby modules with method definition support
* [Class](class.md) - Wraps Ruby classes (inherits from Module)
* [Array](array.md) - Array with STL-compatible random-access iterators
* [Hash](hash.md) - Hash with STL-compatible iterators
* [Identifier](identifier.md) - Lightweight ID wrapper (not a Ruby Object)
* [String](string.md) - String wrapper with C++ string interop
* [Symbol](symbol.md) - Symbol wrapper (Ruby Object)
* [Struct](struct.md) - Dynamic struct class creation

## Quick Examples

**Calling Ruby Methods**

```cpp
Object obj(some_value);
Object result = obj.call("method_name", arg1, arg2);

// With keyword arguments
Hash kwargs;
kwargs["encoding"] = "UTF-8";
Object result = obj.call_kw("method", kwargs);
```

**Working with Arrays**

```cpp
Array a;
a.push(1, false);
a.push(2, false);
a.push(3, false);

// Range-based for loop (C++11)
for (auto elem : a) {
  std::cout << elem << std::endl;
}

// STL algorithms work
auto dist = std::distance(a.begin(), a.end());
```

**Working with Hashes**

```cpp
Hash h;
h["name"] = "Alice";
h["age"] = 30;

// Iterate over entries
for (auto entry : h) {
  std::cout << entry.key << ": " << Object(entry.value) << std::endl;
}

// Get typed values
std::string name = h.get<std::string>("name");
int age = h.get<int>("age");
```

**Defining Modules and Classes**

```cpp
Module m = define_module("MyModule")
  .define_module_function("helper", []() { return 42; })
  .define_constant("VERSION", "1.0.0");

Class c = define_class("MyClass")
  .define_method("process", [](Object self, int x) {
    return x * 2;
  });
```

**Type Conversions**

All wrapper classes automatically convert between C++ and Ruby types:

```cpp
// C++ to Ruby
String s("hello");           // C string to Ruby String
Array a({1, 2, 3});          // Initializer list to Ruby Array
Symbol sym("my_symbol");     // C string to Ruby Symbol

// Ruby to C++
std::string cpp_str = s.str();
std::vector<int> vec = a.to_vector<int>();
std::string sym_str = sym.str();
```
