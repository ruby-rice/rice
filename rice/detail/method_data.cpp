#include "method_data.hpp"
#include "ruby.hpp"

// TODO: This is silly, autoconf...
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#include "../config.hpp"

#ifndef RUBY_VM
/* pre-YARV */
#include <node.h>
#include "env.hpp"
#endif

namespace
{

VALUE DATA_MAGIC = INT2NUM(0xDA7A);

} // namespace

#ifdef RUBY_VM

VALUE
Rice::detail::
method_data()
{
  ID id;
  VALUE origin;
  if (!rb_frame_method_id_and_class(&id, &origin))
  {
    rb_raise(
        rb_eRuntimeError,
        "Cannot get method id and class for function");
  }

  VALUE memo = rb_ivar_get(origin, 0);

  if(rb_type(memo) != T_ARRAY && RARRAY_PTR(memo)[0] != DATA_MAGIC)
  {
    /* This can happen for module functions that are created after
     * the stub function */
    rb_raise(
        rb_eRuntimeError,
        "Cannot find method data for module function");
  }

  return RARRAY_PTR(memo)[1];
}

#else

/* pre-YARV */

VALUE
Rice::detail::
method_data()
{
  VALUE origin = ruby_frame->last_class;
  VALUE memo = rb_ivar_get(origin, 0);
  return RARRAY_PTR(memo)[1];
}

#endif

/* Define a method and attach data to it.
 *
 * The method looks to ruby like a normal aliased CFUNC, with a modified
 * origin class:
 *
 * NODE_FBODY
 *   |- (u1) orig - origin class
 *   |  |- basic
 *   |  |  |- flags - origin class flags + FL_SINGLETON
 *   |  |  +- klass - NODE_MEMO
 *   |  |     |- (u1) cfnc - actual C function to call
 *   |  |     |- (u2) rval - stored data
 *   |  |     +- (u3) 0
 *   |  |- iv_tbl - 0
 *   |  |- m_tbl - 0
 *   |  +- super - actual origin class
 *   |- (u2) mid - name of the method
 *   +- (u3) head - NODE_CFUNC
 *      |- (u1) cfnc - wrapper function to call
 *      +- (u2) argc - function arity
 *
 * Or, on YARV:
 *
 * NODE_FBODY
 *   |- (u1) oid - name of the method
 *   +- (u2) body - NODE_METHOD
 *      |- (u1) clss - origin class
 *      |  |- basic
 *      |  |  |- flags - origin class flags + FL_SINGLETON
 *      |  |  +- klass - NODE_MEMO
 *      |  |     |- (u1) cfnc - actual C function to call
 *      |  |     |- (u2) rval - stored data
 *      |  |     +- (u3) 0
 *      |  |- ptr - rb_classext_t
 *      |  |  |- super - actual origin class
 *      |  |  +- iv_tbl - 0
 *      |  |- m_tbl - 0
 *      |  +- iv_index_tbl - 0?
 *      |- (u2) body - NODE_CFUNC
 *      |  |- (u1) cfnc - wrapper function to call
 *      |  |- (u2) argc - function arity
 *      +- (u3) noex - NOEX_PUBLIC
 *
 * When the wrapper function is called, last_class is set to the origin
 * class found in the FBODY node.  So that the method data will be
 * accessible, and so last_class will point to klass and not to our MEMO
 * node, it is necessary to "fix" the current frame.
 *
 * Pre-YARV, this means we duplicate the current frame and set last_class:
 *
 * ruby_frame
 *   |- last_class - klass
 *   |- prev
 *   |  |- last_class - NODE_MEMO
 *   |  |  |- (u1) cfnc - actual C function to call
 *   |  |  |- (u2) rval - stored data
 *   |  |  +- (u3) 0
 *   |  |- prev - the real previous frame
 *   |  +- ...
 *   +- ...
 *
 * The method data is then accessible via
 * ruby_frame->prev->last_class->rval.
 *
 * On YARV, the current frame is not duplicated; rather, the method data
 * is placed on the stack and is referenced by one of the unused members
 * of the control frame (the program counter):
 *
 * ruby_current_thread->cfp
 *   |- pc - NODE_MEMO
 *   |  |- (u1) cfnc - actual C function to call
 *   |  |- (u2) rval - stored data
 *   |  +- (u3) 0
 *   |- method_class - klass
 *   +- ...
 *
 */
VALUE
Rice::detail::
define_method_with_data(
    VALUE klass, ID id, VALUE (*cfunc)(ANYARGS), int arity, VALUE data)
{
  /* TODO: origin should have #to_s and #inspect methods defined */
#ifdef HAVE_RB_CLASS_BOOT
  VALUE origin = rb_class_boot(klass);
#else
  VALUE origin = rb_class_new(klass);
#endif
  VALUE memo = rb_assoc_new(DATA_MAGIC, data);
  VALUE real_class_name = rb_class_name(klass);
  VALUE origin_class_name = rb_str_plus(
      real_class_name,
      rb_str_new2("<data wrapper>"));

  FL_SET(origin, FL_SINGLETON);
  rb_singleton_class_attached(origin, klass);
  rb_name_class(origin, SYM2ID(rb_str_intern(origin_class_name)));
  rb_ivar_set(origin, 0, memo);

  /* YARV */
  st_data_t dummy_entry;
  rb_define_method(
      origin,
      rb_id2name(id),
      cfunc,
      arity);
  rb_alias(
      origin,
      rb_intern("dummy"),
      id);
  st_lookup(RCLASS_M_TBL(origin), rb_intern("dummy"), &dummy_entry);
  st_insert(RCLASS_M_TBL(klass), id, dummy_entry);

  // Clear the table so we don't try to double-free the method entry
  RCLASS_M_TBL(origin) = st_init_numtable();

  return Qnil;
}

