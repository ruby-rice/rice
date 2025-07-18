#include <rice/detail/Native.hpp>
#include <rice/detail/NativeRegistry.hpp>

using namespace Rice;

extern "C"
inline void Init_Native_Registry()
{
  Module rb_mRice = define_module("Rice");

  Rice::define_class_under<detail::NativeRegistry>(rb_mRice, "NativeRegistry").
    define_method("lookup", [](detail::NativeRegistry& self, VALUE klass) -> Array
    {
      Array result;

      const std::vector<detail::Native*> natives = self.lookup(klass);
      for (detail::Native* native : natives)
      {
        result.push(native, false);
      }

      return result;
    }, Arg("klass").setValue());
}
