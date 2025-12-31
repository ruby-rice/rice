# Smart Pointer Architecture

This document describes how Rice implements smart pointer support and how to add support for custom smart pointer types.

## Overview

Rice's smart pointer support involves three key components:

1. **Wrapper specialization** - A `Wrapper<SmartPtr<T>>` class that stores the smart pointer and provides access to both the smart pointer and its managed object
2. **Type verification** - A `Type<SmartPtr<T>>` specialization that registers the smart pointer type with Rice
3. **Ruby class definition** - A function that creates the Ruby class and exposes methods

## Wrapper Specialization

The `Wrapper` class is responsible for storing C++ objects that are wrapped by Ruby. For smart pointers, you need to specialize `Wrapper` to handle the dual nature of smart pointers: they are both a container and provide access to a contained object.

### Declaration

```cpp
namespace Rice::detail
{
  template<typename T>
  class Wrapper<my_smart_ptr<T>> : public WrapperBase
  {
  public:
    Wrapper(rb_data_type_t* rb_data_type, const my_smart_ptr<T>& data);
    ~Wrapper();
    void* get(rb_data_type_t* requestedType) override;

  private:
    my_smart_ptr<T> data_;
    rb_data_type_t* inner_rb_data_type_;
  };
}
```

Key points:

- Inherit from `WrapperBase`
- Store the smart pointer in `data_`
- Store the `rb_data_type_t*` for the inner type `T` in `inner_rb_data_type_`

### Constructor

The constructor must:

1. Call the `WrapperBase` constructor with the `rb_data_type`
2. Store the smart pointer
3. Look up the `rb_data_type_t*` for the inner type

```cpp
template<typename T>
Wrapper<my_smart_ptr<T>>::Wrapper(rb_data_type_t* rb_data_type, const my_smart_ptr<T>& data)
  : WrapperBase(rb_data_type), data_(data)
{
  using Intrinsic_T = intrinsic_type<T>;

  if constexpr (std::is_fundamental_v<Intrinsic_T>)
  {
    inner_rb_data_type_ = Data_Type<Pointer<Intrinsic_T>>::ruby_data_type();
  }
  else
  {
    inner_rb_data_type_ = Data_Type<Intrinsic_T>::ruby_data_type();
  }
}
```

### Destructor

The destructor must remove the instance from Rice's instance registry:

```cpp
template<typename T>
Wrapper<my_smart_ptr<T>>::~Wrapper()
{
  Registries::instance.instances.remove(this->get(this->rb_data_type_));
}
```

### get() Method

The `get()` method is the heart of smart pointer support. It examines the `requestedType` and returns either:

- A pointer to the smart pointer itself (when the caller wants the smart pointer)
- The raw pointer to the managed object (when the caller wants `T*`)

```cpp
template<typename T>
void* Wrapper<my_smart_ptr<T>>::get(rb_data_type_t* requestedType)
{
  if (rb_typeddata_inherited_p(this->rb_data_type_, requestedType))
  {
    // Caller wants the smart pointer itself
    return &this->data_;
  }
  else if (rb_typeddata_inherited_p(this->inner_rb_data_type_, requestedType))
  {
    // Caller wants the managed object
    return this->data_.get();
  }
  else
  {
    throw Exception(rb_eTypeError, "wrong argument type (expected %s)",
        requestedType->wrap_struct_name);
  }
}
```

This allows Rice to automatically pass either the smart pointer or the raw pointer to C++ methods based on their signature.

## Type Specialization

The `Type` specialization tells Rice how to verify and register the smart pointer type:

```cpp
namespace Rice::detail
{
  template<typename T>
  struct Type<my_smart_ptr<T>>
  {
    static bool verify()
    {
      // First verify the inner type.
      // Note: The is_fundamental check is only needed if your smart pointer
      // supports fundamental types (int, double, etc.)
      bool result = true;
      if constexpr (std::is_fundamental_v<T>)
      {
        result = result && Type<Pointer<T>>::verify();
      }
      else
      {
        result = result && Type<T>::verify();
      }

      // Then register the smart pointer type
      if (result)
      {
        define_my_smart_ptr<T>();
      }

      return result;
    }
  };
}
```

## Ruby Class Definition

Create a function to define the Ruby class with appropriate methods:

```cpp
template<typename T>
Data_Type<my_smart_ptr<T>> define_my_smart_ptr(std::string klassName)
{
  using SmartPtr_T = my_smart_ptr<T>;
  using Data_Type_T = Data_Type<SmartPtr_T>;

  // Generate class name if not provided
  if (klassName.empty())
  {
    detail::TypeMapper<SmartPtr_T> typeMapper;
    klassName = typeMapper.rubyName();
  }

  // Check if already defined (use your own module, not Std)
  Module rb_mModule = define_module("YourModule");
  if (Data_Type_T::check_defined(klassName, rb_mModule))
  {
    return Data_Type_T();
  }

  // Define the Ruby class with methods appropriate for your smart pointer
  Identifier id(klassName);
  Data_Type_T result = define_class_under<detail::intrinsic_type<SmartPtr_T>>(rb_mModule, id).
    define_method("empty?", &SmartPtr_T::operator bool).
    define_method("get", &SmartPtr_T::get);

  // Setup method forwarding to the managed type
  if constexpr (!std::is_void_v<T> && !std::is_fundamental_v<T>)
  {
    detail::define_forwarding(result.klass(), Data_Type<T>::klass());
  }

  return result;
}
```

## Method and Attribute Forwarding

Rice uses Ruby's [`Forwardable`](https://ruby-doc.org/stdlib/libdoc/forwardable/rdoc/Forwardable.html) module to forward method and attribute calls from the smart pointer wrapper to the managed object. This is implemented in `detail::define_forwarding()`.

To avoid conflicts, methods or attributes that are already defined on the wrapper class are not forwarded. For example, if both the smart pointer wrapper and the managed type define a `swap` method, only the wrapper's version is directly accessible. To call the managed object's version, use `ptr.get.swap`.

```cpp
inline void define_forwarding(VALUE wrapper_klass, VALUE wrapped_klass)
{
  protect(rb_require, "forwardable");
  Object forwardable = Object(rb_cObject).const_get("Forwardable");
  Object(wrapper_klass).extend(forwardable.value());

  // Get wrapper class's method and attribute names to avoid conflicts
  std::set<std::string> wrapperMethodSet;
  std::vector<std::string> wrapperMethods = Registries::instance.natives.lookup(wrapper_klass, NativeKind::Method);
  wrapperMethodSet.insert(wrapperMethods.begin(), wrapperMethods.end());
  std::vector<std::string> wrapperReaders = Registries::instance.natives.lookup(wrapper_klass, NativeKind::AttributeReader);
  wrapperMethodSet.insert(wrapperReaders.begin(), wrapperReaders.end());
  std::vector<std::string> wrapperWriters = Registries::instance.natives.lookup(wrapper_klass, NativeKind::AttributeWriter);
  wrapperMethodSet.insert(wrapperWriters.begin(), wrapperWriters.end());

  // Get wrapped class's method and attribute names, including ancestor classes
  std::set<std::string> wrappedMethodSet;
  Class klass(wrapped_klass);
  while (klass.value() != rb_cObject && klass.value() != Qnil)
  {
    std::vector<std::string> methods = Registries::instance.natives.lookup(klass.value(), NativeKind::Method);
    wrappedMethodSet.insert(methods.begin(), methods.end());

    std::vector<std::string> readers = Registries::instance.natives.lookup(klass.value(), NativeKind::AttributeReader);
    wrappedMethodSet.insert(readers.begin(), readers.end());

    std::vector<std::string> writers = Registries::instance.natives.lookup(klass.value(), NativeKind::AttributeWriter);
    wrappedMethodSet.insert(writers.begin(), writers.end());

    klass = klass.superclass();
  }

  // Build arguments: [:get, :method1, :method2, ...]
  // Skip methods already defined on the wrapper class
  Array args;
  args.push(Symbol("get"));
  for (const std::string& method : wrappedMethodSet)
  {
    if (wrapperMethodSet.find(method) == wrapperMethodSet.end())
    {
      args.push(Symbol(method));
    }
  }

  // Call def_delegators
  Object(wrapper_klass).vcall("def_delegators", args);
}
```

## From_Ruby Specialization (Optional)

If your smart pointer has special copy/move semantics (like move-only types), you may need a `From_Ruby` specialization:

```cpp
template <typename T>
class From_Ruby<my_smart_ptr<T>>
{
public:
  From_Ruby() = default;

  explicit From_Ruby(Arg* arg) : arg_(arg)
  {
  }

  double is_convertible(VALUE value)
  {
    switch (rb_type(value))
    {
      case RUBY_T_DATA:
        return Convertible::Exact;
      default:
        return Convertible::None;
    }
  }

  my_smart_ptr<T> convert(VALUE value)
  {
    my_smart_ptr<T>* result = detail::unwrap<my_smart_ptr<T>>(
      value,
      Data_Type<my_smart_ptr<T>>::ruby_data_type(),
      this->arg_ && this->arg_->isOwner());
    return std::move(*result);  // Use std::move for move-only types
  }

private:
  Arg* arg_ = nullptr;
};
```

## Summary

To add support for a custom smart pointer:

1. Specialize `Wrapper<YourSmartPtr<T>>` with a `get()` method that can return either the smart pointer or the managed object
2. Specialize `Type<YourSmartPtr<T>>` to verify and register the type
3. Create a `define_your_smart_ptr<T>()` function to define the Ruby class
4. Optionally specialize `From_Ruby` if your smart pointer has special copy/move semantics

## Complete Code

Here is all the code together for easy copy-paste:

```cpp
namespace Rice::detail
{
  // Wrapper specialization
  template<typename T>
  class Wrapper<my_smart_ptr<T>> : public WrapperBase
  {
  public:
    Wrapper(rb_data_type_t* rb_data_type, const my_smart_ptr<T>& data)
      : WrapperBase(rb_data_type), data_(data)
    {
      using Intrinsic_T = intrinsic_type<T>;

      // Note: The is_fundamental check is only needed if your smart pointer
      // supports fundamental types (int, double, etc.)
      if constexpr (std::is_fundamental_v<Intrinsic_T>)
      {
        inner_rb_data_type_ = Data_Type<Pointer<Intrinsic_T>>::ruby_data_type();
      }
      else
      {
        inner_rb_data_type_ = Data_Type<Intrinsic_T>::ruby_data_type();
      }
    }

    ~Wrapper()
    {
      Registries::instance.instances.remove(this->get(this->rb_data_type_));
    }

    void* get(rb_data_type_t* requestedType) override
    {
      if (rb_typeddata_inherited_p(this->rb_data_type_, requestedType))
      {
        return &this->data_;
      }
      else if (rb_typeddata_inherited_p(this->inner_rb_data_type_, requestedType))
      {
        return this->data_.get();
      }
      else
      {
        throw Exception(rb_eTypeError, "wrong argument type (expected %s)",
            requestedType->wrap_struct_name);
      }
    }

  private:
    my_smart_ptr<T> data_;
    rb_data_type_t* inner_rb_data_type_;
  };

  // Type specialization
  template<typename T>
  struct Type<my_smart_ptr<T>>
  {
    static bool verify()
    {
      // Note: The is_fundamental check is only needed if your smart pointer
      // supports fundamental types (int, double, etc.)
      bool result = true;
      if constexpr (std::is_fundamental_v<T>)
      {
        result = result && Type<Pointer<T>>::verify();
      }
      else
      {
        result = result && Type<T>::verify();
      }

      if (result)
      {
        define_my_smart_ptr<T>();
      }

      return result;
    }
  };
}

namespace Rice
{
  // Ruby class definition
  template<typename T>
  Data_Type<my_smart_ptr<T>> define_my_smart_ptr(std::string klassName = "")
  {
    using SmartPtr_T = my_smart_ptr<T>;
    using Data_Type_T = Data_Type<SmartPtr_T>;

    if (klassName.empty())
    {
      detail::TypeMapper<SmartPtr_T> typeMapper;
      klassName = typeMapper.rubyName();
    }

    Module rb_mModule = define_module("YourModule");
    if (Data_Type_T::check_defined(klassName, rb_mModule))
    {
      return Data_Type_T();
    }

    // Define the Ruby class with methods appropriate for your smart pointer
    Identifier id(klassName);
    Data_Type_T result = define_class_under<detail::intrinsic_type<SmartPtr_T>>(rb_mModule, id).
      define_method("empty?", &SmartPtr_T::operator bool).
      define_method("get", &SmartPtr_T::get);

    if constexpr (!std::is_void_v<T> && !std::is_fundamental_v<T>)
    {
      detail::define_forwarding(result.klass(), Data_Type<T>::klass());
    }

    return result;
  }
}
```
