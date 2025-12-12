# String

```cpp
#include <rice/rice.hpp>
```

`Rice::String` wraps Ruby's String class and provides a C++-style interface to Ruby strings.

---

## Constructors

### String()

Construct a new empty string.

```cpp
String s;  // empty string ""
```

---

### String(VALUE v)

Wrap an existing Ruby string VALUE.

**Parameters:**

* `v` - A Ruby VALUE of type `T_STRING`.

```cpp
VALUE ruby_str = rb_str_new_cstr("hello");
String s(ruby_str);
```

---

### String(Object v)

Wrap an existing Ruby Object that is a string.

**Parameters:**

* `v` - An Object wrapping a string VALUE.

---

### String(Identifier id)

Construct a String from an Identifier.

**Parameters:**

* `id` - An Identifier to convert to a string.

```cpp
Identifier id("my_method");
String s(id);  // "my_method"
```

---

### String(char const* s)

Construct a String from a C string.

**Parameters:**

* `s` - A null-terminated C string.

```cpp
String s("hello");
```

---

### String(std::string const& s)

Construct a String from a C++ string.

**Parameters:**

* `s` - A C++ std::string.

```cpp
std::string cpp_str = "hello";
String s(cpp_str);
```

---

### String(std::string_view const& s)

Construct a String from a string_view.

**Parameters:**

* `s` - A C++ std::string_view.

```cpp
std::string_view sv = "hello";
String s(sv);
```

---

## Static Methods

### format(char const* fmt, args...) → String

Create a formatted string using printf-style formatting.

**Parameters:**

* `fmt` - A printf-style format string.
* `args...` - Arguments to format.

**Returns:**

A formatted String.

```cpp
String s = String::format("Value: %d, Name: %s", 42, "test");
// "Value: 42, Name: test"
```

---

## Public Methods

### length() const → size_t

Get the length of the string in characters.

**Returns:**

The number of characters in the string.

```cpp
String s("hello");
size_t len = s.length();  // 5
```

---

### operator[](ptrdiff_t index) const → char

Get the character at the given index.

**Parameters:**

* `index` - The index (may be negative to count from end).

**Returns:**

The character at that position.

```cpp
String s("hello");
char c = s[0];   // 'h'
char d = s[-1];  // 'o'
```

---

### c_str() const → char const*

Get a pointer to the underlying C string.

**Returns:**

A pointer to the null-terminated string data.

**Warning:**

The pointer is only valid while the String object exists and is not modified.

```cpp
String s("hello");
const char* cstr = s.c_str();
printf("%s\n", cstr);
```

---

### str() const → std::string

Get a copy of the string as a C++ std::string.

**Returns:**

A std::string copy of the Ruby string.

```cpp
String s("hello");
std::string cpp_str = s.str();
```

---

### intern() const → Identifier

Convert the string to an Identifier (intern it as a symbol).

**Returns:**

An Identifier representing the interned string.

```cpp
String s("my_method");
Identifier id = s.intern();
```

---

### unpack<T>() const → Array

Unpack the string into an Array of values.

**Template Parameters:**

* `T` - The type to unpack as (e.g., `int`, `float`).

**Returns:**

An Array of unpacked values.

**Description:**

This is equivalent to Ruby's `String#unpack` with the appropriate format for type T.

```cpp
String binary("\x01\x00\x00\x00\x02\x00\x00\x00", 8);
Array values = binary.unpack<int>();  // [1, 2]
```

---

## Inherited Methods

String inherits all methods from [Object](object.md), including:

* `value()` - Get the underlying VALUE
* `to_s()` - Convert to string representation
* `inspect()` - Get detailed representation
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
  // Create strings various ways
  String s1("hello");
  String s2 = String::format("Count: %d", 42);

  // Get as C++ string
  std::string cpp_str = s1.str();

  // Access characters
  char first = s1[0];   // 'h'
  char last = s1[-1];   // 'o'

  // Get length
  size_t len = s1.length();  // 5

  // Call Ruby methods
  Object upper = s1.call("upcase");  // "HELLO"
  Object reversed = s1.call("reverse");  // "olleh"

  // Concatenation via Ruby
  Object combined = s1.call("+", String(" world"));  // "hello world"

  // Output
  std::cout << s1 << std::endl;  // "hello"
}
```
