# Hash

```cpp
#include <rice/rice.hpp>
```

`Rice::Hash` wraps Ruby's Hash class and provides a C++-style interface with STL-compatible iterators.

---

## Constructors

### Hash()

Construct a new empty hash.

```cpp
Hash h;  // {}
```

---

### Hash(Object v)

Wrap an existing Ruby hash VALUE.

**Parameters:**

* `v` - An Object wrapping a `T_HASH` VALUE.

```cpp
VALUE ruby_hash = rb_hash_new();
Hash h(ruby_hash);
```

---

## Public Methods

### size() const -> size_t

Get the number of key-value pairs in the hash.

**Returns:**

The hash size.

```cpp
Hash h;
h["a"] = 1;
h["b"] = 2;
size_t sz = h.size();  // 2
```

---

### `operator[](Key_T const& key) const` -> Proxy const

Get the value for the given key (read-only).

**Template Parameters:**

* `Key_T` - The key type (automatically converted to Ruby).

**Parameters:**

* `key` - The key whose value should be retrieved.

**Returns:**

A const Proxy that can be implicitly converted to Object.

```cpp
Hash h;
h["name"] = "Alice";
Object value = h["name"];  // "Alice"
```

---

### `operator[](Key_T const& key)` -> Proxy

Get a proxy for the value at the given key (read-write).

**Template Parameters:**

* `Key_T` - The key type (automatically converted to Ruby).

**Parameters:**

* `key` - The key whose value should be retrieved or set.

**Returns:**

A Proxy that allows assignment.

```cpp
Hash h;
h["count"] = 42;       // Set value
h["count"] = 100;      // Update value
h[1] = "one";          // Integer key
h[Symbol("sym")] = 3;  // Symbol key
```

---

### get<Value_T>(Key_T const& key) -> Value_T

Get the value for the given key, converted to a specific C++ type.

**Template Parameters:**

* `Value_T` - The C++ type to convert the value to.
* `Key_T` - The key type (automatically converted to Ruby).

**Parameters:**

* `key` - The key whose value should be retrieved.

**Returns:**

The value converted to type `Value_T`.

**Throws:**

`Exception` if the value cannot be converted to `Value_T`.

```cpp
Hash h;
h["count"] = 42;
h["name"] = "test";

int count = h.get<int>("count");           // 42
std::string name = h.get<std::string>("name");  // "test"
```

---

## Proxy Class

`Hash::Proxy` is a helper class that enables the `hash[key] = value` syntax.

### operator Object() const

Implicitly convert the proxy to an Object (retrieves the value).

```cpp
Hash h;
h["key"] = "value";
Object obj = h["key"];  // Implicit conversion
```

---

### value() const -> VALUE

Explicitly get the underlying VALUE.

**Returns:**

The Ruby VALUE associated with the key.

```cpp
Hash h;
h["key"] = 42;
VALUE v = h["key"].value();
```

---

### operator=(T const& value) -> Object

Assign a value to the key.

**Template Parameters:**

* `T` - The value type (automatically converted to Ruby).

**Parameters:**

* `value` - The value to assign.

**Returns:**

The assigned value as an Object.

```cpp
Hash h;
h["int"] = 42;
h["string"] = "hello";
h["float"] = 3.14;
```

---

## Entry Class

`Hash::Entry` is a helper class for dereferencing iterators, similar to `std::pair`.

### Members

* `Object const key` - The key (also accessible as `first`)
* `Object const& first` - Alias for `key`
* `Proxy value` - The value (also accessible as `second`)
* `Proxy& second` - Alias for `value`

```cpp
Hash h;
h["a"] = 1;
h["b"] = 2;

for (auto entry : h) {
  std::cout << entry.first << " => " << entry.second << std::endl;
}
```

---

## Iterators

Hash provides STL-compatible input iterators for traversing key-value pairs.

### Types

* `Hash::iterator` - Mutable iterator
* `Hash::const_iterator` - Const iterator

### Methods

#### begin() -> iterator

Return an iterator to the first entry.

#### end() -> iterator

Return an iterator past the last entry.

#### begin() const -> const_iterator

Return a const iterator to the first entry.

#### end() const -> const_iterator

Return a const iterator past the last entry.

### Example

```cpp
Hash h;
h["a"] = 1;
h["b"] = 2;
h["c"] = 3;

// Range-based for loop
for (auto entry : h) {
  std::cout << entry.key << " => " << Object(entry.value) << std::endl;
}

// Iterator-based loop
for (auto it = h.begin(); it != h.end(); ++it) {
  std::cout << it->first << " => " << it->second << std::endl;
}
```

---

## Inherited Methods

Hash inherits all methods from [Object](object.md), including:

* `value()` - Get the underlying VALUE
* `call()` - Call Ruby methods like `keys`, `values`, `merge`, etc.
* `is_nil()` - Check if nil

---

## Example

```cpp
#include <rice/rice.hpp>
#include <iostream>

using namespace Rice;

void example()
{
  // Create and populate hash
  Hash h;
  h["name"] = "Alice";
  h["age"] = 30;
  h["active"] = true;

  // Size
  std::cout << "Size: " << h.size() << std::endl;  // 3

  // Access values
  std::string name = h.get<std::string>("name");
  int age = h.get<int>("age");

  // Modify values
  h["age"] = 31;

  // Use various key types
  Hash h2;
  h2[1] = "one";
  h2[2] = "two";
  h2[Symbol("sym")] = "symbol key";

  // Iterate
  std::cout << "Contents:" << std::endl;
  for (auto entry : h) {
    std::cout << "  " << entry.key << ": " << Object(entry.value) << std::endl;
  }

  // Call Ruby methods
  Object keys = h.call("keys");     // ["name", "age", "active"]
  Object values = h.call("values"); // ["Alice", 31, true]

  // Merge hashes
  Hash other;
  other["city"] = "NYC";
  Object merged = h.call("merge", other);
}
```

---

## See Also

* [Object](object.md) - Base class
* [Array](array.md) - Array wrapper class
* [Symbol](symbol.md) - Symbol wrapper class
