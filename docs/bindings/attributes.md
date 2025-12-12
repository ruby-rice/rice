# Attributes

C++ structures, and sometimes classes, often have public member variables that store data. Rice makes it easy to wrap these member variables via the use of `define_attr`:

```cpp
struct MyStruct
{
  int readOnly = 0;
  int writeOnly = 0;
  int readWrite = 0;
};

Data_Type<MyStruct> rb_cMyStrut =
  define_class<MyStruct>("MyStruct")
  .define_constructor(Constructor<MyStruct>())
  .define_attr("read_only",  &MyStruct::readOnly, Rice::AttrAccess::Read)
  .define_attr("write_only", &MyStruct::writeOnly, Rice::AttrAccess::Write)
  .define_attr("read_write", &MyStruct::readWrite);
}
```

Notice the use of `Rice::AttrAccess::Read` to define read-only attributes and `Rice::AttrAccess::Write` for write-only attributes. If you do not specify an AttrAccess value then Rice make the attribute readable and writable.

These attributes can then be accessed in the expected way in Ruby:

```ruby
my_struct = MyStruct.new
a = my_struct.read_only
my_struct.write_only = 5
my_struct.read_write = 10
b = my_struct.read_write
```

Similarly, you can wrap static members via the use of `define_singleton_attr`:

```cpp
struct MyStruct
{
  static int readOnly = 0;
  static int writeOnly = 0;
  static int readWrite = 0;
};

Data_Type<MyStruct> rb_cMyStrut =
  define_class<MyStruct>("MyStruct")
  .define_constructor(Constructor<MyStruct>())
  .define_singleton_attr("read_only", &MyStruct::readOnly, Rice::AttrAccess::Read)
  .define_singleton_attr("write_only", &MyStruct::writeOnly, Rice::AttrAccess::Write)
  .define_singleton_attr("read_write", &MyStruct::readWrite);
}
```

These attributes can then be accessed in the expected way in Ruby:

```ruby
a = MyStruct.read_only
MyStruct.write_only = 5
MyStruct.read_write = 10
b = MyStruct.read_write
```
