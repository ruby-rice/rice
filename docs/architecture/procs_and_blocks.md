# Procs and Blocks

Rice provides bidirectional support for Ruby procs/blocks and C++ function pointers. This enables two key use cases:

1. **C++ to Ruby**: Wrapping C++ function pointers as Ruby procs
2. **Ruby to C++**: Passing Ruby blocks/procs to C++ code

## C++ Function Pointers to Ruby Procs

When a C++ function returns a function pointer, Rice automatically wraps it as a Ruby `Proc`.

### Example

```cpp
int square(int i)
{
  return i * i;
}

auto getSquareFunction()
{
  return square;  // Returns function pointer
}

// Expose to Ruby
define_module_function("square_proc", getSquareFunction);
```

Ruby code can then call:

```ruby
proc = square_proc
proc.call(9)  # => 81
```

### How It Works

The `To_Ruby` specialization for function pointers uses `NativeProc` to create the wrapper:

```cpp
template<typename Return_T, typename ...Parameter_Ts>
class To_Ruby<Return_T(*)(Parameter_Ts...)>
{
  VALUE convert(Proc_T proc)
  {
    return NativeProc<Proc_T>::createRubyProc(proc);
  }
};
```

`NativeProc::createRubyProc`:

1. Creates a `NativeProc` instance storing the C++ function pointer
2. Wraps it in a Ruby proc via `rb_proc_new`
3. Attaches a finalizer to clean up the `NativeProc` when the Ruby proc is garbage collected

When Ruby invokes the proc, `NativeProc::resolve` is called which:

1. Converts Ruby arguments to C++ types
2. Calls the stored function pointer
3. Converts the return value back to Ruby

## Ruby Blocks and Procs to C++

Rice *always* converts Ruby blocks to procs. When Ruby passes a block to a method, Rice captures it via `rb_block_proc` and passes it as the last argument:

```cpp
// In Native.ipp
if (protect(rb_block_given_p))
{
  std::string key = "arg_" + std::to_string(result.size());
  result[key] = protect(rb_block_proc);
}
```

To receive a block in your C++ method, you must:

1. Include a `VALUE` parameter at the end of your function signature
2. Mark that parameter with `Arg("name").setValue()`

If you omit `setValue()`, Rice will attempt to convert the proc using `From_Ruby`, resulting in an error like "Proc cannot be converted to Integer".

### Invoking Procs in C++

Use Rice's `Object` class to invoke the proc. This works uniformly whether Ruby passed a block or an explicit proc:

```cpp
int squareWithProc(int i, VALUE proc)
{
  Object result = Object(proc).call("call", i);
  return From_Ruby<int>().convert(result);
}

define_module_function("square_with_proc", squareWithProc,
  Arg("i"), Arg("proc").setValue());
```

This can be called from Ruby with a block:

```ruby
square_with_proc(7) { |i| i * i }  # => 49
```

Or with an explicit proc:

```ruby
proc = Proc.new { |i| i * i }
square_with_proc(4, proc)  # => 16
```

## Callbacks

For more complex callback scenarios where C++ code stores and later invokes Ruby procs (such as event handlers or async callbacks), see the [Callbacks](../bindings/callbacks.md) documentation. This covers:

- C-style callback registration
- LibFFI closures for multiple callbacks of the same type
- Passing user data through callbacks

## See Also

- [Callbacks](../bindings/callbacks.md) - C-style callback support
- [Method Binding](method_binding.md) - Native hierarchy including NativeProc and NativeCallback
