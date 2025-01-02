namespace Rice
{
  inline Encoding Encoding::utf8()
  {
    return Encoding(rb_utf8_encoding());
  }

  inline Encoding::Encoding(rb_encoding* encoding) : encoding_(encoding)
  {
  }
}

/*namespace Rice::detail
{
  template<>
  struct Type<Encoding>
  {
    static bool verify()
    {
      return true;
    }
  };
  
  template<>
  class To_Ruby<Encoding>
  {
  public:
    VALUE convert(const Encoding& encoding)
    {
    //  return x.value();
    }
  };

  template<>
  class From_Ruby<Encoding>
  {
  public:
    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_SYMBOL:
          return Convertible::Exact;
          break;
      case RUBY_T_STRING:
          return Convertible::TypeCast;
          break;
        default:
          return Convertible::None;
        }
    }

    Encoding convert(VALUE value)
    {
     // return Symbol(value);
    }
  };
}
*/