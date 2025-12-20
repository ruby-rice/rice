# Method Binding

Method binding connects C++ functions and methods to Ruby. This is handled by the `Native` class hierarchy.

## Native Hierarchy

The `Native` base class defines the interface for all bound callables:

```
Native (abstract base)
+-- NativeFunction      - Module/class functions (define_function)
+-- NativeMethod        - Instance methods (define_method)
+-- NativeIterator      - Iterator methods (define_iterator)
+-- NativeAttributeGet  - Attribute readers (define_attr)
+-- NativeAttributeSet  - Attribute writers (define_attr)
+-- NativeProc          - Ruby blocks as C++ callables
+-- NativeCallback      - C-style function pointer callbacks
```

### Native Base Class

```cpp
class Native
{
public:
  // Find the best matching overload and invoke it
  static VALUE resolve(int argc, VALUE* argv, VALUE self);

  // Check if this native matches the given arguments
  virtual Resolved matches(std::map<std::string, VALUE>& values);

  // Invoke the C++ callable
  virtual VALUE operator()(std::map<std::string, VALUE>& values, VALUE self) = 0;

protected:
  std::string name_;
  std::unique_ptr<Return> returnInfo_;
  std::vector<std::unique_ptr<ParameterAbstract>> parameters_;
};
```

Each `Native` subclass:

1. Stores the C++ callable (function pointer, method pointer, or lambda)
2. Stores parameter metadata (names, types, defaults)
3. Implements `matches()` for overload resolution
4. Implements `operator()` to invoke the C++ code

## Method Resolution

When Ruby calls a wrapped method, `Native::resolve()` finds and invokes the correct C++ overload:

1. **Lookup**: Retrieve all `Native` instances registered for this method name from `NativeRegistry`

2. **Match**: For each candidate, call `matches()` to check argument compatibility

3. **Rank**: Score matches by conversion quality using `Convertible` enum

4. **Select**: Pick the candidate with the highest score

5. **Invoke**: Call the winner's `operator()`

### Overload Resolution Example

Given these overloads:

```cpp
void process(int x);           // Overload 1
void process(double x);        // Overload 2
void process(const string& s); // Overload 3
```

When Ruby calls `process(42)`:

- Overload 1: `Exact` match (Ruby Integer -> C++ int)
- Overload 2: `Narrow` match (Ruby Integer -> C++ double)
- Overload 3: `None` (Ruby Integer cannot convert to string)

Overload 1 wins with the `Exact` match.

## Invocation Flow

When a `Native` is invoked:

```cpp
VALUE NativeMethod<T, Func>::operator()(std::map<std::string, VALUE>& values, VALUE self)
{
  // 1. Get the C++ receiver object
  T* receiver = From_Ruby<T*>().convert(self);

  // 2. Convert Ruby arguments to C++ types
  auto args = convertArguments(argc, argv);

  // 3. Call the C++ method
  auto result = std::apply(
    [&](auto&&... args) {
      return (receiver->*method_)(std::forward<decltype(args)>(args)...);
    },
    args
  );

  // 4. Convert result to Ruby
  return To_Ruby<ReturnType>().convert(result);
}
```

## Parameter Handling

Each parameter is described by a `Parameter` object containing:

- **Type information**: Used for conversion and overload resolution
- **Arg metadata**: Name, default value, whether it's optional

### Default Arguments

Default arguments are stored in `Arg` objects:

```cpp
define_method("greet", &Person::greet,
  Arg("name"),
  Arg("greeting") = "Hello");  // Default value
```

When Ruby omits an argument, Rice uses the stored default.

### Keyword Arguments

Rice supports Ruby keyword arguments:

```cpp
define_method("configure", &Config::set,
  Arg("timeout").setKeyword(),
  Arg("retries").setKeyword() = 3);
```

Ruby can then call:

```ruby
obj.configure(timeout: 30, retries: 5)
```

## Registration

When you call `define_method`, Rice:

1. Creates a `NativeMethod` instance storing the method pointer
2. Registers it in `NativeRegistry` under the method name
3. Defines a Ruby method that calls `Native::resolve`

```cpp
template<typename Method_T>
void define_method(const char* name, Method_T method)
{
  // Create native wrapper
  auto native = std::make_unique<NativeMethod<T, Method_T>>(name, method);

  // Register in NativeRegistry
  detail::Registries::instance().natives().add(klass_, std::move(native));

  // Define Ruby method pointing to resolve()
  rb_define_method(klass_, name, &Native::resolve, -1);
}
```

The `-1` arity tells Ruby to pass all arguments to `resolve()`, which then handles overload resolution.

## See Also

- [Types Overview](../types/overview.md) - How arguments are converted
- [Registries](registries.md) - Where natives are stored
- [Methods](../bindings/methods.md) - User guide for defining methods
- [Overloaded Methods](../bindings/overloaded_methods.md) - User guide for overloading
