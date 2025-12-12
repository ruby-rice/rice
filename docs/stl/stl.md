# Overview

Rice supports the C++ standard template library (STL). To enable STL support you MUST include the `rice/stl.hpp` header file after the `rice/rice.hpp` header file.

## Type Mapping

Rice [converts](../types/conversion.md) some STL classes and [wraps](../types/wrapping.md) others.

### Converted

Converted classes include:

* std::complex (to Complex)
* std::monostate (to nil)
* std::optional (to whatever the optional contains)
* std::string (to String)
* std::string_view (to String)
* std::variant (to whatever the variant contains)

### Wrapped

Wrapped classes include:

* std::exception
* std::exception_ptr
* std::map
* std::multimap
* std::pair
* std::set
* std::shared_ptr
* std::type_index
* std::type_info
* std::unordered_map
* std::vector
* std::unique_ptr

In contrast, a builtin class means that Rice will copy data from a C++ object to an equivalent Ruby object. Changes in one object are *not* reflected in the other.

For more information see the [type conversion](../types/conversion.md) section.

## Automatically Generated Ruby Classes

Many STL classes are templates, and thus must be instantiated with concrete types to define actual C++ classes. A `std::vector<int>` is a different class than a `std::vector<std::string>`. This means there can be a large number of STL instantiated classes in a C++ code base.

Rice makes it easy to manually create Ruby classes for instantiated STL templates. The way to do this varies per STL type, but follows a simple naming scheme - `define_pair`, `define_vector`, etc. Please refer to the documentation for each supported STL type.

Rice can also automatically generate Ruby classes. These Ruby classes are added to the `Std` module. Automatic classes work well in cases where Ruby code is accessing, or modifying, wrapped objects created in C++.

Sometimes Ruby also needs to create new instances of these classes. With manually defined class names, this is easy to do. With generated class names you need to understand how Rice creates class names.

Starting in version 4.5, Rice makes use of unicode characters to create class names that "look" like their C++ counterparts. For example, the type `std::pair<std::string, double>` becomes `Std::Pair≺string‚ double≻`.  Or assume there is a vector containing a custom class `MyNamespace::MyClass`. Then generated name would be `Std::Vector≺MyNamespace꞉꞉MyClass≻`.

The unicode characters are:

| Character | Code Point | Name                        |
|:---------:|:----------:|:----------------------------|
| :         | U+A789     | Modified Letter Colon       |
| ≺         | U+227A     | Precedes                    |
| ≻         | U+227B     | Succeeds                    |
| ,         | U+066C     | Arabic Thousands Separator  |
|           | U+u00A0    | Non breaking Space          |

To use this class in Ruby:

```ruby
pair = Std::Pair≺string‚ double≻.new
```

Note manual class names can be defined *after* auto generated class names. Rice only registers one class with Ruby, but it has two constants pointing at it. For example if you call `define_pair<std::string, double>(StringDoublePair)` after the pair has been registered, in Ruby you will have two constants pointing to the class:

```ruby
Std::Pair≺string‚ double≻
Object::StringDoublePair
```

However, the name of the class itself will be defined by however it was first defined. In the case above, the class name will be `Std::Pair≺string‚ double≻`.

In general, it is recommended to use auto-defined STL class names.
