#ifndef Rice__detail__Native_Attribute_Get__hpp_
#define Rice__detail__Native_Attribute_Get__hpp_

#include "Native.hpp"
#include "../traits/attribute_traits.hpp"

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

      using T = typename attribute_traits<Attribute_T>::attr_type;
      using Receiver_T = typename attribute_traits<Attribute_T>::class_type;
      using To_Ruby_T = remove_cv_recursive_t<T>;

    public:
      // Register attribute getter with Ruby
      static void define(VALUE klass, std::string name, Attribute_T attribute);

    public:
      // Disallow creating/copying/moving
      NativeAttributeGet() = delete;
      NativeAttributeGet(const NativeAttribute_T&) = delete;
      NativeAttributeGet(NativeAttribute_T&&) = delete;
      void operator=(const NativeAttribute_T&) = delete;
      void operator=(NativeAttribute_T&&) = delete;

      Resolved matches(int argc, VALUE* argv, VALUE self) override;
      VALUE operator()(int argc, VALUE* argv, VALUE self) override;

    protected:
      NativeAttributeGet(VALUE klass, std::string name, Attribute_T attr);

    private:
      VALUE klass_;
      std::string name_;
      Attribute_T attribute_;
    };
  } // detail
} // Rice

#include "NativeAttributeGet.ipp"

#endif // Rice__detail__Native_Attribute_Get__hpp_
