#ifndef Rice__Arg_Impl_hpp_
#define Rice__Arg_Impl_hpp_

namespace Rice {

  //! Helper for defining default arguments of a method
  /*! This class exposes the ability to define the default values of a
   *  wrapped method. Inspired by how Boost.Python handles keyword and
   *  default arguments, the syntax is simple:
   *
   *    define_method(
   *      "method",
   *      &method,
   *      (Arg("arg1"), Arg("arg2") = 3, Arg("arg3") = true)
   *    );
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
      Arg(const char* name)
        : name_(name)
        , defaultValue(0)
      {}

      //! Copy Constructor
      Arg(const Arg& other)
        : name_(other.name()),
          defaultValue(other.defaultValue ? other.defaultValue->clone() : 0)
      {}

      virtual ~Arg()
      {
        if(defaultValue) {
          delete defaultValue;
        }
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
        defaultValue = new type<Arg_Type>(val);
        return *this;
      }

      //! Check if this Arg has a default value associated with it
      bool hasDefaultValue() const {
        return defaultValue != 0;
      }

      //! Return the default value associated with this Arg
      /*! \return the type saved to this Arg
       */
      template<typename Arg_Type>
      Arg_Type getDefaultValue()
      {
        return static_cast< type<Arg_Type>* >(defaultValue)->held;
      }

      //! Get the name of this Arg
      const char* name() const
      {
        return name_;
      }

    private:

      //! Name of the argument
      const char* name_;

      /**
       * The following is a stripped down version of
       * Boost.Any.
       */

      class type_base
      {
        public:
          virtual ~type_base() {}
          virtual type_base* clone() const = 0;
      };

      template<typename Type>
      class type : public type_base
      {
        public:
          type(Type value)
            :held(value)
          {}

          virtual ~type() { }

          virtual type_base* clone() const
          {
            return new type(held);
          }

          Type held;
      };

    public:

      //! Our saved default value
      type_base* defaultValue;
  };

}

#endif // Rice__Arg_Impl_hpp_
