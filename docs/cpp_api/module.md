# Module

```cpp
#include <rice/rice.hpp>
```

`Rice::Module` wraps Ruby's Module class and provides methods for defining modules and their methods. It inherits from [Object](object.md).

---

## Constructors

### Module()

Construct a Module wrapping `rb_cObject`.

```cpp
Module m;  // wraps Object
```

---

### Module(VALUE v)

Wrap an existing Ruby module VALUE.

**Parameters:**

* `v` - A Ruby VALUE of type `T_MODULE`.

```cpp
Module kernel(rb_mKernel);
```

---

### Module(std::string name, Object under = rb_cObject)

Get or create a module by name.

**Parameters:**

* `name` - The module name.
* `under` - The parent module (default: Object).

```cpp
Module my_mod("MyModule");
Module nested("Nested", my_mod);  // MyModule::Nested
```

---

## Free Functions

### define_module(char const* name) → Module

Define a new module in the global namespace.

**Parameters:**

* `name` - The module name.

**Returns:**

The new or existing Module.

```cpp
Module rb_mMyModule = define_module("MyModule");
```

---

### define_module_under(Object parent, char const* name) → Module

Define a new module under a parent module or class.

**Parameters:**

* `parent` - The parent module or class.
* `name` - The module name.

**Returns:**

The new or existing Module.

```cpp
Module rb_mOuter = define_module("Outer");
Module rb_mInner = define_module_under(rb_mOuter, "Inner");
// Creates Outer::Inner
```

---

### anonymous_module() → Module

Create a new anonymous module.

**Returns:**

A new anonymous Module.

```cpp
Module anon = anonymous_module();
```

---

## Public Methods

### name() const → String

Get the module's name.

**Returns:**

The module name as a String.

```cpp
Module m = define_module("MyModule");
std::string name = m.name().str();  // "MyModule"
```

---

### ancestors() const → Array

Get the module's ancestors.

**Returns:**

An Array of ancestor modules/classes.

```cpp
Module m = define_module("MyModule");
Array ancestors = m.ancestors();
```

---

### singleton_class() const → Class

Get the module's singleton class.

**Returns:**

The singleton Class.

---

### module_eval(String const& s) → Object

Evaluate Ruby code in the context of the module.

**Parameters:**

* `s` - A string containing Ruby code.

**Returns:**

The result of the evaluation.

```cpp
Module m = define_module("MyModule");
m.module_eval("def self.hello; 'world'; end");
```

---

### include_module(Module const& inc) → auto&

Include another module.

**Parameters:**

* `inc` - The module to include.

**Returns:**

Reference to self for method chaining.

```cpp
Module enumerable(rb_mEnumerable);
Module m = define_module("MyModule")
  .include_module(enumerable);
```

---

## Defining Methods

### define_method(name, method, args...) → auto&

Define an instance method on the module.

**Parameters:**

* `name` - The method name.
* `method` - A function pointer, member function pointer, or lambda.
* `args...` - Optional `Arg` objects for default parameters.

**Returns:**

Reference to self for method chaining.

**Description:**

For plain functions or lambdas, the first parameter receives `self` (either as VALUE or as the C++ type).

```cpp
Module m = define_module("MyModule")
  .define_method("greet", [](Object self, std::string name) {
    return "Hello, " + name + "!";
  });
```

---

### define_function(name, func, args...) → auto&

Define an instance method without a self parameter.

**Parameters:**

* `name` - The method name.
* `func` - A function pointer or lambda (no self parameter).
* `args...` - Optional `Arg` objects for default parameters.

**Returns:**

Reference to self for method chaining.

```cpp
int add(int a, int b) { return a + b; }

Module m = define_module("MyModule")
  .define_function("add", &add);
```

---

### define_singleton_method(name, method, args...) → auto&

Define a singleton (class-level) method.

**Parameters:**

* `name` - The method name.
* `method` - A function pointer or lambda (first parameter is self).
* `args...` - Optional `Arg` objects.

**Returns:**

Reference to self for method chaining.

```cpp
Module m = define_module("MyModule")
  .define_singleton_method("version", [](Object self) {
    return "1.0.0";
  });

// Ruby: MyModule.version  #=> "1.0.0"
```

---

### define_singleton_function(name, func, args...) → auto&

Define a singleton method without a self parameter.

**Parameters:**

* `name` - The method name.
* `func` - A function pointer or lambda.
* `args...` - Optional `Arg` objects.

**Returns:**

Reference to self for method chaining.

```cpp
Module m = define_module("MyModule")
  .define_singleton_function("pi", []() {
    return 3.14159;
  });

// Ruby: MyModule.pi  #=> 3.14159
```

---

### define_module_function(name, func, args...) → auto&

Define a module function (accessible as both singleton and instance method).

**Parameters:**

* `name` - The method name.
* `func` - A function pointer or lambda.
* `args...` - Optional `Arg` objects.

**Returns:**

Reference to self for method chaining.

**Note:**

Only valid for modules (raises exception if called on a class).

```cpp
Module m = define_module("MyModule")
  .define_module_function("helper", []() {
    return "helping";
  });

// Ruby:
// MyModule.helper        #=> "helping"
// include MyModule
// helper                 #=> "helping"
```

---

### define_constant(name, value) → auto&

Define a constant in the module.

**Parameters:**

* `name` - The constant name.
* `value` - The constant value (automatically converted to Ruby).

**Returns:**

Reference to self for method chaining.

```cpp
Module m = define_module("MyModule")
  .define_constant("VERSION", "1.0.0")
  .define_constant("MAX_SIZE", 1024);

// Ruby: MyModule::VERSION  #=> "1.0.0"
// Ruby: MyModule::MAX_SIZE #=> 1024
```

---

## Example

```cpp
#include <rice/rice.hpp>

using namespace Rice;

int calculate(int x, int y) {
  return x * y + 1;
}

extern "C"
void Init_my_extension()
{
  Module rb_mMath = define_module("MyMath")
    .define_module_function("calculate", &calculate)
    .define_constant("PI", 3.14159)
    .define_constant("E", 2.71828);
}

// Ruby usage:
// MyMath.calculate(3, 4)  #=> 13
// MyMath::PI              #=> 3.14159
```
