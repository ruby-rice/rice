# Symbol

```cpp
#include <rice/rice.hpp>
```

`Rice::Symbol` wraps Ruby's Symbol class. Symbols are internal identifiers in Ruby - they are singletons and can be thought of as frozen strings. They differ from an [Identifier](identifier.md) in that they are real Ruby Objects, but they can be converted back and forth between Identifier and Symbol.

---

## Constructors

### Symbol(VALUE v)

Wrap an existing Ruby symbol VALUE.

**Parameters:**

* `v` - A Ruby VALUE of type `T_SYMBOL`.

**Throws:**

`TypeError` if the value is not a symbol.

```cpp
VALUE ruby_sym = rb_intern("my_symbol");
VALUE sym_value = rb_id2sym(ruby_sym);
Symbol s(sym_value);
```

---

### Symbol(Object v)

Wrap an existing Ruby Object that is a symbol.

**Parameters:**

* `v` - An Object wrapping a symbol VALUE.

**Throws:**

`TypeError` if the value is not a symbol.

---

### Symbol(Identifier id)

Construct a Symbol from an Identifier.

**Parameters:**

* `id` - An Identifier to convert to a Symbol.

```cpp
Identifier id("my_method");
Symbol s(id);  // :my_method
```

---

### Symbol(char const* s = "")

Construct a Symbol from a null-terminated C string.

**Parameters:**

* `s` - A null-terminated C string (default: empty string).

```cpp
Symbol s1("hello");     // :hello
Symbol s2("my_method"); // :my_method
Symbol s3;              // :""
```

---

### Symbol(std::string const& s)

Construct a Symbol from a C++ string.

**Parameters:**

* `s` - A C++ std::string.

```cpp
std::string name = "calculate";
Symbol s(name);  // :calculate
```

---

### Symbol(std::string_view const& s)

Construct a Symbol from a string_view.

**Parameters:**

* `s` - A C++ std::string_view.

```cpp
std::string_view sv = "process";
Symbol s(sv);  // :process
```

---

## Public Methods

### c_str() const -> char const*

Get a pointer to the symbol's string representation.

**Returns:**

A pointer to the null-terminated string.

**Warning:**

The pointer is only valid while the Symbol object exists.

```cpp
Symbol s("hello");
const char* cstr = s.c_str();
printf("%s\n", cstr);  // "hello"
```

---

### str() const -> std::string

Get a copy of the symbol as a C++ string.

**Returns:**

A std::string copy of the symbol's string representation.

```cpp
Symbol s("my_symbol");
std::string cpp_str = s.str();  // "my_symbol"
```

---

### to_id() const -> Identifier

Convert the Symbol to an Identifier.

**Returns:**

An Identifier representing the same symbol.

```cpp
Symbol s("my_method");
Identifier id = s.to_id();
```

---

## Inherited Methods

Symbol inherits all methods from [Object](object.md), including:

* `value()` - Get the underlying VALUE
* `to_s()` - Convert to string representation
* `inspect()` - Get detailed representation (e.g., `:symbol`)
* `call()` - Call Ruby methods
* `is_nil()` - Check if nil

---

## Example

```cpp
#include <rice/rice.hpp>
#include <iostream>

using namespace Rice;

void example()
{
  // Create symbols from various sources
  Symbol s1("name");
  Symbol s2(std::string("age"));
  Symbol s3(Identifier("active"));

  // Get string representations
  std::cout << s1.str() << std::endl;  // "name"
  std::cout << s1.c_str() << std::endl;  // "name"

  // Convert to Identifier
  Identifier id = s1.to_id();

  // Use as hash keys (common use case)
  Hash h;
  h[Symbol("name")] = "Alice";
  h[Symbol("age")] = 30;

  // Compare symbols (they are singletons)
  Symbol s4("name");
  if (s1.value() == s4.value()) {
    std::cout << "Same symbol!" << std::endl;
  }

  // Call Ruby methods
  Object upper = s1.call("to_s").call("upcase");  // "NAME"
  Object inspected = s1.inspect();  // ":name"
}

// Exposing a method that takes a Symbol parameter
extern "C"
void Init_my_extension()
{
  define_module("MyModule")
    .define_module_function("process", [](Symbol action) {
      std::string action_str = action.str();
      if (action_str == "start") {
        return "Starting...";
      } else if (action_str == "stop") {
        return "Stopping...";
      }
      return "Unknown action";
    });

  // Ruby usage:
  // MyModule.process(:start)  #=> "Starting..."
  // MyModule.process(:stop)   #=> "Stopping..."
}
```

---

## See Also

* [Object](object.md) - Base class
* [Identifier](identifier.md) - Internal identifier (not a Ruby Object)
* [String](string.md) - String wrapper class
