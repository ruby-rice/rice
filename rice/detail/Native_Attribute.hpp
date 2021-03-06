#ifndef Rice__detail__Native_Attribute__hpp_
#define Rice__detail__Native_Attribute__hpp_

#include "ruby.hpp"
#include "NativeArg.hpp"
#include "Exception_Handler_defn.hpp"
#include "Arguments.hpp"

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
class Native_Attribute
{
public:
  // Static member functions that Ruby calls
  static VALUE get(VALUE self);
  static VALUE set(VALUE self, VALUE value);

public:
  Native_Attribute(Attr_T attr, AttrAccess access = AttrAccess::ReadWrite);

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

#include "Native_Attribute.ipp"

#endif // Rice__detail__Native_Attribute__hpp_
