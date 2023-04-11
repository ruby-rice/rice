#ifndef Rice__detail__Native_Attribute__hpp_
#define Rice__detail__Native_Attribute__hpp_

#include "ruby.hpp"
#include "ExceptionHandler_defn.hpp"
#include "MethodInfo.hpp"

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
    template<typename Return_T, typename Attr_T, typename Self_T = void>
    class NativeAttribute
    {
    public:
      using Native_Return_T = Return_T;

      // Static member functions that Ruby calls
      static VALUE get(VALUE self);
      static VALUE set(VALUE self, VALUE value);

    public:
      NativeAttribute(Attr_T attr, AttrAccess access = AttrAccess::ReadWrite);

      // Invokes the wrapped function
      VALUE read(VALUE self);
      VALUE write(VALUE self, VALUE value);

    private:
      Attr_T attr_;
      AttrAccess access_;
    };

    // A plain function or static member call
    template<typename T>
    auto* Make_Native_Attribute(T* attr, AttrAccess access);

    // Lambda function that does not take Self as first parameter
    template<typename Class_T, typename T>
    auto* Make_Native_Attribute(T Class_T::* attr, AttrAccess access);
  } // detail
} // Rice

#include "NativeAttribute.ipp"

#endif // Rice__detail__Native_Attribute__hpp_
