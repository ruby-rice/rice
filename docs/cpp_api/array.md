# Array

```cpp
#include <rice/rice.hpp>
```

`Rice::Array` wraps Ruby's Array class and provides a C++-style interface with STL-compatible iterators.

---

## Constructors

### Array()

Construct a new empty array.

```cpp
Array a;  // []
```

---

### Array(long capacity)

Construct a new array with pre-allocated capacity.

**Parameters:**

* `capacity` - The initial capacity.

```cpp
Array a(100);  // Empty array with space for 100 elements
```

---

### Array(VALUE v)

Wrap an existing Ruby array VALUE.

**Parameters:**

* `v` - A Ruby VALUE of type `T_ARRAY`.

```cpp
VALUE ruby_arr = rb_ary_new();
Array a(ruby_arr);
```

---

### Array(Object v)

Wrap an existing Ruby Object that is an array.

**Parameters:**

* `v` - An Object wrapping an array VALUE.

---

### Array(Iter_T begin, Iter_T end)

Construct an array from an iterator range.

**Template Parameters:**

* `Iter_T` - The iterator type.

**Parameters:**

* `begin` - Iterator to the start of the range.
* `end` - Iterator to the end of the range.

```cpp
std::vector<int> vec = {1, 2, 3, 4, 5};
Array a(vec.begin(), vec.end());
```

---

### Array(T (&a)[n])

Construct an array from a C array.

**Template Parameters:**

* `T` - The element type.
* `n` - The array size.

**Parameters:**

* `a` - A C array.

```cpp
int values[] = {1, 2, 3, 4, 5};
Array a(values);
```

---

## Public Methods

### size() const → long

Get the number of elements in the array.

**Returns:**

The array size.

```cpp
Array a;
a.push(1, false);
a.push(2, false);
long sz = a.size();  // 2
```

---

### `operator[](long index) const` → Object

Get the element at the given index (read-only).

**Parameters:**

* `index` - The index (may be negative to count from end).

**Returns:**

The element as an Object.

```cpp
Array a;
a.push("hello", false);
Object elem = a[0];   // "hello"
Object last = a[-1];  // "hello"
```

---

### `operator[](long index)` → Proxy

Get a proxy for the element at the given index (read-write).

**Parameters:**

* `index` - The index (may be negative).

**Returns:**

A Proxy that allows assignment.

```cpp
Array a;
a.push(1, false);
a[0] = 42;  // Replace first element
```

---

### push(T&& obj, bool takeOwnership = false) → Object

Append an element to the end of the array.

**Parameters:**

* `obj` - The object to append (automatically converted to Ruby).
* `takeOwnership` - If true, Ruby takes ownership of wrapped C++ objects.

**Returns:**

The appended object.

```cpp
Array a;
a.push(1, false);
a.push("hello", false);
a.push(3.14, false);
```

---

### pop() → Object

Remove and return the last element.

**Returns:**

The removed element, or `Qnil` if empty.

```cpp
Array a;
a.push(1, false);
a.push(2, false);
Object last = a.pop();  // 2
```

---

### unshift(T const& obj) → Object

Prepend an element to the beginning of the array.

**Parameters:**

* `obj` - The object to prepend.

**Returns:**

The prepended object.

```cpp
Array a;
a.push(2, false);
a.unshift(1);  // [1, 2]
```

---

### shift() → Object

Remove and return the first element.

**Returns:**

The removed element.

```cpp
Array a;
a.push(1, false);
a.push(2, false);
Object first = a.shift();  // 1
```

---

### join(const char* separator) → String

Join array elements into a string.

**Parameters:**

* `separator` - The separator between elements.

**Returns:**

A String with joined elements.

```cpp
Array a;
a.push("a", false);
a.push("b", false);
a.push("c", false);
String s = a.join(", ");  // "a, b, c"
```

---

### pack<T>() → String

Pack array elements into a binary string.

**Template Parameters:**

* `T` - The type to pack as.

**Returns:**

A String containing packed binary data.

```cpp
Array a;
a.push(1, false);
a.push(2, false);
String binary = a.pack<int>();
```

---

### to_vector<T>() → std::vector<T>

Convert the array to a C++ vector.

**Template Parameters:**

* `T` - The element type.

**Returns:**

A std::vector containing converted elements.

```cpp
Array a;
a.push(1, false);
a.push(2, false);
a.push(3, false);
std::vector<int> vec = a.to_vector<int>();
```

---

## Iterators

Array provides STL-compatible random-access iterators.

### Types

* `Array::iterator` - Mutable iterator
* `Array::const_iterator` - Const iterator

### Methods

#### begin() → iterator

Return an iterator to the first element.

#### end() → iterator

Return an iterator past the last element.

#### begin() const → const_iterator

Return a const iterator to the first element.

#### end() const → const_iterator

Return a const iterator past the last element.

### Example

```cpp
Array a;
a.push(1, false);
a.push(2, false);
a.push(3, false);

// Range-based for loop
for (auto elem : a) {
  std::cout << elem << std::endl;
}

// Iterator-based loop
for (auto it = a.begin(); it != a.end(); ++it) {
  std::cout << *it << std::endl;
}

// Random access
auto it = a.begin();
it += 2;  // Jump to third element
std::cout << *it << std::endl;  // 3

// STL algorithms work with const_iterators
auto dist = std::distance(a.begin(), a.end());  // 3
```

---

## Inherited Methods

Array inherits all methods from [Object](object.md), including:

* `value()` - Get the underlying VALUE
* `call()` - Call Ruby methods like `sort`, `map`, etc.
* `is_nil()` - Check if nil

---

## Example

```cpp
#include <rice/rice.hpp>
#include <iostream>
#include <algorithm>

using namespace Rice;

void example()
{
  // Create and populate array
  Array a;
  a.push(3, false);
  a.push(1, false);
  a.push(4, false);
  a.push(1, false);
  a.push(5, false);

  // Size
  std::cout << "Size: " << a.size() << std::endl;  // 5

  // Access elements
  std::cout << "First: " << a[0] << std::endl;   // 3
  std::cout << "Last: " << a[-1] << std::endl;   // 5

  // Modify element
  a[0] = 10;

  // Iterate
  std::cout << "Elements: ";
  for (auto elem : a) {
    std::cout << elem << " ";
  }
  std::cout << std::endl;

  // Use Ruby methods
  Object sorted = a.call("sort");
  Object sum = a.call("sum");

  // Convert to C++
  std::vector<int> vec = a.to_vector<int>();

  // Push/pop
  a.push(9, false);
  Object popped = a.pop();  // 9

  // Join
  Array strings;
  strings.push("a", false);
  strings.push("b", false);
  String joined = strings.join("-");  // "a-b"
}
```
