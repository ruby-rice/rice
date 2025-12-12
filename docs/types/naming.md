# Auto Generated Classes

A key feature of C++ is its robust template system. C++ templates are not classes, but instead are used to generate concrete classes in a process called [specialization](https://en.cppreference.com/w/cpp/language/template_specialization.html)

This is quite common in the STL library. For example, `std::vector<int>` is a different class than a `std::vector<std::string>` and thus requires two different Ruby wrapper classes. This means there can be a large number of instantiated classes in a C++ code base.

You can use Rice to define each and every one of these classes, but it quickly becomes tedious to both find them in a C++ code base and to define them.

Thus, Rice can auto generate Ruby classes them as needed. But to use these classes in Ruby, you need to understand how Rice creates class names.

Starting in version 4.5, Rice makes use of unicode characters to create class names that "look" like their C++ counterparts. For example, the type `std::pair<std::string, double>` becomes `Std::Pair≺string‚ double≻`.  Similarly, `std::vector<MyNamespace::MyClass>` becomes  `Std::Vector≺MyNamespace꞉꞉MyClass≻`.

The unicode characters are:

| Character | Code Point | Name |
|-----------|------------|------|
| : | U+A789 | Modified Letter Colon |
| ≺ | U+227A | Precedes |
| ≻ | U+227B | Succeeds |
| , | U+066C | Arabic Thousands Separator |
|   | U+u00A0 | Non breaking Space |

To use an auto generated class in Ruby:

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
