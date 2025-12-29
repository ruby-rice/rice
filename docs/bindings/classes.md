# Classes

Rice provides two ways to define Ruby classes: creating new Ruby-only classes and wrapping existing C++ classes.

## Ruby-Only Classes

To create a Ruby class without wrapping a C++ type, use the non-template version of `define_class`:

```cpp
#include <rice/rice.hpp>

using namespace Rice;

extern "C"
void Init_test()
{
  Class rb_cTest = define_class("Test");
}
```

This creates a Ruby class called `Test` that inherits from `Object`. You can then add methods using `define_method` with function pointers or lambdas.

## Wrapping C++ Classes

To wrap an existing C++ class, use the template version of `define_class<T>()`:

```cpp
class MyClass
{
public:
  MyClass();
  void do_something();
};

extern "C"
void Init_test()
{
  Data_Type<MyClass> rb_cMyClass =
    define_class<MyClass>("MyClass")
    .define_constructor(Constructor<MyClass>())
    .define_method("do_something", &MyClass::do_something);
}
```

The template parameter specifies the C++ type being wrapped. This creates a binding between the Ruby class and the C++ class, allowing Rice to automatically manage object lifetimes and method dispatch.

### Data_Type vs Class

When wrapping C++ types, `define_class<T>()` returns a `Data_Type<T>` instead of a `Class`. The `Data_Type<T>` class provides the same interface as `Class` but includes additional type information that Rice uses for type conversions and safety checks.

## Defining Classes Under Modules

To define a class within a module namespace, use `define_class_under`:

```cpp
Module rb_mMyModule = define_module("MyModule");

Data_Type<MyClass> rb_cMyClass =
  define_class_under<MyClass>(rb_mMyModule, "MyClass");
```

This creates `MyModule::MyClass` in Ruby. You can nest classes arbitrarily deep:

```cpp
Module rb_mOuter = define_module("Outer");
Module rb_mInner = define_module_under(rb_mOuter, "Inner");

Data_Type<MyClass> rb_cMyClass =
  define_class_under<MyClass>(rb_mInner, "MyClass");
```

This creates `Outer::Inner::MyClass`.

## Inheritance

### Inheriting from Ruby Classes

For Ruby-only classes, specify a parent class as the second parameter:

```cpp
Class rb_cMySocket = define_class("MySocket", rb_cIO);
```

### Inheriting from C++ Classes

When wrapping C++ class hierarchies, specify the parent C++ type as a second template parameter:

```cpp
class Base
{
public:
  virtual void base_method();
};

class Derived : public Base
{
public:
  void derived_method();
};

extern "C"
void Init_test()
{
  Data_Type<Base> rb_cBase =
    define_class<Base>("Base")
    .define_constructor(Constructor<Base>())
    .define_method("base_method", &Base::base_method);

  Data_Type<Derived> rb_cDerived =
    define_class<Derived, Base>("Derived")
    .define_constructor(Constructor<Derived>())
    .define_method("derived_method", &Derived::derived_method);
}
```

The second template parameter tells Rice about the inheritance relationship, enabling proper type conversions and polymorphic behavior.

If you want to create Ruby classes that inherit from wrapped C++ classes and override virtual methods, see the [Directors](directors.md) section.

## Method Chaining

Most methods on `Class` and `Data_Type` return a reference to self, allowing you to chain method calls:

```cpp
Data_Type<MyClass> rb_cMyClass =
  define_class<MyClass>("MyClass")
  .define_constructor(Constructor<MyClass>())
  .define_method("method1", &MyClass::method1)
  .define_method("method2", &MyClass::method2)
  .define_attr("value", &MyClass::value)
  .define_singleton_function("create", &MyClass::create);
```

## Naming Conventions

When naming your C++ variables that hold Ruby classes, follow Ruby's conventions:

| Prefix | Type | Example |
|--------|------|---------|
| `rb_c` | Class | `rb_cMyClass` |
| `rb_m` | Module | `rb_mMyModule` |
| `rb_e` | Exception | `rb_eMyError` |

## Complete Example

Here's a complete example showing various class definition features:

```cpp
#include <rice/rice.hpp>
#include <rice/stl.hpp>

using namespace Rice;

class Shape
{
public:
  virtual ~Shape() = default;
  virtual double area() const = 0;
};

class Rectangle : public Shape
{
public:
  Rectangle(double width, double height)
    : width_(width), height_(height) {}

  double area() const override { return width_ * height_; }

  double width() const { return width_; }
  double height() const { return height_; }

private:
  double width_;
  double height_;
};

extern "C"
void Init_shapes()
{
  Module rb_mGeometry = define_module("Geometry");

  Data_Type<Shape> rb_cShape =
    define_class_under<Shape>(rb_mGeometry, "Shape")
    .define_method("area", &Shape::area);

  Data_Type<Rectangle> rb_cRectangle =
    define_class_under<Rectangle, Shape>(rb_mGeometry, "Rectangle")
    .define_constructor(Constructor<Rectangle, double, double>())
    .define_method("width", &Rectangle::width)
    .define_method("height", &Rectangle::height);
}
```

Usage in Ruby:

```ruby
rect = Geometry::Rectangle.new(3.0, 4.0)
puts rect.area    # => 12.0
puts rect.width   # => 3.0
puts rect.height  # => 4.0
```
