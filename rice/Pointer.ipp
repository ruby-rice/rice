namespace Rice
{
  template<typename T>
  inline Data_Type<Pointer<T>> define_pointer(std::string klassName)
  {
    using Pointer_T = Pointer<T>;
    using Data_Type_T = Data_Type<Pointer_T>;

    if (klassName.empty())
    {
      detail::TypeMapper<Pointer_T> typeMapper;
      klassName = typeMapper.rubyName();
    }

    Module rb_mRice = define_module("Rice");

    if (Data_Type_T::check_defined(klassName, rb_mRice))
    {
      return Data_Type_T();
    }

    Data_Type<Pointer<T>> result = define_class_under<Pointer_T>(rb_mRice, klassName).
      define_method("buffer", [](VALUE self) -> Buffer<T>
      {
        T* ptr = detail::unwrap<T>(self, Data_Type<Pointer<T>>::ruby_data_type(), false);
        Buffer<T> buffer(ptr);
        return buffer;
      }, Arg("self").setValue());

    return result;
  }
}