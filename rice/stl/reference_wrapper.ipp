#include <functional>

namespace Rice::detail
{
  template<typename T>
  struct Type<std::reference_wrapper<T>>
  {
    constexpr static bool verify()
    {
      return Type<T>::verify();
    }

    static VALUE rubyKlass()
    {
      TypeMapper<T> typeMapper;
      return typeMapper.rubyKlass();
    }
  };

  template<typename T>
  class To_Ruby<std::reference_wrapper<T>>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Arg* arg) : arg_(arg)
    {
    }

    VALUE convert(const std::reference_wrapper<T>& data)
    {
      return To_Ruby<T&>().convert(data.get());
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<typename T>
  class From_Ruby<std::reference_wrapper<T>>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return this->converter_.is_convertible(value);
    }

    std::reference_wrapper<T> convert(VALUE value)
    {
      return this->converter_.convert(value);
    }

  private:
    From_Ruby<T&> converter_;
  };
}
