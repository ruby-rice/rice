namespace Rice
{
  inline void init()
  {
    detail::define_ruby_types();
    define_fundamental_buffer_types();
  };
}