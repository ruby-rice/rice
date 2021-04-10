=================
Memory Management
=================

The trickiest part of wrapping a C++ API is correctly managing memory shared between C++ and Ruby. It is critical to
get this right - otherwise your program *will* crash. The key to getting this right is being crystal clear
on who owns each piece of memory.

Rice manages much of this work for you, but does requires some help in getting it right.

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
