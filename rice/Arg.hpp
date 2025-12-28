#ifndef Rice__Arg__hpp_
#define Rice__Arg__hpp_

#include <any>

namespace Rice
{
  //! Helper for defining default arguments of a method
  /*! This class exposes the ability to define the default values of a
   *  wrapped method. Inspired by how Boost.Python handles keyword and
   *  default arguments, the syntax is simple:
   *
   *  \code
   *    define_method(
   *      "method",
   *      &method,
   *      Arg("arg1"), Arg("arg2") = 3, Arg("arg3") = true
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
    Arg(std::string name);

    // Make Arg polymorphic so dynamic_cast works
    virtual ~Arg() = default;

    //! Set the default value for this Arg
    /*! Set the default value for this argument.
     *  If this isn't called on this Arg, then this
     *  Arg is required in the method call.
     *
     *  \param val the value to store as default
     */
    template<typename Arg_Type>
    Arg& operator=(Arg_Type val);

    //! Check if this Arg has a default value associated with it
    bool hasDefaultValue() const;

    //! Return a reference to the default value associated with this Arg
    /*! \return the type saved to this Arg
     */
    template<typename Arg_Type>
    Arg_Type defaultValue();

    //! Tell the receiving object to keep this argument alive
    //! until the receiving object is freed.
    virtual Arg& keepAlive();
    
    //! Returns if the argument should be kept alive
    bool isKeepAlive() const;

    //! Specifies if the argument should be treated as a value
    virtual Arg& setValue();

    //! Returns if the argument should be treated as a value
    bool isValue() const;

    //! Specifies if the argument should capture a block
    virtual Arg& setBlock();

    //! Returns if the argument should capture a block
    bool isBlock() const;

    //! Specifies if the argument is opaque and Rice should not convert it from Ruby to C++ or vice versa.
    //! This is useful for callbacks and user provided data paramameters.
    virtual Arg& setOpaque();

    //! Returns if the argument should be treated as a value
    bool isOpaque() const;

    //! Specifies C++ will take ownership of this value and Ruby should not free it
    virtual Arg& takeOwnership();
    bool isOwner();

  public:
    std::string name;

  private:
    //! Our saved default value
    std::any defaultValue_;
    bool isValue_ = false;
    bool isBlock_ = false;
    bool isKeepAlive_ = false;
    bool isOwner_ = false;
    bool isOpaque_ = false;
  };

  class ArgBuffer : public Arg
  {
  public:
    ArgBuffer(std::string name);
  };
} // Rice

#endif // Rice__Arg__hpp_