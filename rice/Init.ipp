namespace Rice
{
  inline Init::Init()
  {
    define_fundamental_buffer_types();
    detail::define_ruby_types();
  };
}