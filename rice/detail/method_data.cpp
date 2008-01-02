#include "method_data.hpp"
#include "st.hpp"
#include "ruby.hpp"

#include <stdexcept>

#include "node.h"
#include "env.h"

namespace
{

NODE * method_node(VALUE klass, ID id)
{
  union { VALUE value; RClass * ptr; } class_u = { klass };
  union { ID id; char * charp; } id_u = { id };
  char * node_charp;
  if (!st_lookup(class_u.ptr->m_tbl, id_u.charp, &node_charp))
  {
    throw std::runtime_error("Could not determine method data");
  }
  union { char * charp; RNode * node; } node_u = { node_charp };
  RNode * node = node_u.node;
  if (!node->nd_body)
  {
    throw std::runtime_error("Could not determine method data");
  }
  node = node->nd_body;
  return node;
}

} // namespace

void * Rice::detail::
method_data()
{
#if 0
  // TODO: I think it should be possible (at least on Ruby 1.8.x and
  // newer) to elimate the lookup and get the method's node directly,
  // but I haven't yet been able to get that to work.
  VALUE klass = ruby_frame->last_class;
  ID id = ruby_frame->last_func;
  RNode * node;
  if(   !st_lookup(RCLASS(klass)->m_tbl, (char *)id, (char **)&node)
     || !node->nd_body)
  {
    throw std::runtime_error("Could not determine method data");
  }
  node = node->nd_body;
  return (void *)node->nd_entry;
#else
  NODE * node = method_node(ruby_frame->last_class, ruby_frame->last_func);
  return (void *)node->nd_entry;
#endif
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
  NODE * node = method_node(klass, rb_intern(name));
  node->nd_entry = (global_entry *)data;
}

