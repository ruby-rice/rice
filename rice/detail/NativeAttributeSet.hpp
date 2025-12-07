#ifndef Rice__detail__Native_Attribute_Set__hpp_
#define Rice__detail__Native_Attribute_Set__hpp_

RICE_BEGIN_NAMESPACE
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
      // Register attribute getter/setter with Ruby
      static void define(VALUE klass, std::string name, Attribute_T attribute);

    public:
      // Disallow creating/copying/moving
      NativeAttributeSet() = delete;
      NativeAttributeSet(const NativeAttribute_T&) = delete;
      NativeAttributeSet(NativeAttribute_T&&) = delete;
      void operator=(const NativeAttribute_T&) = delete;
      void operator=(NativeAttribute_T&&) = delete;

      Resolved matches(size_t argc, const VALUE* argv) override;
      VALUE operator()(size_t argc, const VALUE* argv, VALUE self) override;
      std::string toString() override;

      NativeKind kind() override;
      VALUE returnKlass() override;

    protected:
      NativeAttributeSet(VALUE klass, std::string name, Attribute_T attr);

    private:
      VALUE klass_;
      Attribute_T attribute_;
    };
  } // detail
RICE_END_NAMESPACE

#endif // Rice__detail__Native_Attribute_Set__hpp_
