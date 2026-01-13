#ifndef Rice__Type__hpp_
#define Rice__Type__hpp_

namespace Rice::detail
{
  template<typename T>
  struct Type
  {
    static bool verify();
  };

  template<typename T>
  struct Type<T&>
  {
    static bool verify();
  };

  template<typename T>
  struct Type<T&&>
  {
    static bool verify();
  };

  template<typename T>
  struct Type<T*>
  {
    static bool verify();
  };

  template<typename T>
  struct Type<T**>
  {
    static bool verify();
  };

  template<typename T>
  void verifyType();

  template<typename Tuple_T>
  void verifyTypes();
}

#endif // Rice__Type__hpp_
