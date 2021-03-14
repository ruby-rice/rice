#include "../detail/to_ruby.hpp"

namespace Rice::detail
{
  template<typename T>
  struct From_Ruby<std::vector<T>>
  {
    static std::vector<T> convert(VALUE x)
    {
      auto len = RARRAY_LEN(x);
      std::vector<T> vec;
      vec.reserve(len);
      for (long i = 0; i < len; i++) {
        vec.push_back(From_Ruby<T>::convert(rb_ary_entry(x, i)));
      }
      return vec;
    }
  };

  template<typename T>
  struct To_Ruby<std::vector<T>>
  {
    static VALUE convert(std::vector<T> const & x, bool takeOwnership = false)
    {
      auto a = rb_ary_new2(x.size());
      for (const auto& v : x) {
        rb_ary_push(a, To_Ruby<T>::convert(v, takeOwnership));
      }
      return a;
    }
  };
}
