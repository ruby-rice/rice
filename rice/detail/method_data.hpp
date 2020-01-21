#ifndef Rice__detail__method_data__hpp
#define Rice__detail__method_data__hpp

#include "ruby.hpp"

// TODO: This is silly, autoconf...
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#include "../config.hpp"

#define RICE_ID rb_intern("__rice__")

namespace Rice
{

namespace detail
{

  VALUE
  method_data()
  {
    ID id;
    VALUE klass;
    if (!rb_frame_method_id_and_class(&id, &klass))
    {
      rb_raise(
          rb_eRuntimeError,
          "Cannot get method id and class for function");
    }

    if (rb_type(klass) == T_ICLASS) {
      klass = rb_class_of(klass);
    }

    VALUE store = rb_ivar_get(klass, RICE_ID);
    return (store == Qnil) ? Qnil : rb_ivar_get(store, id);
  }

  // Ruby 2.7 now includes a similarly named macro that uses templates to
  // pick the right overload for the underlying function. That doesn't work
  // for our cases because we are using this method dynamically and get a
  // compilation error otherwise. This removes the macro and lets us fall
  // back to the C-API underneath again.
#undef rb_define_method_id

  // Define a method and attach data to it.
  // The method looks to ruby like a normal aliased CFUNC, with a modified
  // origin class.
  //
  // How this works:
  //
  // To store method data and have it registered with the GC, we need a
  // "slot" to put it in.  This "slot" must be recognized and marked by
  // the garbage collector.  There happens to be such a place we can put
  // data, and it has to do with aliased methods.  When Ruby creates an
  // alias for a method, it stores a reference to the original class in
  // the method entry.  The form of the method entry differs from ruby
  // version to ruby version, but the concept is the same across all of
  // them.
  // 
  // In Rice, we make use of this by attach the data to a dummy object 
  // (store) in the class variables table.
  // 
  // When Ruby makes a method call, it stores the class Object and method
  // ID in the current stack frame.  When Ruby calls into Rice, we grab
  // the class and method ID from the stack frame, then pull the data out
  // of the class. The data item is then used to determine how to convert
  // arguments and return type, how to handle exceptions, etc.
  //
  VALUE
  define_method_with_data(
      VALUE klass, ID id, VALUE (*cfunc)(ANYARGS), int arity, VALUE data)
  {
    VALUE store = rb_attr_get(klass, RICE_ID);

    if (store == Qnil) {
      store = rb_obj_alloc(rb_cObject);
      // store is stored in the instance variables table with
      // name "__rice__".
      // since "__rice__" does not have the @ prefix,
      // so it can never be read at the Ruby level.
      rb_ivar_set(klass, RICE_ID, store);
    }

    rb_ivar_set(store, id, data);

    // Create the aliased method on the origin class
    rb_define_method_id(
        klass,
        id,
        cfunc,
        arity);

    return Qnil;
  }

} // namespace detail

} // namespace Rice

#endif // Rice__detail__method_data__hpp

