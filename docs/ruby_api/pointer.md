# Pointer

```ruby
Rice::Pointer<T>
```

`Rice::Pointer<T>` wraps C++ pointers, allowing them to be passed between Ruby and C++ code. Pointers are lightweight wrappers that simply hold a memory address without size or ownership information.

Rice automatically generates `Pointer<T>` classes when it detects C++ APIs that use pointers to fundamental types or arrays of objects. For a pointer to `int*`, Rice creates `Rice::Pointer<int>`.

See [Pointers](../bindings/pointers.md) for usage examples.

---

## Public Instance Methods

### buffer → Buffer

Returns a `Buffer<T>` that can be used to read or manipulate the pointed-to memory.

**Returns:**

A `Buffer<T>` instance wrapping the pointer's memory.

**Description:**

Since Pointers are lightweight and carry no size information, use the returned Buffer to access the memory. You may need to specify the size when reading data.

```ruby
# Assume we received a pointer from a C++ API
pointer = some_cpp_method_returning_pointer()

# Get a buffer to read the data
buffer = pointer.buffer

# Read 10 elements
data = buffer.to_ary(10)
```

---

## Usage Examples

**Passing buffers to C++ APIs**

```ruby
# Create a buffer with data
data = Rice::Buffer<uint8_t>.new([255, 128, 64, 32])

# Get a pointer to pass to C++ API
pointer = data.data

# Pass to C++ method expecting uint8_t*
image = Image.new(2, 2, pointer)
```

**Reading from returned pointers**

```ruby
# C++ method returns a pointer
pointer = matrix.raw_data  # Returns Rice::Pointer<float>

# Convert to buffer to read values
buffer = pointer.buffer

# Read all 100 elements
values = buffer.to_ary(100)
```

---

## Memory Management

Pointer objects do not own or manage memory. They simply hold an address. Memory management is the responsibility of either:

* The `Buffer` that created the pointer (via `data`)
* The C++ code that allocated the memory
* The Ruby `Buffer` that received ownership (via `release`)

**Important:** Never use a Pointer after the memory it points to has been freed. This includes:

* After a Buffer goes out of scope and is garbage collected
* After C++ code frees the memory
* After passing the pointer to a C++ API that takes ownership

---

## Pointer vs Buffer

| Pointer | Buffer |
|---------|--------|
| Lightweight | Full-featured |
| No size info | Tracks size |
| No ownership | Can own memory |
| Pass to C++ | Create from Ruby, read/write elements |

Use **Pointer** when:

* Passing memory addresses to C++ APIs
* Receiving pointers from C++ APIs (then convert to Buffer to read)

Use **Buffer** when:

* Creating data in Ruby to pass to C++
* Reading or manipulating memory contents
* Managing memory lifetime

---

## See Also

* [Buffer](buffer.md) - Full-featured buffer class
* [Pointers](../bindings/pointers.md) - Pointer usage guide
