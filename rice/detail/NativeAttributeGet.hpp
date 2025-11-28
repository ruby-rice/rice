#ifndef Rice__detail__Native_Attribute_Get__hpp_
#define Rice__detail__Native_Attribute_Get__hpp_

namespace Rice
{
  enum class AttrAccess
  {
    ReadWrite,
    Read,
    Write
  };

  namespace detail
  {
    template<typename Attribute_T>
    class NativeAttributeGet: Native
    {
    public:
      using NativeAttribute_T = NativeAttributeGet<Attribute_T>;

      using Attr_T = typename attribute_traits<Attribute_T>::attr_type;
      using Receiver_T = typename attribute_traits<Attribute_T>::class_type;
      using To_Ruby_T = remove_cv_recursive_t<Attr_T>;

    public:
      // Register attribute getter with Ruby
      template<typename...Arg_Ts>
      static void define(VALUE klass, std::string name, Attribute_T attribute, Arg_Ts&...args);

    public:
      // Disallow creating/copying/moving
      NativeAttributeGet() = delete;
      NativeAttributeGet(const NativeAttribute_T&) = delete;
      NativeAttributeGet(NativeAttribute_T&&) = delete;
      void operator=(const NativeAttribute_T&) = delete;
      void operator=(NativeAttribute_T&&) = delete;

      Resolved matches(size_t argc, const VALUE* argv) override;
      VALUE operator()(size_t argc, const VALUE* argv, VALUE self) override;
      std::string toString() override;

      NativeKind kind() override;
      VALUE returnKlass() override;

    protected:
      NativeAttributeGet(VALUE klass, std::string name, Attribute_T attr, std::unique_ptr<Return>&& returnInfo);

    private:
      VALUE klass_;
      Attribute_T attribute_;
    };
  } // detail
} // Rice

#endif // Rice__detail__Native_Attribute_Get__hpp_
