
// Ruby 2.7 now includes a similarly named macro that uses templates to
// pick the right overload for the underlying function. That doesn't work
// for our cases because we are using this method dynamically and get a
// compilation error otherwise. This removes the macro and lets us fall
// back to the C-API underneath again.
#undef rb_define_method_id

namespace Rice
{
  namespace detail
  {
    // Effective Java (2nd edition)
    // https://stackoverflow.com/a/2634715
    inline size_t MethodData::key(VALUE klass, ID id)
    {
      if (rb_type(klass) == T_ICLASS)
      {
        klass = rb_class_of(klass);
      }

      uint32_t prime = 53;
      return (prime + klass) * prime + id;
    }

    template <typename Return_T>
    inline Return_T MethodData::data()
    {
      ID id;
      VALUE klass;
      if (!rb_frame_method_id_and_class(&id, &klass))
      {
        rb_raise(rb_eRuntimeError, "Cannot get method id and class for function");
      }

      auto iter = methodWrappers_.find(key(klass, id));
      if (iter == methodWrappers_.end())
      {
        rb_raise(rb_eRuntimeError, "Could not find data for klass and method id");
      }

      std::any data = iter->second;
      return std::any_cast<Return_T>(data);
    }

    inline void
    MethodData::define_method(VALUE klass, ID id, VALUE(*cfunc)(ANYARGS), int arity, std::any data)
    {
      // Define the method
      rb_define_method_id(klass, id, cfunc, arity);

      // Now store data about it
      methodWrappers_[key(klass, id)] = data;
    }
  }
}
