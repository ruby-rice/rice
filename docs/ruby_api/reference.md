# Reference

```ruby
Rice::Reference<T>
```

`Rice::Reference<T>` wraps C++ references to fundamental types, allowing them to be passed between Ruby and C++ code. References store a copy of the value internally and provide a C++ reference to it when passed to C++ methods expecting `T&` parameters.

Rice automatically generates `Reference<T>` classes when it detects C++ APIs that use references to fundamental types. For a method with an `int&` parameter, Rice creates `Rice::Reference<int>`.

---

## Purpose

References are needed, in addition to pointers, to disambiguate overloaded C++ methods. When C++ has overloaded methods like:

```cpp
void process(int* ptr);   // pointer overload
void process(int& ref);   // reference overload
```

Ruby code can explicitly choose which overload to call:

```ruby
# Call the pointer overload (int*)
buffer = Rice::Buffer<int>.new([42])
obj.process(buffer.ptr)

# Call the reference overload (int&)
ref = Rice::Reference<int>.new(42)
obj.process(ref)
```

Without `Reference<T>`, both would potentially match the reference parameter, making overload selection ambiguous.

---

## Creating References

References are created directly using the constructor:

```ruby
# Create a reference holding the value 100
ref = Rice::Reference<int>.new(100)

# Create a reference to a double
ref = Rice::Reference<double>.new(3.14159)
```

---

## Instance Methods

### `value`

Returns the current value stored in the reference.

```ruby
ref = Rice::Reference<int>.new(42)
ref.value  #=> 42
```

After passing the reference to a C++ method that modifies it:

```ruby
ref = Rice::Reference<int>.new(7)
cpp_object.update_value(ref)  # C++ sets ref to 42
ref.value  #=> 42
```

---

## Usage Examples

**Passing values by reference**

```ruby
# Create a reference with an initial value
ref = Rice::Reference<int>.new(7)

# C++ method modifies the value through the reference
updater.set_to_42(ref)

# Check the modified value
ref.value  #=> 42
```

**Disambiguating overloaded methods**

```ruby
# These call different C++ overloads:
buffer = Rice::Buffer<int>.new([42])
api.process(buffer.ptr)  # calls process(int*)

ref = Rice::Reference<int>.new(42)
api.process(ref)         # calls process(int&)
```

---

## Reference vs Pointer

| Reference | Pointer |
|-----------|---------|
| Matches `T&` parameters | Matches `T*` parameters |
| Stores a single value | Points to external memory |
| For reference overloads | For pointer overloads |

Use **Reference** when:

* Calling C++ methods that take `T&` parameters
* Need to pass a value that C++ can modify
* Disambiguating between pointer and reference overloads

Use **Pointer** when:

* Calling C++ methods that take `T*` parameters
* Passing arrays or buffers to C++ APIs

---

## See Also

* [Buffer](buffer.md) - Full-featured buffer class
* [Pointer](pointer.md) - Lightweight pointer wrapper for `T*`
