=================
Memory Management
=================

The trickiest part of wrapping a C++ API is correctly managing memory shared between C++ and Ruby. It is critical to
get this right - otherwise your program *will* crash. The key to getting this right is being crystal clear
on who owns each piece of memory. Rice manages much of this work for you, but does requires some help in getting it right.

Rice divides native types into two categories, builtin types and external types. Builtin types are types that directly map from C++ to Ruby. Examples include nullptr, bool, numeric types (integer, float, double, complex), char types and strings. All other types are external types.

Builtin types are always copied between C++ and Ruby and vice versa. Since builtin types are always copied, they are disconnected. Therefore, if a Ruby string is converted to a std::string then the two strings are independent and changes in one will *not* be reflected in the other. Also understand that if you allocate a new char* in C++ and pass it to Ruby, then you will get a memory leak because Ruby will copy the contents on the char* but will *not* free the original buffer. Generally you don't have to worry about builtin types because Rice supports them out of the box.

External types, in contrast, are types that are not copied between C++ and Ruby. Instead external types are wrapped in Ruby objects using define_class and friends as described above. The rest of this section discusses how to manage memory of external types.

C++ to Ruby
-----------

As described in the [methods](#methods) section, use the Return class to specify whether ownership of objects
returned from C++ functions should be transferred to Ruby.

In the case where Ruby does *not* take ownership of the object, the transfer happens like this:

====================== ================ =========
Method Return Type (T) C++ to Ruby      Cleanup
====================== ================ =========
Value (T)              Copy constructor Ruby frees the copy, C++ the original
Reference (T&)         No copy          C++ frees the object
Pointer (T*)           No copy          C++ frees the object
====================== ================ =========

In the case where Ruby takes ownership of the object, the transfer happens like this:

====================== ================ =========
Method Return Type (T) C++ to Ruby      Cleanup
====================== ================ =========
Value (T)              Copy constructor Ruby frees the copy
Reference (T&)         Move constructor Ruby frees the copy
Pointer (T*)           No copy          Ruby frees C++ object
====================== ================ =========

Ruby to C++
-----------

For more information see the [keep alive](#keep-alive) section.

C++ Referencing Ruby Objects
----------------------------

When reference Ruby objects from C++, you need to let Ruby know about them so they are not prematurely
garbage collected.

In simple cases, with Objects on the stack, the Ruby GC will automatically find them and you don't have to
do anything. If instead, you allocate an Object on the heap or if it is a member of an object that might be
allocated on the heap, use an Rice::Address_Registration_Guard to register the object with the garbage
collector.

If you create classes or structures that reference Ruby objects, you need to implement a custom mark function:

.. code-block:: cpp

  class MyClass
  {
    VALUE value_;
  }

  namespace Rice
  {
    template<>
    ruby_mark<MyClass>(const MyClass* myClass)
    {
      rb_gc_mark(myClass->value_);
    }
  }

  Data_Type<MyClass> class = define_class<MyClass>("MyClass")
            .define_constructor(Constructor<MyClass>());
