#include <complex>

#include "../detail/ruby.hpp"

namespace Rice::detail
{
  template<typename T>
  struct Type<std::complex<T>>
  {
    static bool verify()
    {
      return true;
    }
  };

  template<typename T>
  class To_Ruby<std::complex<T>>
  {
  public:
    VALUE convert(const std::complex<T>& data)
    {
      std::vector<VALUE> args(2);
      args[0] = To_Ruby<T>().convert(data.real());
      args[1] = To_Ruby<T>().convert(data.imag());
      return protect(rb_funcallv, rb_mKernel, rb_intern("Complex"), (int)args.size(), (const VALUE*)args.data());
    }
  };

  template<typename T>
  class From_Ruby<std::complex<T>>
  {
  public:
    std::complex<T> convert(VALUE value)
    {
      VALUE real = protect(rb_funcallv, value, rb_intern("real"), 0, (const VALUE*)nullptr);
      VALUE imaginary = protect(rb_funcallv, value, rb_intern("imaginary"), 0, (const VALUE*)nullptr);

      return std::complex<T>(From_Ruby<T>().convert(real), From_Ruby<T>().convert(imaginary));
    }

    bool is_convertible(VALUE value)
    {
      return rb_type(value) == RUBY_T_COMPLEX;
    }
  };

  template<typename T>
  class From_Ruby<std::complex<T>&>
  {
  public:
    std::complex<T>& convert(VALUE value)
    {
      VALUE real = protect(rb_funcallv, value, rb_intern("real"), 0, (const VALUE*)nullptr);
      VALUE imaginary = protect(rb_funcallv, value, rb_intern("imaginary"), 0, (const VALUE*)nullptr);
      this->converted_ = std::complex<T>(From_Ruby<T>().convert(real), From_Ruby<T>().convert(imaginary));

      return this->converted_;
    }

    bool is_convertible(VALUE value)
    {
      return rb_type(value) == RUBY_T_COMPLEX;
    }

  private:
    std::complex<T> converted_;
  };
}
