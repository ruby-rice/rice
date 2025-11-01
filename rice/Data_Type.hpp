#ifndef Rice__Data_Type__hpp_
#define Rice__Data_Type__hpp_

#include <set>

namespace Rice
{
  //! A mechanism for binding ruby types to C++ types.
  /*! This class binds run-time types (Ruby VALUEs) to compile-time types
   *  (C++ types).  The binding can occur only once.
   */
  template<typename T>
  class Data_Type : public Class
  {
  public:
    using type = T;

    //! Default constructor which does not bind.
    /*! No member functions must be called on this Data_Type except bind,
     *  until the type is bound.
     */
    Data_Type();

    //! Constructor which takes a Module.
    /*! Binds the type to the given VALUE according to the rules given
     *  above.
     *  \param klass the module to which to bind.
     */
    Data_Type(Module const & v);

    //! Return the Ruby class.
    /*! \return the ruby class to which the type is bound.
     */
    static Class klass();

    //! Return the Ruby data type.
    static rb_data_type_t* ruby_data_type();

    //! Assignment operator which takes a Module
    /*! \param klass must be the class to which this data type is already
     *  bound.
     *  \return *this
     */
    Data_Type& operator=(Module const & klass);

    /*! Creates a singleton method allocate and an instance method called
     *  initialize which together create a new instance of the class.  The
     *  allocate method allocates memory for the object reference and the
     *  initialize method constructs the object.
     *  \param constructor an object that has a static member function
     *  construct() that constructs a new instance of T and sets the object's data
     *  member to point to the new instance.  A helper class Constructor
     *  is provided that does precisely this.
     *  \param args a list of Arg instance used to define default parameters (optional)
     *
     *  For example:
     *  \code
     *    define_class<Foo>("Foo")
     *      .define_constructor(Constructor<Foo>());
     *  \endcode
     */
    template<typename Constructor_T, typename...Rice_Arg_Ts>
    Data_Type<T>& define_constructor(Constructor_T constructor, Rice_Arg_Ts const& ...args);

    /*! Runs a function that should define this Data_Types methods and attributes.
     *  This is useful when creating classes from a C++ class template.
     *
     *  \param builder A function that addes methods/attributes to this class
     *
     *  For example:
     *  \code
     *    void builder(Data_Type<Matrix<T, R, C>>& klass)
     *    {
     *      klass.define_method...
     *      return klass;
     *    }
     *
     *    define_class<<Matrix<T, R, C>>>("Matrix")
     *      .build(&builder);
     *
     *  \endcode
     */
    template<typename Func_T>
    Data_Type<T>& define(Func_T func);

    //! Register a Director class for this class.
    /*! For any class that uses Rice::Director to enable polymorphism
     *  across the languages, you need to register that director proxy
     *  class with this method. Not doing so will cause the resulting 
     *  library to die at run time when it tries to convert the base
     *  type into the Director proxy type.
     *
     *  This method takes no methodInfo, just needs the type of the
     *  Director proxy class.
     *
     *  For example:
     *  \code
     *    class FooDirector : public Foo, public Rice::Director {
     *      ...
     *    };
     *
     *    define_class<Foo>("Foo")
     *      .define_director<FooDirector>()
     *      .define_constructor(Constructor<FooDirector, Rice::Object>());
     *  \endcode
     */
    template<typename Director_T>
    Data_Type<T>& define_director();

    //! Determine if the type is bound.
    /*! \return true if the object is bound, false otherwise.
     */
    static bool is_bound();
    static void check_is_bound();
    static bool is_defined();
    static bool check_defined(const std::string& name, Object parent = rb_cObject);

    // This is only for testing - DO NOT USE!!!
    static void unbind();

    static bool is_descendant(VALUE value);
  
    //! Define an iterator.
    /*! Essentially this is a conversion from a C++-style begin/end
     *  iterator to a Ruby-style \#each iterator.
     *  \param begin a member function pointer to a function that returns
     *  an iterator to the beginning of the sequence.
     *  \param end a member function pointer to a function that returns an
     *  iterator to the end of the sequence.
     *  \param name the name of the iterator.
     *  \return *this
     */

    template<typename Iterator_Func_T>
    Data_Type<T>& define_iterator(Iterator_Func_T begin, Iterator_Func_T end, std::string name = "each");

    template <typename Attribute_T>
    Data_Type<T>& define_attr(std::string name, Attribute_T attribute, AttrAccess access = AttrAccess::ReadWrite, Return returnInfo = Return());
  
    template <typename Attribute_T>
    Data_Type<T>& define_singleton_attr(std::string name, Attribute_T attribute, AttrAccess access = AttrAccess::ReadWrite, Return returnInfo = Return());

    #include "cpp_api/shared_methods.hpp"
  protected:
    //! Bind a Data_Type to a VALUE.
    /*! Throws an exception if the Data_Type is already bound to a
     *  different class.  Any existing instances of the Data_Type will be
     *  bound after this function returns.
     *  \param klass the ruby type to which to bind.
     *  \return *this
     */
    template <typename Base_T = void>
    static Data_Type<T> bind(const Module& klass);

    template<typename T_, typename Base_T>
    friend Rice::Data_Type<T_> define_class_under(Object parent, Identifier id, Class superKlass);

    template<typename T_, typename Base_T>
    friend Rice::Data_Type<T_> define_class_under(Object parent, char const * name);

    template<typename T_, typename Base_T>
    friend Rice::Data_Type<T_> define_class(char const * name);

    template<typename Method_T, typename...Arg_Ts>
    void wrap_native_method(VALUE klass, std::string name, Method_T&& function, const Arg_Ts&...args);

    template <typename Attribute_T>
    Data_Type<T>& define_attr_internal(VALUE klass, std::string name, Attribute_T attribute, AttrAccess access, Return returnInfo);

  private:
    template<typename T_>
    friend class Data_Type;

    static inline VALUE klass_ = Qnil;

    // Typed Data support
    static inline rb_data_type_t* rb_data_type_ = nullptr;

    static inline std::set<Data_Type<T>*>& unbound_instances();
  };

  //! Define a new data class in the namespace given by module.
  /*! This override allows you to specify a Ruby class as the base class versus a 
   *  wrapped C++ class. This functionality is rarely needed - but is essential for
   *  creating new custom Exception classes where the Ruby superclass should be
   *  rb_eStandard
   *  \param T the C++ type of the wrapped class.
   *  \param module the Module in which to define the class.
   *  \param name the name of the new class.
   *  \param superKlass the Ruby super class.
   *  \return the new class.
   */
  template<typename T, typename Base_T = void>
  Data_Type<T> define_class_under(Object parent, Identifier id, Class superKlass = rb_cObject);

  //! Define a new data class in the namespace given by module.
  /*! By default the class will inherit from Ruby's rb_cObject. This
   *  can be overriden via the Base_T template parameter. Note that
   *  Base_T must already have been registered.
   *  \param T the C++ type of the wrapped class.
   *  \param module the the Module in which to define the class.
   *  \return the new class.
   */
  template<typename T, typename Base_T = void>
  Data_Type<T> define_class_under(Object parent, char const* name);

  //! Define a new data class in the default namespace.
  /*! By default the class will inherit from Ruby's rb_cObject. This
   *  can be overriden via the Base_T template parameter. Note that
   *  Base_T must already have been registered.
   *  \param T the C++ type of the wrapped class.
   *  \param module the the Module in which to define the class.
   *  \return the new class.
   */
  template<typename T, typename Base_T = void>
  Data_Type<T> define_class(char const* name);
}

#endif // Rice__Data_Type__hpp_
