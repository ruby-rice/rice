// Rice saves types either as the intrinsic type (MyObject) or pointer (MyObject*).
// It strips out references, const and volatile to avoid an explosion of template classes.
// Pointers are used for C function pointers used in callbacks and for the Buffer class.
namespace Rice::detail
{
  // ------ Type ----------------
  template<typename T>
  inline bool Type<T>::verify()
  {
    return Registries::instance.types.verify<intrinsic_type<T>>();
  }

  template<typename T>
  inline bool Type<T&>::verify()
  {
    return Type<T>::verify();
  }

  template<typename T>
  inline bool Type<T&&>::verify()
  {
    return Type<T>::verify();
  }

  template<typename T>
  inline bool Type<T*>::verify()
  {
    return Type<T>::verify();
  }

  template<typename T>
  inline bool Type<T**>::verify()
  {
    return Type<T>::verify();
  }

  template<typename T>
  void verifyType()
  {
    Type<remove_cv_recursive_t<T>>::verify();
  }

  template<typename Tuple_T, size_t...Is>
  void verifyTypesImpl(std::index_sequence<Is...> indexes)
  {
    (verifyType<std::tuple_element_t<Is, Tuple_T>>(), ...);
  }

  template<typename Tuple_T>
  void verifyTypes()
  {
    std::make_index_sequence<std::tuple_size_v<Tuple_T>> indexes;
    verifyTypesImpl<Tuple_T>(indexes);
  }

  // ---------- RubyKlass ------------
  // Helper template to see if the method rubyKlass is defined on a Type specialization
  template<typename, typename = std::void_t<>>
  struct has_ruby_klass : std::false_type
  {
  };

  template<typename T>
  struct has_ruby_klass<T, std::void_t<decltype(T::rubyKlass())>> : std::true_type
  {
  };
}
