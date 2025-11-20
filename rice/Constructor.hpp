#ifndef Rice__Constructor__hpp_
#define Rice__Constructor__hpp_

namespace Rice
{
  //! Define a Type's Constructor and it's arguments.
  /*! E.g. for the default constructor on a Type:
      \code
        define_class<Test>()
          .define_constructor(Constructor<Test>());
      \endcode
  *
  *  The first template argument must be the type being wrapped.
  *  Additional arguments must be the types of the parameters sent
  *  to the constructor.
  *
  *  For more information, see Rice::Data_Type::define_constructor.
  */
  template<typename T, typename...Parameter_Ts>
  class Constructor;
}
#endif // Rice__Constructor__hpp_