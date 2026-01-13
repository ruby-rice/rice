# Incomplete Types

Rice supports working with incomplete (forward-declared) types. This is essential for wrapping C++ libraries that use the [PIMPL](https://en.cppreference.com/w/cpp/language/pimpl.html) idiom or [opaque](https://en.wikipedia.org/wiki/Opaque_pointer) handle patterns, where implementation details are hidden behind forward declarations.

## What Are Incomplete Types?

An incomplete type is a type that has been declared but not defined. The compiler knows the type exists but doesn't know its size or layout:

```cpp
// Forward declaration - incomplete type
class WidgetImpl;

class Widget {
public:
    // Returns pointer to incomplete type
    WidgetImpl* getImpl();

    // Returns reference to incomplete type
    WidgetImpl& getImplRef();

private:
    WidgetImpl* pImpl_;  // PIMPL idiom
};
```

## Common Patterns

### PIMPL Idiom

The Private Implementation (PIMPL) idiom hides implementation details:

```cpp
// widget.hpp - public header
class WidgetImpl;  // Forward declaration only

class Widget {
public:
    Widget();
    ~Widget();
    void doSomething();
    WidgetImpl* getImpl();

private:
    WidgetImpl* pImpl_;
};

// widget.cpp - implementation (not exposed to users)
class WidgetImpl {
    int value = 42;
    // ... implementation details
};
```

### Opaque Handles

Common in C libraries where handles are passed around without exposing internals:

```cpp
// Forward declaration only - never defined in headers
struct OpaqueHandle;

OpaqueHandle* createHandle();
void destroyHandle(OpaqueHandle* handle);
int getHandleData(OpaqueHandle* handle);
```

## Wrapping Incomplete Types

To wrap functions that use incomplete types, you must register the incomplete type with Rice using `define_class`. Rice doesn't need to know the type's size or layout - it just needs to know the type exists:

```cpp
#include <rice/rice.hpp>

// Forward declaration - WidgetImpl is never defined here
class WidgetImpl;

class Widget {
public:
    WidgetImpl* getImpl();
    WidgetImpl& getImplRef();
};

extern "C" void Init_widget()
{
    using namespace Rice;

    // Register the incomplete type - Rice just needs to know it exists
    define_class<WidgetImpl>("WidgetImpl");

    // Now we can wrap methods that use WidgetImpl
    define_class<Widget>("Widget")
        .define_constructor(Constructor<Widget>())
        .define_method("get_impl", &Widget::getImpl)
        .define_method("get_impl_ref", &Widget::getImplRef);
}
```

## Supported Operations

Rice supports incomplete types in the following contexts:

| Operation                 | Example                          | Supported |
|---------------------------|----------------------------------|-----------|
| Return pointer            | `Impl* getImpl()`                | Yes       |
| Return reference          | `Impl& getImplRef()`             | Yes       |
| Return const reference    | `const Impl& getImplRef() const` | Yes       |
| Parameter pointer         | `void setImpl(Impl* impl)`       | Yes       |
| Parameter reference       | `void process(Impl& impl)`       | Yes       |
| Parameter const reference | `void read(const Impl& impl)`    | Yes       |

## Error Handling

If you try to use an incomplete type without registering it, Rice will raise a Ruby exception:

```cpp
// Without registering OpaqueHandle...
define_global_function("create_handle", &createHandle);

// Calling from Ruby will raise:
// Rice::Exception: Type is not registered with Rice: OpaqueHandle
```

Always register incomplete types with `define_class` before wrapping functions that use them.

## Real-World Example: OpenCV

OpenCV uses the PIMPL idiom extensively. For example, `cv::dnn::Net` has:

```cpp
namespace cv { namespace dnn {
    class Net {
    public:
        struct Impl;  // Forward declaration

        Impl* getImpl() const;
        Impl& getImplRef();

    private:
        Ptr<Impl> impl;
    };
}}
```

To wrap this in Rice:

```cpp
#include <rice/rice.hpp>
#include <opencv2/dnn.hpp>

extern "C" void Init_dnn()
{
    using namespace Rice;

    Module rb_mCv = define_module("Cv");
    Module rb_mDnn = define_module_under(rb_mCv, "Dnn");

    // Register the incomplete Impl type
    define_class_under<cv::dnn::Net::Impl>(rb_mDnn, "Impl");

    // Now wrap Net with its getImpl methods
    define_class_under<cv::dnn::Net>(rb_mDnn, "Net")
        .define_constructor(Constructor<cv::dnn::Net>())
        .define_method("get_impl", &cv::dnn::Net::getImpl)
        .define_method("get_impl_ref", &cv::dnn::Net::getImplRef);
}
```

## How It Works

Rice handles incomplete types by:

1. Storing a pointer to the object without needing to know its size
2. Using type erasure to manage the Ruby VALUE wrapper
3. Tracking the type in Rice's type registry for proper conversion

Since Rice only stores pointers and never copies incomplete types by value, it doesn't need the complete type definition. The actual object lifetime is managed by the C++ code that owns it.

## Limitations

- Cannot create instances of incomplete types from Ruby (no constructor)
- Cannot copy incomplete types by value
- Cannot access members of incomplete types directly
- The incomplete type must be registered before any function using it is called

## See Also

- [Pointers](../bindings/pointers.md) - General pointer handling in Rice
- [References](../bindings/references.md) - Reference handling in Rice
- [Memory Management](../bindings/memory_management.md) - Object lifetime management
