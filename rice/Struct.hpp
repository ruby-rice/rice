#ifndef Rice__ruby_struct__hpp_
#define Ruplu__ruby_struct__hpp_

#include "Array.hpp"
#include "Hash.hpp"
#include "Module.hpp"
#include "Class.hpp"
#include "Builtin_Object.hpp"
#include "Address_Registration_Guard.hpp"

namespace Rice
{
	class Struct;

//! Define a new Struct
Struct define_struct();

//! A wrapper for creating Struct classes.
/*! The Struct class is used for creating new Classes.  Note that the
 *  notation used here differs slightly from the notation inside the
 *  interpreter.
 *
 *  Inside the interpreter, calling Struct.new creates a new Class:
 *  \code
 *  irb(main):001:0> MyStruct = Struct.new(:a, :b, :c)
 *  => S
 *  irb(main):002:0> MyStruct.class
 *  => Class
 *  \endcode
 *
 *  Instantiating that Class creates an instance of that Class:
 *  \code
 *  irb(main):003:0> mystruct_instance = MyStruct.new
 *  => #<struct MyStruct a=nil, b=nil, c=nil>
 *  irb(main):004:0> mystruct_instance.class
 *  => MyStruct
 *  irb(main):005:0> mystruct_instance.class.ancestors
 *  => [MyStruct, Struct, Enumerable, Object, Kernel]
 *  \endcode
 *
 *  Thus, inside the interpreter, MyStruct is a Class which inherits
 *  from Struct, and mystruct_instance is an instance of MyStruct.
 *
 *  At the C++ level, we might do this instead:
 *  \code
 *  Struct rb_cMyStruct = define_struct()
 *    .define_member("a")
 *    .define_member("b")
 *    .define_member("c")
 *    .initialize("MyStruct");
 *  Struct::Instance mystruct_instance(rb_cMyStruct.new_instance());
 *  \endcode
 *
 * Here rb_cMyStruct is an instance of Struct and that mystruct_instance
 * is an instance of Struct::Instance.
 */
class Struct
  : public Module_impl<Class, Struct>
{
public:
  //! Create a new Struct.
  Struct();

  //! Copy constructor.
  Struct(Struct const & s);

  //! Destructor.
  virtual ~Struct();

  //! Define a new Struct member.
  /*! Defines a new member of the Struct.  Must be called before the
   *  Struct is initialized.
   *  \return *this
   */
  Struct & define_member(
      Identifier name);

  //! Initialize the Struct class.
  /*! Must be called after all Struct members have been defined.
   *  \param module the module under which to define the Struct.
   *  \param name the name of the Class at the ruby level.
   */
  Struct & initialize(
      Module module,
      Identifier name);

  //! Get the offset of a member in the Struct.
  /*! Internally, Struct members are stored as a single array of VALUE.
   *  This function determines the offset of a given member in that
   *  array.
   *  \param member the name of the desired member.
   *  \return the index of the given member.
   */
  size_t offset_of(Identifier name) const;

  class Instance;
  friend class Instance;
  friend Struct Rice::define_struct();

  //! Create a new instance of the Struct
  /*! \param args the arguments to the constructor.
   *  \return a new Struct::Instance
   */
  Instance new_instance(Array args = Array()) const;

  //! Swap with another Struct.
  void swap(Struct & other);

  Array members() const { return members_; }

private:
  Array members_;
  Address_Registration_Guard members_guard_;

  Hash member_offset_;
  Address_Registration_Guard member_offset_guard_;
};


//! An instance of a Struct
//! \sa Struct
class Struct::Instance
  : public Builtin_Object<RStruct, T_STRUCT>
{
public:
  //! Create a new Instance of a Struct.
  /*! \param type the Struct type to create.
   *  \param args the initial values for the objects of the instance.
   */
  Instance(
      Struct const & type,
      Array args = Array());

  //! Encapsulate an existing Struct instance.
  /*! \param type the Struct type to encapsulate.
   *  \param s the instance to encapsulate.
   */
  Instance(
      Struct const & type,
      Object s);

  //! Get a member, given its offset.
  /*! \param index the (integral) index into the Struct's internal
   *  array or its name (an Identifier or char const *)
   *  \return the member.
   */
  template<typename T>
  Object operator[](T index);

  //! Swap with another Struct::Instance.
  void swap(Instance & other);

private:
  Struct type_;
};

} // namespace Rice

#include "Struct.ipp"

#endif // Rice__ruby_struct__hpp_

