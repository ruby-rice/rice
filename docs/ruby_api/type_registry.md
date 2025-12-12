# TypeRegistry

```ruby
Rice::TypeRegistry
```

`Rice::TypeRegistry` provides access to all C++ types that have been wrapped by Rice and their mappings to Ruby classes. It is accessed through [Registries](registries.md).

---

## Instance Methods

### klasses → Array

Returns the list of Ruby classes defined by the extension.

**Returns:**

An Array of Ruby Class objects that wrap C++ types.

```ruby
types = Rice::Registries.instance.types
types.klasses.each do |klass|
  puts klass.name
end
```

---

## Example

```ruby
# Get all wrapped classes
type_registry = Rice::Registries.instance.types

puts "Classes defined by extension:"
type_registry.klasses.each do |klass|
  puts "  #{klass.name}"

  # Get ancestors to see inheritance
  ancestors = klass.ancestors.take_while { |a| a != Object }
  if ancestors.length > 1
    puts "    inherits from: #{ancestors[1..-1].map(&:name).join(' < ')}"
  end
end

# Use with native registry
type_registry.klasses.each do |klass|
  methods = Rice::Registries.instance.natives.native_methods(klass)
  puts "\n#{klass.name} methods:"
  methods.each do |method|
    puts "  #{method.name}"
  end
end
```

---

## See Also

* [Registries](registries.md) - Entry point for accessing registries
* [NativeRegistry](native_registry.md) - Registry of wrapped C++ functions and methods
