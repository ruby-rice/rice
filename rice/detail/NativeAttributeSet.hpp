#ifndef Rice__detail__Native_Attribute_Set__hpp_
#define Rice__detail__Native_Attribute_Set__hpp_

namespace Rice
{
  namespace detail
  {
    template<typename Attribute_T>
    class NativeAttributeSet: Native
    {
    public:
      using NativeAttribute_T = NativeAttributeSet<Attribute_T>;
      using Attr_T = typename attribute_traits<Attribute_T>::attr_type;
      using T_Unqualified = remove_cv_recursive_t<Attr_T>;
      using Receiver_T = typename attribute_traits<Attribute_T>::class_type;

    public:
      // Register attribute setter with Ruby
      template<typename...Arg_Ts>
      static void define(VALUE klass, std::string name, Attribute_T attribute, Arg_Ts&...args);

    public:
      // Disallow creating/copying/moving
      NativeAttributeSet() = delete;
      NativeAttributeSet(const NativeAttribute_T&) = delete;
      NativeAttributeSet(NativeAttribute_T&&) = delete;
      void operator=(const NativeAttribute_T&) = delete;
      void operator=(NativeAttribute_T&&) = delete;

      Resolved matches(std::map<std::string, VALUE>& values) override;
      VALUE operator()(std::map<std::string, VALUE>& values, VALUE self) override;
      std::string toString() override;

      NativeKind kind() override;
      VALUE returnKlass() override;

    protected:
      NativeAttributeSet(VALUE klass, std::string name, Attribute_T attr, std::unique_ptr<Parameter<T_Unqualified>> parameter);

    private:
      VALUE klass_;
      Attribute_T attribute_;
      std::unique_ptr<Parameter<T_Unqualified>> parameter_;
    };
  } // detail
} // Rice

#endif // Rice__detail__Native_Attribute_Set__hpp_
