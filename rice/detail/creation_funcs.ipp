#ifndef Rice__detail__creation_funcs__ipp_
#define Rice__detail__creation_funcs__ipp_

namespace Rice
{

namespace detail
{

namespace
{
  inline VALUE ruby_16_new(int argc, VALUE * argv, VALUE klass)
  {
    VALUE obj = rb_funcall(klass, rb_intern("allocate"), 0);
    rb_obj_call_init(obj, argc, argv);
    return obj;
  }
}

inline void define_alloc_func(
    Class const & klass,
    RUBY_VALUE_FUNC allocate_func)
{
#if RICE__RUBY_VERSION_CODE < 170
  klass.define_singleton_method("allocate", allocate_func);
  klass.define_singleton_method("new", ruby_16_new);
#else
  rb_define_alloc_func(klass, allocate_func);
#endif
}

template<typename Initialize_Func_T>
inline void define_creation_funcs(
    Class const & klass,
    RUBY_VALUE_FUNC allocate_func,
    Initialize_Func_T initialize_func)
{
  define_alloc_func(klass, allocate_func);
  klass.define_method("initialize", initialize_func);
}

inline void undef_alloc_func(Class const & klass)
{
#if RICE__RUBY_VERSION_CODE >= 170
  rb_undef_alloc_func(klass);
#else
  rb_undef_method(CLASS_OF(klass), "new");
#endif
}

inline void undef_creation_funcs(Class const & klass)
{
  undef_alloc_func(klass);
  rb_undef_method(klass, "initialize");
}

} // namespace detail

} // namespace Rice

#endif // Rice__detail__creation_funcs__ipp_

