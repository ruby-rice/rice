# Struct

```cpp
#include <rice/rice.hpp>
```

`Rice::Struct` is a wrapper for creating Ruby Struct classes from C++. A Struct provides a convenient way to bundle a number of attributes together.

Note that the notation used here differs slightly from Ruby's notation:

In Ruby:

```ruby
MyStruct = Struct.new(:a, :b, :c)  # Creates a Class
instance = MyStruct.new(1, 2, 3)   # Creates an instance of that Class
```

In Rice C++:

```cpp
Struct rb_cMyStruct = define_struct()
  .define_member("a")
  .define_member("b")
  .define_member("c")
  .initialize(rb_mKernel, "MyStruct");

Struct::Instance instance = rb_cMyStruct.new_instance(Array({1, 2, 3}));
```

---

## Free Functions

### define_struct() -> Struct

Create a new Struct builder.

**Returns:**

A new Struct object ready for member definitions.

```cpp
Struct s = define_struct();
```

---

## Public Methods

### define_member(Identifier name) -> Struct&

Define a new member of the Struct.

**Parameters:**

* `name` - The member name.

**Returns:**

Reference to self for method chaining.

**Throws:**

`std::runtime_error` if called after the Struct is initialized.

**Note:**

Must be called before `initialize()`.

```cpp
Struct s = define_struct()
  .define_member("name")
  .define_member("age")
  .define_member("active");
```

---

### initialize(Module module, Identifier name) -> Struct&

Initialize the Struct class and register it under a module.

**Parameters:**

* `module` - The module under which to define the Struct class.
* `name` - The name of the Struct class in Ruby.

**Returns:**

Reference to self for method chaining.

**Note:**

Must be called after all members have been defined.

```cpp
Module rb_mMyModule = define_module("MyModule");

Struct s = define_struct()
  .define_member("x")
  .define_member("y")
  .initialize(rb_mMyModule, "Point");

// Ruby: MyModule::Point.new(10, 20)
```

---

### members() const -> Array

Get the members defined in the Struct.

**Returns:**

An Array of member names as Symbols.

```cpp
Struct s = define_struct()
  .define_member("a")
  .define_member("b")
  .initialize(rb_mKernel, "MyStruct");

Array m = s.members();  // [:a, :b]
```

---

### new_instance(Array args = Array()) const -> Instance

Create a new instance of the Struct.

**Parameters:**

* `args` - An Array of initial values for the members (default: empty).

**Returns:**

A new `Struct::Instance`.

```cpp
Struct s = define_struct()
  .define_member("name")
  .define_member("age")
  .initialize(rb_mKernel, "Person");

// Create with default (nil) values
Struct::Instance p1 = s.new_instance();

// Create with initial values
Array args;
args.push("Alice", false);
args.push(30, false);
Struct::Instance p2 = s.new_instance(args);
```

---

## Struct::Instance

`Struct::Instance` represents an instance of a Struct class.

### Constructors

#### Instance(Struct const& type, Array args = Array())

Create a new instance of a Struct.

**Parameters:**

* `type` - The Struct type to create.
* `args` - Initial values for the members.

```cpp
Struct::Instance instance(myStruct, Array({1, 2, 3}));
```

---

#### Instance(Struct const& type, Object s)

Encapsulate an existing Struct instance.

**Parameters:**

* `type` - The Struct type.
* `s` - An existing Struct instance Object.

```cpp
Object existing_instance = /* ... */;
Struct::Instance instance(myStruct, existing_instance);
```

---

### `operator[](T index)` -> Object

Get a member by index or name.

**Template Parameters:**

* `T` - The index type (integral, `Identifier`, or `char const*`).

**Parameters:**

* `index` - The member index (0-based) or name.

**Returns:**

The member value as an Object.

```cpp
Struct s = define_struct()
  .define_member("name")
  .define_member("age")
  .initialize(rb_mKernel, "Person");

Array args;
args.push("Alice", false);
args.push(30, false);
Struct::Instance p = s.new_instance(args);

// Access by index
Object name1 = p[0];  // "Alice"
Object age1 = p[1];   // 30

// Access by name (Identifier)
Object name2 = p[Identifier("name")];

// Access by name (C string)
Object age2 = p["age"];
```

---

## Inherited Methods

Struct inherits from [Class](class.md), which means it has access to:

* `define_method` - Add methods to the Struct class
* `define_singleton_method` - Add class methods
* `create` - Create instances

Struct::Instance inherits from `Builtin_Object<T_STRUCT>`, providing access to [Object](object.md) methods.

---

## Example

```cpp
#include <rice/rice.hpp>
#include <iostream>

using namespace Rice;

extern "C"
void Init_my_extension()
{
  Module rb_mMyModule = define_module("MyModule");

  // Define a Point struct with x and y coordinates
  Struct rb_cPoint = define_struct()
    .define_member("x")
    .define_member("y")
    .initialize(rb_mMyModule, "Point");

  // Add a custom method to the Struct
  rb_cPoint.define_method("distance", [](Object self) {
    Object x = self.call("x");
    Object y = self.call("y");
    double dx = detail::From_Ruby<double>().convert(x);
    double dy = detail::From_Ruby<double>().convert(y);
    return std::sqrt(dx*dx + dy*dy);
  });

  // Create instances from C++
  Array args;
  args.push(3.0, false);
  args.push(4.0, false);
  Struct::Instance point = rb_cPoint.new_instance(args);

  // Access members
  Object x = point["x"];  // 3.0
  Object y = point["y"];  // 4.0

  // Call the custom method
  Object distance = Object(point).call("distance");  // 5.0
}

// Ruby usage:
// p = MyModule::Point.new(3.0, 4.0)
// p.x        #=> 3.0
// p.y        #=> 4.0
// p.distance #=> 5.0
```

---

## See Also

* [Class](class.md) - Parent class
* [Object](object.md) - Base object methods
* [Array](array.md) - For passing arguments
