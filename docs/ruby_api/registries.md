# Registries

```ruby
Rice::Registries
```

`Rice::Registries` is the entry point to the introspection API. It is a singleton class that provides access to Rice's internal registries for type and native method information.

---

## Class Methods

### instance → Registries

Returns the singleton instance of `Rice::Registries`.

**Returns:**

The singleton `Registries` instance.

```ruby
registries = Rice::Registries.instance
```

---

## Instance Methods

### natives → NativeRegistry

Returns a reference to the native registry.

**Returns:**

A [NativeRegistry](native_registry.md) instance containing all wrapped C++ functions, methods, and attributes.

```ruby
native_registry = Rice::Registries.instance.natives
native_registry.native_methods(MyClass)
```

---

### types → TypeRegistry

Returns a reference to the type registry.

**Returns:**

A [TypeRegistry](type_registry.md) instance containing all wrapped C++ types.

```ruby
type_registry = Rice::Registries.instance.types
type_registry.klasses.each { |klass| puts klass.name }
```

---

## Example

```ruby
# Get the registries singleton
registries = Rice::Registries.instance

# List all wrapped classes
puts "Wrapped classes:"
registries.types.klasses.each do |klass|
  puts "  #{klass.name}"
end

# Get native methods for a specific class
puts "\nMethods for MyClass:"
registries.natives.native_methods(MyClass).each do |native|
  params = native.parameters.map { |p| "#{p.arg.name}: #{p.klass}" }.join(", ")
  puts "  #{native.name}(#{params}) -> #{native.return_type}"
end
```

---

## See Also

* [TypeRegistry](type_registry.md) - Registry of wrapped C++ types
* [NativeRegistry](native_registry.md) - Registry of wrapped C++ functions and methods
* [Enabling the Introspection API](overview.md#enabling-the-introspection-api) - How to enable the introspection API
