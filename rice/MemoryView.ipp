namespace Rice
{
}

namespace Rice::detail
{
  template<>
  class To_Ruby<unsigned char**>
  {
  public:
    VALUE convert(unsigned char** x)
    {
      std::runtime_error("To_Ruby unsigned char** is not implemented yet");
      return Qnil;
    }
  };

  template<>
  class From_Ruby<unsigned char**>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      std::runtime_error("From_Ruby unsigned char** is not implemented yet");
      return Convertible::None;
    }

    unsigned char** convert(VALUE value)
    {
      std::runtime_error("From_Ruby unsigned char** is not implemented yet");
      return nullptr;
    }

  private:
    Arg* arg_ = nullptr;
  };
}
