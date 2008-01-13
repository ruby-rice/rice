#include "method_data.hpp"
#include "st.hpp"
#include "ruby.hpp"

#include <stdexcept>

#include "node.h"
#include "env.h"

// Every C function in ruby (pre-YARV) is stored like this:
//
// NODE_METHOD
//   |- (u1) noex - method visibility
//   +- (u2) body
//      +- NODE_CFUNC
//          |- (u1) cfnc - function to call
//          +- (u2) argc - number of arguments
//
// We use the unused third member (u3) of the CFUNC node to store method
// data.
//
// We could use the unused third member of the METHOD node, but then the
// method data would not be copied when the method is aliased.  The
// result then would be a segfault whenver the method is called.
//
// Using the third member of the METHOD node would also prevent adding
// the method atomically (rather, the method would be added and then the
// node later modified to contain the data).
//
// When the function is called, it can retrieve the defined data by
// calling method_data().  We then do a lookup in the class to find the
// node for the method's bdoy.  This is necessary, because there's no
// way to get the node of a called function directly.  However, by using
// the class's method table, we can do one lookup instead of two (one to
// find the class data and another to find the method data).

namespace
{

NODE * get_method_node(VALUE klass, ID id)
{
  union { VALUE value; RClass * ptr; } class_u = { klass };
  union { ID id; st_data_t data; } id_u = { id };
  st_data_t node_data;
  if (!st_lookup(class_u.ptr->m_tbl, id_u.data, &node_data))
  {
    throw std::runtime_error("Could not find method node");
  }
  union { st_data_t data; RNode * node; } node_u = { node_data };
  return node_u.node;
}

NODE * get_method_body(VALUE klass, ID id)
{
  NODE * node = get_method_node(klass, id);

  if (!node->nd_body)
  {
    throw std::runtime_error("Method has no body");
  }

  return node->nd_body;
}

} // namespace

void * Rice::detail::
method_data()
{
  NODE * node = get_method_body(
      ruby_frame->last_class,
      ruby_frame->last_func);

  return (void *)node->nd_entry;
}

void Rice::detail::
define_method_with_data(
    VALUE klass,
    char const * name,
    RUBY_METHOD_FUNC func,
    int num_args,
    void * data)
{
  NODE * node = NEW_CFUNC(func, num_args);
  node->nd_entry = (global_entry *)data;
#if RUBY_VERSION_CODE >= 180
  rb_add_method(klass, rb_intern(name), node, NOEX_PUBLIC);
#else
  rb_add_method(klass, rb_intern(name), node, NOEX_PUBLIC|NOEX_CFUNC);
  rb_enable_super(klass, name);
#endif
}

void Rice::detail::
set_method_data(
    VALUE klass,
    char const * name,
    void * data)
{
  NODE * node = get_method_body(klass, rb_intern(name));
  node->nd_entry = (global_entry *)data;
}

