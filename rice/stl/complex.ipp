#include <complex>

#include "../detail/ruby.hpp"

namespace Rice::detail
{
  template<typename T>
  struct is_builtin<std::complex<T>> : public std::true_type {};

  template<typename T>
  class From_Ruby<std::complex<T>>
  {
  public:
    std::complex<T> convert(VALUE value)
    {
      VALUE real = protect(rb_funcall2, value, rb_intern("real"), 0, (const VALUE*)nullptr);
      VALUE imaginary = protect(rb_funcall2, value, rb_intern("imaginary"), 0, (const VALUE*)nullptr);

      return std::complex<T>(From_Ruby<T>().convert(real), From_Ruby<T>().convert(imaginary));
    }
  };

  template<typename T>
  class From_Ruby<std::complex<T>&>
  {
  public:
    std::complex<T>& convert(VALUE value)
    {
      VALUE real = protect(rb_funcall2, value, rb_intern("real"), 0, (const VALUE*)nullptr);
      VALUE imaginary = protect(rb_funcall2, value, rb_intern("imaginary"), 0, (const VALUE*)nullptr);
      this->converted_ = std::complex<T>(From_Ruby<T>().convert(real), From_Ruby<T>().convert(imaginary));

      return this->converted_;
    }

  private:
    std::complex<T> converted_;
  };

  template<typename T>
  struct To_Ruby<std::complex<T>>
  {
    static VALUE convert(const std::complex<T>& data, bool takeOwnership = false)
    {
      std::vector<VALUE> args(2);
      args[0] = To_Ruby<T>::convert(data.real());
      args[1] = To_Ruby<T>::convert(data.imag());
      return protect(rb_funcall2, rb_mKernel, rb_intern("Complex"), (int)args.size(), (const VALUE*)args.data());
    }
  };
}