#ifndef Rice__Arg_Impl_hpp_
#define Rice__Arg_Impl_hpp_

namespace Rice {

  /**
   * Arg is how you define the details of the arguments
   * a given method takes. Inspired by how Boost.Python handles
   * keyword and default arguments, the syntax is simple:
   *
   *   define_method("method", method, (Arg("arg1"), Arg("arg2") = 3, Arg("arg3") = true))
   *
   * This is simply saying that the method ::method takes three arguments
   * (arg1, arg2, arg3) and that arg2 defaults to 2 and arg3 defaults to true.
   */
  class Arg
  {
    public:
      /**
       * Initialize the Arg with the name of the argument.
       * We require the name of the argument because 1) it makes code
       * easier to read and 2) hopefully Ruby gets keyword arguments
       * in the future and this means Rice will be ready for it.
       */
      Arg(const char* name)
        : name_(name)
        , defaultValue(0)
      {}

      Arg(const Arg& other)
        : name_(other.name()),
          defaultValue(other.defaultValue ? other.defaultValue->clone() : 0)
      {}

      virtual ~Arg()
      {
      }

      /**
       * Set the default value for this argument.
       * If this isn't called on this Arg, then this
       * Arg is required in the method call
       */
      template<typename Arg_Type>
      Arg& operator=(Arg_Type const& val)
      {
        defaultValue = new type<Arg_Type>(val);
        return *this;
      }

      /**
       * Does this argument have a default value on it?
       */
      bool hasDefaultValue() const {
        return defaultValue != 0;
      }

      /**
       * Get the default value of this argument,
       * if one exists
       */
      template<typename Arg_Type>
      Arg_Type getDefaultValue() const
      {
        return static_cast< type<Arg_Type>* >(defaultValue)->held;
      }

      const char* name() const
      {
        return name_;
      }

    private:

      /** Name of the argument */
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

      /** Our saved default value */
      type_base* defaultValue;
  };

}

#endif // Rice__Arg_Impl_hpp_
