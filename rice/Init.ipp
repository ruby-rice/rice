namespace Rice
{
  inline void init()
  {
    define_fundamental_buffer_types();
    detail::define_ruby_types();
  };
}