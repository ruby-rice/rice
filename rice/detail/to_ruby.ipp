template<typename T>
Rice::Object Rice::detail::to_ruby_<T>::
convert(T const & x)
{
  // Should produce a compile-time error
  enum { no_to_ruby_conversion_defined = -1 };
  char x_[no_to_ruby_conversion_defined];
  return Qnil;
}

