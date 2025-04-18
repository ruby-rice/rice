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
  };

  template<>
  class RubyType<void>
  {
  public:
    static inline std::set<ruby_value_type> Exact = { };
    static inline std::set<ruby_value_type> Castable = { };
    static inline std::set<ruby_value_type> Narrowable = { };
  };
}

namespace Rice::detail
{
  template<typename T>
  inline Data_Type<T> define_ruby_type()
  {
    std::string name = detail::typeName(typeid(T*));
    std::string klassName = detail::rubyClassName(name);
    Identifier id(klassName);

    Module rb_mRice = define_module("Rice");
    return define_class_under<T>(rb_mRice, id);
  }

  inline void define_ruby_types()
  {
    define_ruby_type<bool>();
    define_ruby_type<char>();
    define_ruby_type<signed char>();
    define_ruby_type<unsigned char>();
    define_ruby_type<short>();
    define_ruby_type<unsigned short>();
    define_ruby_type<int>();
    define_ruby_type<unsigned int>();
    define_ruby_type<long>();
    define_ruby_type<unsigned long>();
    define_ruby_type<long long>();
    define_ruby_type<unsigned long long>();
    define_ruby_type<float>();
    define_ruby_type<double>();
    define_ruby_type<void>();
  }
}