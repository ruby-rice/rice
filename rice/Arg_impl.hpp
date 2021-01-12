#ifndef Rice__Arg_Impl_hpp_
#define Rice__Arg_Impl_hpp_

#include <any>
#include <string>

namespace Rice {

  //! Helper for defining default arguments of a method
  /*! This class exposes the ability to define the default values of a
   *  wrapped method. Inspired by how Boost.Python handles keyword and
   *  default arguments, the syntax is simple:
   *
   *  \code
   *    define_method(
   *      "method",
   *      &method,
   *      (Arg("arg1"), Arg("arg2") = 3, Arg("arg3") = true)
   *    );
   *  \endcode
   *
   *  which means "for method &method, it takes 3 arguments
   *  [arg1, arg2, arg3]. Of these arguments, arg2's default is 3
   *  and arg3's default is true.
   *
   *  It may be required to explicitly cast the type of the default
   *  value to prevent compilation errors.
   */
  class Arg
  {
    public:
      //! Initialize a new Arg with the name of the argument
      /*! We require the name of the argument because 1) it makes code
       *  easier to read and 2) hopefully Ruby gets keyword arguments
       *  in the future and this means Rice will be ready for it.
       */
      Arg(std::string name)
        : name_(name)
      {
      }

      //! Copy Constructor
      Arg(const Arg& other)
        : name_(other.name_), defaultValue_(other.defaultValue_)
      {
      }

      //! Set the default value for this Arg
      /*! Set the default value for this argument.
       *  If this isn't called on this Arg, then this
       *  Arg is required in the method call.
       *
       *  \param val the value to store as default
       */
      template<typename Arg_Type>
      Arg& operator=(Arg_Type val)
      {
        this->defaultValue_ = val;
        return *this;
      }

      //! Check if this Arg has a default value associated with it
      bool hasDefaultValue() const
      {
        return this->defaultValue_.has_value();
      }

      //! Return a reference to the default value associated with this Arg
      /*! \return the type saved to this Arg
       */
      template<typename Arg_Type>
      Arg_Type& defaultValue()
      {
        return std::any_cast<Arg_Type&>(this->defaultValue_);
      }

      //! Get the name of this Arg
      const std::string name() const
      {
        return name_;
      }

    private:
      //! Name of the argument
      const std::string name_;

      //! Our saved default value
      std::any defaultValue_;
  };

}

#endif // Rice__Arg_Impl_hpp_
