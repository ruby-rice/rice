// TODO: This is silly, autoconf...
/*#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#include "../config.hpp"
*/

#define RICE_ID rb_intern("__rice__")

inline VALUE
Rice::detail::
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

// Define a method and attach metadata about the method to its owning class.
// This is done by wrapping data in a VALUE (via Data_Object) and then
// adding it as a hidden ivar to a storage object (an instance of a Ruby
// object) that is itself added as a hidden ivar to the method's class.
// 
// When Ruby makes a method call, it stores the class Object and method
// ID in the current stack frame.  When Ruby calls into Rice, we grab
// the class and method ID from the stack frame, then pull the data out
// of the class. The data item is then used to determine how to convert
// arguments and return type, how to handle exceptions, etc.
//
inline VALUE
Rice::detail::
define_method_with_data(
    VALUE klass, ID id, VALUE (*cfunc)(ANYARGS), int arity, VALUE data)
{
  VALUE store = rb_attr_get(klass, RICE_ID);

  if (store == Qnil)
  {
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