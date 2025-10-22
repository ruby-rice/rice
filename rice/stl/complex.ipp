#include <complex>

namespace Rice::detail
{
  template<typename T>
  struct Type<std::complex<T>>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cComplex;
    }
  };

  template<typename T>
  class To_Ruby<std::complex<T>>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
    {
    }

    VALUE convert(const std::complex<T>& data)
    {
      std::vector<VALUE> args(2);
      args[0] = To_Ruby<T>().convert(data.real());
      args[1] = To_Ruby<T>().convert(data.imag());
      return protect(rb_funcallv, rb_mKernel, rb_intern("Complex"), (int)args.size(), (const VALUE*)args.data());
    }

  private:
    Return* returnInfo_ = nullptr;
  };

  template<typename T>
  class To_Ruby<std::complex<T>&>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
    {
    }

    VALUE convert(const std::complex<T>& data)
    {
      std::vector<VALUE> args(2);
      args[0] = To_Ruby<T>().convert(data.real());
      args[1] = To_Ruby<T>().convert(data.imag());
      return protect(rb_funcallv, rb_mKernel, rb_intern("Complex"), (int)args.size(), (const VALUE*)args.data());
    }

  private:
    Return* returnInfo_ = nullptr;
  };

  template<typename T>
  class From_Ruby<std::complex<T>>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_COMPLEX:
          return Convertible::Exact;
          break;
        default:
          return Convertible::None;
      }
    }

    std::complex<T> convert(VALUE value)
    {
      VALUE real = protect(rb_funcallv, value, rb_intern("real"), 0, (const VALUE*)nullptr);
      VALUE imaginary = protect(rb_funcallv, value, rb_intern("imaginary"), 0, (const VALUE*)nullptr);

      return std::complex<T>(From_Ruby<T>().convert(real), From_Ruby<T>().convert(imaginary));
    }
  };

  template<typename T>
  class From_Ruby<std::complex<T>&>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_COMPLEX:
          return Convertible::Exact;
          break;
        default:
          return Convertible::None;
      }
    }

    std::complex<T>& convert(VALUE value)
    {
      VALUE real = protect(rb_funcallv, value, rb_intern("real"), 0, (const VALUE*)nullptr);
      VALUE imaginary = protect(rb_funcallv, value, rb_intern("imaginary"), 0, (const VALUE*)nullptr);
      this->converted_ = std::complex<T>(From_Ruby<T>().convert(real), From_Ruby<T>().convert(imaginary));

      return this->converted_;
    }

  private:
    std::complex<T> converted_;
  };
}
