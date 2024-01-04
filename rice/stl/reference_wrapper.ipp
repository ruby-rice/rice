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
  };

  template<typename T>
  class To_Ruby<std::reference_wrapper<T>>
  {
  public:
    VALUE convert(const std::reference_wrapper<T>& data, bool takeOwnership = false)
    {
      return To_Ruby<T&>().convert(data.get());
    }
  };

  template<typename T>
  class From_Ruby<std::reference_wrapper<T>>
  {
  public:
    bool is_convertible(VALUE value)
    {
      return true;
    }

    std::reference_wrapper<T> convert(VALUE value)
    {
      return this->converter_.convert(value);
    }

  private:
    From_Ruby<T&> converter_;
  };
}
