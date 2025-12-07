RICE_USE_NAMESPACE

extern "C"
inline void Init_Native_Registry()
{
  Module rb_mRice = RICE_DEFINE_MODULE_RICE;

  define_class_under<detail::NativeRegistry>(rb_mRice, "NativeRegistry").
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
