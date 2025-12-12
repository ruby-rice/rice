# Registries

Rice maintains several registries to track bindings at runtime. These enable type lookup, method resolution, and introspection.

## Registries Singleton

All registries are accessed through a singleton:

```cpp
class Registries
{
public:
  static Registries& instance();

  TypeRegistry& types();
  NativeRegistry& natives();
  InstanceRegistry& instances();
};
```

## TypeRegistry

Maps C++ types to Ruby classes.

```cpp
class TypeRegistry
{
public:
  // Register a type binding
  void add(std::type_index type, VALUE klass, rb_data_type_t* rbType);

  // Look up Ruby class for a C++ type
  std::pair<VALUE, rb_data_type_t*> lookup(std::type_index type);

  // Check if a type is registered
  bool isDefined(std::type_index type);
};
```

**Used when:**

- `To_Ruby<T>` needs to find the Ruby class to wrap an object
- `From_Ruby<T>` validates that a Ruby object is the correct type
- Introspection APIs list all wrapped classes

**Key design:**

- Uses `std::type_index` as the key (from `typeid(T)`)
- Stores both the Ruby class VALUE and the `rb_data_type_t` pointer
- Thread-safe for reads after initialization

## NativeRegistry

Stores all wrapped C++ functions, methods, and attributes.

```cpp
class NativeRegistry
{
public:
  // Register a native for a class/module
  void add(VALUE klass, std::shared_ptr<Native> native);

  // Get all natives for a method name
  std::vector<Native*> lookup(VALUE klass, std::string_view name);

  // Get all natives for a class (for introspection)
  std::vector<Native*> lookup(VALUE klass);
};
```

**Used when:**

- `Native::resolve()` finds overload candidates
- Introspection APIs enumerate methods for a class

**Key design:**

- Indexed by (klass, method_name) for fast lookup during calls
- Also indexed by klass alone for introspection
- Stores `shared_ptr<Native>` for shared ownership

## InstanceRegistry

Tracks which Ruby objects wrap which C++ instances.

```cpp
class InstanceRegistry
{
public:
  // Register a Ruby object wrapping a C++ pointer
  void add(void* ptr, VALUE ruby_object);

  // Find Ruby object for a C++ pointer
  std::optional<VALUE> lookup(void* ptr);

  // Remove mapping (when Ruby object is collected)
  void remove(void* ptr);
};
```

**Used when:**

- Returning a C++ object that's already wrapped (avoids double-wrapping)
- Looking up `self` for method calls

**Key design:**

- Uses raw `void*` as key (the C++ object address)
- Weak references to Ruby objects (doesn't prevent GC)
- Automatically cleaned up when Ruby objects are collected

## ModuleRegistry

Tracks defined modules for introspection.

```cpp
class ModuleRegistry
{
public:
  void add(VALUE module);
  std::vector<VALUE> modules();
};
```

**Used when:**

- Introspection APIs list all defined modules

## Lifecycle

### Initialization

Registries are populated during extension initialization:

```cpp
extern "C" void Init_my_extension()
{
  // Each define_class/define_method populates registries
  define_class<Foo>("Foo")
    .define_method("bar", &Foo::bar);
}
```

After `Init_my_extension` returns, registries should be treated as read-only.

### Thread Safety

- **Writes**: Only during initialization (single-threaded)
- **Reads**: Thread-safe (multiple threads can call methods)

Ruby's GVL ensures that method calls don't race with each other, but registries are also designed for lock-free reads.

### Garbage Collection

`InstanceRegistry` interacts with Ruby's GC:

- Entries use weak references
- When Ruby collects a wrapped object, its entry is removed
- The C++ destructor (via `Wrapper`) triggers cleanup

## Introspection API

The registries power Rice's Ruby introspection API:

```ruby
# Access via Rice::Registries singleton
registries = Rice::Registries.instance

# List all wrapped classes
registries.types.klasses.each { |k| puts k.name }

# Get methods for a class
registries.natives.native_methods(MyClass).each do |native|
  puts native.name
end
```

See [Ruby API](../ruby_api/overview.md) for the full Ruby API documentation.

## See Also

- [Registries](../ruby_api/registries.md) - Ruby API for Registries
- [TypeRegistry](../ruby_api/type_registry.md) - Ruby API for TypeRegistry
- [NativeRegistry](../ruby_api/native_registry.md) - Ruby API for NativeRegistry
