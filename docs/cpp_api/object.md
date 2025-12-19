# Object

```cpp
#include <rice/rice.hpp>
```

`Rice::Object` is the base class for all Rice wrapper classes. It wraps a Ruby `VALUE` and provides a C++-style interface to Ruby's object system.

---

## Constructors

### Object()

Construct a new Object wrapping `Qnil`.

```cpp
Object obj;  // wraps nil
```

---

### Object(VALUE value)

Wrap an existing Ruby VALUE.

**Parameters:**

* `value` - A Ruby VALUE to wrap.

```cpp
VALUE ruby_str = rb_str_new_cstr("hello");
Object obj(ruby_str);
```

---

## Constants

The following constants are provided for convenience:

```cpp
Rice::Nil    // wraps Qnil
Rice::True   // wraps Qtrue
Rice::False  // wraps Qfalse
Rice::Undef  // wraps Qundef
```

---

## Public Methods

### value() const → VALUE

Get the underlying Ruby VALUE.

**Returns:**

The wrapped VALUE.

```cpp
Object obj(some_value);
VALUE v = obj.value();
```

---

### test() const → bool

Test if the object is truthy.

**Returns:**

`false` if the object is `nil` or `false`; `true` otherwise.

```cpp
Object obj(Qtrue);
if (obj.test()) {
  // ...
}
```

---

### operator bool() const

Implicit conversion to bool. Same as `test()`.

```cpp
Object obj(some_value);
if (obj) {
  // object is truthy
}
```

---

### is_nil() const → bool

Check if the object is nil.

**Returns:**

`true` if the object is nil, `false` otherwise.

```cpp
Object obj;
if (obj.is_nil()) {
  // object is nil
}
```

---

### class_of() const → Class

Get the object's class.

**Returns:**

The object's class as a `Rice::Class`.

```cpp
Object obj(rb_str_new_cstr("hello"));
Class klass = obj.class_of();  // String class
```

---

### class_name() const → String

Get the name of the object's class.

**Returns:**

The class name as a `Rice::String`.

```cpp
Object obj(rb_str_new_cstr("hello"));
std::string name = obj.class_name().str();  // "String"
```

---

### to_s() const → String

Get a string representation of the object.

**Returns:**

The result of calling `to_s` on the Ruby object.

```cpp
Object obj(rb_fix_new(42));
std::string str = obj.to_s().str();  // "42"
```

---

### inspect() const → String

Get a detailed string representation of the object.

**Returns:**

The result of calling `inspect` on the Ruby object.

```cpp
Object obj(rb_str_new_cstr("hello"));
std::string str = obj.inspect().str();  // "\"hello\""
```

---

### rb_type() const → int

Get the Ruby type of the underlying VALUE.

**Returns:**

One of Ruby's type constants (e.g., `T_STRING`, `T_ARRAY`, `T_OBJECT`).

```cpp
Object obj(rb_str_new_cstr("hello"));
if (obj.rb_type() == T_STRING) {
  // it's a string
}
```

---

### object_id() const → VALUE

Get the object's unique identifier.

**Returns:**

The result of calling `object_id` on the Ruby object.

```cpp
Object obj(some_value);
VALUE id = obj.object_id();
```

---

### is_a(Object klass) const → bool

Check if the object is an instance of a class or its descendants.

**Parameters:**

* `klass` - A class or module to check against.

**Returns:**

`true` if the object is an instance of the class or any subclass.

```cpp
Object obj(rb_str_new_cstr("hello"));
if (obj.is_a(rb_cString)) {
  // obj is a String
}
```

---

### is_instance_of(Object klass) const → bool

Check if the object is a direct instance of a class.

**Parameters:**

* `klass` - A class to check against.

**Returns:**

`true` if the object is a direct instance of the class (not a subclass).

```cpp
Object obj(some_value);
if (obj.is_instance_of(rb_cString)) {
  // obj is exactly a String, not a subclass
}
```

---

### respond_to(Identifier id) const → bool

Check if the object responds to a method.

**Parameters:**

* `id` - The method name.

**Returns:**

`true` if the object responds to the method.

```cpp
Object obj(some_value);
if (obj.respond_to("to_s")) {
  // obj can be converted to string
}
```

---

### is_equal(const Object& other) const → bool

Check if two objects are the same object (`equal?`).

**Parameters:**

* `other` - Another object to compare.

**Returns:**

`true` if the objects are the same object.

---

### is_eql(const Object& other) const → bool

Check if two objects are equivalent (`eql?`).

**Parameters:**

* `other` - Another object to compare.

**Returns:**

`true` if the objects are equivalent.

---

### compare(Object const& other) const → int

Compare this object to another using `<=>`.

**Parameters:**

* `other` - Another object to compare.

**Returns:**

Negative if `self < other`, zero if equal, positive if `self > other`.

```cpp
Object a(rb_fix_new(1));
Object b(rb_fix_new(2));
int result = a.compare(b);  // negative
```

---

### freeze()

Freeze the object, preventing further modifications.

```cpp
Object obj(rb_str_new_cstr("hello"));
obj.freeze();
```

---

### is_frozen() const → bool

Check if the object is frozen.

**Returns:**

`true` if the object is frozen.

---

### instance_eval(String const& s) → Object

Evaluate a string in the context of the object.

**Parameters:**

* `s` - A string containing Ruby code.

**Returns:**

The result of evaluating the code.

```cpp
Object obj(some_value);
Object result = obj.instance_eval("self.class.name");
```

---

### call(Identifier id, args...) const → Object

Call a method on the object.

**Parameters:**

* `id` - The method name.
* `args...` - Arguments to pass to the method (automatically converted to Ruby).

**Returns:**

The return value of the method call.

```cpp
Object str(rb_str_new_cstr("hello"));
Object result = str.call("upcase");  // "HELLO"

Object arr(rb_ary_new());
arr.call("push", 1, 2, 3);
```

---

### call_kw(Identifier id, args...) const → Object

Call a method with keyword arguments.

**Parameters:**

* `id` - The method name.
* `args...` - Arguments where the last argument is a Hash of keyword arguments.

**Returns:**

The return value of the method call.

```cpp
Hash kw;
kw["encoding"] = String("UTF-8");
Object result = obj.call_kw("some_method", kw);
```

---

### vcall(Identifier id, Array args) → Object

Call a method with arguments from an Array.

**Parameters:**

* `id` - The method name.
* `args` - An Array of arguments.

**Returns:**

The return value of the method call.

---

## Instance Variables

### iv_set(Identifier name, T const& value)

Set an instance variable.

**Parameters:**

* `name` - The variable name (including `@`).
* `value` - The value to set (automatically converted to Ruby).

```cpp
Object obj(some_value);
obj.iv_set("@my_var", 42);
```

---

### iv_get(Identifier name) const → Object

Get an instance variable.

**Parameters:**

* `name` - The variable name (including `@`).

**Returns:**

The value of the instance variable.

```cpp
Object value = obj.iv_get("@my_var");
```

---

### attr_get(Identifier name) const → Object

Get an instance variable without warning if unset.

**Parameters:**

* `name` - The variable name.

**Returns:**

The value of the instance variable, or nil if not set.

---

## Constants

### const_get(Identifier name) const → Object

Get a constant.

**Parameters:**

* `name` - The constant name.

**Returns:**

The value of the constant.

```cpp
Module kernel(rb_mKernel);
Object version = kernel.const_get("RUBY_VERSION");
```

---

### const_defined(Identifier name) const → bool

Check if a constant is defined.

**Parameters:**

* `name` - The constant name.

**Returns:**

`true` if the constant is defined.

---

### const_set(Identifier name, Object value) → Object

Set a constant.

**Parameters:**

* `name` - The constant name.
* `value` - The value to set.

**Returns:**

The value.

---

### const_set_maybe(Identifier name, Object value) → Object

Set a constant only if not already defined.

**Parameters:**

* `name` - The constant name.
* `value` - The value to set.

**Returns:**

The value.

---

### remove_const(Identifier name)

Remove a constant.

**Parameters:**

* `name` - The constant name to remove.

---

## Operators

### operator==(Object const& lhs, Object const& rhs) → bool

Test equality using Ruby's `==` operator.

```cpp
if (obj1 == obj2) {
  // objects are equal
}
```

---

### operator!=(Object const& lhs, Object const& rhs) → bool

Test inequality.

---

### operator<(Object const& lhs, Object const& rhs) → bool

Test if lhs is less than rhs using Ruby's `<` operator.

---

### operator>(Object const& lhs, Object const& rhs) → bool

Test if lhs is greater than rhs using Ruby's `>` operator.

---

### operator<<(std::ostream& out, Object const& obj) → std::ostream&

Output the object to a stream (calls `to_s`).

```cpp
Object obj(rb_fix_new(42));
std::cout << obj << std::endl;  // prints "42"
```
