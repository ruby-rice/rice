#ifndef Rice__detail__creation_funcs__ipp_
#define Rice__detail__creation_funcs__ipp_

namespace Rice
{

namespace detail
{

template<typename Initialize_Func_T>
inline void define_creation_funcs(
    Class const & klass,
    RUBY_VALUE_FUNC allocate_func,
    Initialize_Func_T initialize_func)
{
  rb_define_alloc_func(klass, allocate_func);
  klass.define_method("initialize", initialize_func);
}

inline void undef_alloc_func(Class const & klass)
{
  rb_undef_alloc_func(klass);
}

inline void undef_creation_funcs(Class const & klass)
{
  undef_alloc_func(klass);
  rb_undef_method(klass, "initialize");
}

} // namespace detail

} // namespace Rice

#endif // Rice__detail__creation_funcs__ipp_

