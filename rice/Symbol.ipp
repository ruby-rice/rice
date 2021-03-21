inline Rice::Symbol::
Symbol(VALUE value) : Object(value)
{
  detail::protect(rb_check_type, value, (int)T_SYMBOL);
}

inline Rice::Symbol::
Symbol(Object value): Object(value)
{
  detail::protect(rb_check_type, value.value(), (int)T_SYMBOL);
}

inline Rice::Symbol::
Symbol(char const * s)
  : Object(ID2SYM(rb_intern(s)))
{
}

inline Rice::Symbol::
Symbol(std::string const & s)
  : Object(ID2SYM(rb_intern(s.c_str())))
{
}

inline Rice::Symbol::
Symbol(Identifier id)
  : Object(ID2SYM(id))
{
}

inline char const * Rice::Symbol::
c_str() const
{
  return to_id().c_str();
}

inline std::string Rice::Symbol::
str() const
{
  return to_id().str();
}

inline Rice::Identifier Rice::Symbol::
to_id() const
{
  return rb_to_id(value());
}

namespace Rice
{
  namespace detail
  {
    template<>
    struct From_Ruby<Symbol>
    {
      static Object convert(VALUE value)
      {
        return Object(value);
      }
    };

    template<>
    struct To_Ruby<Symbol>
    {
      static VALUE convert(Object const& x, bool takeOwnership = false)
      {
        return x.value();
      }
    };
  }
}
