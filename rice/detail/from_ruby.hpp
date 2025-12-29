#ifndef Rice__detail__from_ruby__hpp_
#define Rice__detail__from_ruby__hpp_

namespace Rice::detail
{
  //! Convert a Ruby object to C++.
  /*! If the Ruby object can be converted to an immediate value, returns a
   *  copy of the Ruby object.  If the Ruby object is holding a C++
   *  object and the type specified is a pointer to that type, returns a
   *  pointer to that object.
   *
   *  Conversions from ruby to a pointer type are automatically generated
   *  when a type is bound using Data_Type.  If no conversion exists an
   *  exception is thrown.
   *
   *  \param T the C++ type to which to convert.
   *  \param x the Ruby object to convert.
   *  \return a C++ representation of the Ruby object.
   *
   *  Example:
   *  \code
   *    Object x = INT2NUM(42);
   *    std::cout << From_Ruby<int>::convert(x);
   *
   *    Data_Object<Foo> foo(new Foo);
   *    std::cout << *From_Ruby<Foo *>(foo) << std::endl;
   *  \endcode
   */

  template <typename T>
  class From_Ruby;

  // Overload resolution scoring constants
  struct Convertible
  {
    static constexpr double Exact = 1.0;           // Perfect type match
    static constexpr double None = 0.0;            // Cannot convert
    static constexpr double IntToFloat = 0.5;      // Domain change penalty when converting int to float
    static constexpr double SignedToUnsigned = 0.5;// Penalty for signed to unsigned (can't represent negatives)
    static constexpr double FloatToInt = 0.5;      // Domain change penalty when converting float to int (lossy)
    static constexpr double ConstMismatch = 0.99;  // Penalty for const mismatch
  };
}

#endif // Rice__detail__From_Ruby2__hpp_
