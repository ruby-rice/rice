# Buffer

```ruby
Rice::Buffer<T>
```

`Rice::Buffer<T>` manages contiguous blocks of memory containing C++ values. Buffers enable Ruby code to create, read, and manipulate memory that can be passed to C++ APIs expecting pointers or arrays.

Rice automatically generates `Buffer<T>` classes for fundamental types (`int`, `double`, etc.) when it detects C++ APIs that use pointers. For wrapped types, you may need to use `ArgBuffer` to tell Rice to generate the appropriate Buffer class.

See [Buffers](../bindings/buffers.md) for usage examples.

---

## Public Class Methods

### new(value) → Buffer

Creates a new buffer from a Ruby value.

**Parameters:**

* `value` - A Ruby Array, String, or single value to convert to a buffer.

**Returns:**

A new Buffer instance that owns the allocated memory.

**Examples:**

```ruby
# From an array of integers
buffer = Rice::Buffer<int>.new([1, 2, 3, 4, 5])

# From a string (for char buffers)
buffer = Rice::Buffer<char>.new("hello")

# From a single value
buffer = Rice::Buffer<double>.new(3.14)
```

---

### new(value, size) → Buffer

Creates a new buffer from a Ruby value with an explicit size.

**Parameters:**

* `value` - A Ruby Array, String, Pointer, or single value.
* `size` - The number of elements in the buffer.

**Returns:**

A new Buffer instance.

**Description:**

The `size` parameter is primarily used when creating a buffer from a `Pointer`, since pointers do not carry size information.

**Examples:**

```ruby
# Create buffer from pointer with known size
buffer = Rice::Buffer<int>.new(pointer, 10)
```

---

## Public Instance Methods

### size → Integer

Returns the number of elements in the buffer.

**Returns:**

The number of elements (not bytes) in the buffer.

**Examples:**

```ruby
buffer = Rice::Buffer<int>.new([1, 2, 3, 4, 5])
buffer.size  #=> 5
```

---

### to_s → String

Returns a human-readable description of the buffer.

**Returns:**

A string describing the buffer's type and size.

**Examples:**

```ruby
buffer = Rice::Buffer<int>.new([1, 2, 3])
buffer.to_s  #=> "Buffer<type: int*, size: 3>"
```

---

### bytes → String

Returns the raw bytes of the buffer as a binary-encoded Ruby string.

**Returns:**

A String with binary encoding containing the buffer's raw memory. Returns `nil` if the buffer is empty.

**Description:**

The size of the returned string is `size * sizeof(T)` bytes. The buffer's `size` must be set for this method to work correctly.

**Examples:**

```ruby
buffer = Rice::Buffer<int>.new([1, 2, 3])
buffer.bytes  #=> "\x01\x00\x00\x00\x02\x00\x00\x00\x03\x00\x00\x00"
```

---

### bytes(count) → String

Returns the raw bytes for the first `count` elements.

**Parameters:**

* `count` - The number of elements to include.

**Returns:**

A String with binary encoding containing `count * sizeof(T)` bytes.

**Examples:**

```ruby
buffer = Rice::Buffer<int>.new([1, 2, 3, 4, 5])
buffer.bytes(2)  #=> "\x01\x00\x00\x00\x02\x00\x00\x00"
```

---

### to_ary → Array

Converts the buffer to a Ruby Array.

**Returns:**

An Array containing the buffer's elements converted to Ruby objects. Returns `nil` if the buffer is empty.

**Description:**

The buffer's `size` must be set for this method to work correctly.

**Examples:**

```ruby
buffer = Rice::Buffer<int>.new([1, 2, 3])
buffer.to_ary  #=> [1, 2, 3]
```

---

### to_ary(count) → Array

Converts the first `count` elements to a Ruby Array.

**Parameters:**

* `count` - The number of elements to include.

**Returns:**

An Array containing `count` elements.

**Examples:**

```ruby
buffer = Rice::Buffer<int>.new([1, 2, 3, 4, 5])
buffer.to_ary(3)  #=> [1, 2, 3]
```

---

### [](index) → Object

Returns the element at the specified index.

**Parameters:**

* `index` - The zero-based index of the element.

**Returns:**

The element at the given index.

**Raises:**

`IndexError` if the index is out of bounds.

**Examples:**

```ruby
buffer = Rice::Buffer<int>.new([10, 20, 30])
buffer[0]  #=> 10
buffer[2]  #=> 30
```

---

### []=(index, value) → value

Sets the element at the specified index.

**Parameters:**

* `index` - The zero-based index of the element.
* `value` - The new value.

**Returns:**

The assigned value.

**Raises:**

`IndexError` if the index is out of bounds.

**Description:**

This method is only available for non-const, copy-assignable types.

**Examples:**

```ruby
buffer = Rice::Buffer<int>.new([10, 20, 30])
buffer[1] = 25
buffer[1]  #=> 25
```

---

### data → Pointer

Returns a `Pointer<T>` to the buffer's memory.

**Returns:**

A `Pointer<T>` instance pointing to the buffer's internal memory.

**Description:**

Use this method to pass the buffer to C++ APIs that expect pointers. The buffer retains ownership of the memory.

**Examples:**

```ruby
buffer = Rice::Buffer<uint8_t>.new([1, 2, 3, 4])
matrix = Matrix.new(2, 2, buffer.data)
```

---

### release → Pointer

Returns a `Pointer<T>` and releases ownership of the memory.

**Returns:**

A `Pointer<T>` instance pointing to the buffer's internal memory.

**Description:**

After calling `release`, the buffer no longer owns the memory and will not free it when garbage collected. Use this when passing memory to C++ APIs that take ownership.

**Examples:**

```ruby
buffer = Rice::Buffer<uint8_t>.new([1, 2, 3, 4])
# C++ API takes ownership of the memory
matrix = Matrix.new(2, 2, buffer.release)
```

---

## Memory Management

Buffers manage their own memory by default. When a Buffer is created from a Ruby Array or String, it allocates new memory and copies the data. When the Buffer is garbage collected, it frees this memory.

When a Buffer is created from a `Pointer`, it does not own the memory and will not free it.

Use `release` to transfer ownership of the memory to a C++ API. After calling `release`, the buffer will not free the memory when garbage collected.

**Important:** Ensure that either the Buffer or the C++ code owns the memory, but not both. Double-free errors will occur if both attempt to free the same memory.

---

## See Also

* [Pointer](pointer.md) - Lightweight pointer wrapper
* [Buffers](../bindings/buffers.md) - Buffer usage guide
