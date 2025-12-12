# Class

```cpp
#include <rice/rice.hpp>
```

`Rice::Class` wraps Ruby's Class and provides methods for defining classes. It inherits from [Module](module.md) and therefore [Object](object.md).

---

## Constructors

### Class()

Construct a Class wrapping `rb_cObject`.

```cpp
Class c;  // wraps Object class
```

---

### Class(VALUE v)

Wrap an existing Ruby class VALUE.

**Parameters:**

* `v` - A Ruby VALUE of type `T_CLASS`.

```cpp
Class string_class(rb_cString);
```

---

## Free Functions

### define_class(char const* name, const Class& superclass = rb_cObject) → Class

Define a new class in the global namespace.

**Parameters:**

* `name` - The class name.
* `superclass` - The parent class (default: Object).

**Returns:**

The new or existing Class.

```cpp
Class rb_cAnimal = define_class("Animal");
Class rb_cDog = define_class("Dog", rb_cAnimal);
```

---

### define_class_under(Object parent, char const* name, const Class& superclass = rb_cObject) → Class

Define a new class under a parent module or class.

**Parameters:**

* `parent` - The parent module or class.
* `name` - The class name.
* `superclass` - The parent class (default: Object).

**Returns:**

The new or existing Class.

```cpp
Module rb_mMyModule = define_module("MyModule");
Class rb_cMyClass = define_class_under(rb_mMyModule, "MyClass");
// Creates MyModule::MyClass
```

---

### anonymous_class() → Class

Create a new anonymous class.

**Returns:**

A new anonymous Class.

```cpp
Class anon = anonymous_class();
```

---

## Public Methods

### name() const → std::string

Get the fully-qualified class name.

**Returns:**

The class name including parent modules.

```cpp
Module m = define_module("MyModule");
Class c = define_class_under(m, "MyClass");
std::string name = c.name();  // "MyModule::MyClass"
```

---

### base_name() const → std::string

Get the class name without parent modules.

**Returns:**

Just the class name.

```cpp
Module m = define_module("MyModule");
Class c = define_class_under(m, "MyClass");
std::string name = c.base_name();  // "MyClass"
```

---

### create(args...) → Object

Create a new instance of the class.

**Parameters:**

* `args...` - Arguments to pass to the constructor.

**Returns:**

A new instance of the class.

```cpp
Class rb_cString(rb_cString);
Object str = rb_cString.create("hello");
```

---

### undef_creation_funcs() → Class&

Prevent instances from being created from Ruby.

**Returns:**

Reference to self for method chaining.

**Description:**

Undefines `allocate` and `initialize`, preventing `new` from working.

```cpp
Class c = define_class("Singleton")
  .undef_creation_funcs();

// Ruby: Singleton.new  #=> raises NoMethodError
```

---

## Inherited Methods

Class inherits all methods from [Module](module.md), including:

* `define_method`
* `define_function`
* `define_singleton_method`
* `define_singleton_function`
* `define_constant`
* `include_module`

See [Module](module.md) for documentation of these methods.

---

## Example

```cpp
#include <rice/rice.hpp>

using namespace Rice;

class Rectangle {
public:
  Rectangle(int w, int h) : width_(w), height_(h) {}
  int area() const { return width_ * height_; }
  int width() const { return width_; }
  int height() const { return height_; }
private:
  int width_, height_;
};

extern "C"
void Init_shapes()
{
  Class rb_cRectangle = define_class<Rectangle>("Rectangle")
    .define_constructor(Constructor<Rectangle, int, int>())
    .define_method("area", &Rectangle::area)
    .define_method("width", &Rectangle::width)
    .define_method("height", &Rectangle::height);
}

// Ruby usage:
// rect = Rectangle.new(3, 4)
// rect.area    #=> 12
// rect.width   #=> 3
// rect.height  #=> 4
```

---

## See Also

* [Module](module.md) - Parent class with method definition helpers
* `Data_Type<T>` - For wrapping C++ classes with full type support
