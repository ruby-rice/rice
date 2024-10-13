
// Ruby 2.7 now includes a similarly named macro that uses templates to
// pick the right overload for the underlying function. That doesn't work
// for our cases because we are using this method dynamically and get a
// compilation error otherwise. This removes the macro and lets us fall
// back to the C-API underneath again.
#undef rb_define_method_id

#include "RubyFunction.hpp"

namespace Rice::detail
{
  // Effective Java (2nd edition)
  // https://stackoverflow.com/a/2634715
  inline size_t NativeRegistry::key(VALUE klass, ID id)
  {
    uint32_t prime = 53;
    return (prime + klass) * prime + id;
  }

  inline void NativeRegistry::add(VALUE klass, ID method_id, std::any callable)
  {
    if (rb_type(klass) == T_ICLASS)
    {
      klass = detail::protect(rb_class_of, klass);
    }

    auto range = this->natives_.equal_range(key(klass, method_id));
    for (auto it = range.first; it != range.second; ++it)
    {
      const auto [k, m, d] = it->second;

      if (k == klass && m == method_id)
      {
        std::get<2>(it->second) = callable;
        return;
      }
    }

    this->natives_.emplace(std::make_pair(key(klass, method_id), std::make_tuple(klass, method_id, callable)));
  }

  template <typename Return_T>
  inline Return_T NativeRegistry::lookup()
  {
    ID method_id;
    VALUE klass;
    if (!rb_frame_method_id_and_class(&method_id, &klass))
    {
      rb_raise(rb_eRuntimeError, "Cannot get method id and class for function");
    }

    return this->lookup<Return_T>(klass, method_id);
  }

  template <typename Return_T>
  inline Return_T NativeRegistry::lookup(VALUE klass, ID method_id)
  {
    if (rb_type(klass) == T_ICLASS)
    {
      klass = detail::protect(rb_class_of, klass);
    }

    auto range = this->natives_.equal_range(key(klass, method_id));
    for (auto it = range.first; it != range.second; ++it)
    {
      const auto [k, m, d] = it->second;

      if (k == klass && m == method_id)
      {
        auto* ptr = std::any_cast<Return_T>(&d);
        if (!ptr)
        {
          rb_raise(rb_eRuntimeError, "Unexpected return type for %s#%s", rb_class2name(klass), rb_id2name(method_id));
        }
        return *ptr;
      }
    }

    rb_raise(rb_eRuntimeError, "Could not find data for klass and method id");
  }
}
