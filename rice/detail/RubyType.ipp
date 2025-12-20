namespace Rice::detail
{
  template<>
  class RubyType<bool>
  {
  public:
    using FromRuby_T = bool(*)(VALUE);

    static inline FromRuby_T fromRuby = RB_TEST;
    static inline std::string packTemplate = "not supported";
    static inline std::string name = "bool";
  };

  template<>
  class RubyType<char>
  {
  public:
    using FromRuby_T = char(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2char_inline;
    static inline std::string packTemplate = CHAR_MIN < 0 ? "c*" : "C*";
    static inline std::string name = "String";
  };

  template<>
  class RubyType<signed char>
  {
  public:
    using FromRuby_T =  char(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2char_inline;
    static inline std::string packTemplate = "c*";
    static inline std::string name = "String";
  };

  template<>
  class RubyType<unsigned char>
  {
  public:
    using FromRuby_T = char(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2char_inline;
    static inline std::string packTemplate = "C*";
    static inline std::string name = "String";
  };

  template<>
  class RubyType<short>
  {
  public:
    using FromRuby_T = short(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2short_inline;
    static inline std::string packTemplate = "s*";
    static inline std::string name = "Integer";
  };

  template<>
  class RubyType<unsigned short>
  {
  public:
    using FromRuby_T = unsigned short(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2ushort;
    static inline std::string packTemplate = "S*";
    static inline std::string name = "Integer";
  };

  template<>
  class RubyType<int>
  {
  public:
    using FromRuby_T = int(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2int_inline;
    static inline std::string packTemplate = "i*";
    static inline std::string name = "Integer";
  };

  template<>
  class RubyType<unsigned int>
  {
  public:
    using FromRuby_T = unsigned int(*)(VALUE);

    static inline FromRuby_T fromRuby = RB_NUM2UINT;
    static inline std::string packTemplate = "I*";
    static inline std::string name = "Integer";
  };

  template<>
  class RubyType<long>
  {
  public:
    using FromRuby_T = long(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2long_inline;
    static inline std::string packTemplate = "l_*";
    static inline std::string name = "Integer";
  };

  template<>
  class RubyType<unsigned long>
  {
  public:
    using FromRuby_T = unsigned long(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2ulong_inline;
    static inline std::string packTemplate = "L_*";
    static inline std::string name = "Integer";
  };

  template<>
  class RubyType<long long>
  {
  public:
    using FromRuby_T = long long(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2ll_inline;
    static inline std::string packTemplate = "q_*";
    static inline std::string name = "Integer";
  };

  template<>
  class RubyType<unsigned long long>
  {
  public:
    using FromRuby_T = unsigned long long(*)(VALUE);

    static inline FromRuby_T fromRuby = RB_NUM2ULL;
    static inline std::string packTemplate = "Q_*";
    static inline std::string name = "Integer";
  };

  template<>
  class RubyType<float>
  {
  public:
    using FromRuby_T = double(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2dbl;
    static inline std::string packTemplate = "f*";
    static inline std::string name = "Float";
  };

  template<>
  class RubyType<double>
  {
  public:
    using FromRuby_T = double(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2dbl;
    static inline std::string packTemplate = "d*";
    static inline std::string name = "Float";
  };

  template<>
  class RubyType<void>
  {
  public:
    static inline std::string name = "void";
  };
}
