RICE_BEGIN_NAMESPACE
  inline Symbol::Symbol(VALUE value) : Object(value)
  {
    detail::protect(rb_check_type, value, (int)T_SYMBOL);
  }

  inline Symbol::Symbol(Object value) : Object(value)
  {
    detail::protect(rb_check_type, value.value(), (int)T_SYMBOL);
  }

  inline Symbol::Symbol(char const* s)
    : Object(detail::protect(rb_id2sym, detail::protect(rb_intern, s)))
  {
  }

  inline Symbol::Symbol(std::string const& s)
    : Object(detail::protect(rb_id2sym, detail::protect(rb_intern2, s.c_str(), (long)s.length())))
  {
  }

  inline Symbol::Symbol(std::string_view const& view)
    : Object(detail::protect(rb_id2sym, detail::protect(rb_intern2, view.data(), (long)view.length())))
  {
  }

  inline Symbol::Symbol(Identifier id) : Object(detail::protect(rb_id2sym, id))
  {
  }

  inline char const* Symbol::c_str() const
  {
    return to_id().c_str();
  }

  inline std::string Symbol::str() const
  {
    return to_id().str();
  }

  inline Identifier Symbol::to_id() const
  {
    return rb_to_id(value());
  }
RICE_END_NAMESPACE

RICE_DETAIL_BEGIN_NAMESPACE
  template<>
  struct Type<Symbol>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cSymbol;
    }
  };
  
  template<>
  class To_Ruby<Symbol>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Arg* arg) : arg_(arg)
    {
    }

    VALUE convert(Symbol const& x)
    {
      return x.value();
    }

  private:
    Arg* arg_ = nullptr;
  };


  template<>
  class To_Ruby<Symbol&>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Arg* arg) : arg_(arg)
    {
    }

    VALUE convert(Symbol const& x)
    {
      return x.value();
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<Symbol>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_SYMBOL:
          return Convertible::Exact;
          break;
      case RUBY_T_STRING:
          return Convertible::Cast;
          break;
        default:
          return Convertible::None;
        }
    }

    Symbol convert(VALUE value)
    {
      return Symbol(value);
    }

  private:
    Arg* arg_ = nullptr;
  };
RICE_DETAIL_END_NAMESPACE
