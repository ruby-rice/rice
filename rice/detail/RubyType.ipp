namespace Rice::detail
{
  template<>
  class RubyType<bool>
  {
  public:
    using FromRuby_T = bool(*)(VALUE);

    static inline FromRuby_T fromRuby = RB_TEST;
    static inline std::set<ruby_value_type> Exact = { RUBY_T_TRUE, RUBY_T_FALSE };
    static inline std::set<ruby_value_type> Castable = { RUBY_T_NIL };
    static inline std::set<ruby_value_type> Narrowable = {  };
    static inline std::string packTemplate = "not supported";

    static rb_data_type_t* ruby_data_type()
    {
      return &ruby_data_type_;
    }

    static VALUE klass()
    {
      if (klass_ == Qnil)
      {
        std::string name = detail::typeName(typeid(RubyType<bool>));
        std::string klassName = detail::makeClassName(name);
        Identifier id(klassName);
        Module rb_mRice = define_module("Rice");
        Class klass = define_class_under(rb_mRice, id);
        klass.undef_creation_funcs();
        klass_ = klass;
      }
      return klass_;
    }

  private:
    static inline rb_data_type_t ruby_data_type_ = { "bool*" };
    static inline VALUE klass_ = Qnil;
  };

  template<>
  class RubyType<char>
  {
  public:
    using FromRuby_T = char(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2char_inline;
    static inline std::set<ruby_value_type> Exact = { };
    static inline std::set<ruby_value_type> Castable = { RUBY_T_STRING };
    static inline std::set<ruby_value_type> Narrowable = { RUBY_T_FIXNUM };
    static inline std::string packTemplate = CHAR_MIN < 0 ? "c*" : "C*";

    static rb_data_type_t* ruby_data_type()
    {
      return &ruby_data_type_;
    }

    static VALUE klass()
    {
      if (klass_ == Qnil)
      {
        std::string name = detail::typeName(typeid(RubyType<char>));
        std::string klassName = detail::makeClassName(name);
        Identifier id(klassName);
        Module rb_mRice = define_module("Rice");
        Class klass = define_class_under(rb_mRice, id);
        klass.undef_creation_funcs();
        klass_ = klass;
      }
      return klass_;
    }

  private:
    static inline rb_data_type_t ruby_data_type_ = { "char*" };
    static inline VALUE klass_ = Qnil;
  };

  template<>
  class RubyType<signed char>
  {
  public:
    // Hack - need to later typecast
    using FromRuby_T = char(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2char_inline;
    static inline std::set<ruby_value_type> Exact = { };
    static inline std::set<ruby_value_type> Castable = { RUBY_T_STRING };
    static inline std::set<ruby_value_type> Narrowable = { RUBY_T_FIXNUM };
    static inline std::string packTemplate = "c*";

    static rb_data_type_t* ruby_data_type()
    {
      return &ruby_data_type_;
    }

    static VALUE klass()
    {
      if (klass_ == Qnil)
      {
        std::string name = detail::typeName(typeid(RubyType<signed char>));
        std::string klassName = detail::makeClassName(name);
        Identifier id(klassName);
        Module rb_mRice = define_module("Rice");
        Class klass = define_class_under(rb_mRice, id);
        klass.undef_creation_funcs();
        klass_ = klass;
      }
      return klass_;
    }

  private:
    static inline rb_data_type_t ruby_data_type_ = { "signed char*" };
    static inline VALUE klass_ = Qnil;
  };

  template<>
  class RubyType<unsigned char>
  {
  public:
    // Hack - need to later typecast, although char's in ruby are unsigned
    using FromRuby_T = char(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2char_inline;
    static inline std::set<ruby_value_type> Exact = { };
    static inline std::set<ruby_value_type> Castable = { RUBY_T_STRING };
    static inline std::set<ruby_value_type> Narrowable = { RUBY_T_FIXNUM };
    static inline std::string packTemplate = "C*";

    static rb_data_type_t* ruby_data_type()
    {
      return &ruby_data_type_;
    }

    static VALUE klass()
    {
      if (klass_ == Qnil)
      {
        std::string name = detail::typeName(typeid(RubyType<unsigned char>));
        std::string klassName = detail::makeClassName(name);
        Identifier id(klassName);
        Module rb_mRice = define_module("Rice");
        Class klass = define_class_under(rb_mRice, id);
        klass.undef_creation_funcs();
        klass_ = klass;
      }
      return klass_;
    }

  private:
    static inline rb_data_type_t ruby_data_type_ = { "unsigned char*" };
    static inline VALUE klass_ = Qnil;
  };

  template<>
  class RubyType<short>
  {
  public:
    using FromRuby_T = short(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2short_inline;
    static inline std::set<ruby_value_type> Exact = { };
    static inline std::set<ruby_value_type> Castable = { };
    static inline std::set<ruby_value_type> Narrowable = { RUBY_T_FIXNUM };
    static inline std::string packTemplate = "s*";

    static rb_data_type_t* ruby_data_type()
    {
      return &ruby_data_type_;
    }

    static VALUE klass()
    {
      if (klass_ == Qnil)
      {
        std::string name = detail::typeName(typeid(RubyType<short>));
        std::string klassName = detail::makeClassName(name);
        Identifier id(klassName);
        Module rb_mRice = define_module("Rice");
        Class klass = define_class_under(rb_mRice, id);
        klass.undef_creation_funcs();
        klass_ = klass;
      }
      return klass_;
    }

  private:
    static inline rb_data_type_t ruby_data_type_ = { "short*" };
    static inline VALUE klass_ = Qnil;
  };

  template<>
  class RubyType<unsigned short>
  {
  public:
    using FromRuby_T = unsigned short(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2ushort;
    static inline std::set<ruby_value_type> Exact = { };
    static inline std::set<ruby_value_type> Castable = { };
    static inline std::set<ruby_value_type> Narrowable = { RUBY_T_FIXNUM };
    static inline std::string packTemplate = "S*";

    static rb_data_type_t* ruby_data_type()
    {
      return &ruby_data_type_;
    }

    static VALUE klass()
    {
      if (klass_ == Qnil)
      {
        std::string name = detail::typeName(typeid(RubyType<unsigned short>));
        std::string klassName = detail::makeClassName(name);
        Identifier id(klassName);
        Module rb_mRice = define_module("Rice");
        Class klass = define_class_under(rb_mRice, id);
        klass.undef_creation_funcs();
        klass_ = klass;
      }
      return klass_;
    }

  private:
    static inline rb_data_type_t ruby_data_type_ = { "unsigned short*" };
    static inline VALUE klass_ = Qnil;
  };

  template<>
  class RubyType<int>
  {
  public:
    using FromRuby_T = int(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2int_inline;
    static inline std::set<ruby_value_type> Exact = { RUBY_T_FIXNUM };
    static inline std::set<ruby_value_type> Castable = { };
    // We allow bignum to integer because Ruby switches to bignum at about 2 billion on 64 bit systems,
    // while int can go up to 4 billion
    static inline std::set<ruby_value_type> Narrowable = { RUBY_T_BIGNUM };
    static inline std::string packTemplate = "i*";

    static rb_data_type_t* ruby_data_type()
    {
      return &ruby_data_type_;
    }

    static VALUE klass()
    {
      if (klass_ == Qnil)
      {
        std::string name = detail::typeName(typeid(RubyType<int>));
        std::string klassName = detail::makeClassName(name);
        Identifier id(klassName);
        Module rb_mRice = define_module("Rice");
        Class klass = define_class_under(rb_mRice, id);
        klass.undef_creation_funcs();
        klass_ = klass;
      }
      return klass_;
    }

  private:
    static inline rb_data_type_t ruby_data_type_ = {"int*"};
    static inline VALUE klass_ = Qnil;
  };

  template<>
  class RubyType<unsigned int>
  {
  public:
    using FromRuby_T = unsigned int(*)(VALUE);

    static inline FromRuby_T fromRuby = RB_NUM2UINT;
    static inline std::set<ruby_value_type> Exact = { RUBY_T_FIXNUM };
    static inline std::set<ruby_value_type> Castable = { };
    // We allow bignum to integer because Ruby switches to bignum at about 2 billion on 64 bit systems,
    // while int can go up to 4 billion
    static inline std::set<ruby_value_type> Narrowable = { RUBY_T_BIGNUM };
    static inline std::string packTemplate = "I*";

    static rb_data_type_t* ruby_data_type()
    {
      return &ruby_data_type_;
    }

    static VALUE klass()
    {
      if (klass_ == Qnil)
      {
        std::string name = detail::typeName(typeid(RubyType<unsigned int>));
        std::string klassName = detail::makeClassName(name);
        Identifier id(klassName);
        Module rb_mRice = define_module("Rice");
        Class klass = define_class_under(rb_mRice, id);
        klass.undef_creation_funcs();
        klass_ = klass;
      }
      return klass_;
    }

  private:
    static inline rb_data_type_t ruby_data_type_ = { "unsigned int*" };
    static inline VALUE klass_ = Qnil;
  };

  template<>
  class RubyType<long>
  {
  public:
    using FromRuby_T = long(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2long_inline;
    static inline std::set<ruby_value_type> Exact = { RUBY_T_FIXNUM };
    static inline std::set<ruby_value_type> Castable = { };
    static inline std::set<ruby_value_type> Narrowable = { RUBY_T_BIGNUM };
    static inline std::string packTemplate = "l_*";

    static rb_data_type_t* ruby_data_type()
    {
      return &ruby_data_type_;
    }

    static VALUE klass()
    {
      if (klass_ == Qnil)
      {
        std::string name = detail::typeName(typeid(RubyType<long>));
        std::string klassName = detail::makeClassName(name);
        Identifier id(klassName);
        Module rb_mRice = define_module("Rice");
        Class klass = define_class_under(rb_mRice, id);
        klass.undef_creation_funcs();
        klass_ = klass;
      }
      return klass_;
    }

  private:
    static inline rb_data_type_t ruby_data_type_ = { "long*" };
    static inline VALUE klass_ = Qnil;
  };

  template<>
  class RubyType<unsigned long>
  {
  public:
    using FromRuby_T = unsigned long(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2ulong_inline;
    static inline std::set<ruby_value_type> Exact = { RUBY_T_FIXNUM  };
    static inline std::set<ruby_value_type> Castable = { };
    static inline std::set<ruby_value_type> Narrowable = { RUBY_T_BIGNUM};
    static inline std::string packTemplate = "L_*";

    static rb_data_type_t* ruby_data_type()
    {
      return &ruby_data_type_;
    }

    static VALUE klass()
    {
      if (klass_ == Qnil)
      {
        std::string name = detail::typeName(typeid(RubyType<unsigned long>));
        std::string klassName = detail::makeClassName(name);
        Identifier id(klassName);
        Module rb_mRice = define_module("Rice");
        Class klass = define_class_under(rb_mRice, id);
        klass.undef_creation_funcs();
        klass_ = klass;
      }
      return klass_;
    }

  private:
    static inline rb_data_type_t ruby_data_type_ = { "unsigned long*" };
    static inline VALUE klass_ = Qnil;
  };

  template<>
  class RubyType<long long>
  {
  public:
    using FromRuby_T = long long(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2ll_inline;
    static inline std::set<ruby_value_type> Exact = { RUBY_T_FIXNUM, RUBY_T_BIGNUM };
    static inline std::set<ruby_value_type> Castable = { };
    static inline std::set<ruby_value_type> Narrowable = { };
    static inline std::string packTemplate = "q_*";

    static rb_data_type_t* ruby_data_type()
    {
      return &ruby_data_type_;
    }

    static VALUE klass()
    {
      if (klass_ == Qnil)
      {
        std::string name = detail::typeName(typeid(RubyType<long long>));
        std::string klassName = detail::makeClassName(name);
        Identifier id(klassName);
        Module rb_mRice = define_module("Rice");
        Class klass = define_class_under(rb_mRice, id);
        klass.undef_creation_funcs();
        klass_ = klass;
      }
      return klass_;
    }

  private:
    static inline rb_data_type_t ruby_data_type_ = { "long long*" };
    static inline VALUE klass_ = Qnil;
  };

  template<>
  class RubyType<unsigned long long>
  {
  public:
    using FromRuby_T = unsigned long long(*)(VALUE);

    static inline FromRuby_T fromRuby = RB_NUM2ULL;
    static inline std::set<ruby_value_type> Exact = { RUBY_T_FIXNUM, RUBY_T_BIGNUM };
    static inline std::set<ruby_value_type> Castable = { };
    static inline std::set<ruby_value_type> Narrowable = { };
    static inline std::string packTemplate = "Q_*";

    static rb_data_type_t* ruby_data_type()
    {
      return &ruby_data_type_;
    }

    static VALUE klass()
    {
      if (klass_ == Qnil)
      {
        std::string name = detail::typeName(typeid(RubyType<unsigned long long>));
        std::string klassName = detail::makeClassName(name);
        Identifier id(klassName);
        Module rb_mRice = define_module("Rice");
        Class klass = define_class_under(rb_mRice, id);
        klass.undef_creation_funcs();
        klass_ = klass;
      }
      return klass_;
    }

  private:
    static inline rb_data_type_t ruby_data_type_ = { "unsigned long long*" };
    static inline VALUE klass_ = Qnil;
  };

  template<>
  class RubyType<float>
  {
  public:
    using FromRuby_T = double(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2dbl;
    static inline std::set<ruby_value_type> Exact = { };
    static inline std::set<ruby_value_type> Castable = { RUBY_T_FIXNUM };
    static inline std::set<ruby_value_type> Narrowable = { RUBY_T_FLOAT };
    static inline std::string packTemplate = "f*";

    static rb_data_type_t* ruby_data_type()
    {
      return &ruby_data_type_;
    }

    static VALUE klass()
    {
      if (klass_ == Qnil)
      {
        std::string name = detail::typeName(typeid(RubyType<float>));
        std::string klassName = detail::makeClassName(name);
        Identifier id(klassName);
        Module rb_mRice = define_module("Rice");
        Class klass = define_class_under(rb_mRice, id);
        klass.undef_creation_funcs();
        klass_ = klass;
      }
      return klass_;
    }

  private:
    static inline rb_data_type_t ruby_data_type_ = { "float*" };
    static inline VALUE klass_ = Qnil;
  };

  template<>
  class RubyType<double>
  {
  public:
    using FromRuby_T = double(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2dbl;
    static inline std::set<ruby_value_type> Exact = { RUBY_T_FLOAT };
    static inline std::set<ruby_value_type> Castable = { RUBY_T_FIXNUM, RUBY_T_BIGNUM };
    static inline std::set<ruby_value_type> Narrowable = { };
    static inline std::string packTemplate = "d*";

    static rb_data_type_t* ruby_data_type()
    {
      return &ruby_data_type_;
    }

    static VALUE klass()
    {
      if (klass_ == Qnil)
      {
        std::string name = detail::typeName(typeid(RubyType<double>));
        std::string klassName = detail::makeClassName(name);
        Identifier id(klassName);
        Module rb_mRice = define_module("Rice");
        Class klass = define_class_under(rb_mRice, id);
        klass.undef_creation_funcs();
        klass_ = klass;
      }
      return klass_;
    }

  private:
    static inline rb_data_type_t ruby_data_type_ = { "double*" };
    static inline VALUE klass_ = Qnil;
  };

  template<>
  class RubyType<void>
  {
  public:
    static inline std::set<ruby_value_type> Exact = { };
    static inline std::set<ruby_value_type> Castable = { };
    static inline std::set<ruby_value_type> Narrowable = { };

    static rb_data_type_t* ruby_data_type()
    {
      return &ruby_data_type_;
    }

    static VALUE klass()
    {
      if (klass_ == Qnil)
      {
        std::string name = detail::typeName(typeid(RubyType<void>));
        std::string klassName = detail::makeClassName(name);
        Identifier id(klassName);
        Module rb_mRice = define_module("Rice");
        Class klass = define_class_under(rb_mRice, id);
        klass.undef_creation_funcs();
        klass_ = klass;
      }
      return klass_;
    }

  private:
    static inline rb_data_type_t ruby_data_type_ = { "void*" };
    static inline VALUE klass_ = Qnil;
  };
}