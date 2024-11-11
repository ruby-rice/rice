#ifndef Rice__detail__Native_Attribute_Set__hpp_
#define Rice__detail__Native_Attribute_Set__hpp_

#include "Native.hpp"
#include "../traits/attribute_traits.hpp"

namespace Rice
{
  namespace detail
  {
    template<typename Attribute_T>
    class NativeAttributeSet: Native
    {
    public:
      using NativeAttribute_T = NativeAttributeSet<Attribute_T>;

      using T = typename attribute_traits<Attribute_T>::attr_type;
      using T_Unqualified = remove_cv_recursive_t<T>;
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

      Resolved matches(int argc, VALUE* argv, VALUE self) override;
      VALUE operator()(int argc, VALUE* argv, VALUE self) override;

    protected:
      NativeAttributeSet(VALUE klass, std::string name, Attribute_T attr);

    private:
      VALUE klass_;
      std::string name_;
      Attribute_T attribute_;
    };
  } // detail
} // Rice

#include "NativeAttributeSet.ipp"

#endif // Rice__detail__Native_Attribute_Set__hpp_
