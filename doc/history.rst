=========
Why Rice?
=========

Motivation
----------

There are a number of common problems when writing C or C++ extensions
for Ruby:

**Type safety**. It is easy to mix-up integral types such as ID and
VALUE. Some of the functions in the Ruby API are not consistent with
which types they take (e.g. rb_const_defined takes an ID and
rb_mod_remove_const takes a Symbol).

**DRY principle**. Specifying the number of arguments that each wrapped
function takes is easy to get wrong. Adding a new argument to the
function means that the number of arguments passed to rb_define_method
must also be updated.

**Type conversion**. There are many different functions to convert data
to and from ruby types. Many of them have different semantics or
different forms. For example, to convert a string, one might use the
StringValue macro, but to convert a fixnum, one might use FIX2INT.
Unwrapping previously wrapped C data uses yet another form.

**Exception safety**. It is imperative that C++ exceptions never make
their way into C code, and it is also imperative that a Ruby exception
never escape while there are objects on the stack with nontrivial
destructors. Rules for when it is okay to use which exceptions are
difficult to get right, especially as code is maintained through time.

**Thread safety**. Because the Ruby interpreter is not thread-safe,
the Ruby interpreter must not be run from more than one thread.
Because of tricks the GC and scheduler play with the C stack, it's not
enough to ensure that only one thread runs the interpreter at any
given time; once the interpreter has been run from one thread, it must
only ever be run from that thread in the future. Additionally,
because Ruby copies the stack when it switches threads, C++ code must
be careful not to access objects in one Ruby thread that were created
on the stack in another Ruby thread.

**C-based API**. The Ruby API is not always convenient for accessing
Ruby data structurs such as Hash and Array, especially when writing C++
code, as the interface for these containers is not consistent with
standard containers.

**Calling convention**. Function pointers passed into the Ruby API must
follow the C calling convention. This means that it is not possible to
pass a pointer to a template function or static member function (that
is, it will work on some platforms, but isn't portable).

**Inheritance**. When wrapping C++ objects, it is easy to store a
pointer to a derived class, but then methods in the base class must have
knowledge of the derived class in order to unwrap the object. It is
possible to always store a pointer to the base class and then
dynamic_cast the pointer to the derived type when necessary, but this
can be slow and cumbersome, and it isn't likely to work with multiple
inheritance. A system that properly handles inheritance for all corner
cases is nontrivial.

**Multiple inheritance**. C++ supports true multiple inheritance, but
the Ruby object model uses single inheritance with mixins. When
wrapping a library whose public interface uses multiple inheritance,
care must be taken in constructing the mapping.

**GC safety**. All live Ruby objects must be marked during the garbage
collector's mark phase, otherwise they will be prematurely destroyed.
The general rule is that object references stored on the heap should be
either registered with rb_gc_register_address or marked by a data
object's mark function; object references stored on the stack will be
automatically marked, provided the Ruby interpreter was properly
initialized at startup.

**Callbacks**. C implements callbacks via function pointers, while Ruby
typically implements callbacks via procs. Writing an adapter function
to call the proc is not difficult, but there is much opportunity for
error (particularly with exception-safety).

**Data serialization**. By default data objects defined at the C layer
are not marshalable. The user must explicitly define functions to
marshal the data member-by-member.

Rice addresses these issues in many ways:

**Type safety**. Rice provides encapsulation for all builtin types,
such as Object, Identifier, Class, Module, and String. It
automatically checks the dynamic type of an object before constructing
an instance of a wrapper.

**DRY principle**. Rice uses introspection through the use of templates
and function overloading to automatically determine the number and types
of arguments to functions. Default arguments must still be handled
explicitly, however.

**Type conversions**. Rice provides cast-style to_ruby<> and
from_ruby<> template functions to simplify explicit type conversions.
Automatic type conversions for parameters and return values are
generated for all wrapped functions.

**Exception safety**. Rice automatically converts common exceptions and
provides a mechanism for converting user-defined exception types. Rice
also provides convenience functions for converting exceptions when
calling back into ruby code.

**Thread safety**. Rice provides no mechanisms for dealing with thread
safety. Many common thread safety issues should be alleviated by YARV,
which supports POSIX threads.

**C++-based API**. Rice provides an object-oriented C++-style API to
most common functions in the Ruby C API.

**Calling convention**. Rice automatically uses C calling convention
for all function pointers passed into the Ruby API.

**Inheritance**. Rice provides automatic conversion to the base class
type when a wrapped member function is called on the base class.

**Multiple inheritance**. Rice provides no mechanism for multiple
inheritance. Multiple inheritance can be simulated via mixins, though
this is not yet as easy as it could be.

**GC safety**. Rice provides a handful of convenience classes for
interacting with the garbage collector. There are still basic rules
which must be followed to ensure that objects get properly destroyed.

**Callbacks**. Rice provides a handful of convenience classes for
dealing with callbacks.

**Data serialization**. Rice provides no mechanism for data
serialization, but it is likely this may be added in a future release.

History
-------

Rice originated as Excruby, a project to interface with C++-based trading
software at Automated Trading Desk in Mount Pleasant, South Carolina.
The Ruby bindings for Swig were at the time less mature than they are
today, and did not suit the needs of the project.

Excruby was written not as a wrapper for the Ruby API, but rather as a
set of helper functions and classes for interfacing with the Ruby
interpreter in an exception-safe manner. Over the course of five years,
the project grew into wrappers for pieces of the API, but the original
helper functions remained as part of the public interface.

This created confusion for the users of the library, because there were
multiple ways of accomplishing most tasks -- directly through the C API,
through a low-level wrapper around the C API, and through a high-level
abstraction of the lower-level interfaces.

Rice was then born in an attempt to clean up the interface. Rice keeps
the lower-level wrappers, but as an implementation detail; the public
interface is truly a high-level abstraction around the Ruby C API.
