#ifndef Rice__hpp_
#define Rice__hpp_

// Ruby

// =========   ruby.hpp   =========

/*! \file
 * \brief Hacks for addressing incompatibilities between various Ruby
 * versions.
 */

#include <cmath>

#include <ruby.h>
#include <ruby/encoding.h>

// ruby.h has a few defines that conflict with Visual Studio's STL
#if defined(_MSC_VER)
#undef write
#undef read
#undef bind
#endif

// And some c library conflicts
#undef isnan
#undef snprintf
#undef vsnprintf

//! A function that takes a VALUE as a parameter and returns a VALUE.
// TODO: Casting from a C++ function to an extern "C" function won't
// work on all platforms.  I'm not sure what to do about this.
extern "C" typedef VALUE (*RUBY_VALUE_FUNC)(VALUE);

// Fix Ruby RUBY_METHOD_FUNC from macro to typedef
#if defined(RUBY_METHOD_FUNC)
# undef RUBY_METHOD_FUNC
  extern "C" typedef VALUE (*RUBY_METHOD_FUNC)(ANYARGS);
#endif

// This is a terrible hack for Ruby 3.1 and maybe earlier to avoid crashes when test_Attribute unit cases
// are run. If ruby_options is called to initialize the interpeter (previously it was not), when
// the attribute unit tests intentionally cause exceptions to happen, the exception is correctly processed.
// However any calls back to Ruby, for example to get the exception message, crash because the ruby 
// execution context tag has been set to null. This does not happen in newer versions of Ruby. It is
// unknown if this happens in real life or just the test caes.
// Should be removed when Rice no longer supports Ruby 3.1
#if RUBY_API_VERSION_MAJOR == 3 && RUBY_API_VERSION_MINOR < 2
  constexpr bool oldRuby = true;
#elif RUBY_API_VERSION_MAJOR < 3
  constexpr bool oldRuby = true;
#else
  constexpr bool oldRuby = false;
#endif 





// C++ headers -h has to come after Ruby on MacOS for reasons I do not understand
#include <cstdio>
#include <string>
#include <typeinfo>
#include <typeindex>

#ifdef _MSC_VER
  // Prevent _strdup deprecated message on MSVC
  #define strdup _strdup 
#endif

// Traits

// =========   rice_traits.hpp   =========

#include <ostream>
#include <type_traits>
#include <vector>

namespace Rice
{
  namespace detail
  {
    // Get the base_type of T - without pointer, reference, const or volatile. We call remove_pointer_t twice 
    // for T**
    template<typename T>
    using intrinsic_type = typename std::remove_cv_t<std::remove_pointer_t<std::remove_pointer_t<std::remove_reference_t<T>>>>;

    // Recursively remove const/volatile
    template<typename T>
    struct remove_cv_recursive
    {
      using type = T;
    };

    template<typename T>
    struct remove_cv_recursive<T const volatile>
    {
      using type = typename remove_cv_recursive<T>::type;
    };

    template<typename T>
    struct remove_cv_recursive<T volatile>
    {
      using type = typename remove_cv_recursive<T>::type;
    };

    template<typename T>
    struct remove_cv_recursive<T const>
    {
      using type = typename remove_cv_recursive<T>::type;
    };

    template<typename T>
    struct remove_cv_recursive<T&>
    {
      using type = typename remove_cv_recursive<T>::type&;
    };

   /* template<typename T>
    struct remove_cv_recursive<T&&>
    {
      using type = typename remove_cv_recursive<T>::type&&;
    };*/

    template<typename T>
    struct remove_cv_recursive<T*>
    {
      using type = typename remove_cv_recursive<T>::type*;
    };

    template<typename T>
    using remove_cv_recursive_t = typename remove_cv_recursive<T>::type;

    // Does the Type work with ostreams? This is used to implement #to_s
    template<typename T, typename = void>
    struct is_ostreamable : std::false_type {};

    template<typename T>
    struct is_ostreamable<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<const T>())>> : std::true_type {};

    template<typename T>
    constexpr bool is_ostreamable_v = is_ostreamable<T>::value;

    // Is the type comparable?
    template<typename T, typename SFINAE = void>
    struct is_comparable : std::false_type {};

    template<typename T>
    struct is_comparable<T, std::void_t<
      // Does the class implement operator== and does it return a boolean value?
      decltype(std::declval<T>() == std::declval<T>() && true)
      >> : std::true_type {};

    template<typename T>
    constexpr bool is_comparable_v = is_comparable<T>::value;

    template <typename U, typename V>
    struct is_comparable<std::pair<U, V>>
    {
      static const bool value = is_comparable_v<U> && is_comparable_v<V>;
    };

    template <typename T>
    struct is_comparable<std::vector<T>>
    {
      static const bool value = is_comparable_v<T>;
    };

    // -- Tuple Helpers ---
    template<typename T>
    struct tuple_shift;

    template<typename T, typename...Arg_Ts>
    struct tuple_shift<std::tuple<T, Arg_Ts...>>
    {
      using type = std::tuple<Arg_Ts...>;
    };
     
    template<template<typename, typename...> typename T, typename...Arg_Ts>
    struct tuple_map;

    template<template<typename, typename...> typename T, typename...Arg_Ts>
    struct tuple_map<T, std::tuple<Arg_Ts...>>
    {
      using type = std::tuple<T<remove_cv_recursive_t<Arg_Ts>>...>;
    };

    template<class T>
    struct is_pointer_pointer : std::false_type {};

    template<class T>
    struct is_pointer_pointer<T**> : std::true_type {};

    template<class T>
    struct is_pointer_pointer<T** const> : std::true_type {};

    template<class T>
    struct is_pointer_pointer<T* const * const> : std::true_type {};

    template<class T>
    struct is_pointer_pointer<const T* const* const> : std::true_type {};

    template<class T>
    constexpr bool is_pointer_pointer_v = is_pointer_pointer<T>::value;

  } // detail
} // Rice


// =========   function_traits.hpp   =========

#include <tuple>

namespace Rice::detail
{
  // Base class
  template<typename Function_T>
  struct function_traits;

  template<typename Return_T, typename Class_T, typename...Arg_Ts>
  struct function_traits<Return_T(Class_T, Arg_Ts...)>
  {
    using arg_types = std::tuple<Arg_Ts...>;

    static constexpr std::size_t arity = sizeof...(Arg_Ts);

    template<std::size_t N>
    using nth_arg = typename std::tuple_element<N, arg_types>::type;

    using return_type = Return_T;
    using class_type = Class_T;
  };

  // Functors and lambdas with operator()
  template<typename Function_T>
  struct function_traits : public function_traits<decltype(&Function_T::operator())>
  {
  private:
    using functor_t = function_traits<decltype(&Function_T::operator())>;

  public:
    using arg_types = typename functor_t::arg_types;
    static constexpr std::size_t arity = functor_t::arity - 1;
    using class_type = std::nullptr_t;
  };

  // C functions and static member functions passed by pointer
  template<typename Return_T, typename ...Arg_Ts>
  struct function_traits<Return_T(*)(Arg_Ts...)> : public function_traits<Return_T(std::nullptr_t, Arg_Ts...)>
  {
    using Function_T = Return_T(*)(Arg_Ts...);
  };
  
  // C functions passed by pointer that take one or more defined parameter than a variable 
  // number of parameters (the second ...)
  template<typename Return_T, typename ...Arg_Ts>
  struct function_traits<Return_T(*)(Arg_Ts..., ...)> : public function_traits<Return_T(std::nullptr_t, Arg_Ts...)>
  {
  };

  // C Functions or static member functions passed by reference
  template<typename Return_T, typename ...Arg_Ts>
  struct function_traits<Return_T(&)(Arg_Ts...)> : public function_traits<Return_T(std::nullptr_t, Arg_Ts...)>
  {
  };

  // Member Functions on C++ classes
  template<typename Return_T, typename Class_T, typename...Arg_Ts>
  struct function_traits<Return_T(Class_T::*)(Arg_Ts...)> : public function_traits<Return_T(Class_T*, Arg_Ts...)>
  {
  };

  // const member Functions on C++ classes
  template<typename Return_T, typename Class_T, typename...Arg_Ts>
  struct function_traits<Return_T(Class_T::*)(Arg_Ts...) const> : public function_traits<Return_T(Class_T*, Arg_Ts...)>
  {
  };

  // noexcept member Functions on C++ classes
  template<typename Return_T, typename Class_T, typename...Arg_Ts>
  struct function_traits<Return_T(Class_T::*)(Arg_Ts...) noexcept> : public function_traits<Return_T(Class_T*, Arg_Ts...)>
  {
  };


  // const noexcept member Functions on C++ classes
  template<typename Return_T, typename Class_T, typename...Arg_Ts>
  struct function_traits<Return_T(Class_T::*)(Arg_Ts...) const noexcept> : public function_traits<Return_T(Class_T*, Arg_Ts...)>
  {
  };

  /*// Functors and lambdas
  template<class Function_T>
  struct function_traits<Function_T&> : public function_traits<Function_T>
  {
  };

  template<class Function_T>
  struct function_traits<Function_T&&> : public function_traits<Function_T>
  {
  };*/
}

// =========   method_traits.hpp   =========

#include <tuple>

namespace Rice::detail
{
  // Declare struct
  template<typename Function_T, bool IsMethod, typename = void>
  struct method_traits;

  // Functions that do not have a self parameter:
  //   doSomething(int a)
  template<typename Function_T, bool IsMethod>
  struct method_traits<Function_T, IsMethod, std::enable_if_t<!IsMethod>>
  {
    using Return_T = typename function_traits<Function_T>::return_type;
    using Class_T = std::nullptr_t;
    using Arg_Ts = typename function_traits<Function_T>::arg_types;
    static constexpr std::size_t arity = std::tuple_size_v<Arg_Ts>;
  };

  /* Functions that have a self parameter and thus we treat them as free standing
    "methods" versus member functions. 
    
       doSomething(VALUE self, int a) */
  template<typename Function_T, bool IsMethod>
  struct method_traits<Function_T, IsMethod, std::enable_if_t<IsMethod && std::is_same_v<typename function_traits<Function_T>::class_type, std::nullptr_t>>>
  {
    using Return_T = typename function_traits<Function_T>::return_type;
    using Class_T = typename function_traits<Function_T>::template nth_arg<0>;
    using Arg_Ts = typename tuple_shift<typename function_traits<Function_T>::arg_types>::type;
    static constexpr std::size_t arity = std::tuple_size_v<Arg_Ts>;
  };

  // Member functions that have an implied self parameter of an object instance
  //   foo.doSomething(int a)
  template<typename Function_T, bool IsMethod>
  struct method_traits<Function_T, IsMethod, std::enable_if_t<IsMethod && !std::is_same_v<typename function_traits<Function_T>::class_type, std::nullptr_t>>>
  {
    using Return_T = typename function_traits<Function_T>::return_type;
    using Class_T = typename function_traits<Function_T>::class_type;
    using Arg_Ts = typename function_traits<Function_T>::arg_types;
    static constexpr std::size_t arity = std::tuple_size_v<Arg_Ts>;
  };
}

// =========   attribute_traits.hpp   =========

#include <tuple>

namespace Rice::detail
{
  // Base class
  template<typename Attribute_T>
  struct attribute_traits;

  template<typename Attr_T>
  struct attribute_traits<Attr_T*>
  {
    using attr_type = Attr_T;
    using class_type = std::nullptr_t;
  };

  template<typename Attr_T, typename Class_T>
  struct attribute_traits<Attr_T Class_T::*> 
  {
    using attr_type = Attr_T;
    using class_type = Class_T;
  };
}

// Helpers

// =========   TupleIterator.hpp   =========

// See https://www.cppstories.com/2022/tuple-iteration-apply/
template <typename Tuple_T, typename Function_T>
void for_each_tuple(Tuple_T&& tuple, Function_T&& callable)
{
    std::apply([&callable](auto&& ...args)
      {
        (callable(std::forward<decltype(args)>(args)), ...);
      }, std::forward<Tuple_T>(tuple));
}


// Code for C++ to call Ruby

// =========   Exception.hpp   =========

#include <stdexcept>

namespace Rice
{
  //! A placeholder for Ruby exceptions.
  /*! You can use this to safely throw a Ruby exception using C++ syntax:
   *  \code
   *    VALUE foo(VALUE self) {
   *      RUBY_TRY {
   *        throw Rice::Exception(rb_eMyException, "uh oh!");
   *      RUBY_CATCH
   *    }
   *  \endcode
   */
  class Exception
    : public std::exception
  {
  public:
    //! Construct a Exception with a Ruby exception instance
    explicit Exception(VALUE exception);

    //! Construct a Exception with printf-style formatting.
    /*! \param exc either an exception object or a class that inherits
     *  from Exception.
     *  \param fmt a printf-style format string
     *  \param ... the arguments to the format string.
     */
    template <typename... Arg_Ts>
    Exception(const Exception& other, char const* fmt, Arg_Ts&&...args);

    //! Construct a Exception with printf-style formatting.
    /*! \param exc either an exception object or a class that inherits
     *  from Exception.
     *  \param fmt a printf-style format string
     *  \param ... the arguments to the format string.
     */
    template <typename... Arg_Ts>
    Exception(const VALUE exceptionType, char const* fmt, Arg_Ts&&...args);

    //! Destructor
    virtual ~Exception() noexcept = default;

    //! Get message as a char const *.
    /*! If message is a non-string object, then this function will attempt
     *  to throw an exception (which it can't do because of the no-throw
     *  specification).
     *  \return the underlying C pointer of the underlying message object.
     */
    virtual char const* what() const noexcept override;

    //! Returns the Ruby exception class
    VALUE class_of() const;

    //! Returns an instance of a Ruby exception
    VALUE value() const;

  private:
    // TODO: Do we need to tell the Ruby gc about an exception instance?
    mutable VALUE exception_ = Qnil;
    mutable std::string message_;
  };
} // namespace Rice


// =========   JumpException.hpp   =========

namespace Rice
{
  //! A placeholder for Ruby longjmp data.
  /*! When a Ruby exception is caught, the tag used for the longjmp is stored in
   *  a Jump_Tag, then later passed to rb_jump_tag() when there is no more
   *  C++ code to pass over.
   */
  class JumpException : public std::exception
  {
  public:
    // Copied from vm_core.h
    enum ruby_tag_type {
      RUBY_TAG_NONE = 0x0,
      RUBY_TAG_RETURN = 0x1,
      RUBY_TAG_BREAK = 0x2,
      RUBY_TAG_NEXT = 0x3,
      RUBY_TAG_RETRY = 0x4,
      RUBY_TAG_REDO = 0x5,
      RUBY_TAG_RAISE = 0x6,
      RUBY_TAG_THROW = 0x7,
      RUBY_TAG_FATAL = 0x8,
      RUBY_TAG_MASK = 0xf
    };

  public:
    JumpException(ruby_tag_type tag);
    virtual const char* what() const noexcept override;

  public:
    //! The tag being held.
    ruby_tag_type tag;

  private:
    void createMessage();

  private:
    std::string message_;
  };
} // namespace Rice


// =========   JumpException.ipp   =========
namespace Rice
{
  inline JumpException::JumpException(ruby_tag_type tag) : tag(tag)
  {
    this->createMessage();
  }

  inline const char* JumpException::what() const noexcept
  {
    return this->message_.c_str();
  }

  inline void JumpException::createMessage()
  {
    switch (this->tag)
    {
      case RUBY_TAG_NONE:
        this->message_ = "No error";
        break;
      case RUBY_TAG_RETURN:
        this->message_ = "Unexpected return";
        break;
      case RUBY_TAG_NEXT:
        this->message_ = "Unexpected next";
        break;
      case RUBY_TAG_BREAK:
        this->message_ = "Unexpected break";
        break;
      case RUBY_TAG_REDO:
        this->message_ = "Unexpected redo";
        break;
      case RUBY_TAG_RETRY:
        this->message_ = "Retry outside of rescue clause";
        break;
      case RUBY_TAG_THROW:
        this->message_ = "Unexpected throw";
      case RUBY_TAG_RAISE:
        this->message_ = "Ruby exception was thrown";
        break;
      case RUBY_TAG_FATAL:
        this->message_ = "Fatal error";
        break;
      case RUBY_TAG_MASK:
        this->message_ = "Mask error";
        break;
    }
  }
} // namespace Rice

// =========   RubyFunction.hpp   =========

namespace Rice::detail
{
  /* This is functor class that wraps calls to a Ruby C API method. It is needed because
     rb_protect only supports calling methods that take one argument. Thus 
     we invoke rb_protect telling it to invoke Ruby_Function::call with an 
     instance of a Ruby_Function. That instance then in turn calls the original
     Ruby method passing along its required arguments. */

  template<typename Function_T, typename...Arg_Ts>
  class RubyFunction
  {
  public:
    using Return_T = typename function_traits<Function_T>::return_type;

  public:
    RubyFunction(Function_T func, const Arg_Ts&... args);
    Return_T operator()();

  private:
    Function_T func_;
    std::tuple<Arg_Ts...> args_;
  };

  template<typename Function_T, typename ...Arg_Ts>
  auto protect(Function_T func, Arg_Ts...args);
}


// =========   RubyFunction.ipp   =========

#include <any>

namespace Rice::detail
{
  template<typename Function_T, typename...Arg_Ts>
  inline RubyFunction<Function_T, Arg_Ts...>::RubyFunction(Function_T func, const Arg_Ts&... args)
    : func_(func), args_(std::forward_as_tuple(args...))
  {
  }

  template<typename Function_T, typename...Arg_Ts>
  inline typename RubyFunction<Function_T, Arg_Ts...>::Return_T RubyFunction<Function_T, Arg_Ts...>::operator()()
  {
    // Setup a thread local variable to capture the result of the Ruby function call.
    // We use thread_local because the lambda has to be captureless so it can
    // be converted to a function pointer callable by C.
    // The thread local variable avoids having to cast the result to VALUE and then 
    // back again to Return_T. The problem with that is the translation is not lossless
    // in some cases - for example a double with value of -1.0 does not roundrip.
    // 
    thread_local std::any result;

    // Callback that will invoke the Ruby function
    using Functor_T = RubyFunction<Function_T, Arg_Ts...>;
    auto callback = [](VALUE value) -> VALUE
    {
      Functor_T* functor = (Functor_T*)value;

      if constexpr (std::is_same_v<Return_T, void>)
      {
        std::apply(functor->func_, functor->args_);
      }
      else
      {
        result = std::apply(functor->func_, functor->args_);
      }

      return Qnil;
    };

    // Now call rb_protect which will invoke the callback lambda above
    int state = (int)JumpException::RUBY_TAG_NONE;
    rb_protect(callback, (VALUE)this, &state);

    // Did anything go wrong?
    if (state == JumpException::RUBY_TAG_NONE)
    {
      if constexpr (!std::is_same_v<Return_T, void>)
      {
        return std::any_cast<Return_T>(result);
      }
    }
    else
    {
      VALUE err = rb_errinfo();
      if (state == JumpException::RUBY_TAG_RAISE && RB_TEST(err))
      {
        rb_set_errinfo(Qnil);
        throw Rice::Exception(err);
      }
      else
      {
        throw Rice::JumpException((Rice::JumpException::ruby_tag_type)state);
      }
    }
  }
    
  // Create a functor for calling a Ruby function and define some aliases for readability.
  template<typename Function_T, typename ...Arg_Ts>
  auto protect(Function_T func, Arg_Ts...args)
  {
    auto rubyFunction = RubyFunction<Function_T, Arg_Ts...>(func, std::forward<Arg_Ts>(args)...);
    return rubyFunction();
  }
}
// C++ API declarations

// =========   Type.hpp   =========

#include <regex>

namespace Rice::detail
{
  template<typename T>
  struct Type
  {
    static bool verify();
  };

  // Return the name of a type
  std::string typeName(const std::type_info& typeInfo);
  std::string typeName(const std::type_index& typeIndex);
  std::string makeClassName(const std::string& typeInfoName);
  std::string findGroup(std::string& string, size_t start = 0);
  void replaceAll(std::string& string, std::regex regex, std::string replacement);

  template<typename T>
  void verifyType();

  template<typename Tuple_T>
  void verifyTypes();
}


// =========   Encoding.hpp   =========

namespace Rice
{
  //! A wrapper for a Ruby encoding
  class Encoding
  {
  public:
    static Encoding utf8();

    //! Wrap an existing encoding.
    Encoding(rb_encoding* encoding);

    //! Implicit conversion to VALUE.
    operator rb_encoding* () const
    {
      return this->encoding_;
    }

    operator VALUE () const
    {
      return detail::protect(rb_enc_from_encoding, this->encoding_);
    }

  private:
    rb_encoding* encoding_;
  };
} // namespace Rice



// =========   Identifier.hpp   =========

namespace Rice
{
  class Symbol;

  //! A wrapper for the ID type
  /*! An ID is ruby's internal representation of a Symbol object.
   */
  class Identifier
  {
  public:
    //! Construct a new Identifier from an ID.
    Identifier(ID id);

    //! Construct a new Identifier from a Symbol.
    Identifier(Symbol const& symbol);

    //! Construct a new Identifier from a c string.
    Identifier(char const* s, Encoding encoding = Encoding::utf8());

    //! Construct a new Identifier from a string.
    Identifier(std::string const& string, Encoding encoding = Encoding::utf8());

    //! Return a string representation of the Identifier.
    char const* c_str() const;

    //! Return a string representation of the Identifier.
    std::string str() const;

    //! Return the underlying ID
    ID id() const { return id_; }

    //! Return the underlying ID
    operator ID() const { return id_; }

    //! Return the ID as a Symbol
    VALUE to_sym() const;

  private:
    ID id_;
  };
} // namespace Rice


// =========   Identifier.ipp   =========
namespace Rice
{
  inline Identifier::Identifier(ID id) : id_(id)
  {
  }

  inline Identifier::Identifier(char const* name, Encoding encoding)
  {
    this->id_ = detail::protect(rb_intern3, name, (long)strlen(name), encoding);
  }

  inline Identifier::Identifier(const std::string& name, Encoding encoding)
  {
    this->id_ = detail::protect(rb_intern3, name.c_str(), (long)name.size(), encoding);
  }

  inline char const* Identifier::c_str() const
  {
    return detail::protect(rb_id2name, id_);
  }

  inline std::string Identifier::str() const
  {
    return c_str();
  }

  inline VALUE Identifier::to_sym() const
  {
    return ID2SYM(id_);
  }
}
// =========   Object.hpp   =========

/*! \file Object.hpp
 */

#include <iosfwd>

namespace Rice
{
  class Class;
  class String;
  class Array;

  //! The base class for all Objects
  /*! Perhaps the name "Object" is a misnomer, because this class really
   *  holds an object reference, not an object.
   */
  class Object
  {
  public:
    //! Encapsulate an existing ruby object.
    Object(VALUE value = Qnil) : value_(value) {}

    //! Destructor
    virtual ~Object();

    // Enable copying
    Object(const Object& other) = default;
    Object& operator=(const Object& other) = default;

    // Enable moving
    Object(Object&& other);
    Object& operator=(Object&& other);

    //! Returns false if the object is nil or false; returns true
    //! otherwise.
    // Having this conversion also prevents accidental conversion to
    // undesired integral types (e.g. long or int) by making the
    // conversion ambiguous.
    bool test() const { return RTEST(value_); }

    //! Returns false if the object is nil or false; returns true
    //! otherwise.
    operator bool() const { return test(); }

    //! Returns true if the object is nil, false otherwise.
    bool is_nil() const { return NIL_P(value_); }

    //! Implicit conversion to VALUE.
    operator VALUE() const { return value_; }

    //! Explicitly get the encapsulated VALUE.
    // Returns a const ref so that Address_Registration_Guard can access
    // the address where the VALUE is stored
    VALUE const volatile& value() const { return value_; }

    //! Get the class of an object.
    /*! \return the object's Class.
     */
    Class class_of() const;

    //! Compare this object to another object.
    /*! Gets the result of self <=> other and returns the result.  The
     *  result will be less than zero if self < other, greater than zero
     *  if self > other, and equal to zero if self == other.
     */
    int compare(Object const& other) const;

    //! Return a string representation of an object.
    /*! \return the result of calling to_s on the object.  A String is not
     *  returned, because it is not possible to return an instance of a
     *  derived class.
     */
    String to_s() const;

    //! Return the name of an object's class.
    String class_name() const;

    //! Inspect the object.
    /*! \return the result of calling inspect on the object.  A String is
     *  not returned, because it is not possible to return an instance of
     *  a derived class.
     */
    String inspect() const;

    //! Freeze the object.
    void freeze();

    //! Determine if the object is frozen.
    /*! \return true if the object is frozen, false otherwise.
     */
    bool is_frozen() const;

    //! Evaluate the given string in the context of the object.
    /*! This is equivalant to calling obj.instance_eval(s) from inside the
     *  interpreter.
     *  \return the result of the expression.
     */
    Object instance_eval(String const& s);

    //! Return the type of the underlying C object.
    /*! This is equivalent to calling rb_type(obj).
     * \return the type of the underlying C object (e.g. T_DATA, T_ARRAY,
     * etc.).
     */
    int rb_type() const;

    //! Return the object's id
    VALUE object_id() const;

    //! Determine whether the object is an instance of a class/module.
    /*! \param klass a class or module.
     *  \return true if the object is an instance of the given
     *  class/module or one of its descendants.
     */
    bool is_a(Object klass) const;

    //! Determine if the objects responds to a method.
    /*! \param id the name of the method
     *  \return true if the objects responds to the method, false
     *  otherwise.
     */
    bool respond_to(Identifier id) const;

    //! Determine whether class is the object's class.
    /*! \param klass a class.
     *  \return true if the object is an instance of the given class.
     */
    bool is_instance_of(Object klass) const;

    //! Determine whether the Ruby VALUEs wrapped by this
    //! object are the same object. Maps to Object::equal?
    /*! \param other a Object.
     */
    bool is_equal(const Object& other) const;

    //! Determine whether the Ruby VALUEs wrapped by this
    //! object are equivalent. Maps to Object::eql?
    /*! \param other a Object.
     */
    bool is_eql(const Object& other) const;

    //! Set an instance variable.
    /*! \param name the name of the instance variable to set (including
     *  the leading @ sign)
     *  \param value the value of the variable, which will be converted to
     *  a Ruby type if necessary.
     */
    template<typename T>
    void iv_set(Identifier name, T const& value);

    //! Get the value of an instance variable.
    /*! \param name the name of the instance variable to get
     *  \return the value of the instance variable
     */
    Object iv_get(Identifier name) const;

    //! Get the value of an instance variable, but don't warn if it is
    //unset.
    /*! \param name the name of the instance variable to get
     *  \return the value of the instance variable
     */
    Object attr_get(Identifier name) const;

    //! Call the Ruby method specified by 'id' on object 'obj'.
    /*! Pass in arguments (arg1, arg2, ...).  The arguments will be converted to
    *  Ruby objects with to_ruby<>. To call methods expecting keyword arguments,
    *  use call_kw.
    *
    *  E.g.:
    *  \code
    *    Rice::Object obj = x.call("foo", "one", 2);
    *  \endcode
    *
    *  If a return type is specified, the return value will automatically be
    *  converted to that type as long as 'from_ruby' exists for that type.
    *
    *  E.g.:
    *  \code
    *    float ret = x.call<float>("foo", z, 42);
    *  \endcode
    */
    template<typename ...Arg_Ts>
    Object call(Identifier id, Arg_Ts... args) const;

    //! Call the Ruby method specified by 'id' on object 'obj'.
    /*! Pass in arguments (arg1, arg2, ...).  The arguments will be converted to
    *  Ruby objects with to_ruby<>. The final argument must be a Hash and will be treated
    *  as keyword arguments to the function.
    *
    *  E.g.:
    *  \code
    *    Rice::Hash kw;
    *    kw[":argument"] = String("one")
    *    Rice::Object obj = x.call_kw("foo", kw);
    *  \endcode
    *
    *  If a return type is specified, the return value will automatically be
    *  converted to that type as long as 'from_ruby' exists for that type.
    *
    *  E.g.:
    *  \code
    *    float ret = x.call_kw<float>("foo", kw);
    *  \endcode
    */
    template<typename ...Arg_Ts>
    Object call_kw(Identifier id, Arg_Ts... args) const;

    //! Vectorized call.
    /*! Calls the method identified by id with the list of arguments
     *  identified by args.
     *  \param id the name of the method to call
     *  \param args the arguments to the method
     *  \return the return value of the method call
     */
    Object vcall(Identifier id, Array args);

    //! Get a constant.
    /*! \param name the name of the constant to get.
     *  \return the value of the constant.
     */
    Object const_get(Identifier name) const;

    //! Determine whether a constant is defined.
    /*! \param name the name of the constant to check.
     *  \return true if the constant is defined in this module or false
     *  otherwise.
     */
    bool const_defined(Identifier name) const;

    //! Set a constant.
    /*! \param name the name of the constant to set.
      *  \param value the value of the constant.
      *  \return *this
      */
    inline Object const_set(Identifier name, Object value);

    //! Set a constant if it not already set.
    /*! \param name the name of the constant to set.
      *  \param value the value of the constant.
      *  \return *this
      */
    inline Object const_set_maybe(Identifier name, Object value);

    //! Remove a constant.
    /*! \param name the name of the constant to remove.
     */
    void remove_const(Identifier name);

  protected:
    //! Set the encapsulated value.
    void set_value(VALUE v);

  private:
    volatile VALUE value_;
  };

  std::ostream& operator<<(std::ostream& out, Object const& obj);

  bool operator==(Object const& lhs, Object const& rhs);
  bool operator!=(Object const& lhs, Object const& rhs);
  bool operator<(Object const& lhs, Object const& rhs);
  bool operator>(Object const& lhs, Object const& rhs);

  extern Object const Nil;
  extern Object const True;
  extern Object const False;
  extern Object const Undef;
} // namespace Rice


// =========   Builtin_Object.hpp   =========

namespace Rice
{
  //! A smartpointer-like wrapper for Ruby builtin objects.
  /*! A builtin object is one of Ruby's internal types, e.g. RArray or
   *  RString.  Every builtin type structure has a corresponding integer
   *  type number (e.g T_ARRAY for RArray or T_STRING for RString).  This
   *  class is a wrapper for those types of objects, primarily useful as a
   *  base class for other wrapper classes like Array and Hash.
   */
  template<int Builtin_Type>
  class Builtin_Object
    : public Object
  {
  public:
    //! Wrap an already allocated Ruby object.
    /*! Checks to see if the object is an object of type Builtin_Type; a
     *  C++ exception is thrown if this is not the case.
     *  \param value the object to be wrapped.
     */
    Builtin_Object(Object value);

    RObject& operator*() const; //!< Return a reference to obj_
    RObject* operator->() const; //!< Return a pointer to obj_
    RObject* get() const;       //!< Return a pointer to obj_
  };
} // namespace Rice


// =========   String.hpp   =========

namespace Rice
{
  //! A Wraper for the ruby String class.
  /*! This class provides a C++-style interface to ruby's String class and
   *  its associated rb_str_* functions.
   *
   *  Example:
   *  \code
   *    String s(String::format("%s: %d", "foo", 42));
   *    std::cout << s.length() << std::endl;
   *  \endcode
   */
  class String
    : public Builtin_Object<T_STRING>
  {
  public:
    //! Construct a new string.
    String();

    //! Wrap an existing string.
    String(VALUE v);

    //! Wrap an existing string.
    String(Object v);

    //! Construct a String from an Identifier.
    String(Identifier id);

    //! Construct a String from a null-terminated C string.
    String(char const* s);

    //! Construct a String from an std::string.
    String(std::string const& s);

    //! Construct a String from an std::string_view.
    String(std::string_view const& s);

    //! Format a string using printf-style formatting.
    template <typename... Arg_Ts>
    static inline String format(char const* fmt, Arg_Ts&&...args);

    //! Get the length of the String.
    /*! \return the length of the string.
     */
    size_t length() const;

    //! Get the character at the given index.
    /*! \param index the desired index.
     *  \return the character at the given index.
     */
    char operator[](ptrdiff_t index) const;

    //! Return a pointer to the beginning of the underlying C string.
    char const* c_str() const;

    //! Return a copy of the string as an std::string.
    std::string str() const;

    //! Create an Identifier from the String.
    /*! Calls rb_intern to create an ID.
     *  \return an Identifier holding the ID returned from rb_intern.
     */
    Identifier intern() const;
  };
} // namespace Rice


// =========   Symbol.hpp   =========

namespace Rice
{
  //! A wrapper for ruby's Symbol class.
  /*! Symbols are internal identifiers in ruby.  They are singletons and
   *  can be thought of as frozen strings.  They differ from an Identifier
   *  in that they are in fact real Objects, but they can be converted
   *  back and forth between Identifier and Symbol.
   */
  class Symbol
    : public Object
  {
  public:
    //! Wrap an existing symbol.
    Symbol(VALUE v);

    //! Wrap an existing symbol.
    Symbol(Object v);

    //! Construct a Symbol from an Identifier.
    Symbol(Identifier id);

    //! Construct a Symbol from a null-terminated C string.
    Symbol(char const* s = "");

    //! Construct a Symbol from an std::string.
    Symbol(std::string const& s);

    //! Construct a Symbol from an std::string_view.
    Symbol(std::string_view const& s);

    //! Return a string representation of the Symbol.
    char const* c_str() const;

    //! Return a string representation of the Symbol.
    std::string str() const;

    //! Return the Symbol as an Identifier.
    Identifier to_id() const;
  };
} // namespace Rice



// =========   Array.hpp   =========

#include <iterator>
#include <memory>

namespace Rice
{
  //! A wrapper for the ruby Array class.
  /*! This class provides a C++-style interface to ruby's Array class and
   *  its associated rb_ary_* functions.
   *  Example:
   *  \code
   *    Array a;
   *    a.push(String("some string"));
   *    a.push(42);
   *  \endcode
   */
  class Array
    : public Builtin_Object<T_ARRAY>
  {
  public:
    //! Construct a new array
    Array();

    //! Construct a new array with specified size
    Array(long capacity);

    //! Wrap an existing array
    /*! \param v a ruby object, which must be of type T_ARRAY.
     */
    Array(Object v);

    //! Wrap an existing array
    /*! \param v a ruby object, which must be of type T_ARRAY.
     */
    Array(VALUE v);

    //! Construct an array from a sequence.
    /*! \param begin an iterator to the beginning of the sequence.
     *  \param end an iterator to the end of the sequence.
     */
    template<typename Iter_T>
    Array(Iter_T begin, Iter_T end);

    //! Construct an Array from a C array.
    /*! \param a a C array of type T and size n.
     */
    template<typename T, long n>
    Array(T const (&a)[n]);

  public:
    //! Return the size of the array.
    long size() const;

    //! Return the element at the given index.
    /*! \param index The index of the desired element.  The index may be
     *  negative, to indicate an offset from the end of the array.  If the
     *  index is out of bounds, this function has undefined behavior.
     *  \return the element at the given index.
     */
    Object operator[](long index) const;

    //! Converts a Ruby Array into a C++ array where the elements are
    //! contiguous. This is similar to a std::vector, but instead a
    //! std::unique_ptr<T> is returned. This allows the calling code to
    //! pass ownership to its callers if needed (for exmaple, From_Ruby<T>#convert)
    //! Note this method is designed convenience, not for speed or efficieny.
    //! C++ APIs that take large chunks of memory should not be passed Ruby Arrrays.
    //! \return std::unique_ptr that is owned by the caller.
    template<typename T>
    std::unique_ptr<T[]> pack();

  private:
    //! A helper class so array[index]=value can work.
    class Proxy;

  public:
    //! Return a reference to the element at the given index.
    /*! \param index The index of the desired element.  The index may be
     *  negative, to indicate an offset from the end of the array.  If the
     *  index is out of bounds, this function has undefined behavior.
     *  \return the element at the given index.
     */
    Proxy operator[](long index);

    //! Push an element onto the end of the array
    /*! \param v an object to push onto the array.
     *  \return the object which was pushed onto the array.
     */
    template<typename T>
    Object push(T const& obj);

    //! Pop an element from the end of the array
    /*! \return the object which was popped from the array, or Qnil if
     *  the array was empty.
     */
    Object pop();

    //! Unshift an element onto the beginning of the array
    /*! \param v an object to unshift onto the array.
     *  \return the object which was unshifted onto the array.
     */
    template<typename T>
    Object unshift(T const& obj);

    //! Shift an element from the beginning of the array
    /*! \return the object which was shifted from the array.
     */
    Object shift();

  private:
    template<typename Array_Ptr_T, typename Value_T>
    class Iterator;

    long position_of(long index) const;

  public:
    //! An iterator.
    typedef Iterator<Array*, Proxy> iterator;

    //! A const iterator.
    typedef Iterator<Array const*, Object> const_iterator;

    //! Return an iterator to the beginning of the array.
    iterator begin();

    //! Return a const iterator to the beginning of the array.
    const_iterator begin() const;

    //! Return an iterator to the end of the array.
    iterator end();

    //! Return a const iterator to the end of the array.
    const_iterator end() const;

    //! Return the content of the array as a std::vector
    template<typename T>
    std::vector<T> to_vector();
  };

  //! A helper class so array[index]=value can work.
  class Array::Proxy
  {
  public:
    //! Construct a new Proxy
    Proxy(Array array, long index);

    //! Implicit conversions
    operator Object() const;

    //! Explicit conversion to VALUE.
    VALUE value() const;

    //! Assignment operator.
    template<typename T>
    Object operator=(T const& value);

  private:
    Array array_;
    long index_;
  };

  //! A helper class for implementing iterators for a Array.
  // TODO: This really should be a random-access iterator.
  template<typename Array_Ptr_T, typename Value_T>
  class Array::Iterator
  {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = Value_T;
    using difference_type = long;
    using pointer = Object*;
    using reference = Value_T&;

    Iterator(Array_Ptr_T array, long index);

    template<typename Array_Ptr_T_, typename Value_T_>
    Iterator(Iterator<Array_Ptr_T_, Value_T_> const& rhs);

    template<typename Array_Ptr_T_, typename Value_T_>
    Iterator& operator=(Iterator<Array_Ptr_T_, Value_T_> const& rhs);

    Iterator& operator++();
    Iterator operator++(int);
    Value_T operator*();
    Object* operator->();

    template<typename Array_Ptr_T_, typename Value_T_>
    bool operator==(Iterator<Array_Ptr_T_, Value_T_> const& rhs) const;

    template<typename Array_Ptr_T_, typename Value_T_>
    bool operator!=(Iterator<Array_Ptr_T_, Value_T_> const& rhs) const;

    Array_Ptr_T array() const;
    long index() const;

  private:
    Array_Ptr_T array_;
    long index_;

    Object tmp_;
  };
} // namespace Rice


// =========   Hash.hpp   =========

#include <iterator>
#include <type_traits>

namespace Rice
{
  //! A wrapper for the ruby Hash class.
  //! This class provides a C++-style interface to ruby's Hash class and
  //! its associated rb_hash_* functions.
  //! Example:
  //! \code
  //!   Hash h;
  //!   h[42] = String("foo");
  //!   h[10] = String("bar");
  //!   std::cout << String(h[42]) << std::endl;
  //! \endcode
  class Hash: public Builtin_Object<T_HASH>
  {
  public:
    //! Construct a new hash.
    Hash();

    //! Wrap an existing hash.
    /*! \param v the hash to wrap.
     */
    Hash(Object v);

    //! Return the number of elements in the hash.
    size_t size() const;

  private:
    //! A helper class so hash[key]=value can work.
    class Proxy;

  public:
    //! Get the value for the given key.
    /*! \param key the key whose value should be retrieved from the hash.
     *  \return the value associated with the given key.
     */
    template<typename Key_T>
    Proxy const operator[](Key_T const& key) const;

    //! Get the value for the given key.
    /*! \param key the key whose value should be retrieved from the hash.
     *  \return the value associated with the given key.
     */
    template<typename Key_T>
    Proxy operator[](Key_T const& key);

    //! Get the value for the given key
    /*! The returned value is converted to the type given by Value_T.
     *  \param key the key whose value should be retrieved from the hash.
     *  \return the value associated with the given key, converted to C++
     *  type Value_T.
     */
    template<typename Value_T, typename Key_T>
    Value_T get(Key_T const& key);

    //! A helper class for dereferencing iterators
    class Entry;

    //! A helper class for implementing iterators for a Hash.
    template<typename Hash_Ptr_T, typename Value_T>
    class Iterator;

  public:
    //! An iterator.
    typedef Iterator<Hash*, Entry> iterator;

    //! A const iterator.
    typedef Iterator<Hash const*, Entry const> const_iterator;

  public:
    //! Return an iterator to the beginning of the hash.
    iterator begin();

    //! Return a const iterator to the beginning of the hash.
    const_iterator begin() const;

    //! Return an iterator to the end of the hash.
    iterator end();

    //! Return a const to the end of the hash.
    const_iterator end() const;
  };

  //! A helper class so hash[key]=value can work.
  class Hash::Proxy
  {
  public:
    //! Construct a new Proxy.
    Proxy(Hash* hash, Object key);

    //! Implicit conversion to Object.
    operator Object() const;

    //! Explicit conversion to VALUE.
    VALUE value() const;

    //! Assignment operator.
    template<typename T>
    Object operator=(T const& value);

  private:
    Hash* hash_;
    Object key_;
  };

  //! A helper class for dereferencing iterators
  /*! This class is intended to look similar to an std::pair.
   */
  class Hash::Entry
  {
  public:
    //! Construct a new Entry.
    Entry(Hash* hash, Object key);

    //! Copy constructor.
    Entry(Entry const& entry);

    Object const key;          //!< The key
    Object const& first;      //!< An alias for the key

    Proxy value;              //!< The value
    Proxy& second;           //!< An alias for the value

    Entry& operator=(Entry const& rhs);

    friend bool operator<(Entry const& lhs, Entry const& rhs);
  };

  bool operator<(Hash::Entry const& lhs, Hash::Entry const& rhs);

  //! A helper class for implementing iterators for a Hash.
  template<typename Hash_Ptr_T, typename Value_T>
  class Hash::Iterator
  {
  public:
    using iterator_category = std::input_iterator_tag;
    using value_type = Value_T;
    using difference_type = long;
    using pointer = Object*;
    using reference = Value_T&;

    //! Construct a new Iterator.
    Iterator(Hash_Ptr_T hash);

    //! Construct a new Iterator with a given start-at index point
    Iterator(Hash_Ptr_T hash, int start_at);

    //! Construct an Iterator from another Iterator of a different const
    //! qualification.
    template<typename Iterator_T>
    Iterator(Iterator_T const& iterator);

    //! Preincrement operator.
    Iterator& operator++();

    //! Postincrement operator.
    Iterator operator++(int);

    //! Dereference operator.
    Value_T operator*();

    //! Dereference operator.
    Value_T* operator->();

    //! Equality operator.
    bool operator==(Iterator const& rhs) const;

    //! Inequality operator.
    bool operator!=(Iterator const& rhs) const;

    template<typename Hash_Ptr_T_, typename Value_T_>
    friend class Hash::Iterator;

  protected:
    Object current_key();

    Array hash_keys();

  private:
    Hash_Ptr_T hash_;
    long current_index_;
    VALUE keys_;

    mutable typename std::remove_const<Value_T>::type tmp_;
  };
} // namespace Rice



// Type Conversion declarations

// =========   RubyType.hpp   =========

#include <set>

namespace Rice::detail
{
  template <typename T>
  class RubyType
  {
  public:
    static std::set<ruby_value_type> types();
    static std::set<ruby_value_type> convertibleFrom();
    static T(*converter)(VALUE);
    static std::string packTemplate;
  };
}


// =========   RubyType.ipp   =========
namespace Rice::detail
{
  template<>
  class RubyType<bool>
  {
  public:
    using FromRuby_T = bool(*)(VALUE);

    static inline FromRuby_T fromRuby = RB_TEST;
    static inline std::set<ruby_value_type> Exact = { RUBY_T_TRUE, RUBY_T_FALSE };
    static inline std::set<ruby_value_type> Castable = { RUBY_T_NIL };
    static inline std::set<ruby_value_type> Narrowable = {  };
    static inline std::string packTemplate = "not supported";
  };

  template<>
  class RubyType<char>
  {
  public:
    using FromRuby_T = char(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2char_inline;
    static inline std::set<ruby_value_type> Exact = { };
    static inline std::set<ruby_value_type> Castable = { RUBY_T_STRING };
    static inline std::set<ruby_value_type> Narrowable = { RUBY_T_FIXNUM };
    static inline std::string packTemplate = CHAR_MIN < 0 ? "c*" : "C*";
  };

  template<>
  class RubyType<signed char>
  {
  public:
    // Hack - need to later typecast
    using FromRuby_T = char(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2char_inline;
    static inline std::set<ruby_value_type> Exact = { };
    static inline std::set<ruby_value_type> Castable = { RUBY_T_STRING };
    static inline std::set<ruby_value_type> Narrowable = { RUBY_T_FIXNUM };
    static inline std::string packTemplate = "c*";
  };

  template<>
  class RubyType<unsigned char>
  {
  public:
    // Hack - need to later typecast, although char's in ruby are unsigned
    using FromRuby_T = char(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2char_inline;
    static inline std::set<ruby_value_type> Exact = { };
    static inline std::set<ruby_value_type> Castable = { RUBY_T_STRING };
    static inline std::set<ruby_value_type> Narrowable = { RUBY_T_FIXNUM };
    static inline std::string packTemplate = "C*";
  };

  template<>
  class RubyType<short>
  {
  public:
    using FromRuby_T = short(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2short_inline;
    static inline std::set<ruby_value_type> Exact = { };
    static inline std::set<ruby_value_type> Castable = { };
    static inline std::set<ruby_value_type> Narrowable = { RUBY_T_FIXNUM };
    static inline std::string packTemplate = "s*";
  };

  template<>
  class RubyType<unsigned short>
  {
  public:
    using FromRuby_T = unsigned short(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2ushort;
    static inline std::set<ruby_value_type> Exact = { };
    static inline std::set<ruby_value_type> Castable = { };
    static inline std::set<ruby_value_type> Narrowable = { RUBY_T_FIXNUM };
    static inline std::string packTemplate = "S*";
  };

  template<>
  class RubyType<int>
  {
  public:
    using FromRuby_T = int(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2int_inline;
    static inline std::set<ruby_value_type> Exact = { RUBY_T_FIXNUM };
    static inline std::set<ruby_value_type> Castable = { };
    static inline std::set<ruby_value_type> Narrowable = { };
    static inline std::string packTemplate = "i*";
  };

  template<>
  class RubyType<unsigned int>
  {
  public:
    using FromRuby_T = unsigned int(*)(VALUE);

    static inline FromRuby_T fromRuby = RB_NUM2UINT;
    static inline std::set<ruby_value_type> Exact = { RUBY_T_FIXNUM };
    static inline std::set<ruby_value_type> Castable = { };
    static inline std::set<ruby_value_type> Narrowable = { };
    static inline std::string packTemplate = "I*";
  };

  template<>
  class RubyType<long>
  {
  public:
    using FromRuby_T = long(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2long_inline;
    static inline std::set<ruby_value_type> Exact = { RUBY_T_FIXNUM };
    static inline std::set<ruby_value_type> Castable = { };
    static inline std::set<ruby_value_type> Narrowable = { };
    static inline std::string packTemplate = "l_*";
  };

  template<>
  class RubyType<unsigned long>
  {
  public:
    using FromRuby_T = unsigned long(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2ulong_inline;
    static inline std::set<ruby_value_type> Exact = { RUBY_T_FIXNUM  };
    static inline std::set<ruby_value_type> Castable = { };
    static inline std::set<ruby_value_type> Narrowable = { };
    static inline std::string packTemplate = "L_*";
  };

  template<>
  class RubyType<long long>
  {
  public:
    using FromRuby_T = long long(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2ll_inline;
    static inline std::set<ruby_value_type> Exact = { RUBY_T_FIXNUM, RUBY_T_BIGNUM };
    static inline std::set<ruby_value_type> Castable = { };
    static inline std::set<ruby_value_type> Narrowable = { };
    static inline std::string packTemplate = "q_*";
  };

  template<>
  class RubyType<unsigned long long>
  {
  public:
    using FromRuby_T = unsigned long long(*)(VALUE);

    static inline FromRuby_T fromRuby = RB_NUM2ULL;
    static inline std::set<ruby_value_type> Exact = { RUBY_T_FIXNUM, RUBY_T_BIGNUM };
    static inline std::set<ruby_value_type> Castable = { };
    static inline std::set<ruby_value_type> Narrowable = { };
    static inline std::string packTemplate = "Q_*";
  };


  template<>
  class RubyType<float>
  {
  public:
    using FromRuby_T = double(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2dbl;
    static inline std::set<ruby_value_type> Exact = { };
    static inline std::set<ruby_value_type> Castable = { RUBY_T_FIXNUM };
    static inline std::set<ruby_value_type> Narrowable = { RUBY_T_FLOAT };
    static inline std::string packTemplate = "f*";
  };

  template<>
  class RubyType<double>
  {
  public:
    using FromRuby_T = double(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2dbl;
    static inline std::set<ruby_value_type> Exact = { RUBY_T_FLOAT };
    static inline std::set<ruby_value_type> Castable = { RUBY_T_FIXNUM, RUBY_T_BIGNUM };
    static inline std::set<ruby_value_type> Narrowable = { };
    static inline std::string packTemplate = "d*";
  };
}
// =========   Wrapper.hpp   =========

namespace Rice
{
namespace detail
{

class Wrapper
{
public:
  Wrapper(bool isOwner = false);
  virtual ~Wrapper() = default;
  virtual void* get() = 0;

  void ruby_mark();
  void addKeepAlive(VALUE value);
  void setOwner(bool value);

protected:
  bool isOwner_ = false;

private:
  // We use a vector for speed and memory locality versus a set which does
  // not scale well when getting to tens of thousands of objects (not expecting
  // that to happen...but just in case)
  std::vector<VALUE> keepAlive_;
};

template <typename T, typename Wrapper_T = void>
VALUE wrap(VALUE klass, rb_data_type_t* rb_type, T& data, bool isOwner);

template <typename T, typename Wrapper_T = void>
VALUE wrap(VALUE klass, rb_data_type_t* rb_type, T* data, bool isOwner);

template <typename T>
T* unwrap(VALUE value, rb_data_type_t* rb_type, bool transferOwnership);

Wrapper* getWrapper(VALUE value, rb_data_type_t* rb_type);

template <typename T>
void replace(VALUE value, rb_data_type_t* rb_type, T* data, bool isOwner);

Wrapper* getWrapper(VALUE value);

} // namespace detail
} // namespace Rice



// =========   to_ruby.hpp   =========

namespace Rice
{
  namespace detail
  {
    //! Convert a C++ object to Ruby.
    /*! If x is a pointer, wraps the pointee as a Ruby object.  If x is an
     *  Object, returns x.
     *
     *  If no conversion exists a compile-time error is generated.
     *
     *  \param x the object to convert.
     *  \return a Ruby representation of the C++ object.
     *
     *  Example:
     *  \code
     *    rb_p(to_ruby(42));
     *
     *    Foo * p_foo = new Foo();
     *    rb_p(to_ruby(p_foo));
     *  \endcode
     */
    template <typename T>
    class To_Ruby;
   
    // Helper template function that let's users avoid having to specify the template type - its deduced
    template <typename T>
    VALUE to_ruby(T&& x)
    {
      using Unqualified_T = remove_cv_recursive_t<T>;
      return To_Ruby<Unqualified_T>().convert(std::forward<T>(x));
    }

    // Helper template function that let's users avoid having to specify the template type - its deduced
    template <typename T>
    VALUE to_ruby(T* x)
    {
      using Unqualified_T = remove_cv_recursive_t<T>;
      return To_Ruby<Unqualified_T*>().convert(x);
    }
  } // detail
} // Rice


// =========   from_ruby.hpp   =========

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

  enum class Convertible: uint8_t
  {
      None =   0b000,
      Narrowable = 0b001,
      Cast =   0b011,
      Exact =  0b111,
  };
}



// =========   Arg.hpp   =========

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
    Arg_Type& defaultValue();

    //! Tell the receiving object to keep this argument alive
    //! until the receiving object is freed.
    Arg& keepAlive();
    
    //! Returns if the argument should be kept alive
    bool isKeepAlive() const;

    //! Specifies if the argument should be treated as a value
    Arg& setValue();

    //! Returns if the argument should be treated as a value
    bool isValue() const;

    //! Specifies C++ will take ownership of this value and Ruby should not free it
    Arg& transferOwnership();
    bool isTransfer();

  public:
    const std::string name;
    int32_t position = -1;

  private:
    //! Our saved default value
    std::any defaultValue_;
    bool isValue_ = false;
    bool isKeepAlive_ = false;
    bool isTransfer_ = false;
  };
} // Rice


// =========   Arg.ipp   =========
namespace Rice
{
  inline Arg::Arg(std::string name) : name(name)
  {
  }

  template<typename Arg_Type>
  inline Arg& Arg::operator=(Arg_Type val)
  {
    this->defaultValue_ = val;
    return *this;
  }

  //! Check if this Arg has a default value associated with it
  inline bool Arg::hasDefaultValue() const
  {
    return this->defaultValue_.has_value();
  }

  //! Return a reference to the default value associated with this Arg
  /*! \return the type saved to this Arg
    */
  template<typename Arg_Type>
  inline Arg_Type& Arg::defaultValue()
  {
    return std::any_cast<Arg_Type&>(this->defaultValue_);
  }

  inline Arg& Arg::keepAlive()
  {
    this->isKeepAlive_ = true;
    return *this;
  }

  inline bool Arg::isKeepAlive() const
  {
    return this->isKeepAlive_;
  }

  inline Arg& Arg::setValue()
  {
    isValue_ = true;
    return *this;
  }

  inline bool Arg::isValue() const
  {
    return isValue_;
  }

  inline Arg& Arg::transferOwnership()
  {
    this->isTransfer_ = true;
    return *this;
  }

  inline bool Arg::isTransfer()
  {
    return this->isTransfer_;
  }


} // Rice
// =========   Return.hpp   =========

namespace Rice
{
  //! Helper for defining Return argument of a method

  class Return
  {
  public:
    //! Specifies Ruby should take ownership of the returned value
    Return& takeOwnership();

    //! Does Ruby own the returned value?
    bool isOwner();

    //! Specifies the returned value is a Ruby value
    Return& setValue();

    //! Is the returned value a Ruby value?
    bool isValue() const;

    //! Tell the returned object to keep alive the receving object
    Return& keepAlive();

    //! Is the returned value being kept alive?
    bool isKeepAlive() const;

  private:
    bool isKeepAlive_ = false;
    bool isOwner_ = false;
    bool isValue_ = false;
  };
} // Rice


// =========   Return.ipp   =========
#include <any>

namespace Rice
{
  inline Return& Return::takeOwnership()
  {
    this->isOwner_ = true;
    return *this;
  }

  inline bool Return::isOwner()
  {
    return this->isOwner_;
  }

  inline Return& Return::setValue()
  {
    this->isValue_ = true;
    return *this;
  }

  inline bool Return::isValue() const
  {
    return this->isValue_;
  }

  inline Return& Return::keepAlive()
  {
    this->isKeepAlive_ = true;
    return *this;
  }

  inline bool Return::isKeepAlive() const
  {
    return this->isKeepAlive_;
  }
}  // Rice

// =========   to_ruby.ipp   =========

namespace Rice
{
  namespace detail
  {
    template<>
    class To_Ruby<void>
    {
    public:
      VALUE convert(const void*)
      {
        throw std::runtime_error("Converting from void pointer is not implemented");
        return Qnil;
      }
    };

    template<>
    class To_Ruby<std::nullptr_t>
    {
    public:
      VALUE convert(std::nullptr_t const)
      {
        return Qnil;
      }
    };

    template<>
    class To_Ruby<short>
    {
    public:
      VALUE convert(const short& native)
      {
#ifdef rb_int2num_inline
        return protect(rb_int2num_inline, (int)native);
#else
        return RB_INT2NUM(native);
#endif
      }
    };

    template<>
    class To_Ruby<short&>
    {
    public:
      VALUE convert(const short& native)
      {
#ifdef rb_int2num_inline
        return protect(rb_int2num_inline, (int)native);
#else
        return RB_INT2NUM(native);
#endif
      }
    };

    template<>
    class To_Ruby<int>
    {
    public:
      VALUE convert(const int& native)
      {
#ifdef rb_int2num_inline
        return protect(rb_int2num_inline, (int)native);
#else
        return RB_INT2NUM(native);
#endif
      }

      VALUE convert(const volatile int& native)
      {
#ifdef rb_int2num_inline
        return protect(rb_int2num_inline, (int)native);
#else
        return RB_INT2NUM(native);
#endif
      }
    };

    template<>
    class To_Ruby<int&>
    {
    public:
      VALUE convert(const int& native)
      {
#ifdef rb_int2num_inline
        return protect(rb_int2num_inline, (int)native);
#else
        return RB_INT2NUM(native);
#endif
      }
    };

    template<>
    class To_Ruby<long>
    {
    public:
      VALUE convert(const long& native)
      {
        return protect(rb_long2num_inline, native);
      }
    };

    template<>
    class To_Ruby<long&>
    {
    public:
      VALUE convert(const long& native)
      {
        return protect(rb_long2num_inline, native);
      }
    };

    template<>
    class To_Ruby<long long>
    {
    public:
      VALUE convert(const long long& native)
      {
        return protect(rb_ll2inum, native);
      }
    };

    template<>
    class To_Ruby<long long&>
    {
    public:
      VALUE convert(const long long& native)
      {
        return protect(rb_ll2inum, native);
      }
    };

    template<>
    class To_Ruby<unsigned short>
    {
    public:
      VALUE convert(const unsigned short& native)
      {
#ifdef rb_int2num_inline
        return protect(rb_uint2num_inline, (unsigned int)native);
#else
        return RB_UINT2NUM(native);
#endif
      }
    };

    template<>
    class To_Ruby<unsigned short&>
    {
    public:
      VALUE convert(const unsigned short& native)
      {
#ifdef rb_int2num_inline
        return protect(rb_uint2num_inline, (unsigned int)native);
#else
        return RB_UINT2NUM(native);
#endif
      }
    };

    template<>
    class To_Ruby<unsigned int>
    {
    public:
      VALUE convert(const unsigned int& native)
      {
#ifdef rb_int2num_inline
        return protect(rb_uint2num_inline, (unsigned int)native);
#else
        return RB_UINT2NUM(native);
#endif
      }
    };

    template<>
    class To_Ruby<unsigned int&>
    {
    public:
      VALUE convert(const unsigned int& native)
      {
#ifdef rb_int2num_inline
        return protect(rb_uint2num_inline, (unsigned int)native);
#else
        return RB_UINT2NUM(native);
#endif
      }
    };

    template<>
    class To_Ruby<unsigned long>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
      {
      }

      VALUE convert(const unsigned long& native)
      {
        if (this->returnInfo_ && this->returnInfo_->isValue())
        {
          return native;
        }
        else
        {
          return protect(rb_ulong2num_inline, native);
        }
      }

    private:
      Return* returnInfo_ = nullptr;
    };

    template<>
    class To_Ruby<unsigned long&>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
      {
      }

      VALUE convert(const unsigned long& native)
      {
        if (this->returnInfo_ && this->returnInfo_->isValue())
        {
          return native;
        }
        else
        {
          return protect(rb_ulong2num_inline, native);
        }
      }

    private:
      Return* returnInfo_ = nullptr;
    };

    template<>
    class To_Ruby<unsigned long long>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
      {
      }

      VALUE convert(const unsigned long long& native)
      {
        if (this->returnInfo_ && this->returnInfo_->isValue())
        {
          return native;
        }
        else
        {
          return protect(rb_ull2inum, (unsigned long long)native);
        }
      }

      VALUE convert(const volatile unsigned long long& native)
      {
        if (this->returnInfo_ && this->returnInfo_->isValue())
        {
          return native;
        }
        else
        {
          return protect(rb_ull2inum, (unsigned long long)native);
        }
      }
    private:
      Return* returnInfo_ = nullptr;
    };

    template<>
    class To_Ruby<unsigned long long&>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
      {
      }

      VALUE convert(const unsigned long long& native)
      {
        if (this->returnInfo_ && this->returnInfo_->isValue())
        {
          return native;
        }
        else
        {
          return protect(rb_ull2inum, (unsigned long long)native);
        }
      }

    private:
      Return* returnInfo_ = nullptr;
    };

    template<>
    class To_Ruby<float>
    {
    public:
      VALUE convert(const float& native)
      {
        return protect(rb_float_new, (double)native);
      }
    };

    template<>
    class To_Ruby<float&>
    {
    public:
      VALUE convert(const float& native)
      {
        return protect(rb_float_new, (double)native);
      }
    };

    template<>
    class To_Ruby<double>
    {
    public:
      VALUE convert(const double& native)
      {
        return protect(rb_float_new, native);
      }
    };

    template<>
    class To_Ruby<double&>
    {
    public:
      VALUE convert(const double& native)
      {
        return protect(rb_float_new, native);
      }
    };

    template<>
    class To_Ruby<bool>
    {
    public:
      VALUE convert(const bool& native)
      {
        return native ? Qtrue : Qfalse;
      }
    };

    template<>
    class To_Ruby<bool&>
    {
    public:
      VALUE convert(const bool& native)
      {
        return native ? Qtrue : Qfalse;
      }
    };

    template<>
    class To_Ruby<char>
    {
    public:
      VALUE convert(const char& native)
      {
        return To_Ruby<int>().convert(native);
      }
    };

    template<>
    class To_Ruby<char&>
    {
    public:
      VALUE convert(const char& native)
      {
        return To_Ruby<int>().convert(native);
      }
    };

    template<>
    class To_Ruby<char*>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
      {
      }

      VALUE convert(const char* buffer)
      {
        if (!buffer)
        {
          return Qnil;
        }
        else if (strlen(buffer) > 0 && buffer[0] == ':')
        {
          size_t symbolLength = strlen(buffer) - 1;
          char* symbol = new char[symbolLength];
          strncpy(symbol, buffer + 1, symbolLength);
          ID id = protect(rb_intern2, symbol, (long)symbolLength);
          delete[] symbol;
          return protect(rb_id2sym, id);
        }
        else if (this->returnInfo_ && this->returnInfo_->isOwner())
        {
          // This copies the buffer but does not free it. So Ruby is not really
          // taking ownership of it. But there isn't a Ruby API for creating a string
          // from an existing buffer and later freeing it.
          return protect(rb_usascii_str_new_cstr, buffer);
        }
        else
        {
          // Does NOT copy the passed in buffer and does NOT free it when the string is GCed
          long size = (long)strlen(buffer);
          return protect(rb_usascii_str_new_static, buffer, size);
        }
      }

    private:
      Return* returnInfo_ = nullptr;
    };

    template<int N>
    class To_Ruby<char[N]>
    {
    public:
      VALUE convert(const char buffer[])
      {
        if (N > 0 && buffer[0] == ':')
        {
          // N count includes a NULL character at the end of the string
          constexpr size_t symbolLength = N - 1;
          char symbol[symbolLength];
          strncpy(symbol, buffer + 1, symbolLength);
          ID id = protect(rb_intern, symbol);
          return protect(rb_id2sym, id);
        }
        else
        {
          long size = (long)strlen(buffer);
          return protect(rb_usascii_str_new_static, buffer, size);
        }
      }
    };

    template<>
    class To_Ruby<unsigned char>
    {
    public:
      VALUE convert(const unsigned char& native)
      {
        return To_Ruby<unsigned int>().convert(native);
      }
    };

    template<>
    class To_Ruby<unsigned char&>
    {
    public:
      VALUE convert(const unsigned char& native)
      {
        return To_Ruby<unsigned int>().convert(native);
      }
    };

    template<>
    class To_Ruby<signed char>
    {
    public:
      VALUE convert(const signed char& native)
      {
        return To_Ruby<signed int>().convert(native);
      }
    };

    template<>
    class To_Ruby<signed char&>
    {
    public:
      VALUE convert(const signed char& native)
      {
        return To_Ruby<signed int>().convert(native);
      }
    };
  }
}
// =========   from_ruby.ipp   =========
#include <limits>
#include <optional>
#include <stdexcept>

/* This file implements conversions from Ruby to native values fo fundamental types 
   such as bool, int, float, etc. It also includes conversions for chars and strings */
namespace Rice::detail
{
  inline Convertible operator&(Convertible left, Convertible right)
  {
    return static_cast<Convertible>(static_cast<uint8_t>(left) & static_cast<uint8_t>(right));
  }

  inline Convertible operator|(Convertible left, Convertible right)
  {
    return static_cast<Convertible>(static_cast<uint8_t>(left) | static_cast<uint8_t>(right));
  }

  inline bool operator<(Convertible left, Convertible right)
  {
    return static_cast<uint8_t>(left) < static_cast<uint8_t>(right);
  }

  // ===========  Helpers  ============
  template<typename T>
  class FromRubyFundamental
  {
  public:
    using RubyType_T = RubyType<T>;

    static Convertible is_convertible(VALUE value)
    {
      ruby_value_type valueType = rb_type(value);

      if (RubyType_T::Exact.find(valueType) != RubyType_T::Exact.end())
      {
        return Convertible::Exact;
      }
      else if (RubyType_T::Castable.find(valueType) != RubyType_T::Castable.end())
      {
        return Convertible::Cast;
      }
      else if (RubyType_T::Narrowable.find(valueType) != RubyType_T::Narrowable.end())
      {
        return Convertible::Narrowable;
      }
      else
      {
        return Convertible::None;
      }
    }

    static T convert(VALUE value)
    {
      return (T)protect(RubyType_T::fromRuby, value);
    }
  };

  template<typename T>
  class FromRubyFundamental<T*>
  {
  public:
    using RubyType_T = RubyType<T>;

    static Convertible is_convertible(VALUE value)
    {
      ruby_value_type valueType = rb_type(value);

      if (RubyType_T::Exact.find(valueType) != RubyType_T::Exact.end())
      {
        return Convertible::Exact;
      }
      else if (valueType == RUBY_T_ARRAY)
      {
        return Convertible::Exact;
      }
      else if (valueType == RUBY_T_STRING)
      {
        // Maybe we should check for ascii8bit encoding?
        //if (RB_ENCODING_IS_ASCII8BIT(value))
        return Convertible::Exact;
      }

      return Convertible::None;
    }

    static std::unique_ptr<T[]> convert(VALUE value)
    {
      ruby_value_type valueType = rb_type(value);
      switch (valueType)
      {
        case RUBY_T_NIL:
        {
          return nullptr;
          break;
        }
        case RUBY_T_ARRAY:
        {
          Array array(value);
          return array.pack<T>();
          break;
        }
        case RUBY_T_STRING:
        {
          long size = RSTRING_LEN(value);
          // Put in null character
          std::unique_ptr<T[]> dest = std::make_unique<T[]>(size + 1);
          dest.get()[size] = 0;
          memcpy(dest.get(), RSTRING_PTR(value), size);

          return std::move(dest);
          break;
        }
        default:
        {
          if (RubyType_T::Exact.find(valueType) != RubyType_T::Exact.end() ||
              RubyType_T::Castable.find(valueType) != RubyType_T::Castable.end() ||
              RubyType_T::Narrowable.find(valueType) != RubyType_T::Narrowable.end())
          {
            std::unique_ptr<T[]> dest = std::make_unique<T[]>(1);
            *(dest.get()) = (T)protect(RubyType_T::fromRuby, value);
            return std::move(dest);
          }
          else
          {
            std::string typeName = detail::typeName(typeid(T));
            throw Exception(rb_eTypeError, "wrong argument type %s (expected % s*)",
              detail::protect(rb_obj_classname, value), typeName.c_str());
          }
        }
      }
    }
  };

  // ===========  bool  ============
  template<>
  class From_Ruby<bool>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<bool>::is_convertible(value);
    }

    bool convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<bool>();
      }
      else
      {
        return FromRubyFundamental<bool>::convert(value);
      }
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<bool&>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<bool>::is_convertible(value);
    }

    bool& convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<bool>();
      }
      else
      {
        this->converted_ = FromRubyFundamental<bool>::convert(value);
        return this->converted_;
      }
    }

  private:
    Arg* arg_ = nullptr;
    bool converted_ = false;
  };

  template<>
  class From_Ruby<bool*>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    ~From_Ruby()
    {
      if (this->arg_ && this->arg_->isTransfer())
      {
        this->converted_.release();
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<bool>::is_convertible(value);
    }

    bool* convert(VALUE value)
    {
      this->converted_ = FromRubyFundamental<bool*>::convert(value);
      return this->converted_.get();
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<bool[]> converted_;
  };

  // ===========  char  ============
  template<>
  class From_Ruby<char>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<char>::is_convertible(value);
    }

    char convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<char>();
      }
      else
      {
        return FromRubyFundamental<char>::convert(value);
      }
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<char&>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<char>::is_convertible(value);
    }

    char& convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<char>();
      }
      else
      {
        this->converted_ = FromRubyFundamental<char>::convert(value);
        return this->converted_;
      }
    }

  private:
    Arg* arg_ = nullptr;
    char converted_ = 0;
  };

  template<>
  class From_Ruby<char*>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    ~From_Ruby()
    {
      if (this->arg_ && this->arg_->isTransfer())
      {
        this->converted_.release();
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<char*>::is_convertible(value);
    }

    char* convert(VALUE value)
    {
      this->converted_ = FromRubyFundamental<char*>::convert(value);
      return this->converted_.get();
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<char[]> converted_;
  };
    
  template<>
  class From_Ruby<const char*>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    ~From_Ruby()
    {
      if (this->arg_ && this->arg_->isTransfer())
      {
        this->converted_.release();
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<char*>::is_convertible(value);
    }

    char* convert(VALUE value)
    {
      this->converted_ = FromRubyFundamental<char*>::convert(value);
      return this->converted_.get();
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<char[]> converted_;
  };

  // ===========  unsigned char  ============
  template<>
  class From_Ruby<unsigned char>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned char>::is_convertible(value);
    }

    unsigned char convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<unsigned char>();
      }
      else
      {
        return FromRubyFundamental<unsigned char>::convert(value);
      }
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<unsigned char&>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned char>::is_convertible(value);
    }

    unsigned char& convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<unsigned char>();
      }
      else
      {
        this->converted_ = FromRubyFundamental<unsigned char>::convert(value);
        return this->converted_;
      }
    }

  private:
    Arg* arg_ = nullptr;
    unsigned char converted_ = 0;
  };

  template<>
  class From_Ruby<unsigned char*>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    ~From_Ruby()
    {
      if (this->arg_ && this->arg_->isTransfer())
      {
        this->converted_.release();
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned char*>::is_convertible(value);
    }

    unsigned char* convert(VALUE value)
    {
      this->converted_ = FromRubyFundamental<unsigned char*>::convert(value);
      return this->converted_.get();
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<unsigned char[]> converted_;
  };

  // ===========  signed char  ============
  template<>
  class From_Ruby<signed char>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<signed char>::is_convertible(value);
    }

    signed char convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<signed char>();
      }
      else
      {
        return FromRubyFundamental<signed char>::convert(value);
      }
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<signed char*>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    ~From_Ruby()
    {
      if (this->arg_ && this->arg_->isTransfer())
      {
        this->converted_.release();
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<signed char*>::is_convertible(value);
    }

    signed char* convert(VALUE value)
    {
      this->converted_ = FromRubyFundamental<signed char*>::convert(value);
      return this->converted_.get();
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<signed char[]> converted_;
  };

  // ===========  double  ============
  template<>
  class From_Ruby<double>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<double>::is_convertible(value);
    }

    double convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<double>();
      }
      else
      {
        return FromRubyFundamental<double>::convert(value);
      }
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<double&>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<double>::is_convertible(value);
    }

    double& convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<double>();
      }
      else
      {
        this->converted_ = FromRubyFundamental<double>::convert(value);
        return this->converted_;
      }
    }

  private:
    Arg* arg_ = nullptr;
    double converted_;
  };

  template<>
  class From_Ruby<double*>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    ~From_Ruby()
    {
      if (this->arg_ && this->arg_->isTransfer())
      {
        this->converted_.release();
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<double*>::is_convertible(value);
    }

    double* convert(VALUE value)
    {
      this->converted_ = FromRubyFundamental<double*>::convert(value);
      return this->converted_.get();
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<double[]> converted_;
  };

  // ===========  float  ============
  template<>
  class From_Ruby<float>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<float>::is_convertible(value);
    }

    float convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<float>();
      }
      else
      {
        return FromRubyFundamental<float>::convert(value);
      }
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<float&>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<float>::is_convertible(value);
    }

    float& convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<float>();
      }
      else
      {
        this->converted_ = FromRubyFundamental<float>::convert(value);
        return this->converted_;
      }
    }

  private:
    Arg* arg_ = nullptr;
    float converted_;
  };

  template<>
  class From_Ruby<float*>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    ~From_Ruby()
    {
      if (this->arg_ && this->arg_->isTransfer())
      {
        this->converted_.release();
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<float*>::is_convertible(value);
    }

    float* convert(VALUE value)
    {
      this->converted_ = FromRubyFundamental<float*>::convert(value);
      return this->converted_.get();
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<float[]> converted_;
  };

  // ===========  int  ============
  template<>
  class From_Ruby<int>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      Convertible result = FromRubyFundamental<int>::is_convertible(value);

      // Is this an enum? If so we want to support converting it to an integer
      if (result == Convertible::None && rb_type(value) == RUBY_T_DATA)
      {
        static ID id = protect(rb_intern, "to_int");
        if (protect(rb_respond_to, value, id))
        {
          result = Convertible::Cast;
        }
      }
      return result;
    }

    int convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<int>();
      }
      else
      {
        return FromRubyFundamental<int>::convert(value);
      }
    }
  
  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<int&>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<int>::is_convertible(value);
    }

    int& convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<int>();
      }
      else
      {
        this->converted_ = FromRubyFundamental<int>::convert(value);
        return this->converted_;
      }
    }

  private:
    Arg* arg_ = nullptr;
    int converted_ = 0;
  };

  template<>
  class From_Ruby<int*>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    ~From_Ruby()
    {
      if (this->arg_ && this->arg_->isTransfer())
      {
        this->converted_.release();
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<int*>::is_convertible(value);
    }

    int* convert(VALUE value)
    {
      this->converted_ = FromRubyFundamental<int*>::convert(value);
      return this->converted_.get();
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<int[]> converted_;
  };

  // ===========  unsigned int  ============
  template<>
  class From_Ruby<unsigned int>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned int>::is_convertible(value);
    }

    unsigned int convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<unsigned int>();
      }
      else
      {
        return FromRubyFundamental<unsigned int>::convert(value);
      }
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<unsigned int&>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned int>::is_convertible(value);
    }

    unsigned int& convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<unsigned int>();
      }
      else
      {
        this->converted_ = FromRubyFundamental<unsigned int>::convert(value);
        return this->converted_;
      }
    }

  private:
    Arg* arg_ = nullptr;
    unsigned int converted_ = 0;
  };

  template<>
  class From_Ruby<unsigned int*>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    ~From_Ruby()
    {
      if (this->arg_ && this->arg_->isTransfer())
      {
        this->converted_.release();
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned int*>::is_convertible(value);
    }

    unsigned int* convert(VALUE value)
    {
      this->converted_ = FromRubyFundamental<unsigned int*>::convert(value);
      return this->converted_.get();
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<unsigned int[]> converted_;;
  };

  // ===========  long  ============
  template<>
  class From_Ruby<long>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<long>::is_convertible(value);
    }

    long convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<long>();
      }
      else
      {
        return FromRubyFundamental<long>::convert(value);
      }
    }
  
  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<long&>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<long>::is_convertible(value);
    }

    long& convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<long>();
      }
      else
      {
        this->converted_ = FromRubyFundamental<long>::convert(value);
        return this->converted_;
      }
    }

  private:
    Arg* arg_ = nullptr;
    long converted_ = 0;
  };

  template<>
  class From_Ruby<long*>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    ~From_Ruby()
    {
      if (this->arg_ && this->arg_->isTransfer())
      {
        this->converted_.release();
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<long*>::is_convertible(value);
    }

    long* convert(VALUE value)
    {
      this->converted_ = FromRubyFundamental<long*>::convert(value);
      return this->converted_.get();
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<long[]> converted_;
  };

  // ===========  unsigned long  ============
  template<>
  class From_Ruby<unsigned long>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned long>::is_convertible(value);
    }

    unsigned long convert(VALUE value)
    {
      if (this->arg_ && this->arg_->isValue())
      {
        return (unsigned long)value;
      }
      else if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<unsigned long>();
      }
      else
      {
        return FromRubyFundamental<unsigned long>::convert(value);
      }
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<unsigned long&>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned long>::is_convertible(value);
    }

    unsigned long& convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<unsigned long>();
      }
      else
      {
        this->converted_ = FromRubyFundamental<unsigned long>::convert(value);
        return this->converted_;
      }
    }

  private:
    Arg* arg_ = nullptr;
    unsigned long converted_ = 0;
  };

  template<>
  class From_Ruby<unsigned long*>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    ~From_Ruby()
    {
      if (this->arg_ && this->arg_->isTransfer())
      {
        this->converted_.release();
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned long*>::is_convertible(value);
    }

    unsigned long* convert(VALUE value)
    {
      this->converted_ = FromRubyFundamental<unsigned long*>::convert(value);
      return this->converted_.get();
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<unsigned long[]> converted_;
  };

  // ===========  unsigned long long  ============
  template<>
  class From_Ruby<unsigned long long>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned long long>::is_convertible(value);
    }

    unsigned long long convert(VALUE value)
    {
      if (this->arg_ && this->arg_->isValue())
      {
        return value;
      }
      else if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<unsigned long long>();
      }
      else
      {
        return FromRubyFundamental<unsigned long long>::convert(value);
      }
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<unsigned long long&>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned long long>::is_convertible(value);
    }

    unsigned long long& convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<unsigned long long>();
      }
      else
      {
        this->converted_ = FromRubyFundamental<unsigned long long>::convert(value);
        return this->converted_;
      }
    }

  private:
    Arg* arg_ = nullptr;
    unsigned long long converted_ = 0;
  };

  template<>
  class From_Ruby<unsigned long long*>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    ~From_Ruby()
    {
      if (this->arg_ && this->arg_->isTransfer())
      {
        this->converted_.release();
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned long long*>::is_convertible(value);
    }

    unsigned long long* convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }
      else
      {
        this->converted_ = FromRubyFundamental<unsigned long long*>::convert(value);
        return this->converted_.get();
      }
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<unsigned long long[]> converted_;
  };

// ===========  long long  ============
  template<>
  class From_Ruby<long long>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<long long>::is_convertible(value);
    }

    long long convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<long long>();
      }
      else
      {
        return FromRubyFundamental<long long>::convert(value);
      }
    }
  
  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<long long&>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<long long>::is_convertible(value);
    }

    long long& convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<long long>();
      }
      else
      {
        this->converted_ = FromRubyFundamental<long long>::convert(value);
        return this->converted_;
      }
    }

  private:
    Arg* arg_ = nullptr;
    long long converted_ = 0;
  };

  template<>
  class From_Ruby<long long*>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    ~From_Ruby()
    {
      if (this->arg_ && this->arg_->isTransfer())
      {
        this->converted_.release();
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<long long*>::is_convertible(value);
    }

    long long* convert(VALUE value)
    {
      this->converted_ = FromRubyFundamental<long long*>::convert(value);
      return this->converted_.get();
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<long long[]> converted_;
  };

  // ===========  short  ============
  template<>
  class From_Ruby<short>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<short>::is_convertible(value);
    }

    short convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<short>();
      }
      else
      {
        return FromRubyFundamental<short>::convert(value);
      }
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<short&>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<short>::is_convertible(value);
    }

    short& convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<short>();
      }
      else
      {
        this->converted_ = FromRubyFundamental<short>::convert(value);
        return this->converted_;
      }
    }

  private:
    Arg* arg_ = nullptr;
    short converted_ = 0;
  };

  template<>
  class From_Ruby<short*>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    ~From_Ruby()
    {
      if (this->arg_ && this->arg_->isTransfer())
      {
        this->converted_.release();
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<short*>::is_convertible(value);
    }

    short* convert(VALUE value)
    {
      this->converted_ = FromRubyFundamental<short*>::convert(value);
      return this->converted_.get();
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<short[]> converted_;
  };

  // ===========  unsigned short  ============
  template<>
  class From_Ruby<unsigned short>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned short>::is_convertible(value);
    }

    unsigned short convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<unsigned short>();
      }
      else
      {
        return FromRubyFundamental<unsigned short>::convert(value);
      }
    }
  
  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<unsigned short&>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned short>::is_convertible(value);
    }

    unsigned short& convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<unsigned short>();
      }
      else
      {
        this->converted_ = FromRubyFundamental<unsigned short>::convert(value);
        return this->converted_;
      }
    }

  private:
    Arg* arg_ = nullptr;
    unsigned short converted_ = 0;
  };

  template<>
  class From_Ruby<unsigned short*>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    ~From_Ruby()
    {
      if (this->arg_ && this->arg_->isTransfer())
      {
        this->converted_.release();
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned short*>::is_convertible(value);
    }

    unsigned short* convert(VALUE value)
    {
      this->converted_ = FromRubyFundamental<unsigned short*>::convert(value);
      return this->converted_.get();
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<unsigned short[]> converted_;
  };

  template<>
  class From_Ruby<void*>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
      if (this->arg_->isTransfer())
      {
        throw Exception(rb_eTypeError, "Cannot transfer ownership of string data to C++ void pointer");
      }
    }

    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          return Convertible::Exact;
          break;
        }
        case RUBY_T_STRING:
        {
          if (RB_ENCODING_IS_ASCII8BIT(value))
            return Convertible::Exact;
          else
            return Convertible::None;
          break;
        }
        case RUBY_T_NIL:
        {
          return Convertible::Exact;
          break;
        }
        default:
        {
          return Convertible::None;
        }
      }
    }

    void* convert(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          // Since C++ is not telling us type information, we need to extract it
          // from the Ruby object.
          const rb_data_type_t* rb_type = RTYPEDDATA_TYPE(value);
          return detail::unwrap<void>(value, (rb_data_type_t*)rb_type, this->arg_ && this->arg_->isTransfer());
          break;
        }
        case RUBY_T_STRING:
        {
          // String must be formatted in a way the receiver understands! This likely means it was created
          // by Array.pack. Once the underlying string goes away the passed in data becomes invalid!
          return (void*)RSTRING_PTR(value);
          break;
        }
        case RUBY_T_NIL:
        {
          return nullptr;
          break;
        }
        default:
          throw Exception(rb_eTypeError, "wrong argument type %s (expected % s)",
            detail::protect(rb_obj_classname, value), "pointer");
      }
    }
  private:
    Arg* arg_ = nullptr;
  };
}
// =========   Proc.hpp   =========


// Registries

// =========   TypeRegistry.hpp   =========

#include <optional>
#include <unordered_map>
#include <set>
#include <regex>


/* The type registry keeps track of all C++ types wrapped by Rice. When a native function returns 
   an instance of a class/struct we look up its type to verity that it has been registered. 
   
   We have to do this to support C++ inheritance. If a C++ function returns a pointer/reference
   to an Abstract class, the actual returned object will be a Child class. However, all we know
   from the C++ method signature is that it is an Absract class - thus the need for a registry.*/

namespace Rice::detail
{
  class TypeRegistry
  {
  public:
    template <typename T>
    void add(VALUE klass, rb_data_type_t* rbType);

    template <typename T>
    void remove();

    template <typename T>
    bool isDefined();

    template <typename T>
    std::pair<VALUE, rb_data_type_t*> getType();

    template <typename T>
    bool verify();
      
    template <typename T>
    std::pair<VALUE, rb_data_type_t*> figureType(const T& object);

    // Validate types and throw if any types are unverified
    void validateTypes();

    // Clear unverified types. This is mostly for unit tests
    void clearUnverifiedTypes();

  private:
    std::optional<std::pair<VALUE, rb_data_type_t*>> lookup(const std::type_info& typeInfo);
    void raiseUnverifiedType(const std::string& typeName);

    std::unordered_map<std::type_index, std::pair<VALUE, rb_data_type_t*>> registry_{};
    std::set<std::type_index> unverified_{};
  };
}


// =========   TypeRegistry.ipp   =========
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <typeindex>


namespace Rice::detail
{
  template <typename T>
  inline void TypeRegistry::add(VALUE klass, rb_data_type_t* rbType)
  {
    std::type_index key(typeid(T));
    registry_[key] = std::pair(klass, rbType);
  }

  /* Special case void. Rice defines classes using the class name not a pointer to the
     class. Thus define_class<void> is more consistent with Rice then 
     define_class<void*>. However, the types of void and void* are different so we need
     this special case.
     
     It is possible to support define_class<void*>, but it requires changing the static
     assertions on Data_Type and Data_Object and thus seems less desirable (and less 
     consistent as mentioned above).*/
  template <>
  inline void TypeRegistry::add<void>(VALUE klass, rb_data_type_t* rbType)
  {
    // The special case, use void*
    std::type_index key(typeid(void*));
    registry_[key] = std::pair(klass, rbType);
  }

  template <typename T>
  inline void TypeRegistry::remove()
  {
    std::type_index key(typeid(T));
    registry_.erase(key);
  }

  template <typename T>
  inline bool TypeRegistry::isDefined()
  {
    std::type_index key(typeid(T));
    auto iter = registry_.find(key);
    return iter != registry_.end();
  }

  template <typename T>
  std::pair<VALUE, rb_data_type_t*> TypeRegistry::getType()
  {
    std::type_index key(typeid(T));
    auto iter = registry_.find(key);
    if (iter != registry_.end())
    {
      return iter->second;
    }
    else
    {
      this->raiseUnverifiedType(typeid(T).name());
      // Make compiler happy
      return std::make_pair(Qnil, nullptr);
    }
  }

  // Special case void. See comment for add above.
  template <>
  inline bool TypeRegistry::isDefined<void>()
  {
    std::type_index key(typeid(void*));
    auto iter = registry_.find(key);
    return iter != registry_.end();
  }

  template <typename T>
  inline bool TypeRegistry::verify()
  {
    if (isDefined<T>())
    {
      return true;
    }
    else
    {
      const std::type_info& typeInfo = typeid(T);
      std::type_index key(typeInfo);
      this->unverified_.insert(key);
      return false;
    }
  }

  inline std::optional<std::pair<VALUE, rb_data_type_t*>> TypeRegistry::lookup(const std::type_info& typeInfo)
  {
    std::type_index key(typeInfo);
    auto iter = registry_.find(key);

    if (iter == registry_.end())
    {
      return std::nullopt;
    }
    else
    {
      return iter->second;
    }
  }

  template <typename T>
  inline std::pair<VALUE, rb_data_type_t*> TypeRegistry::figureType(const T& object)
  {
    // First check and see if the actual type of the object is registered
    std::optional<std::pair<VALUE, rb_data_type_t*>> result = lookup(typeid(object));

    if (result)
    {
      return result.value();
    }

    // If not, then we are willing to accept an ancestor class specified by T. This is needed
    // to support Directors. Classes inherited from Directors are never actually registered
    // with Rice - and what we really want it to return the C++ class they inherit from.
    const std::type_info& typeInfo = typeid(T);
    result = lookup(typeInfo);
    if (result)
    {
      return result.value();
    }

    raiseUnverifiedType(detail::typeName(typeInfo));
    // Make the compiler happy
    return std::pair<VALUE, rb_data_type_t*>(Qnil, nullptr);
  }

  inline void TypeRegistry::validateTypes()
  {
    // Loop over the unverified types and delete each on that is found in the registry
    // the registry and raise an exception for the first one that is not
    for (auto iter = this->unverified_.begin(); iter != this->unverified_.end(); )
    {
      const std::type_index& typeIndex = *iter;
      bool isDefined = this->registry_.find(typeIndex) != this->registry_.end();
      if (isDefined)
      {
        iter = this->unverified_.erase(iter);
      }
      else
      {
        iter++;
      }
    }

    if (this->unverified_.empty())
    {
      return;
    }

    std::stringstream stream;
    stream << "The following types are not registered with Rice:" << "\n";

    for (const std::type_index& typeIndex : this->unverified_)
    {
      stream << "  " << typeName(typeIndex) << "\n";
    }

    throw std::invalid_argument(stream.str());
  }

  inline void TypeRegistry::clearUnverifiedTypes()
  {
    this->unverified_.clear();
  }

  inline void TypeRegistry::raiseUnverifiedType(const std::string& typeName)
  {
    std::string message = "Type is not registered with Rice: " + typeName;
    throw std::invalid_argument(message);
  }
}
// =========   InstanceRegistry.hpp   =========

#include <map>

namespace Rice::detail
{
  class InstanceRegistry
  {
  public:
    template <typename T>
    VALUE lookup(T& cppInstance);

    template <typename T>
    VALUE lookup(T* cppInstance);
    VALUE lookup(void* cppInstance);

    void add(void* cppInstance, VALUE rubyInstance);
    void remove(void* cppInstance);
    void clear();

  public:
    bool isEnabled = false;

  private:
    std::map<void*, VALUE> objectMap_;
  };
} // namespace Rice::detail



// =========   InstanceRegistry.ipp   =========
#include <memory>

namespace Rice::detail
{
  template <typename T>
  inline VALUE InstanceRegistry::lookup(T& cppInstance)
  {
    return this->lookup((void*)&cppInstance);
  }

  template <typename T>
  inline VALUE InstanceRegistry::lookup(T* cppInstance)
  {
    return this->lookup((void*)cppInstance);
  }

  inline VALUE InstanceRegistry::lookup(void* cppInstance)
  {
    if (!this->isEnabled)
      return Qnil;

    auto it = this->objectMap_.find(cppInstance);
    if (it != this->objectMap_.end())
    {
      return it->second;
    }
    else
    {
      return Qnil;
    }
  }

  inline void InstanceRegistry::add(void* cppInstance, VALUE rubyInstance)
  {
    if (this->isEnabled)
    {
      this->objectMap_[cppInstance] = rubyInstance;
    }
  }

  inline void InstanceRegistry::remove(void* cppInstance)
  {
    this->objectMap_.erase(cppInstance);
  }

  inline void InstanceRegistry::clear()
  {
    this->objectMap_.clear();
  }
} // namespace


// =========   DefaultHandler.hpp   =========

namespace Rice::detail
{
  class DefaultHandler
  {
  public:
    void operator()() const;
  };
}

// =========   DefaultHandler.ipp   =========
namespace Rice::detail
{
  inline void Rice::detail::DefaultHandler::operator()() const
  {
    // This handler does nothing, it just rethrows the exception so it can be handled
    throw;
  }
}
// =========   HandlerRegistry.hpp   =========

#include <functional>

namespace Rice::detail
{
  class HandlerRegistry
  {
  public:
    HandlerRegistry();
    void set(std::function<void()> handler);
    std::function<void()> handler() const;

  private:
    std::function<void()> handler_;
  };
} // namespace Rice::detail



// =========   HandlerRegistry.ipp   =========
namespace Rice::detail
{
  inline HandlerRegistry::HandlerRegistry() : handler_(DefaultHandler())
  {
  }

  inline void HandlerRegistry::set(std::function<void()> handler)
  {
    this->handler_ = handler;
  }

  inline std::function<void()> HandlerRegistry::handler() const
  {
    return this->handler_;
  }
}

// =========   Native.hpp   =========

namespace Rice::detail
{
  class Native;

  class Resolved
  {
  public:
    inline bool operator<(Resolved other);
    inline bool operator>(Resolved other);

    Convertible convertible;
    double parameterMatch;
    Native* native;
  };

  class Native
  {
  public:
    static VALUE resolve(int argc, VALUE* argv, VALUE self);
  public:
    virtual ~Native() = default;
    VALUE call(int argc, VALUE* argv, VALUE self);

    virtual Resolved matches(int argc, const VALUE* argv, VALUE self) = 0;
    virtual VALUE operator()(int argc, const VALUE* argv, VALUE self) = 0;
  };
}


// =========   NativeRegistry.hpp   =========

#include <map>
#include <memory>
#include <utility>

/* The Native Registry tracks C++ instance that are used to invoke C++ methods for Ruby. 
   These objects include instances of the NativeFunction, NativeIterator, NativeAttributeGet
   and NativeAttributeSet Each instance is specialized to call a specific C++ function, method
   or attribute that is exposed to Ruby.
   
   The registry stores these C++ instances using a map of vectors. The map is keyed on the 
   the Ruby class (VALUE) and method id (ID). The value is a vector of Native pointers stored
   in a std::unique_ptr. Thus the registry takes ownership of the pointers when calling
   code adds them to the registry. The value is a vector to support C++ method overloading.
   
   Note - when an existing Ruby class is redefined using rb_define_class, its VALUE stays the same
   but all its methods and fields are reset. Thus any call to rb_define_class must be followed
   by calling the reset method on the registry. Although redefinition shouldn't happen in 
   production code it happens in many places in the unit tests. */
   
namespace Rice::detail
{
  class NativeRegistry
  {
  public:
    void add(VALUE klass, ID methodId, std::unique_ptr<Native>& native);
    void reset(VALUE klass);
    const std::vector<std::unique_ptr<Native>>& lookup(VALUE klass, ID methodId);

  private:
    // Key - Ruby klass/method
    // Value - Vector of Native pointers owned by the registry (thus wrapped in std::unique_ptr)
    std::map<std::pair<VALUE, ID>, std::vector<std::unique_ptr<Native>>> natives_ = {};
  };
}


// =========   NativeRegistry.ipp   =========

namespace Rice::detail
{
  inline void NativeRegistry::add(VALUE klass, ID methodId, std::unique_ptr<Native>& native)
  {
    if (rb_type(klass) == T_ICLASS)
    {
      klass = detail::protect(rb_class_of, klass);
    }

    // Create the key
    std::pair<VALUE, ID> key = std::make_pair(klass, methodId);

    // Lookup items for method
    std::vector<std::unique_ptr<Native>>& natives = this->natives_[key];

    natives.push_back(std::move(native));
  }

  inline void NativeRegistry::reset(VALUE klass)
  {
    for (auto iter = this->natives_.begin(); iter != this->natives_.end();)
    {
      // Iter points to a std::pair<std::pair<VALUE, ID>, std::vector<NativeRegistryItem>
      if (iter->first.first == klass)
      {
        iter = this->natives_.erase(iter);
      }
      else
      {
        ++iter;
      }
    }
  }
  
  inline const std::vector<std::unique_ptr<Native>>& NativeRegistry::lookup(VALUE klass, ID methodId)
  {
    if (rb_type(klass) == T_ICLASS)
    {
      klass = detail::protect(rb_class_of, klass);
    }

    // Create the key
    std::pair<VALUE, ID> key = std::make_pair(klass, methodId);

    // Lookup items for method
    return this->natives_[key];
  }
}

// =========   Registries.hpp   =========

namespace Rice::detail
{
  class Registries
  {
  public:
    static Registries instance;

  public:
    HandlerRegistry handlers;
    InstanceRegistry instances;
    NativeRegistry natives;
    TypeRegistry types;
  };
}


// =========   Registries.ipp   =========
namespace Rice::detail
{
  //Initialize static variables here.
  inline Registries Registries::instance;
}


// =========   Type.ipp   =========
#ifdef __GNUC__
#include <cxxabi.h>
#include <cstdlib>
#include <cstring>
#endif

namespace Rice::detail
{
  template<typename T>
  bool Type<T>::verify()
  {
    if constexpr (std::is_fundamental_v<T>)
    {
      return true;
    }
    else
    {
      return Registries::instance.types.verify<T>();
    }
  }

  template<>
  struct Type<void>
  {
    static bool verify()
    {
      return true;
    }
  };

  template<typename T>
  void verifyType()
  {
    Type<intrinsic_type<T>>::verify();
  }

  template<typename Tuple_T, size_t...Is>
  void verifyTypesImpl()
  {
    (verifyType<typename std::tuple_element<Is, Tuple_T>::type>(), ...);
  }

  template<typename Tuple_T>
  void verifyTypes()
  {
    if constexpr (std::tuple_size<Tuple_T>::value > 0)
    {
      verifyTypesImpl<Tuple_T, std::tuple_size<Tuple_T>::value - 1>();
    }
  }

  inline std::string demangle(char const* mangled_name)
  {
#ifdef __GNUC__
    struct Helper
    {
      Helper(
        char const* mangled_name)
        : name_(0)
      {
        int status = 0;
        name_ = abi::__cxa_demangle(mangled_name, 0, 0, &status);
      }

      ~Helper()
      {
        std::free(name_);
      }

      char* name_;

    private:
      Helper(Helper const&);
      void operator=(Helper const&);
    };

    Helper helper(mangled_name);
    if (helper.name_)
    {
      return helper.name_;
    }
    else
    {
      return mangled_name;
    }
#else
    return mangled_name;
#endif
  }

  inline std::string typeName(const std::type_info& typeInfo)
  {
    return demangle(typeInfo.name());
  }

  inline std::string typeName(const std::type_index& typeIndex)
  {
    return demangle(typeIndex.name());
  }

  // Find text inside of < > taking into account nested groups.
  // 
  // Example:
  //  
  //   std::vector<std::vector<int>, std::allocator<std::vector, std::allocator<int>>>
  inline std::string findGroup(std::string& string, size_t offset)
  {
    int depth = 0;

    auto begin = string.begin() + offset;
    auto start = begin;
    for (auto iter = begin; iter != string.end(); iter++)
    {
      if (*iter == '<')
      {
        if (depth == 0)
        {
          start = iter;
        }
        depth++;
      }
      else if (*iter == '>')
      {
        depth--;
        if (depth == 0)
        {
          // Add + 1 to include current ">" character
          return string.substr(offset + (start - begin), 1 + (iter - start));
        }
        else if (depth < 0)
        {
          throw std::runtime_error("Unbalanced Group");
        }
      }
    }
    throw std::runtime_error("Unbalanced Group");
  }

  inline void replaceAll(std::string& string, std::regex regex, std::string replacement)
  {
    std::smatch match;
    while (std::regex_search(string, match, regex))
    {
      string = std::regex_replace(string, regex, replacement);
    }
  }

  inline void removeGroup(std::string& string, std::regex regex)
  {
    std::smatch match;
    while (std::regex_search(string, match, regex))
    {
      std::string group = findGroup(string, match.position());
      group = match.str() + group;
      string.erase(match.position(), group.length());
    }
  }

  inline std::string makeClassName(const std::string& typeInfoName)
  {
    std::string base = typeInfoName;

    // Remove class keyword
    auto classRegex = std::regex("class +");
    base = std::regex_replace(typeInfoName, classRegex, "");

    // Remove struct keyword
    auto structRegex = std::regex("struct +");
    base = std::regex_replace(base, structRegex, "");

    // Remove std::__[^:]*::
    auto stdClangRegex = std::regex("std::__[^:]+::");
    base = std::regex_replace(base, stdClangRegex, "");
      
    // Remove std::
    auto stdRegex = std::regex("std::");
    base = std::regex_replace(base, stdRegex, "");

    // Replace basic_string with string
    auto basicStringRegex = std::regex(R"(basic_string)");
    replaceAll(base, basicStringRegex, "string");
      
    // Remove allocators
    std::regex allocatorRegex(R"(,\s*allocator)");
    removeGroup(base, allocatorRegex);

    // Remove char_traits
    std::regex charTraitsRegex(R"(,\s*char_traits)");
    removeGroup(base, charTraitsRegex);

    // Remove less (std::map)
    std::regex lessRegex(R"(,\s*less)");
    removeGroup(base, lessRegex);

    // Remove hash (std::unordered_map)
    std::regex hashRegex(R"(,\s*hash)");
    removeGroup(base, hashRegex);

    // Remove equal_to (std::unordered_map)
    std::regex equalRegex(R"(,\s*equal_to)");
    removeGroup(base, equalRegex);

    // Remove spaces before pointers
    auto ptrRegex = std::regex(R"(\s+\*)");
    base = std::regex_replace(base, ptrRegex, "*");

    // Remove __ptr64
    std::regex ptr64Regex(R"(\s*__ptr64\s*)");
    base = std::regex_replace(base, ptr64Regex, "");

    // Replace " >" with ">"
    auto trailingAngleBracketSpaceRegex = std::regex(R"(\s+>)");
    replaceAll(base, trailingAngleBracketSpaceRegex, ">");

    // One space after a comma (MSVC has no spaces, GCC one space)
    auto commaSpaceRegex = std::regex(R"(,(\S))");
    replaceAll(base, commaSpaceRegex, ", $1");

    // Capitalize first letter
    base[0] = std::toupper(base[0]);

    // Replace :: or _ and capitalize the next letter
    std::regex namespaceRegex(R"((_|::)(\w))");
    std::smatch namespaceMatch;
    while (std::regex_search(base, namespaceMatch, namespaceRegex))
    {
      std::string replacement = namespaceMatch[2];
      std::transform(replacement.begin(), replacement.end(), replacement.begin(), ::toupper);
      base.replace(namespaceMatch.position(), namespaceMatch.length(), replacement);
    }

    // Replace spaces with unicode U+u00A0 (Non breaking Space)
    auto spaceRegex = std::regex(R"(\s+)");
    replaceAll(base, spaceRegex, "\u00A0");

    // Replace < with unicode U+227A (Precedes)
    auto lessThanRegex = std::regex("<");
    //replaceAll(base, lessThanRegex, u8"≺");
    replaceAll(base, lessThanRegex, "\u227A");

    // Replace > with unicode U+227B (Succeeds)
    auto greaterThanRegex = std::regex(">");
    //replaceAll(base, greaterThanRegex, u8"≻");
    replaceAll(base, greaterThanRegex, "\u227B");

    // Replace , with Unicode Character (U+066C) - Single Low-9 Quotation Mark
    auto commaRegex = std::regex(R"(,\s*)");
    replaceAll(base, commaRegex, "\u201A");

    return base;
  }
}

// Code for Ruby to call C++

// =========   Exception.ipp   =========

namespace Rice
{
  inline Exception::Exception(VALUE exception) : exception_(exception)
  {
  }

  template <typename... Arg_Ts>
  inline Exception::Exception(const Exception& other, char const* fmt, Arg_Ts&&...args)
    : Exception(other.class_of(), fmt, std::forward<Arg_Ts>(args)...)
  {
  }

  template <typename... Arg_Ts>
  inline Exception::Exception(const VALUE exceptionClass, char const* fmt, Arg_Ts&&...args)
  {
    #if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wformat-security"
    #endif

    size_t size = std::snprintf(nullptr, 0, fmt, std::forward<Arg_Ts>(args)...);
    this->message_ = std::string(size, '\0');

    // size+1 avoids truncating the string. Otherwise snprintf writes n - 1 characters
    // to allow space for null character but we don't need that since std::string
    // will add a null character internally at n + 1
    std::snprintf(&this->message_[0], size + 1, fmt, std::forward<Arg_Ts>(args)...);

    #if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic pop
    #endif

    // Now create the Ruby exception
    this->exception_ = detail::protect(rb_exc_new2, exceptionClass, this->message_.c_str());
  }

  inline char const* Exception::what() const noexcept
  {
    if (this->message_.empty())
    {
      // This isn't protected because if it fails then either we could eat the exception
      // (not good) or crash the program (better)
      VALUE rubyMessage = rb_funcall(this->exception_, rb_intern("message"), 0);
      this->message_ = std::string(RSTRING_PTR(rubyMessage), RSTRING_LEN(rubyMessage));
    }
    return this->message_.c_str();
  }

  inline VALUE Exception::class_of() const
  {
    return detail::protect(rb_class_of, this->exception_);
  }

  inline VALUE Exception::value() const
  {
    return this->exception_;
  }
}

// =========   cpp_protect.hpp   =========

#include <regex>
#include <stdexcept>

#if __has_include(<filesystem>)
  #include <filesystem>
  namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
  #include <experimental/filesystem>
  namespace fs = std::experimental::filesystem;
#else
  #error "no filesystem include found :'("
#endif


namespace Rice::detail
{
  template <typename Callable_T>
  auto cpp_protect(Callable_T&& func)
  {
    try
    {
      return func();
    }
    catch (...)
    {
      try
      {
        std::function<void()> handler = detail::Registries::instance.handlers.handler();
        handler();
      }
      catch (::Rice::Exception const& ex)
      {
        rb_exc_raise(ex.value());
      }
      catch (::Rice::JumpException const& ex)
      {
        rb_jump_tag(ex.tag);
      }
      catch (std::bad_alloc const& ex)
      {
        /* This won't work quite right if the rb_exc_new2 fails; not
           much we can do about that, since Ruby doesn't give us access
           to a pre-allocated NoMemoryError object */
        rb_exc_raise(rb_exc_new2(rb_eNoMemError, ex.what()));
      }
      catch (std::domain_error const& ex)
      {
        rb_exc_raise(rb_exc_new2(rb_eFloatDomainError, ex.what()));
      }
      catch (std::invalid_argument const& ex)
      {
        rb_exc_raise(rb_exc_new2(rb_eArgError, ex.what()));
      }
      catch (fs::filesystem_error const& ex)
      {
        rb_exc_raise(rb_exc_new2(rb_eIOError, ex.what()));
      }
      catch (std::length_error const& ex)
      {
        rb_exc_raise(rb_exc_new2(rb_eIndexError, ex.what()));
      }
      catch (std::out_of_range const& ex)
      {
        rb_exc_raise(rb_exc_new2(rb_eIndexError, ex.what()));
      }
      catch (std::overflow_error const& ex)
      {
        rb_exc_raise(rb_exc_new2(rb_eRangeError, ex.what()));
      }
      catch (std::range_error const& ex)
      {
        rb_exc_raise(rb_exc_new2(rb_eRangeError, ex.what()));
      }
      catch (std::regex_error const& ex)
      {
        rb_exc_raise(rb_exc_new2(rb_eRegexpError, ex.what()));
      }
      catch (std::system_error const& ex)
      {
        rb_exc_raise(rb_exc_new2(rb_eSystemCallError, ex.what()));
      }
      catch (std::underflow_error const& ex)
      {
        rb_exc_raise(rb_exc_new2(rb_eRangeError, ex.what()));
      }
      catch (std::exception const& ex)
      {
        rb_exc_raise(rb_exc_new2(rb_eRuntimeError, ex.what()));
      }
      catch (...)
      {
        rb_exc_raise(rb_exc_new2(rb_eRuntimeError, "Unknown C++ exception thrown"));
      }
      throw std::runtime_error("Should never get here - just making compilers happy");
    }
  }
}

// =========   Wrapper.ipp   =========
#include <memory>

namespace Rice::detail
{
  inline Wrapper::Wrapper(bool isOwner): isOwner_(isOwner)
  {
  }

  inline void Wrapper::ruby_mark()
  {
    for (VALUE value : this->keepAlive_)
    {
      rb_gc_mark(value);
    }
  }

  inline void Wrapper::addKeepAlive(VALUE value)
  {
    this->keepAlive_.push_back(value);
  }

  inline void Wrapper::setOwner(bool value)
  {
    this->isOwner_ = value;
  }

  template <typename T>
  class WrapperValue : public Wrapper
  {
  public:
    WrapperValue(T& data): data_(std::move(data))
    {
    }

    ~WrapperValue()
    {
      Registries::instance.instances.remove(this->get());
    }

    void* get() override
    {
      return (void*)&this->data_;
    }

  private:
    T data_;
  };

  template <typename T>
  class WrapperReference : public Wrapper
  {
  public:
    WrapperReference(T& data): data_(data)
    {
    }

    ~WrapperReference()
    {
      Registries::instance.instances.remove(this->get());
    }

    void* get() override
    {
      return (void*)&this->data_;
    }

  private:
    T& data_;
  };

  template <typename T>
  class WrapperPointer : public Wrapper
  {
  public:
    WrapperPointer(T* data, bool isOwner) : Wrapper(isOwner), data_(data)
    {
    }

    ~WrapperPointer()
    {
      Registries::instance.instances.remove(this->get());

      if constexpr (std::is_destructible_v<T>)
      {
        if (this->isOwner_)
        {
          delete this->data_;
        }
      }
    }

    void* get() override
    {
      return (void*)this->data_;
    }

  private:
    T* data_ = nullptr;
  };

  // ---- Helper Functions -------
  template <typename T, typename Wrapper_T>
  inline VALUE wrap(VALUE klass, rb_data_type_t* rb_type, T& data, bool isOwner)
  {
    VALUE result = Registries::instance.instances.lookup(&data);

    if (result != Qnil)
      return result;

    Wrapper* wrapper = nullptr;

    // Is this a pointer but cannot be copied? For example a std::unique_ptr
    if constexpr (!std::is_void_v<Wrapper_T> && !std::is_copy_constructible_v<Wrapper_T>)
    {
      wrapper = new Wrapper_T(std::move(data));
      result = TypedData_Wrap_Struct(klass, rb_type, wrapper);
    }
    // Is this a pointer or smart pointer like std::shared_ptr
    else if constexpr (!std::is_void_v<Wrapper_T>)
    {
      wrapper = new Wrapper_T(data);
      result = TypedData_Wrap_Struct(klass, rb_type, wrapper);
    }
    // If ruby is the owner than copy the object if possible
    else if (isOwner)
    {
      if constexpr (std::is_copy_constructible_v<T> || std::is_move_constructible_v<T>)
      {
        wrapper = new WrapperValue<T>(data);
        result = TypedData_Wrap_Struct(klass, rb_type, wrapper);
      }
      else
      {
        std::string message = "Ruby was directed to take ownership of a C++ object but it does not have an accessible copy or move constructor. Type: " +
          typeName(typeid(T));
        throw std::runtime_error(message);
      }
    }
    // Ruby is not the owner so just wrap the reference
    else
    {
      wrapper = new WrapperReference<T>(data);
      result = TypedData_Wrap_Struct(klass, rb_type, wrapper);
    }

    Registries::instance.instances.add(wrapper->get(), result);

    return result;
  };

  template <typename T, typename Wrapper_T>
  inline VALUE wrap(VALUE klass, rb_data_type_t* rb_type, T* data, bool isOwner)
  {
    VALUE result = Registries::instance.instances.lookup(data);

    if (result != Qnil)
      return result;

    Wrapper* wrapper = nullptr;

    if constexpr (!std::is_void_v<Wrapper_T>)
    {
      wrapper = new Wrapper_T(data);
      result = TypedData_Wrap_Struct(klass, rb_type, wrapper);
    }
    else
    {
      wrapper = new WrapperPointer<T>(data, isOwner);
      result = TypedData_Wrap_Struct(klass, rb_type, wrapper);
    }

    Registries::instance.instances.add(wrapper->get(), result);
    return result;
  };

  template <typename T>
  inline T* unwrap(VALUE value, rb_data_type_t* rb_type, bool transferOwnership)
  {
    Wrapper* wrapper = getWrapper(value, rb_type);
    if (transferOwnership)
      wrapper->setOwner(false);

    if (wrapper == nullptr)
    {
      std::string message = "Wrapped C++ object is nil. Did you override " + 
                            std::string(detail::protect(rb_obj_classname, value)) + 
                            "#initialize and forget to call super?";

      throw std::runtime_error(message);
    }

    return static_cast<T*>(wrapper->get());
  }
    
  inline Wrapper* getWrapper(VALUE value, rb_data_type_t* rb_type)
  {
    Wrapper* wrapper = nullptr;
    TypedData_Get_Struct(value, Wrapper, rb_type, wrapper);
    return wrapper;
  }

  inline Wrapper* getWrapper(VALUE value)
  {
    // Turn off spurious warning on g++ 12
    #if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Warray-bounds"
    #endif

    return RTYPEDDATA_P(value) ? static_cast<Wrapper*>(RTYPEDDATA_DATA(value)) : nullptr;
    
    #if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic pop
    #endif
  }

  template <typename T>
  inline void replace(VALUE value, rb_data_type_t* rb_type, T* data, bool isOwner)
  {
    WrapperPointer<T>* wrapper = nullptr;
    TypedData_Get_Struct(value, WrapperPointer<T>, rb_type, wrapper);
    if (wrapper)
    {
      Registries::instance.instances.remove(wrapper->get());
      delete wrapper;
    }

    wrapper = new WrapperPointer<T>(data, isOwner);
    RTYPEDDATA_DATA(value) = wrapper;

    Registries::instance.instances.add(data, value);
  }
} // namespace

// =========   MethodInfo.hpp   =========

#include <vector>

namespace Rice
{
  class MethodInfo
  {
  public:
    template <typename...Arg_Ts>
    MethodInfo(size_t argCount, const Arg_Ts&...args);

    /**
      * Get the rb_scan_args format string for this
      * list of arguments.
      */
    std::string formatString();

    /**
      * Add a defined Arg to this list of Arguments
      */
    void addArg(const Arg& arg);

    Arg& arg(size_t pos);

    // Iterator support
    std::vector<Arg>::iterator begin();
    std::vector<Arg>::iterator end();

    Return returnInfo;

  private:
    template <typename Arg_T>
    void processArg(const Arg_T& arg);

    std::vector<Arg> args_;
  };
}

// =========   MethodInfo.ipp   =========
#include <sstream>

namespace Rice
{
  template <typename...Arg_Ts>
  inline MethodInfo::MethodInfo(size_t argCount, const Arg_Ts&...args)
  {
    // Process the passed in arguments
    (this->processArg(args), ...);

    // Fill in any missing arguments
    for (size_t i = this->args_.size(); i < argCount; i++)
    {
      Arg arg("arg_" + std::to_string(i));
      this->addArg(arg);
    }

    // TODO - so hacky but update the Arg positions
    for (uint32_t i = 0; i < this->args_.size(); i++)
    {
      this->args_[i].position = i;
    }
  }

  template <typename Arg_T>
  inline void MethodInfo::processArg(const Arg_T& arg)
  {
    if constexpr (std::is_same_v<Arg_T, Arg>)
    {
      this->addArg(arg);
    }
    else
    {
      this->returnInfo = arg;
    }
  }

  inline void MethodInfo::addArg(const Arg& arg)
  {
    this->args_.push_back(arg);
  }

  inline std::string MethodInfo::formatString()
  {
    size_t required = 0;
    size_t optional = 0;

    for (const Arg& arg : this->args_)
    {
      if (arg.hasDefaultValue())
      {
        optional++;
      }
      else
      {
        required++;
      }
    }

    return std::to_string(required) + std::to_string(optional);
  }

  inline Arg& MethodInfo::arg(size_t pos)
  {
    return args_[pos];
  }

  inline std::vector<Arg>::iterator MethodInfo::begin()
  {
    return this->args_.begin();
  }

  inline std::vector<Arg>::iterator MethodInfo::end()
  {
    return this->args_.end();
  }
}

// =========   Native.ipp   =========

namespace Rice::detail
{
  inline bool Resolved::operator<(Resolved other)
  {
    if (this->convertible != other.convertible)
    {
      return this->convertible < other.convertible;
    }
    else
    {
      return this->parameterMatch < other.parameterMatch;
    }
  }

  inline bool Resolved::operator>(Resolved other)
  {
    if (this->convertible != other.convertible)
    {
      return this->convertible > other.convertible;
    }
    else
    {
      return this->parameterMatch > other.parameterMatch;
    }
  }

  inline VALUE Native::resolve(int argc, VALUE* argv, VALUE self)
  {
    /* This method is called from Ruby and is responsible for determining the correct
       Native object (ie, NativeFunction, NativeIterator, NativeAttributeGet and 
       NativeAttributeSet) that shoudl be used to invoke the underlying C++ code.
       Most of the time there will be a single Native object registered for a C++ function,
       method, constructor, iterator or attribute. However, there can be multiple Natives 
       when a C++ function/method/construtor is overloaded.

       In that case, the code iterates over each Native and calls its matches method. The matches
       method returns a Resolved object which includes a Convertible field and parameterMatch field.
       The Convertible field is an enum that specifies if the types of the values supplied by Ruby
       match the types of the C++ function parameters. Allowed values include  can be Exact (example Ruby into to C++ int),
       TypeCast (example Ruby into to C++ float) or None (cannot be converted). 

       The parameterMatch field is simply the number or arguments provided by Ruby divided by the
       number of arguments required by C++. These numbers can be different because C++ method 
       parameters can have default values.

       Taking these two values into account, the method sorts the Natives and picks the one with the 
       highest score (Convertible::Exact and 1.0 for parameterMatch). Thus given these two C++ functions:

       void some_method(int a);
       void some_mtehod(int a, float b = 2.0).

       A call from ruby of some_method(1) will exactly match both signatures, but the first one 
       will be chosen because the parameterMatch will be 1.0 for the first overload but 0.5
       for the second. */

    Native* native = nullptr;

    ID methodId;
    VALUE klass;
    if (!rb_frame_method_id_and_class(&methodId, &klass))
    {
      rb_raise(rb_eRuntimeError, "Cannot get method id and class for function");
    }

    // Execute the function but make sure to catch any C++ exceptions!
    return cpp_protect([&]
    {
      Identifier id(methodId);
      std::string methodName = id.str();
      std::string className = rb_class2name(klass);

      const std::vector<std::unique_ptr<Native>>& natives = Registries::instance.natives.lookup(klass, methodId);

      if (natives.size() == 1)
      {
        native = natives.front().get();
      }
      else if (natives.size() == 0)
      {
        Identifier identifier(methodId);
        rb_raise(rb_eArgError, "Could not find method call for %s#%s", rb_class2name(klass), identifier.c_str());
      }
      else
      {
        // Loop over every native to see how well they match the Ruby parameters
        std::vector<Resolved> resolves;
        std::transform(natives.begin(), natives.end(), 
          std::back_inserter(resolves), 
          [&](const std::unique_ptr<Native>& native)
          {
            return native->matches(argc, argv, self);
          });

        // Now sort from best to worst
        std::sort(resolves.begin(), resolves.end(), std::greater{});

        // Get the best one
        Resolved resolved = resolves.front();

        // Did it match?
        if (resolved.convertible != Convertible::None)
        {
          native = resolved.native;
        }
        else
        {
          // Special case == to make the RubyMine debugger work, it liks calling == with a Module as
          // the other argument, thus breaking if C++ operator== is implemented.
          Identifier identifier(methodId);
          if (identifier.str() == "==")
          {
            return detail::protect(rb_call_super, argc, argv);
          }
          else
          {
            std::ostringstream message;
            message << "Could not resolve method call for %s#%s" << "\n"
                    << "  %d overload(s) were evaluated based on the types of Ruby parameters provided.";
            rb_raise(rb_eArgError, message.str().c_str(), rb_class2name(klass), identifier.c_str(), natives.size());
          }
        }
      }

      // Call the C++ function
      return (*native)(argc, argv, self);
    });
  }
}

// =========   NativeAttributeGet.hpp   =========

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
    template<typename Attribute_T>
    class NativeAttributeGet: Native
    {
    public:
      using NativeAttribute_T = NativeAttributeGet<Attribute_T>;

      using T = typename attribute_traits<Attribute_T>::attr_type;
      using Receiver_T = typename attribute_traits<Attribute_T>::class_type;
      using To_Ruby_T = remove_cv_recursive_t<T>;

    public:
      // Register attribute getter with Ruby
      static void define(VALUE klass, std::string name, Attribute_T attribute);

    public:
      // Disallow creating/copying/moving
      NativeAttributeGet() = delete;
      NativeAttributeGet(const NativeAttribute_T&) = delete;
      NativeAttributeGet(NativeAttribute_T&&) = delete;
      void operator=(const NativeAttribute_T&) = delete;
      void operator=(NativeAttribute_T&&) = delete;

      Resolved matches(int argc, const VALUE* argv, VALUE self) override;
      VALUE operator()(int argc, const VALUE* argv, VALUE self) override;

    protected:
      NativeAttributeGet(VALUE klass, std::string name, Attribute_T attr);

    private:
      VALUE klass_;
      std::string name_;
      Attribute_T attribute_;
    };
  } // detail
} // Rice


// =========   NativeAttributeGet.ipp   =========
#include <array>
#include <algorithm>


namespace Rice::detail
{
  template<typename Attribute_T>
  void NativeAttributeGet<Attribute_T>::define(VALUE klass, std::string name, Attribute_T attribute)
  {
    // Create a NativeAttributeGet that Ruby will call to read/write C++ variables
    NativeAttribute_T* nativeAttribute = new NativeAttribute_T(klass, name, std::forward<Attribute_T>(attribute));
    std::unique_ptr<Native> native(nativeAttribute);

    detail::protect(rb_define_method, klass, name.c_str(), (RUBY_METHOD_FUNC)&Native::resolve, -1);

    // Add to native registry. Since attributes cannot be overridden, there is no need to set the
    // matches or calls function pointer. Instead Ruby can call the static call method defined on
    // this class (&NativeAttribute_T::get).
    Identifier identifier(name);
    detail::Registries::instance.natives.add(klass, identifier.id(), native);
  }

  template<typename Attribute_T>
  inline Resolved NativeAttributeGet<Attribute_T>::matches(int argc, const VALUE* argv, VALUE self)
  {
    if (argc == 0)
      return Resolved { Convertible::Exact, 1, this };
    else
      return Resolved{ Convertible::None, 0, this };
  }
  
  template<typename Attribute_T>
  NativeAttributeGet<Attribute_T>::NativeAttributeGet(VALUE klass, std::string name, Attribute_T attribute)
    : klass_(klass), name_(name), attribute_(attribute)
  {
  }

  template<typename Attribute_T>
  inline VALUE NativeAttributeGet<Attribute_T>::operator()(int argc, const VALUE* argv, VALUE self)
  {
    if constexpr (std::is_member_object_pointer_v<Attribute_T>)
    {
      Receiver_T* nativeSelf = From_Ruby<Receiver_T*>().convert(self);
      return To_Ruby<To_Ruby_T>().convert(nativeSelf->*attribute_);
    }
    else
    {
      return To_Ruby<To_Ruby_T>().convert(*attribute_);
    }
  }
} // Rice

// =========   NativeAttributeSet.hpp   =========

namespace Rice
{
  namespace detail
  {
    template<typename Attribute_T>
    class NativeAttributeSet: Native
    {
    public:
      using NativeAttribute_T = NativeAttributeSet<Attribute_T>;
      using Attr_T = typename attribute_traits<Attribute_T>::attr_type;
      using T_Unqualified = remove_cv_recursive_t<Attr_T>;
      using Receiver_T = typename attribute_traits<Attribute_T>::class_type;

    public:
      // Register attribute getter/setter with Ruby
      static void define(VALUE klass, std::string name, Attribute_T attribute);

    public:
      // Disallow creating/copying/moving
      NativeAttributeSet() = delete;
      NativeAttributeSet(const NativeAttribute_T&) = delete;
      NativeAttributeSet(NativeAttribute_T&&) = delete;
      void operator=(const NativeAttribute_T&) = delete;
      void operator=(NativeAttribute_T&&) = delete;

      Resolved matches(int argc, const VALUE* argv, VALUE self) override;
      VALUE operator()(int argc, const VALUE* argv, VALUE self) override;

    protected:
      NativeAttributeSet(VALUE klass, std::string name, Attribute_T attr);

    private:
      VALUE klass_;
      std::string name_;
      Attribute_T attribute_;
    };
  } // detail
} // Rice


// =========   NativeAttributeSet.ipp   =========
#include <array>
#include <algorithm>


namespace Rice::detail
{
  template<typename Attribute_T>
  void NativeAttributeSet<Attribute_T>::define(VALUE klass, std::string name, Attribute_T attribute)
  {
    // Create a NativeAttributeSet that Ruby will call to read/write C++ variables
    NativeAttribute_T* nativeAttribute = new NativeAttribute_T(klass, name, std::forward<Attribute_T>(attribute));
    std::unique_ptr<Native> native(nativeAttribute);

    // Define the write method name
    std::string setter = name + "=";

    // Tell Ruby to invoke the static method write to get the attribute value
    detail::protect(rb_define_method, klass, setter.c_str(), (RUBY_METHOD_FUNC)&Native::resolve, -1);

    // Add to native registry. Since attributes cannot be overridden, there is no need to set the
    // matches or calls function pointer. Instead Ruby can call the static call method defined on
    // this class (&NativeAttribute_T::set).
    Identifier identifier(setter);
    detail::Registries::instance.natives.add(klass, identifier.id(), native);
  }

  template<typename Attribute_T>
  NativeAttributeSet<Attribute_T>::NativeAttributeSet(VALUE klass, std::string name, Attribute_T attribute)
    : klass_(klass), name_(name), attribute_(attribute)
  {
  }

  template<typename Attribute_T>
  inline Resolved NativeAttributeSet<Attribute_T>::matches(int argc, const VALUE* argv, VALUE self)
  {
    if (argc == 1)
      return Resolved{ Convertible::Exact, 1, this };
    else
      return Resolved{ Convertible::None, 0, this };
  }

  template<typename Attribute_T>
  inline VALUE NativeAttributeSet<Attribute_T>::operator()(int argc, const VALUE* argv, VALUE self)
  {
    if constexpr (std::is_fundamental_v<intrinsic_type<Attr_T>> && std::is_pointer_v<Attr_T>)
    {
      static_assert(true, "An fundamental value, such as an integer, cannot be assigned to an attribute that is a pointer.");
    }
    else if constexpr (std::is_same_v<intrinsic_type<Attr_T>, std::string> && std::is_pointer_v<Attr_T>)
    {
      static_assert(true, "An string cannot be assigned to an attribute that is a pointer.");
    }

    if (argc != 1)
    {
      throw std::runtime_error("Incorrect number of parameters for setting attribute. Attribute: " + this->name_);
    }

    VALUE value = argv[0];
    
    if constexpr (!std::is_null_pointer_v<Receiver_T> && 
                  !std::is_const_v<Attr_T> && 
                  (std::is_fundamental_v<Attr_T> || std::is_assignable_v<Attr_T, Attr_T>))
    {
      Receiver_T* nativeSelf = From_Ruby<Receiver_T*>().convert(self);
      nativeSelf->*attribute_ = From_Ruby<T_Unqualified>().convert(value);
    }
    else if constexpr (std::is_null_pointer_v<Receiver_T> && 
                       !std::is_const_v<Attr_T> &&
                       (std::is_fundamental_v<Attr_T> || std::is_assignable_v<Attr_T, Attr_T>))
    {
      *attribute_ = From_Ruby<T_Unqualified>().convert(value);
    }
    else
    {
      // Should never get here because define_attr won't compile this code, but just in case!
      throw std::invalid_argument("Could not set attribute. Attribute: " + this->name_);
    }

    return value;
  }
} // Rice

// =========   NativeFunction.hpp   =========

namespace Rice::detail
{
  //! The NativeFunction class calls C++ functions/methods/lambdas on behalf of Ruby
  /*! The NativeFunction class is an intermediate between Ruby and C++. Every method
   *  defined in Rice is associated with a NativeFuntion instance that is stored in
   *  a unordered_map maintained by the MethodData class. The key is the Ruby class
   *  and method.
   * 
   *  When Ruby calls into C++ it invokes the static NativeFunction.call method. This
   *  method then looks up the NativeFunction instance and calls its ->() operator.
   *
   *  The instance then converts each of the arguments passed from Ruby into their
   *  C++ equivalents. It then retrieves the C++ object (if there is one, Ruby could
   *  be calling a free standing method or lambda). Then it calls the C++ method
   *  and gets back the result. If there is a result (so not void), it is converted
   *  from a C++ object to a Ruby object and returned back to Ruby.
   * 
   *  This class make heavy use of C++ Template metaprogramming to determine
   *  the types and parameters a method takes. It then uses that information
   *  to perform type conversion Ruby to C++.
   *   
   *  @tparam Receiver_T - The type of C++ class wrapped by Ruby. Althought NativeFunction
   *    can derive the C++ class (Receiver_T), it can differ per member function. For example,
   *    std::map has a size() method but that is actually implemented on an ancestor class _Tree.
   *    Thus Receiver_T is std::map but Function_T::Receiver_T is _Tree. This breaks Rice in two ways. 
   *    First, _Tree is not a registered type. Second, Rice would return a _Tree instance back to
   *    C++ and not a std::map.
   *  @tparam Function_T - A template that represents the C++ function
   *    to call. This typename is automatically deduced by the compiler.
   *  @tparam IsMethod - A boolean specifying whether the function has
   *    a self parameter or not. Rice differentiates these two cases by
   *    calling them methods (self) or functions (no self).
   */

  template<typename Class_T, typename Function_T, bool IsMethod>
  class NativeFunction: Native
  {
  public:
    using NativeFunction_T = NativeFunction<Class_T, Function_T, IsMethod>;

    // We remove const to avoid an explosion of To_Ruby specializations and Ruby doesn't
    // have the concept of constants anyways
    using Return_T = typename function_traits<Function_T>::return_type;
    using Receiver_T = typename method_traits<Function_T, IsMethod>::Class_T;
    using Arg_Ts = typename method_traits<Function_T, IsMethod>::Arg_Ts;
    static constexpr std::size_t arity = method_traits<Function_T, IsMethod>::arity;
    using From_Ruby_Args_Ts = typename tuple_map<From_Ruby, Arg_Ts>::type;
    using To_Ruby_T = remove_cv_recursive_t<Return_T>;

    // Register function with Ruby
    static void define(VALUE klass, std::string method_name, Function_T function, MethodInfo* methodInfo);

    // Proc entry
    static VALUE procEntry(VALUE yielded_arg, VALUE callback_arg, int argc, const VALUE* argv, VALUE blockarg);
  public:
    // Disallow creating/copying/moving
    NativeFunction() = delete;
    NativeFunction(const NativeFunction_T&) = delete;
    NativeFunction(NativeFunction_T&&) = delete;
    void operator=(const NativeFunction_T&) = delete;
    void operator=(NativeFunction_T&&) = delete;

    Resolved matches(int argc, const VALUE* argv, VALUE self) override;
    VALUE operator()(int argc, const VALUE* argv, VALUE self) override;

    NativeFunction(Function_T function);
    NativeFunction(VALUE klass, std::string method_name, Function_T function, MethodInfo* methodInfo);
  protected:

  private:
    template<typename T, std::size_t I>
    From_Ruby<T> createFromRuby();
      
    // Create NativeArgs which are used to convert values from Ruby to C++
    template<std::size_t...I>
    From_Ruby_Args_Ts createFromRuby(std::index_sequence<I...>& indices);

    // Convert C++ value to Ruby
    To_Ruby<To_Ruby_T> createToRuby();
      
    // Convert Ruby argv pointer to Ruby values
    std::vector<VALUE> getRubyValues(int argc, const VALUE* argv);

    template<typename Arg_T, int I>
    Arg_T getNativeValue(std::vector<VALUE>& values);

    // Convert Ruby values to C++ values
    template<typename std::size_t...I>
    Arg_Ts getNativeValues(std::vector<VALUE>& values, std::index_sequence<I...>& indices);

    // Figure out what self is
    Receiver_T getReceiver(VALUE self);

    // Throw an exception when wrapper cannot be extracted
    [[noreturn]] void noWrapper(const VALUE klass, const std::string& wrapper);

    // Do we need to keep alive any arguments?
    void checkKeepAlive(VALUE self, VALUE returnValue, std::vector<VALUE>& rubyValues);

    // Call the underlying C++ function
    VALUE invokeNativeFunction(Arg_Ts&& nativeArgs);
    VALUE invokeNativeMethod(VALUE self, Arg_Ts&& nativeArgs);

  private:
    VALUE klass_;
    std::string method_name_;
    Function_T function_;
    From_Ruby_Args_Ts fromRubys_;
    To_Ruby<To_Ruby_T> toRuby_;
    std::unique_ptr<MethodInfo> methodInfo_;
  };
}


// =========   NativeFunction.ipp   =========
#include <array>
#include <algorithm>
#include <stdexcept>
#include <sstream>


namespace Rice::detail
{
  template<typename Class_T, typename Function_T, bool IsMethod>
  void NativeFunction<Class_T, Function_T, IsMethod>::define(VALUE klass, std::string method_name, Function_T function, MethodInfo* methodInfo)
  {
    // Have we defined this method yet in Ruby?
    Identifier identifier(method_name);
    const std::vector<std::unique_ptr<Native>>& natives = Registries::instance.natives.lookup(klass, identifier.id());
    if (natives.empty())
    {
      // Tell Ruby to invoke the static resolved method defined above
      detail::protect(rb_define_method, klass, method_name.c_str(), (RUBY_METHOD_FUNC)&Native::resolve, -1);
    }

    // Create a NativeFunction instance and save it to the NativeRegistry. There may be multiple
    // NativeFunction instances for a specific method because C++ supports method overloading.
    NativeFunction_T* nativeFunction = new NativeFunction_T(klass, method_name, std::forward<Function_T>(function), methodInfo);
    std::unique_ptr<Native> native(nativeFunction);
    detail::Registries::instance.natives.add(klass, identifier.id(), native);
  }

  // Ruby calls this method when invoking a proc that was defined as a C++ function
  template<typename Class_T, typename Function_T, bool IsMethod>
  VALUE NativeFunction<Class_T, Function_T, IsMethod>::procEntry(VALUE yielded_arg, VALUE callback_arg, int argc, const VALUE* argv, VALUE blockarg)
  {
    NativeFunction_T* native = (NativeFunction_T*)callback_arg;
    return (*native)(argc, argv, Qnil);
  }

  template<typename Class_T, typename Function_T, bool IsMethod>
  NativeFunction<Class_T, Function_T, IsMethod>::NativeFunction(VALUE klass, std::string method_name, Function_T function, MethodInfo* methodInfo)
    : klass_(klass), method_name_(method_name), function_(function), methodInfo_(methodInfo)
  {
    // Create a tuple of NativeArgs that will convert the Ruby values to native values. For 
    // builtin types NativeArgs will keep a copy of the native value so that it 
    // can be passed by reference or pointer to the native function. For non-builtin types
    // it will just pass the value through.
    auto indices = std::make_index_sequence<std::tuple_size_v<Arg_Ts>>{};
    this->fromRubys_ = this->createFromRuby(indices);

    this->toRuby_ = this->createToRuby();
  }

  template<typename Class_T, typename Function_T, bool IsMethod>
  NativeFunction<Class_T, Function_T, IsMethod>::NativeFunction(Function_T function)
    : NativeFunction(Qnil, "", function, new MethodInfo(arity))
  {
  }

  template<typename Class_T, typename Function_T, bool IsMethod>
  To_Ruby<typename NativeFunction<Class_T, Function_T, IsMethod>::To_Ruby_T> NativeFunction<Class_T, Function_T, IsMethod>::createToRuby()
  {
    // Does the From_Ruby instantiation work with ReturnInfo?
    if constexpr (std::is_constructible_v<To_Ruby<To_Ruby_T>, Return*>)
    {
      return To_Ruby<To_Ruby_T>(&this->methodInfo_->returnInfo);
    }
    else
    {
      return To_Ruby<To_Ruby_T>();
    }
  }

  template<typename Class_T, typename Function_T, bool IsMethod>
  template<typename T, std::size_t I>
  From_Ruby<T> NativeFunction<Class_T, Function_T, IsMethod>::createFromRuby()
  {
    // Does the From_Ruby instantiation work with Arg?
    if constexpr (std::is_constructible_v<From_Ruby<T>, Arg*>)
    {
      return From_Ruby<T>(&this->methodInfo_->arg(I));
    }
    else
    {
      return From_Ruby<T>();
    }
  }

  template<typename Class_T, typename Function_T, bool IsMethod>
  template<std::size_t... I>
  typename NativeFunction<Class_T, Function_T, IsMethod>::From_Ruby_Args_Ts NativeFunction<Class_T, Function_T, IsMethod>::createFromRuby(std::index_sequence<I...>& indices)
  {
    return std::make_tuple(createFromRuby<remove_cv_recursive_t<typename std::tuple_element<I, Arg_Ts>::type>, I>()...);
  }

  template<typename Class_T, typename Function_T, bool IsMethod>
  Resolved NativeFunction<Class_T, Function_T, IsMethod>::matches(int argc, const VALUE* argv, VALUE self)
  {
    // Return false if Ruby provided more arguments than the C++ method takes
    if (argc > arity)
      return Resolved{ Convertible::None, 0, this };

    Resolved result { Convertible::Exact, 1, this };

    MethodInfo* methodInfo = this->methodInfo_.get();
    int index = 0;

    // Loop over each FromRuby instance
    for_each_tuple(this->fromRubys_,
      [&](auto& fromRuby)
      {
        Convertible convertible = Convertible::None;

        Arg& arg = methodInfo->arg(index);

        // Is a VALUE being passed directly to C++ ?
        if (arg.isValue() && index < argc)
        {
          convertible = Convertible::Exact;
        }
        // If index is less than argc then check with FromRuby if the VALUE is convertible
        // to C++.
        else if (index < argc)
        {
          VALUE value = argv[index];
          convertible = fromRuby.is_convertible(value);
        }
        // Last check if a default value has been set
        else if (arg.hasDefaultValue())
        {
          convertible = Convertible::Exact;
        }

        result.convertible = result.convertible & convertible;

        index++;
      });

    if constexpr (arity > 0)
      result.parameterMatch = (double)argc / arity;

    return result;
  }

  template<typename Class_T, typename Function_T, bool IsMethod>
  std::vector<VALUE> NativeFunction<Class_T, Function_T, IsMethod>::getRubyValues(int argc, const VALUE* argv)
  {
    // Block handling. If we find a block and there is a missing argument error, then we assume the
    // block is being used as a callback for C/C++. The easiest thing to do is keep the format string the 
    // same but convert the block to a proc and add it to argv.
    std::vector<VALUE> argvCopy(argv, argv + argc);
    if (rb_block_given_p() && argc < arity)
    {
      argc++;
      VALUE proc = rb_block_proc();
      argvCopy.push_back(proc);
    }

    // Setup a tuple for the leading rb_scan_args arguments
    std::string scanFormat = this->methodInfo_->formatString();
    std::tuple<int, VALUE*, const char*> rbScanArgs = std::forward_as_tuple(argc, argvCopy.data(), scanFormat.c_str());

    // Create a vector to store the VALUEs that will be returned by rb_scan_args
    std::vector<VALUE> rbScanValues(std::tuple_size_v<Arg_Ts>, Qnil);

    // Convert the vector to an array so it can be concatenated to a tuple. As importantly
    // fill it with pointers to rbScanValues
    std::array<VALUE*, std::tuple_size_v<Arg_Ts>> rbScanValuePointers;
    std::transform(rbScanValues.begin(), rbScanValues.end(), rbScanValuePointers.begin(),
      [](VALUE& value)
      {
        return &value;
      });

    // Combine the tuples and call rb_scan_args
    std::apply(rb_scan_args, std::tuple_cat(rbScanArgs, rbScanValuePointers));

    return rbScanValues;
  }

  template<typename Class_T, typename Function_T, bool IsMethod>
  template<typename Arg_T, int I>
  Arg_T NativeFunction<Class_T, Function_T, IsMethod>::getNativeValue(std::vector<VALUE>& values)
  {
    /* In general the compiler will convert T to const T, but that does not work for converting
       T** to const T** (see see https://isocpp.org/wiki/faq/const-correctness#constptrptr-conversion)
       which comes up in the OpenCV bindings.
     
       An alternative solution is updating From_Ruby#convert to become a templated function that specifies
       the return type. That works but requires a lot more code changes for this one case and is not 
       backwards compatible. */
    if constexpr (is_pointer_pointer_v<Arg_T> && !std::is_convertible_v<remove_cv_recursive_t<Arg_T>, Arg_T>)
    {
      return (Arg_T)std::get<I>(this->fromRubys_).convert(values[I]);
    }
    else
    {
      return std::get<I>(this->fromRubys_).convert(values[I]);
    }
  }

  template<typename Class_T, typename Function_T, bool IsMethod>
  template<std::size_t... I>
  typename NativeFunction<Class_T, Function_T, IsMethod>::Arg_Ts NativeFunction<Class_T, Function_T, IsMethod>::getNativeValues(std::vector<VALUE>& values,
     std::index_sequence<I...>& indices)
  {
    /* Loop over each value returned from Ruby and convert it to the appropriate C++ type based
       on the arguments (Arg_Ts) required by the C++ function. Arg_T may have const/volatile while
       the associated From_Ruby<T> template parameter will not. Thus From_Ruby produces non-const values 
       which we let the compiler convert to const values as needed. This works except for 
       T** -> const T**, see comment in getNativeValue method. */
    return std::forward_as_tuple(this->getNativeValue<std::tuple_element_t<I, Arg_Ts>, I>(values)...);
  }

  template<typename Class_T, typename Function_T, bool IsMethod>
  typename NativeFunction<Class_T, Function_T, IsMethod>::Receiver_T NativeFunction<Class_T, Function_T, IsMethod>::getReceiver(VALUE self)
  {
    // There is no self parameter
    if constexpr (std::is_same_v<Receiver_T, std::nullptr_t>)
    {
      return nullptr;
    }
    // Self parameter is a Ruby VALUE so no conversion is needed
    else if constexpr (std::is_same_v<Receiver_T, VALUE>)
    {
      return self;
    }
    /* This case happens when a class wrapped by Rice is calling a method
       defined on an ancestor class. For example, the std::map size method
       is defined on _Tree not map. Rice needs to know the actual type
       that was wrapped so it can correctly extract the C++ object from 
       the Ruby object. */
    else if constexpr (!std::is_same_v<intrinsic_type<Receiver_T>, Class_T> && 
                        std::is_base_of_v<intrinsic_type<Receiver_T>, Class_T> &&
                        std::is_pointer_v<Receiver_T>)
    {
      Class_T* instance = From_Ruby<Class_T*>().convert(self);
      return dynamic_cast<Receiver_T>(instance);
    }
    else if constexpr (!std::is_same_v<intrinsic_type<Receiver_T>, Class_T> &&
                        std::is_base_of_v<intrinsic_type<Receiver_T>, Class_T> &&
                        std::is_reference_v<Receiver_T>)
    {
      Class_T& instance = From_Ruby<Class_T&>().convert(self);
      return dynamic_cast<Receiver_T>(instance);
    }
    // Self parameter could be derived from Object or it is an C++ instance and
    // needs to be unwrapped from Ruby
    else
    {
      return From_Ruby<Receiver_T>().convert(self);
    }
  }

  template<typename Class_T, typename Function_T, bool IsMethod>
  VALUE NativeFunction<Class_T, Function_T, IsMethod>::invokeNativeFunction(Arg_Ts&& nativeArgs)
  {
    if constexpr (std::is_void_v<Return_T>)
    {
      std::apply(this->function_, std::forward<Arg_Ts>(nativeArgs));
      return Qnil;
    }
    else
    {
      // Call the native method and get the result
      Return_T nativeResult = std::apply(this->function_, std::forward<Arg_Ts>(nativeArgs));

      // Return the result
      return this->toRuby_.convert(nativeResult);
    }
  }

  template<typename Class_T, typename Function_T, bool IsMethod>
  VALUE NativeFunction<Class_T, Function_T, IsMethod>::invokeNativeMethod(VALUE self, Arg_Ts&& nativeArgs)
  {
    Receiver_T receiver = this->getReceiver(self);
    auto selfAndNativeArgs = std::tuple_cat(std::forward_as_tuple(receiver), std::forward<Arg_Ts>(nativeArgs));

    if constexpr (std::is_void_v<Return_T>)
    {
      std::apply(this->function_, std::forward<decltype(selfAndNativeArgs)>(selfAndNativeArgs));
      return Qnil;
    }
    else
    {
      Return_T nativeResult = std::apply(this->function_, std::forward<decltype(selfAndNativeArgs)>(selfAndNativeArgs));

      // Special handling if the method returns self. If so we do not want
      // to create a new Ruby wrapper object and instead return self.
      if constexpr (std::is_same_v<intrinsic_type<Return_T>, intrinsic_type<Receiver_T>>)
      {
        if constexpr (std::is_pointer_v<Return_T> && std::is_pointer_v<Receiver_T>)
        {
          if (nativeResult == receiver)
            return self;
        }
        else if constexpr (std::is_pointer_v<Return_T> && std::is_reference_v<Receiver_T>)
        {
          if (nativeResult == &receiver)
            return self;
        }
        else if constexpr (std::is_reference_v<Return_T> && std::is_pointer_v<Receiver_T>)
        {
          if (&nativeResult == receiver)
            return self;
        }
        else if constexpr (std::is_reference_v<Return_T> && std::is_reference_v<Receiver_T>)
        {
          if (&nativeResult == &receiver)
            return self;
        }
      }

      return this->toRuby_.convert(nativeResult);
    }
  }

  template<typename Class_T, typename Function_T, bool IsMethod>
  void NativeFunction<Class_T, Function_T, IsMethod>::noWrapper(const VALUE klass, const std::string& wrapper)
  {
    std::stringstream message;

    message << "When calling the method `";
    message << this->method_name_;
    message << "' we could not find the wrapper for the '";
    message << rb_obj_classname(klass);
    message << "' ";
    message << wrapper;
    message << " type. You should not use keepAlive() on a Return or Arg that is a builtin Rice type.";

    throw std::runtime_error(message.str());
  }

  template<typename Class_T, typename Function_T, bool IsMethod>
  void NativeFunction<Class_T, Function_T, IsMethod>::checkKeepAlive(VALUE self, VALUE returnValue, std::vector<VALUE>& rubyValues)
  {
    // Self will be Qnil for wrapped procs
    if (self == Qnil)
      return;

    // selfWrapper will be nullptr if this(self) is a builtin type and not an external(wrapped) type
    // it is highly unlikely that keepAlive is used in this case but we check anyway
    Wrapper* selfWrapper = getWrapper(self);

    // Check function arguments
    for (const Arg& arg : (*this->methodInfo_))
    {
      if (arg.isKeepAlive())
      {
        if (selfWrapper == nullptr)
        {
          noWrapper(self, "self");
        }
        selfWrapper->addKeepAlive(rubyValues[arg.position]);
      }
    }

    // Check return value
    if (this->methodInfo_->returnInfo.isKeepAlive())
    {
      if (selfWrapper == nullptr)
      {
        noWrapper(self, "self");
      }

      // returnWrapper will be nullptr if returnValue is a built-in type and not an external(wrapped) type
      Wrapper* returnWrapper = getWrapper(returnValue);
      if (returnWrapper == nullptr)
      {
        noWrapper(returnValue, "return");
      }
      returnWrapper->addKeepAlive(self);
    }
  }

  template<typename Class_T, typename Function_T, bool IsMethod>
  VALUE NativeFunction<Class_T, Function_T, IsMethod>::operator()(int argc, const VALUE* argv, VALUE self)
  {
    // Get the ruby values
    std::vector<VALUE> rubyValues = this->getRubyValues(argc, argv);

    auto indices = std::make_index_sequence<std::tuple_size_v<Arg_Ts>>{};

    // Convert the Ruby values to native values
    Arg_Ts nativeValues = this->getNativeValues(rubyValues, indices);

    // Now call the native method
    VALUE result = Qnil;
    if constexpr (std::is_same_v<Receiver_T, std::nullptr_t>)
    {
      result = this->invokeNativeFunction(std::forward<Arg_Ts>(nativeValues));
    }
    else
    {
      result = this->invokeNativeMethod(self, std::forward<Arg_Ts>(nativeValues));
    }

    // Check if any function arguments or return values need to have their lifetimes tied to the receiver
    this->checkKeepAlive(self, result, rubyValues);

    return result;
  }
}

// =========   NativeIterator.hpp   =========

namespace Rice::detail
{
  template<typename T, typename Iterator_Func_T>
  class NativeIterator: Native
  {
  public:
    using NativeIterator_T = NativeIterator<T, Iterator_Func_T>;
    using Iterator_T = typename function_traits<Iterator_Func_T>::return_type;
    using Value_T = typename std::iterator_traits<Iterator_T>::value_type;
    using Reference_T = typename std::iterator_traits<Iterator_T>::reference;
    using Difference_T = typename std::iterator_traits<Iterator_T>::difference_type;
    using To_Ruby_T = remove_cv_recursive_t<Reference_T>;

  public:
    // Register function with Ruby
    void static define(VALUE klass, std::string method_name, Iterator_Func_T begin, Iterator_Func_T end);

  public:
    // Disallow creating/copying/moving
    NativeIterator() = delete;
    NativeIterator(const NativeIterator_T&) = delete;
    NativeIterator(NativeIterator_T&&) = delete;
    void operator=(const NativeIterator_T&) = delete;
    void operator=(NativeIterator_T&&) = delete;

    Resolved matches(int argc, const VALUE* argv, VALUE self) override;
    VALUE operator()(int argc, const VALUE* argv, VALUE self) override;

  protected:
    NativeIterator(VALUE klass, std::string method_name, Iterator_Func_T begin, Iterator_Func_T end);

  private:
    VALUE createRubyEnumerator(VALUE self);

  private:
    VALUE klass_;
    std::string method_name_;
    Iterator_Func_T begin_;
    Iterator_Func_T end_;
  };
}


// =========   NativeIterator.ipp   =========
#include <iterator>
#include <functional>
#include <type_traits>

namespace Rice::detail
{
  template <typename T, typename Iterator_Func_T>
  inline void NativeIterator<T, Iterator_Func_T>::define(VALUE klass, std::string method_name, Iterator_Func_T begin, Iterator_Func_T end)
  {
    // Tell Ruby to invoke the resolveIterator static method defined above
    detail::protect(rb_define_method, klass, method_name.c_str(), (RUBY_METHOD_FUNC)&Native::resolve, -1);

    // Create a NativeIterator instance and save it to the NativeRegistry. There may be multiple
    // NativeFunction instances for a specific method because C++ supports method overloading.
    NativeIterator_T* nativeIterator = new NativeIterator_T(klass, method_name, begin, end);
    std::unique_ptr<Native> native(nativeIterator);

    Identifier identifier(method_name);
    detail::Registries::instance.natives.add(klass, identifier.id(), native);
  }

  template <typename T, typename Iterator_Func_T>
  inline NativeIterator<T, Iterator_Func_T>::NativeIterator(VALUE klass, std::string method_name, Iterator_Func_T begin, Iterator_Func_T end) :
    klass_(klass), method_name_(method_name), begin_(begin), end_(end)
  {
  }

  template<typename T, typename Iterator_Func_T>
  inline Resolved NativeIterator<T, Iterator_Func_T>::matches(int argc, const VALUE* argv, VALUE self)
  {
    return Resolved{ Convertible::Exact, 1.0, this };
  }

  template<typename T, typename Iterator_Func_T>
  inline VALUE NativeIterator<T, Iterator_Func_T>::createRubyEnumerator(VALUE self)
  {
    auto rb_size_function = [](VALUE recv, VALUE argv, VALUE eobj) -> VALUE
    {
      // Since we can't capture VALUE self from above (because then we can't send
      // this lambda to rb_enumeratorize_with_size), extract it from recv
      return cpp_protect([&]
      {
        // Get the iterator instance
        // Class is easy
        VALUE klass = protect(rb_class_of, recv);
        // Read the method_id from an attribute we added to the enumerator instance
        Identifier identifier = protect(rb_ivar_get, eobj, rb_intern("rice_method"));

        const std::vector<std::unique_ptr<Native>>& natives = detail::Registries::instance.natives.lookup(klass, identifier.id());
        NativeIterator_T* iterator = static_cast<NativeIterator_T*>(natives.back().get());

        // Get the wrapped C++ instance
        T* receiver = detail::From_Ruby<T*>().convert(recv);

        // Get the distance
        Iterator_T begin = std::invoke(iterator->begin_, *receiver);
        Iterator_T end = std::invoke(iterator->end_, *receiver);
        Difference_T distance = std::distance(begin, end);

        return detail::To_Ruby<Difference_T>().convert(distance);
      });
    };

    Identifier identifier(this->method_name_);
    VALUE enumerator = protect(rb_enumeratorize_with_size, self, identifier.to_sym(), 0, nullptr, rb_size_function);
    
    // Hack the enumerator object by storing name_ on the enumerator object so
    // the rb_size_function above has access to it
    protect(rb_ivar_set, enumerator, rb_intern("rice_method"), identifier.id());

    return enumerator;
  }

  template<typename T, typename Iterator_Func_T>
  inline VALUE NativeIterator<T, Iterator_Func_T>::operator()(int argc, const VALUE* argv, VALUE self)
  {
    if (!protect(rb_block_given_p))
    {
      return createRubyEnumerator(self);
    }
    else
    {
      T* receiver = detail::From_Ruby<T*>().convert(self);
      Iterator_T it = std::invoke(this->begin_, *receiver);
      Iterator_T end = std::invoke(this->end_, *receiver);

      for (; it != end; ++it)
      {
        protect(rb_yield, detail::To_Ruby<To_Ruby_T>().convert(*it));
      }

      return self;
    }
  }
}
// =========   NativeCallbackFFI.hpp   =========

#ifdef HAVE_LIBFFI

#include <ffi.h>

namespace Rice::detail
{
  template<typename Return_T, typename ...Arg_Ts>
  class NativeCallbackFFI
  {
  public:
    using Callback_T = Return_T(Arg_Ts...);
    using Tuple_T = std::tuple<Arg_Ts...>;
    static void ffiCallback(ffi_cif* cif, void* ret, void* args[], void* instance);
  public:
    NativeCallbackFFI(VALUE proc);
    NativeCallbackFFI(const NativeCallbackFFI&) = delete;
    NativeCallbackFFI(NativeCallbackFFI&&) = delete;
    void operator=(const NativeCallbackFFI&) = delete;
    void operator=(NativeCallbackFFI&&) = delete;

    Return_T operator()(Arg_Ts...args);
    Callback_T* callback();

  private:
    template <typename Arg_T>
    static ffi_type* ffiType();

    template<std::size_t... I>
    static Tuple_T convertArgsToTuple(void* args[], std::index_sequence<I...>& indices);

    static inline std::array<ffi_type*, sizeof...(Arg_Ts)> args_ = { ffiType<Arg_Ts>()... };
    static inline ffi_cif cif_;
    static inline ffi_closure* closure_ = nullptr;
    static inline Callback_T* callback_ = nullptr;
 
  private:
    VALUE proc_;
  };
}
#endif // HAVE_LIBFFI


// =========   NativeCallbackFFI.ipp   =========
#ifdef HAVE_LIBFFI
#include <ffi.h>

namespace Rice::detail
{
  template<typename Return_T, typename ...Arg_Ts>
  template<typename Arg_T>
  ffi_type* NativeCallbackFFI<Return_T, Arg_Ts...>::ffiType()
  {
    std::map<std::type_index, ffi_type*> nativeToFfiMapping = { 
      {std::type_index(typeid(bool)),               &ffi_type_uint8},
      {std::type_index(typeid(bool)),               &ffi_type_uint8},
      {std::type_index(typeid(char)),               &ffi_type_schar},
      {std::type_index(typeid(unsigned char)),      &ffi_type_uchar},
      {std::type_index(typeid(signed char)),        &ffi_type_schar},
      {std::type_index(typeid(uint8_t)),            &ffi_type_uint8},
      {std::type_index(typeid(unsigned short)),     &ffi_type_uint8},
      {std::type_index(typeid(int8_t)),             &ffi_type_sint8},
      {std::type_index(typeid(short)),              &ffi_type_sint8},
      {std::type_index(typeid(uint16_t)),           &ffi_type_uint16},
      {std::type_index(typeid(int16_t)),            &ffi_type_sint16},
      {std::type_index(typeid(uint32_t)),           &ffi_type_uint32},
      {std::type_index(typeid(unsigned int)),       &ffi_type_uint32},
      {std::type_index(typeid(signed int)),         &ffi_type_uint32},
      {std::type_index(typeid(int32_t)),            &ffi_type_sint32},
      {std::type_index(typeid(uint64_t)),           &ffi_type_uint64},
      {std::type_index(typeid(unsigned long long)), &ffi_type_uint64},
      {std::type_index(typeid(int64_t)),            &ffi_type_sint64},
      {std::type_index(typeid(signed long long)),   &ffi_type_sint64},
      {std::type_index(typeid(float)),              &ffi_type_float},
      {std::type_index(typeid(double)),             &ffi_type_double},
      {std::type_index(typeid(void)),               &ffi_type_pointer},
      {std::type_index(typeid(long double)),        &ffi_type_longdouble}
    };

    if (sizeof(long) == 32)
    {
      nativeToFfiMapping[std::type_index(typeid(unsigned long))] = &ffi_type_uint32;
      nativeToFfiMapping[std::type_index(typeid(long))] = &ffi_type_sint32;
    }
    else if (sizeof(long) == 64)
    {
      nativeToFfiMapping[std::type_index(typeid(unsigned long))] = &ffi_type_uint64;
      nativeToFfiMapping[std::type_index(typeid(long))] = &ffi_type_sint64;
    }
    
    if (std::is_pointer_v<Arg_T>)
    {
      return &ffi_type_pointer;
    }
    else
    {
      const std::type_index& key = std::type_index(typeid(Arg_T));
      return nativeToFfiMapping[key];
    }
  }

  template<typename Return_T, typename ...Arg_Ts>
  template<std::size_t... I>
  typename NativeCallbackFFI<Return_T, Arg_Ts...>::Tuple_T NativeCallbackFFI<Return_T, Arg_Ts...>::convertArgsToTuple(void* args[], std::index_sequence<I...>& indices)
  {
    /* Loop over each value returned from Ruby and convert it to the appropriate C++ type based
       on the arguments (Arg_Ts) required by the C++ function. Arg_T may have const/volatile while
       the associated From_Ruby<T> template parameter will not. Thus From_Ruby produces non-const values
       which we let the compiler convert to const values as needed. This works except for
       T** -> const T**, see comment in getNativeValue method. */
    return std::forward_as_tuple(*(std::tuple_element_t<I, Tuple_T>*)(args[I])...);
  }

  template<typename Return_T, typename ...Arg_Ts>
  void NativeCallbackFFI<Return_T, Arg_Ts...>::ffiCallback(ffi_cif* cif, void* ret, void* args[], void* instance)
  {
    using Self_T = NativeCallbackFFI<Return_T, Arg_Ts...>;
    Self_T* self = (Self_T*)instance;

    auto indices = std::make_index_sequence<sizeof...(Arg_Ts)>{};

    if constexpr (sizeof...(Arg_Ts) == 0)
    {
      *(Return_T*)ret = self->operator()();
    }
    else
    {
      std::tuple<Arg_Ts...> tuple = convertArgsToTuple(args, indices);
      *(Return_T*)ret = std::apply(*self, tuple);
    }
  }

  template<typename Return_T, typename ...Arg_Ts>
  NativeCallbackFFI<Return_T, Arg_Ts...>::NativeCallbackFFI(VALUE proc) : proc_(proc)
  {
    if (cif_.bytes == 0)
    {
      ffi_prep_cif(&cif_, FFI_DEFAULT_ABI, sizeof...(Arg_Ts), &ffi_type_pointer, args_.data());
    }

    this->closure_ = (ffi_closure *)ffi_closure_alloc(sizeof(ffi_closure), (void**)(&this->callback_));
    ffi_status status = ffi_prep_closure_loc(this->closure_, &cif_, ffiCallback, (void*)this, nullptr);
  }

  template<typename Return_T, typename ...Arg_Ts>
  typename NativeCallbackFFI<Return_T, Arg_Ts...>::Callback_T* NativeCallbackFFI<Return_T, Arg_Ts...>::callback()
  {
    return (Callback_T*)this->callback_;
  }

  template<typename Return_T, typename ...Arg_Ts>
  Return_T NativeCallbackFFI<Return_T, Arg_Ts...>::operator()(Arg_Ts...args)
  {
    static Identifier id("call");
    std::array<VALUE, sizeof...(Arg_Ts)> values = { detail::To_Ruby<detail::remove_cv_recursive_t<Arg_Ts>>().convert(args)... };
    VALUE result = detail::protect(rb_funcallv, this->proc_, id.id(), (int)values.size(), (const VALUE*)values.data());
    
    if constexpr (!std::is_void_v<Return_T>)
    {
      static From_Ruby<Return_T> fromRuby;
      return fromRuby.convert(result);
    }
  }
}
#endif // HAVE_LIBFFI
// =========   NativeCallbackSimple.hpp   =========

namespace Rice::detail
{
  template<typename Return_T, typename ...Arg_Ts>
  class NativeCallbackSimple
  {
  public:
    static Return_T callback(Arg_Ts...args);
    static inline VALUE proc = Qnil;

  public:
    NativeCallbackSimple()  = delete;
    NativeCallbackSimple(const NativeCallbackSimple&) = delete;
    NativeCallbackSimple(NativeCallbackSimple&&) = delete;
    void operator=(const NativeCallbackSimple&) = delete;
    void operator=(NativeCallbackSimple&&) = delete;
  };
}

// =========   NativeCallbackSimple.ipp   =========
namespace Rice::detail
{
  template<typename Return_T, typename ...Arg_Ts>
  Return_T NativeCallbackSimple<Return_T, Arg_Ts...>::callback(Arg_Ts...args)
  {
    static Identifier id("call");
    std::array<VALUE, sizeof...(Arg_Ts)> values = { detail::To_Ruby<detail::remove_cv_recursive_t<Arg_Ts>>().convert(args)... };
    VALUE result = detail::protect(rb_funcallv, proc, id.id(), (int)sizeof...(Arg_Ts), values.data());
    if constexpr (!std::is_void_v<Return_T>)
    {
      static From_Ruby<Return_T> fromRuby;
      return fromRuby.convert(result);
    }
  }
}
// =========   Proc.ipp   =========
namespace Rice::detail
{
  // Note Return_T(Arg_Ts...) is intentional versus Return_T(*)(Arg_Ts...). That is
  // because the Type machinery strips all pointers/references/const/val etc to avoid
  // having an explosion of Type definitions
  template<typename Return_T, typename ...Arg_Ts>
  struct Type<Return_T(Arg_Ts...)>
  {
    static bool verify()
    {
      return true;
    }
  };

  // Wraps a C++ function as a Ruby proc
  template<typename Return_T, typename ...Arg_Ts>
  class To_Ruby<Return_T(*)(Arg_Ts...)>
  {
  public:
    using Proc_T = Return_T(*)(Arg_Ts...);

    VALUE convert(Proc_T proc)
    {
      using NativeFunction_T = NativeFunction<void, Proc_T, false>;
      // TODO - this is a memory leak - we never free this pointer
      auto native = new NativeFunction_T(proc);
      VALUE result = rb_proc_new(NativeFunction_T::procEntry, (VALUE)native);
      return result;
    }
  };

  // Makes a Ruby proc callable as C callback
  template<typename Return_T, typename ...Arg_Ts>
  class From_Ruby<Return_T(*)(Arg_Ts...)>
  {
  public:
    using Callback_T = Return_T(*)(Arg_Ts...);

    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      if (protect(rb_obj_is_proc, value) == Qtrue || protect(rb_proc_lambda_p, value))
      {
        return Convertible::Exact;
      }
      else
      {
        return Convertible::None;
      }
    }

#ifdef HAVE_LIBFFI
    Callback_T convert(VALUE value)
    {
      using NativeCallback_T = NativeCallbackFFI<Return_T, Arg_Ts...>;
      NativeCallback_T* nativeCallback = new NativeCallback_T(value);
      return nativeCallback->callback();
    }
#else
    Callback_T convert(VALUE value)
    {
      using NativeCallback_T = NativeCallbackSimple<Return_T, Arg_Ts...>;
      NativeCallback_T::proc = value;
      return &NativeCallback_T::callback;
    }
#endif

  private:
    Arg* arg_ = nullptr;
  };
}

// C++ API definitions

// =========   Encoding.ipp   =========
namespace Rice
{
  inline Encoding Encoding::utf8()
  {
    return Encoding(rb_utf8_encoding());
  }

  inline Encoding::Encoding(rb_encoding* encoding) : encoding_(encoding)
  {
  }
}

/*namespace Rice::detail
{
  template<>
  struct Type<Encoding>
  {
    static bool verify()
    {
      return true;
    }
  };
  
  template<>
  class To_Ruby<Encoding>
  {
  public:
    VALUE convert(const Encoding& encoding)
    {
    //  return x.value();
    }
  };

  template<>
  class From_Ruby<Encoding>
  {
  public:
    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_SYMBOL:
          return Convertible::Exact;
          break;
      case RUBY_T_STRING:
          return Convertible::Cast;
          break;
        default:
          return Convertible::None;
        }
    }

    Encoding convert(VALUE value)
    {
     // return Symbol(value);
    }
  };
}
*/
// =========   Object.ipp   =========
namespace Rice
{
  inline const Object Nil(Qnil);
  inline const Object True(Qtrue);
  inline const Object False(Qfalse);
  inline const Object Undef(Qundef);

  // Ruby auto detects VALUEs in the stack, so when an Object gets deleted make sure
  // to clean up in case it is on the stack
  inline Object::~Object()
  {
    this->value_ = Qnil;
  }

  // Move constructor
  inline Object::Object(Object&& other)
  {
    this->value_ = other.value_;
    other.value_ = Qnil;
  }

  // Move assignment
  inline Object& Object::operator=(Object&& other)
  {
    this->value_ = other.value_;
    other.value_ = Qnil;
    return *this;
  }

  template<typename ...Arg_Ts>
  inline Object Object::call(Identifier id, Arg_Ts... args) const
  {
    /* IMPORTANT - We store VALUEs in an array that is a local variable.
       That allows the Ruby garbage collector to find them when scanning
       the stack and thus mark them. If instead we use a vector, then Ruby's GC
       can't find the VALUEs and may garbage collect them before they are sent
       to the destination method resulting in a segmentation fault. This is
       easy to duplicate by setting GC.stress to true and calling a constructor
       that takes multiple values like a std::pair wrapper. */
    std::array<VALUE, sizeof...(Arg_Ts)> values = { detail::To_Ruby<detail::remove_cv_recursive_t<Arg_Ts>>().convert(args)... };
    return detail::protect(rb_funcallv, value(), id.id(), (int)values.size(), (const VALUE*)values.data());
  }

  template<typename ...Arg_Ts>
  inline Object Object::call_kw(Identifier id, Arg_Ts... args) const
  {
    /* IMPORTANT - See call() above */
    std::array<VALUE, sizeof...(Arg_Ts)> values = { detail::To_Ruby<detail::remove_cv_recursive_t<Arg_Ts>>().convert(args)... };
    return detail::protect(rb_funcallv_kw, value(), id.id(), (int)values.size(), (const VALUE*)values.data(), RB_PASS_KEYWORDS);
  }

  template<typename T>
  inline void Object::iv_set(Identifier name, T const& value)
  {
    detail::protect(rb_ivar_set, this->value(), name.id(), detail::To_Ruby<T>().convert(value));
  }

  inline int Object::compare(Object const& other) const
  {
    Object result = call("<=>", other);
    return detail::From_Ruby<int>().convert(result);
  }

  inline bool Object::is_equal(const Object& other) const
  {
    VALUE result = detail::protect(rb_equal, this->value_, other.value_);
    return RB_TEST(result);
  }

  inline bool Object::is_eql(const Object& other) const
  {
    VALUE result = detail::protect(rb_eql, this->value_, other.value_);
    return RB_TEST(result);
  }

  inline void Object::freeze()
  {
    detail::protect(rb_obj_freeze, value());
  }

  inline bool Object::is_frozen() const
  {
    return RB_OBJ_FROZEN(value());
  }

  inline int Object::rb_type() const
  {
    return ::rb_type(this->value());
  }

  inline VALUE Object::object_id() const
  {
    return detail::protect(rb_obj_id, this->value());
  }

  inline bool Object::is_a(Object klass) const
  {
    VALUE result = detail::protect(rb_obj_is_kind_of, this->value(), klass.value());
    return RB_TEST(result);
  }

  inline bool Object::respond_to(Identifier id) const
  {
    return bool(rb_respond_to(this->value(), id.id()));
  }

  inline bool Object::is_instance_of(Object klass) const
  {
    VALUE result = detail::protect(rb_obj_is_instance_of, this->value(), klass.value());
    return RB_TEST(result);
  }

  inline Object Object::iv_get(Identifier name) const
  {
    return detail::protect(rb_ivar_get, this->value(), name.id());
  }

  inline Object Object::attr_get(Identifier name) const
  {
    return detail::protect(rb_attr_get, this->value(), name.id());
  }

  inline void Object::set_value(VALUE v)
  {
    value_ = v;
  }

  inline Object Object::const_get(Identifier name) const
  {
    return detail::protect(rb_const_get, this->value(), name.id());
  }

  inline bool Object::const_defined(Identifier name) const
  {
    size_t result = detail::protect(rb_const_defined, this->value(), name.id());
    return bool(result);
  }

  inline Object Object::const_set(Identifier name, Object value)
  {
    detail::protect(rb_const_set, this->value(), name.id(), value.value());
    return value;
  }

  inline Object Object::const_set_maybe(Identifier name, Object value)
  {
    if (!this->const_defined(name))
    {
      this->const_set(name, value);
    }
    return value;
  }

  inline void Object::remove_const(Identifier name)
  {
    detail::protect(rb_mod_remove_const, this->value(), name.to_sym());
  }

  inline bool operator==(Object const& lhs, Object const& rhs)
  {
    VALUE result = detail::protect(rb_equal, lhs.value(), rhs.value());
    return result == Qtrue ? true : false;
  }

  inline bool operator!=(Object const& lhs, Object const& rhs)
  {
    return !(lhs == rhs);
  }

  inline bool operator<(Object const& lhs, Object const& rhs)
  {
    Object result = lhs.call("<", rhs);
    return result.test();
  }

  inline bool operator>(Object const& lhs, Object const& rhs)
  {
    Object result = lhs.call(">", rhs);
    return result.test();
  }
}

namespace Rice::detail
{
  template<>
  struct Type<Object>
  {
    static bool verify()
    {
      return true;
    }
  };

  template<>
  class To_Ruby<Object>
  {
  public:
    static VALUE convert(Object const& x)
    {
      return x.value();
    }
  };

  template<>
  class To_Ruby<Object&>
  {
  public:
    static VALUE convert(Object const& x)
    {
      return x.value();
    }
  };

  template<>
  class From_Ruby<Object>
  {
  public:
    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_OBJECT:
          return Convertible::Exact;
          break;
        default:
          return Convertible::None;
      }
    }

    Object convert(VALUE value)
    {
      return Object(value);
    }
  };
}

// =========   Builtin_Object.ipp   =========
#include <algorithm>

namespace Rice
{
  namespace detail
  {
    inline VALUE check_type(Object value, int type)
    {
      detail::protect(rb_check_type, value.value(), type);
      return Qnil;
    }
  }

  template<int Builtin_Type>
  inline Builtin_Object<Builtin_Type>::Builtin_Object(Object value) : Object(value)
  {
    detail::check_type(value, Builtin_Type);
  }

  template<int Builtin_Type>
  inline RObject& Builtin_Object<Builtin_Type>::operator*() const
  {
    return *ROBJECT(this->value());
  }

  template<int Builtin_Type>
  inline RObject* Builtin_Object<Builtin_Type>::operator->() const
  {
    return ROBJECT(this->value());
  }

  template<int Builtin_Type>
  inline RObject* Builtin_Object<Builtin_Type>::get() const
  {
    return ROBJECT(this->value());
  }
} // namespace Rice

// =========   String.ipp   =========
namespace Rice
{
  inline String::String() : Builtin_Object<T_STRING>(detail::protect(rb_str_new2, ""))
  {
  }

  inline String::String(VALUE v) : Builtin_Object<T_STRING>(v)
  {
  }

  inline String::String(Object v) : Builtin_Object<T_STRING>(v)
  {
  }

  inline String::String(char const* s) : Builtin_Object<T_STRING>(detail::protect(rb_utf8_str_new_cstr, s))
  {
  }

  inline String::String(std::string const& s) : Builtin_Object<T_STRING>(detail::protect(rb_utf8_str_new, s.data(), (long)s.length()))
  {
  }

  inline String::String(std::string_view const& s) : Builtin_Object<T_STRING>(detail::protect(rb_utf8_str_new, s.data(), (long)s.length()))
  {
  }

  inline String::String(Identifier id) : Builtin_Object<T_STRING>(detail::protect(rb_utf8_str_new_cstr, id.c_str()))
  {
  }

  template <typename... Arg_Ts>
  inline String String::format(char const* fmt, Arg_Ts&&...args)
  {
    size_t size = std::snprintf(nullptr, 0, fmt, std::forward<Arg_Ts>(args)...);
    std::string temp(size, '\0');

    // size+1 avoids truncating the string. Otherwise snprintf writes n - 1 characters
    // to allow space for null character but we don't need that since std::string
    // will add a null character internally at n + 1
    std::snprintf(&temp[0], size + 1, fmt, std::forward<Arg_Ts>(args)...);

    String s = String(temp.c_str());
    return s;
  }

  inline size_t String::length() const
  {
    return RSTRING_LEN(value());
  }

  inline char String::operator[](ptrdiff_t index) const
  {
    return RSTRING_PTR(value())[index];
  }

  inline char const* String::c_str() const
  {
    return RSTRING_PTR(value());
  }

  inline std::string String::str() const
  {
    return std::string(RSTRING_PTR(value()), length());
  }

  inline Identifier String::intern() const
  {
    return rb_intern(c_str());
  }
}

namespace Rice::detail
{
  template<>
  struct Type<String>
  {
    static bool verify()
    {
      return true;
    }
  };
  
  template<>
  class To_Ruby<String>
  {
  public:
    VALUE convert(String const& x)
    {
      return x.value();
    }
  };

  template<>
  class From_Ruby<String>
  {
  public:
    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_STRING:
          return Convertible::Exact;
          break;
        default:
          return Convertible::None;
      }
    }

    String convert(VALUE value)
    {
      return String(value);
    }
  };
}
// =========   Array.ipp   =========

namespace Rice
{
  inline Array::Array() : Builtin_Object<T_ARRAY>(detail::protect(rb_ary_new))
  {
  }

  inline Array::Array(long capacity) : Builtin_Object<T_ARRAY>(detail::protect(rb_ary_new_capa, capacity))
  {
  }

  inline Array::Array(Object v) : Builtin_Object<T_ARRAY>(v)
  {
  }

  inline Array::Array(VALUE v) : Builtin_Object<T_ARRAY>(v)
  {
  }

  template<typename Iter_T>
  inline Array::Array(Iter_T it, Iter_T end) : Builtin_Object<T_ARRAY>(detail::protect(rb_ary_new))
  {
    for (; it != end; ++it)
    {
      push(*it);
    }
  }

  template<typename T, long n>
  inline Array::Array(T const (&a)[n]) : Builtin_Object<T_ARRAY>(detail::protect(rb_ary_new))
  {
    for (long j = 0; j < n; ++j)
    {
      push(a[j]);
    }
  }

  inline long Array::size() const
  {
    return RARRAY_LEN(this->value());
  }

  template<typename T>
  std::unique_ptr<T[]> Array::pack()
  {
    String string = this->call("pack", detail::RubyType<T>::packTemplate);
    VALUE value = string.value();
    std::unique_ptr<T[]> result = std::make_unique<T[]>(this->size());
    memcpy(result.get(), RSTRING_PTR(value), RSTRING_LEN(value));
    return std::move(result);
  }

  inline Object Array::operator[](long index) const
  {
    return detail::protect(rb_ary_entry, value(), position_of(index));
  }

  inline Array::Proxy Array::operator[](long index)
  {
    return Proxy(*this, position_of(index));
  }

  template<typename T>
  inline Object Array::push(T const& obj)
  {
    return detail::protect(rb_ary_push, value(), detail::To_Ruby<T>().convert(obj));
  }

  inline Object Array::pop()
  {
    return detail::protect(rb_ary_pop, value());
  }

  template<typename T>
  inline Object Array::unshift(T const& obj)
  {
    return detail::protect(rb_ary_unshift, value(), detail::To_Ruby<T>().convert(obj));
  }

  inline Object Array::shift()
  {
    return detail::protect(rb_ary_shift, value());
  }

  inline long Array::position_of(long index) const
  {
    if (index < 0)
    {
      return size() + index;
    }
    else
    {
      return static_cast<long>(index);
    }
  }

  template<typename T>
  std::vector<T> Array::to_vector()
  {
    long size = this->size();
    std::vector<T> result(size);

    for (long i = 0; i < size; i++)
    {
      VALUE element = detail::protect(rb_ary_entry, this->value(), i);
      result[i] = detail::From_Ruby<T>().convert(element);
    }

    return result;
  }

  inline Array::Proxy::Proxy(Array array, long index)
    : array_(array)
    , index_(index)
  {
  }

  inline VALUE Array::Proxy::value() const
  {
    return detail::protect(rb_ary_entry, array_.value(), index_);
  }

  inline Array::Proxy::operator Object() const
  {
    return Object(this->value());
  }

  template<typename T>
  Object Array::Proxy::operator=(T const& value)
  {
    Object o = detail::To_Ruby<T>().convert(value);
    detail::protect(rb_ary_store, array_.value(), index_, o.value());
    return o;
  }

  template<typename Array_Ptr_T, typename Value_T>
  inline Array::Iterator<Array_Ptr_T, Value_T>::Iterator(Array_Ptr_T array, long index) :
    array_(array), index_(index)
  {
  }

  template<typename Array_Ptr_T, typename Value_T>
  template<typename Array_Ptr_T_, typename Value_T_>
  inline
    Array::Iterator<Array_Ptr_T, Value_T>::Iterator(Iterator<Array_Ptr_T_, Value_T_> const& rhs) :
      array_(rhs.array()) , index_(rhs.index()), tmp_()
  {
  }

  template<typename Array_Ptr_T, typename Value_T>
  template<typename Array_Ptr_T_, typename Value_T_>
  inline Array::Iterator<Array_Ptr_T, Value_T>& Array::Iterator<Array_Ptr_T, Value_T>::operator=(Iterator<Array_Ptr_T_, Value_T_> const& rhs)
  {
    array_ = rhs.array_;
    index_ = rhs.index_;
    return *this;
  }

  template<typename Array_Ptr_T, typename Value_T>
  inline Array::Iterator<Array_Ptr_T, Value_T>& Array::Iterator<Array_Ptr_T, Value_T>::operator++()
  {
    ++index_;
    return *this;
  }

  template<typename Array_Ptr_T, typename Value_T>
  inline Array::Iterator<Array_Ptr_T, Value_T> Array::Iterator<Array_Ptr_T, Value_T>::operator++(int)
  {
    Array copy(*this);
    ++(*this);
    return *this;
  }

  template<typename Array_Ptr_T, typename Value_T>
  inline Value_T Array::Iterator<Array_Ptr_T, Value_T>::operator*()
  {
    return (*array_)[index_];
  }

  template<typename Array_Ptr_T, typename Value_T>
  inline Object* Array::Iterator<Array_Ptr_T, Value_T>::operator->()
  {
    tmp_ = (*array_)[index_];
    return &tmp_;
  }

  template<typename Array_Ptr_T, typename Value_T>
  template<typename Array_Ptr_T_, typename Value_T_>
  inline bool Array::Iterator<Array_Ptr_T, Value_T>::operator==(Iterator<Array_Ptr_T_, Value_T_> const& rhs) const
  {
    return array_->value() == rhs.array_->value() && index_ == rhs.index_;
  }

  template<typename Array_Ptr_T, typename Value_T>
  template<typename Array_Ptr_T_, typename Value_T_>
  inline bool Array::Iterator<Array_Ptr_T, Value_T>::operator!=(Iterator<Array_Ptr_T_, Value_T_> const& rhs) const
  {
    return !(*this == rhs);
  }

  template<typename Array_Ptr_T, typename Value_T>
  Array_Ptr_T Array::Iterator<Array_Ptr_T, Value_T>::array() const
  {
    return array_;
  }

  template<typename Array_Ptr_T, typename Value_T>
  long Array::Iterator<Array_Ptr_T, Value_T>::index() const
  {
    return index_;
  }

  inline Array::iterator Array::begin()
  {
    return iterator(this, 0);
  }

  inline Array::const_iterator Array::begin() const
  {
    return const_iterator(this, 0);
  }

  inline Array::iterator Array::end()
  {
    return iterator(this, size());
  }

  inline Array::const_iterator Array::end() const
  {
    return const_iterator(this, size());
  }
}

namespace Rice::detail
{
  template<>
  struct Type<Array>
  {
    static bool verify()
    {
      return true;
    }
  };

  template<>
  class To_Ruby<Array>
  {
  public:
    VALUE convert(Array const& x)
    {
      return x.value();
    }
  };

  template<>
  class To_Ruby<Array&>
  {
  public:
    VALUE convert(Array const& x)
    {
      return x.value();
    }
  };

  template<>
  class To_Ruby<Array*>
  {
  public:
    VALUE convert(Array const* x)
    {
      return x->value();
    }
  };

  template<>
  class From_Ruby<Array>
  {
  public:
    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_ARRAY:
          return Convertible::Exact;
          break;
        default:
          return Convertible::None;
      }
    }

    Array convert(VALUE value)
    {
      return Array(value);
    }
  };
}

// =========   Hash.ipp   =========
#include <algorithm>

namespace Rice
{
  inline Hash::Hash() : Builtin_Object<T_HASH>(detail::protect(rb_hash_new))
  {
  }

  inline Hash::Hash(Object v) : Builtin_Object<T_HASH>(v)
  {
  }

  inline size_t Hash::size() const
  {
    return RHASH_SIZE(this->value());
  }

  inline Hash::Proxy::Proxy(Hash* hash, Object key) : hash_(hash), key_(key)
  {
  }

  inline Hash::Proxy::operator Object() const
  {
    return value();
  }

  inline VALUE Hash::Proxy::value() const
  {
    return detail::protect(rb_hash_aref, hash_->value(), key_.value());
  }

  template<typename T>
  inline Object Hash::Proxy::operator=(T const& value)
  {
    return detail::protect(rb_hash_aset, hash_->value(), key_.value(), detail::To_Ruby<T>().convert(value));
  }

  template<typename Key_T>
  inline Hash::Proxy const Hash::operator[](Key_T const& key) const
  {
    return Proxy(*this, detail::To_Ruby<Key_T>().convert(key));
  }

  template<typename Key_T>
  inline Hash::Proxy Hash::operator[](Key_T const& key)
  {
    return Proxy(this, detail::To_Ruby<Key_T>().convert(key));
  }

  template<typename Value_T, typename Key_T>
  inline Value_T Hash::get(Key_T const& key)
  {
    Object ruby_key(detail::To_Ruby<Key_T>().convert(key));
    Object value = operator[](ruby_key);
    try
    {
      return detail::From_Ruby<Value_T>().convert(value);
    }
    catch (Exception const& ex)
    {
      String s_key(ruby_key.to_s());
      throw Exception(
        ex,
        "%s while converting value for key %s",
        ex.what(),
        s_key.c_str());
    }
  }

  inline Hash::Entry::Entry(Hash* hash, Object key) :
    key(key), first(Hash::Entry::key), value(hash, key), second(Hash::Entry::value)
  {
  }

  inline Hash::Entry::Entry(Entry const& entry) :
    key(entry.key), first(Hash::Entry::key), value(entry.value), second(Hash::Entry::value)
  {
  }

  inline Hash::Entry& Hash::Entry::operator=(Hash::Entry const& other)
  {
    const_cast<Object&>(key) = const_cast<Object&>(other.key);

    this->value = other.value;
    this->second = this->value;

    return *this;
  }

  template<typename Hash_Ptr_T, typename Value_T>
  inline Hash::Iterator<Hash_Ptr_T, Value_T>::Iterator(Hash_Ptr_T hash)
    : hash_(hash), current_index_(0), keys_(Qnil), tmp_(const_cast<Hash*>(hash), Qnil)
  {
  }

  template<typename Hash_Ptr_T, typename Value_T>
  inline Hash::Iterator<Hash_Ptr_T, Value_T>::Iterator(Hash_Ptr_T hash, int start_at)
    : hash_(hash), current_index_(start_at), keys_(Qnil), tmp_(const_cast<Hash*>(hash), Qnil)
  {
  }

  template<typename Hash_Ptr_T, typename Value_T>
  template<typename Iterator_T>
  inline Hash::Iterator<Hash_Ptr_T, Value_T>::Iterator(Iterator_T const& iterator) :
    hash_(iterator.hash_), current_index_(iterator.current_index_), keys_(Qnil), tmp_(iterator.hash_, Qnil)
  {
  }

  template<typename Hash_Ptr_T, typename Value_T>
  inline Hash::Iterator<Hash_Ptr_T, Value_T>& Hash::Iterator<Hash_Ptr_T, Value_T>::operator++()
  {
    // Ensure we're within the range
    if (current_index_ < hash_keys().size())
    {
      current_index_++;
    }

    return *this;
  }

  template<typename Hash_Ptr_T, typename Value_T>
  inline Hash::Iterator<Hash_Ptr_T, Value_T> Hash::Iterator<Hash_Ptr_T, Value_T>::operator++(int)
  {
    Iterator copy(*this);
    ++(*this);
    return copy;
  }

  template<typename Hash_Ptr_T, typename Value_T>
  inline Value_T Hash::Iterator<Hash_Ptr_T, Value_T>::operator*()
  {
    return Value_T(const_cast<Hash*>(hash_), current_key());
  }

  template<typename Hash_Ptr_T, typename Value_T>
  inline Value_T* Hash::Iterator<Hash_Ptr_T, Value_T>::operator->()
  {
    this->tmp_ = Entry(const_cast<Hash*>(hash_), current_key());
    return &tmp_;
  }

  template<typename Hash_Ptr_T, typename Value_T>
  inline bool Hash::Iterator<Hash_Ptr_T, Value_T>::operator==(Iterator const& rhs) const
  {
    return hash_->value() == rhs.hash_->value()
      && current_index_ == rhs.current_index_;
  }

  template<typename Hash_Ptr_T, typename Value_T>
  inline bool Hash::Iterator<Hash_Ptr_T, Value_T>::operator!=(Iterator const& rhs) const
  {
    return !(*this == rhs);
  }

  template<typename Hash_Ptr_T, typename Value_T>
  inline Object Hash::Iterator<Hash_Ptr_T, Value_T>::current_key()
  {
    return hash_keys()[current_index_];
  }

  template<typename Hash_Ptr_T, typename Value_T>
  inline Array Hash::Iterator<Hash_Ptr_T, Value_T>::hash_keys()
  {
    if (NIL_P(keys_))
    {
      keys_ = rb_funcall(hash_->value(), rb_intern("keys"), 0, 0);
    }

    return Array(keys_);
  }

  inline Hash::iterator Hash::begin()
  {
    return iterator(this);
  }

  inline Hash::const_iterator Hash::begin() const
  {
    return const_iterator(this);
  }

  inline Hash::iterator Hash::end()
  {
    return iterator(this, (int)size());
  }

  inline Hash::const_iterator Hash::end() const
  {
    return const_iterator(this, (int)size());
  }

  inline bool operator<(Rice::Hash::Entry const& lhs, Rice::Hash::Entry const& rhs)
  {
    Rice::Object lhs_key(lhs.key);
    Rice::Object rhs_key(rhs.key);
    if (lhs_key < rhs_key)
    {
      return true;
    }
    else if (lhs_key > rhs_key)
    {
      return false;
    }
    else if (Rice::Object(lhs.value.value()) < Rice::Object(rhs.value.value()))
    {
      return true;
    }
    else
    {
      return false;
    }
  }
}

namespace Rice::detail
{
  template<>
  struct Type<Hash>
  {
    static bool verify()
    {
      return true;
    }
  };

  template<>
  class To_Ruby<Hash>
  {
  public:
    VALUE convert(Hash const& x)
    {
      return x.value();
    }
  };

  template<>
  class From_Ruby<Hash>
  {
  public:
    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_HASH:
          return Convertible::Exact;
          break;
        default:
          return Convertible::None;
      }
    }

    Hash convert(VALUE value)
    {
      return Hash(value);
    }
  };
}

// =========   Symbol.ipp   =========
namespace Rice
{
  inline Symbol::Symbol(VALUE value) : Object(value)
  {
    detail::protect(rb_check_type, value, (int)T_SYMBOL);
  }

  inline Symbol::Symbol(Object value) : Object(value)
  {
    detail::protect(rb_check_type, value.value(), (int)T_SYMBOL);
  }

  inline Symbol::Symbol(char const* s)
    : Object(detail::protect(rb_id2sym, detail::protect(rb_intern, s)))
  {
  }

  inline Symbol::Symbol(std::string const& s)
    : Object(detail::protect(rb_id2sym, detail::protect(rb_intern2, s.c_str(), (long)s.length())))
  {
  }

  inline Symbol::Symbol(std::string_view const& view)
    : Object(detail::protect(rb_id2sym, detail::protect(rb_intern2, view.data(), (long)view.length())))
  {
  }

  inline Symbol::Symbol(Identifier id) : Object(detail::protect(rb_id2sym, id))
  {
  }

  inline char const* Symbol::c_str() const
  {
    return to_id().c_str();
  }

  inline std::string Symbol::str() const
  {
    return to_id().str();
  }

  inline Identifier Symbol::to_id() const
  {
    return rb_to_id(value());
  }
}

namespace Rice::detail
{
  template<>
  struct Type<Symbol>
  {
    static bool verify()
    {
      return true;
    }
  };
  
  template<>
  class To_Ruby<Symbol>
  {
  public:
    VALUE convert(Symbol const& x)
    {
      return x.value();
    }
  };

  template<>
  class From_Ruby<Symbol>
  {
  public:
    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_SYMBOL:
          return Convertible::Exact;
          break;
      case RUBY_T_STRING:
          return Convertible::Cast;
          break;
        default:
          return Convertible::None;
        }
    }

    Symbol convert(VALUE value)
    {
      return Symbol(value);
    }
  };
}


// =========   Module.hpp   =========

namespace Rice
{
  template <typename T>
  void validateType();

  //! A helper for defining a Module and its methods.
  /*! This class provides a C++-style interface to ruby's Module class and
   *  for defining methods on that module.
   *
   *  Many of the methods are defined in Module_impl.hpp so that they can
   *  return a reference to the most derived type.
   */
   // TODO: we can't inherit from Builtin_Object, because Class needs
   // type T_CLASS and Module needs type T_MODULE
  class Module : public Object
  {
  public:
    //! Default construct a Module and initialize it to rb_cObject.
    Module();

    //! Construct a Module from an existing Module object.
    Module(VALUE v);

    //! Construct a Module from an string that references a Module
    Module(std::string name, Object under = rb_cObject);

    //! Return the name of the module.
    String name() const;

    //! Return an array containing the Module's ancestors.
    /*! You will need to include Array.hpp to use this function.
     */
    Array ancestors() const;

    //! Return the module's singleton class.
    /*! You will need to include Class.hpp to use this function.
     */
    Class singleton_class() const;

    //! Evaluate the given string in the context of the module.
    /*! This is equivalant to calling obj.module_eval(s) from inside the
     *  interpreter.
     *  \return the result of the expression.
     */
    Object module_eval(String const& s);

    // Include these methods to call methods from Module but return
// an instance of the current classes. This is an alternative to
// using CRTP.


//! Include a module.
/*! \param inc the module to be included.
*  \return *this
*/
inline auto& include_module(Module const& inc)
{
  detail::protect(rb_include_module, this->value(), inc.value());
  return *this;
}

//! Define an instance method.
/*! The method's implementation can be a member function, plain function
 *  or lambda. The easiest case is a member function where the Ruby
 *  method maps one-to-one to the C++ method. In the case of a
 *  plain function or lambda, the first argument must be SELF - ie,
 *  the current object. If the type of the first argument is VALUE, then
 *  the current Ruby object is passed. If the type is a C++ class,
 *  then the C++ object is passed. If you don't want to include the
 *  SELF argument see define_function.
 *  Rice will automatically convert method parameters from Ruby to C++ and
 *  convert the return value from C++ to Ruby.
 *  \param name the name of the method
 *  \param func the implementation of the function, either a function
 *  pointer or a member function pointer.
 *  \param args a list of Arg instance used to define default parameters.
 *  \return *this
 */
template<typename Function_T, typename...Arg_Ts>
inline auto& define_method(std::string name, Function_T&& func, const Arg_Ts&...args)
{
  MethodInfo* methodInfo = new MethodInfo(detail::method_traits<Function_T, true>::arity, args...);
  this->wrap_native_call<true>(this->value(), name, std::forward<Function_T>(func), methodInfo);
  return *this;
}

//! Define an instance function.
/*! The function implementation is a plain function or a static
 *  member function.
 *  Rice will automatically convert method method from Ruby to C++ and
 *  then convert the return value from C++ to Ruby.
 *  \param name the name of the method
 *  \param func the implementation of the function, either a function
 *  pointer or a member function pointer.
 *  \param args a list of Arg instance used to define default parameters (optional)
 *  \return *this
 */
template<typename Function_T, typename...Arg_Ts>
inline auto& define_function(std::string name, Function_T&& func, const Arg_Ts&...args)
{
  MethodInfo* methodInfo = new MethodInfo(detail::method_traits<Function_T, false>::arity, args...);
  this->wrap_native_call<false>(this->value(), name, std::forward<Function_T>(func), methodInfo);
  return *this;
}

//! Define a singleton method.
/*! The method's implementation can be a static member function,
*   plain function or lambda. In all cases the first argument
*   must be SELF - ie, the current object. If it is specified as a VALUE, then
 *  the current Ruby object is passed. If it is specified as a C++ class,
 *  then the C++ object is passed. If you don't want to include the
 *  SELF argument see define_singleton_function.
 *  Rice will automatically convert method method from Ruby to C++ and
 *  then convert the return value from C++ to Ruby.
 *  \param name the name of the method
 *  \param func the implementation of the function, either a function
 *  pointer or a member function pointer.
 *  \param args a list of Arg instance used to define default parameters (optional)
 *  \return *this
 */
template<typename Function_T, typename...Arg_Ts>
inline auto& define_singleton_method(std::string name, Function_T&& func, const Arg_Ts&...args)
{
  MethodInfo* methodInfo = new MethodInfo(detail::method_traits<Function_T, true>::arity, args...);
  this->wrap_native_call<true>(rb_singleton_class(*this), name, std::forward<Function_T>(func), methodInfo);
  return *this;
}

//! Define a singleton method.
/*! The method's implementation can be a static member function, plain
 *  function or lambda.
 . A wrapper will be
 * generated which will convert the method
 *  from ruby types to C++ types before calling the function.  The return
 *  value will be converted back to ruby.
 *  \param name the name of the method
 *  \param func the implementation of the function, either a function
 *  pointer or a member function pointer.
 *  \param args a list of Arg instance used to define default parameters (optional)
 *  \return *this
 */
template<typename Function_T, typename...Arg_Ts>
inline auto& define_singleton_function(std::string name, Function_T&& func, const Arg_Ts& ...args)
{
  MethodInfo* methodInfo = new MethodInfo(detail::method_traits<Function_T, false>::arity, args...);
  this->wrap_native_call<false>(rb_singleton_class(*this), name, std::forward<Function_T>(func), methodInfo);
  return *this;
}

//! Define a module function.
/*! A module function is a function that can be accessed either as a
 *  singleton method or as an instance method. It wrap a plain
 *  function, static member function or lambda.
 *  Rice will automatically convert method method from Ruby to C++ and
 *  then convert the return value from C++ to Ruby.
 *  \param name the name of the method
 *  \param func the implementation of the function, either a function
 *  pointer or a member function pointer.
 *  \param args a list of Arg instance used to define default parameters (optional)
 *  \return *this
 */
template<typename Function_T, typename...Arg_Ts>
inline auto& define_module_function(std::string name, Function_T&& func, const Arg_Ts& ...args)
{
  if (this->rb_type() != T_MODULE)
  {
    throw std::runtime_error("can only define module functions for modules");
  }

  define_function(name, std::forward<Function_T>(func), args...);
  define_singleton_function(name, std::forward<Function_T>(func), args...);
  return *this;
}

//! Define a constant
template<typename Constant_T>
inline auto& define_constant(std::string name, Constant_T value)
{
  using Base_T = detail::remove_cv_recursive_t<Constant_T>;
  detail::protect(rb_define_const, this->value(), name.c_str(), detail::To_Ruby<Base_T>().convert(value));
  return *this;
}
  protected:
    template<bool IsMethod, typename Function_T>
    void wrap_native_call(VALUE klass, std::string name, Function_T&& function, MethodInfo* methodInfo);
  };

  //! Define a new module in the namespace given by module.
  /*! \param module the module in which to define the new module.
   *  \param name the name of the new module.
   */
  Module define_module_under(Object module, char const * name);

  //! Define a new module in the default namespace.
  /*! \param name the name of the new module.
   */
  Module define_module(char const * name);

  //! Create a new anonymous module.
  /*! \return the new module.
   */
  Module anonymous_module();
}

// =========   Module.ipp   =========

namespace Rice
{
  inline Module::Module() : Object(rb_cObject)
  {
  }

  inline Module::Module(VALUE value) : Object(value)
  {
    if (::rb_type(value) != T_CLASS && ::rb_type(value) != T_MODULE)
    {
      throw Exception(
        rb_eTypeError,
        "Expected a Module but got a %s",
        detail::protect(rb_obj_classname, value)); // TODO: might raise an exception
    }
  }

  //! Construct a Module from an string that references a Module
  inline Module::Module(std::string name, Object under)
  {
    VALUE result = under.const_get(name);

    if (::rb_type(result) != T_MODULE)
    {
      throw Exception(
        rb_eTypeError,
        "Expected a Module but got a %s",
        detail::protect(rb_obj_classname, result)); // TODO: might raise an exception
    }

    this->set_value(result);
  }

  template<bool IsMethod, typename Function_T>
  inline void Module::wrap_native_call(VALUE klass, std::string name, Function_T&& function, MethodInfo* methodInfo)
  {
    // Make sure the return type and arguments have been previously seen by Rice
    using traits = detail::method_traits<Function_T, IsMethod>;
    detail::verifyType<typename traits::Return_T>();
    detail::verifyTypes<typename traits::Arg_Ts>();

    // Define a NativeFunction to bridge Ruby to C++
    detail::NativeFunction<VALUE, Function_T, IsMethod>::define(klass, name, std::forward<Function_T>(function), methodInfo);
  }

  inline Module define_module_under(Object module, char const* name)
  {
    return detail::protect(rb_define_module_under, module.value(), name);
  }

  inline Module define_module(char const* name)
  {
    return detail::protect(rb_define_module, name);
  }

  inline Module anonymous_module()
  {
    VALUE klass = detail::protect(rb_module_new);
    VALUE singleton = detail::protect(rb_singleton_class, klass);

    // Ruby will reuse addresses previously assigned to other modules
    // that have subsequently been garbage collected
    detail::Registries::instance.natives.reset(klass);
    detail::Registries::instance.natives.reset(singleton);

    return klass;
  }
}

namespace Rice::detail
{
  template<>
  struct Type<Module>
  {
    static bool verify()
    {
      return true;
    }
  };

  template<>
  class To_Ruby<Module>
  {
  public:
    VALUE convert(Module const& x)
    {
      return x.value();
    }
  };

  template<>
  class From_Ruby<Module>
  {
  public:
    Module convert(VALUE value)
    {
      return Module(value);
    }
  };
}

// =========   Class.hpp   =========

/*!
 *  \example inheritance/animals.cpp
 *  \example callbacks/sample_callbacks.cpp
 */

namespace Rice
{
  //! A helper for defining a Class and its methods.
  /*! This class provides a C++-style interface to ruby's Class class and
   *  for defining methods on that class.
   */
  class Class: public Module
  {
  public:
    //! Default construct a new class wrapper and initialize it to
    //! rb_cObject.
    Class() = default;

    //! Construct a new class wrapper from a ruby object of type T_CLASS.
    Class(VALUE v);

    //! Disallow creation of an instance from Ruby code.
    /*! Undefines the singleton method allocate (or new, if using a
     *  version of ruby prior to 1.7) and the instance method initialize.
     */
    Class & undef_creation_funcs();

    // Create a new instance
    template<typename ...Arg_Ts>
    Object create(Arg_Ts ...args);

    //! Class name
    /*! \return std::string.
     */
    const std::string name() const;

    // Include these methods to call methods from Module but return
// an instance of the current classes. This is an alternative to
// using CRTP.


//! Include a module.
/*! \param inc the module to be included.
*  \return *this
*/
inline auto& include_module(Module const& inc)
{
  detail::protect(rb_include_module, this->value(), inc.value());
  return *this;
}

//! Define an instance method.
/*! The method's implementation can be a member function, plain function
 *  or lambda. The easiest case is a member function where the Ruby
 *  method maps one-to-one to the C++ method. In the case of a
 *  plain function or lambda, the first argument must be SELF - ie,
 *  the current object. If the type of the first argument is VALUE, then
 *  the current Ruby object is passed. If the type is a C++ class,
 *  then the C++ object is passed. If you don't want to include the
 *  SELF argument see define_function.
 *  Rice will automatically convert method parameters from Ruby to C++ and
 *  convert the return value from C++ to Ruby.
 *  \param name the name of the method
 *  \param func the implementation of the function, either a function
 *  pointer or a member function pointer.
 *  \param args a list of Arg instance used to define default parameters.
 *  \return *this
 */
template<typename Function_T, typename...Arg_Ts>
inline auto& define_method(std::string name, Function_T&& func, const Arg_Ts&...args)
{
  MethodInfo* methodInfo = new MethodInfo(detail::method_traits<Function_T, true>::arity, args...);
  this->wrap_native_call<true>(this->value(), name, std::forward<Function_T>(func), methodInfo);
  return *this;
}

//! Define an instance function.
/*! The function implementation is a plain function or a static
 *  member function.
 *  Rice will automatically convert method method from Ruby to C++ and
 *  then convert the return value from C++ to Ruby.
 *  \param name the name of the method
 *  \param func the implementation of the function, either a function
 *  pointer or a member function pointer.
 *  \param args a list of Arg instance used to define default parameters (optional)
 *  \return *this
 */
template<typename Function_T, typename...Arg_Ts>
inline auto& define_function(std::string name, Function_T&& func, const Arg_Ts&...args)
{
  MethodInfo* methodInfo = new MethodInfo(detail::method_traits<Function_T, false>::arity, args...);
  this->wrap_native_call<false>(this->value(), name, std::forward<Function_T>(func), methodInfo);
  return *this;
}

//! Define a singleton method.
/*! The method's implementation can be a static member function,
*   plain function or lambda. In all cases the first argument
*   must be SELF - ie, the current object. If it is specified as a VALUE, then
 *  the current Ruby object is passed. If it is specified as a C++ class,
 *  then the C++ object is passed. If you don't want to include the
 *  SELF argument see define_singleton_function.
 *  Rice will automatically convert method method from Ruby to C++ and
 *  then convert the return value from C++ to Ruby.
 *  \param name the name of the method
 *  \param func the implementation of the function, either a function
 *  pointer or a member function pointer.
 *  \param args a list of Arg instance used to define default parameters (optional)
 *  \return *this
 */
template<typename Function_T, typename...Arg_Ts>
inline auto& define_singleton_method(std::string name, Function_T&& func, const Arg_Ts&...args)
{
  MethodInfo* methodInfo = new MethodInfo(detail::method_traits<Function_T, true>::arity, args...);
  this->wrap_native_call<true>(rb_singleton_class(*this), name, std::forward<Function_T>(func), methodInfo);
  return *this;
}

//! Define a singleton method.
/*! The method's implementation can be a static member function, plain
 *  function or lambda.
 . A wrapper will be
 * generated which will convert the method
 *  from ruby types to C++ types before calling the function.  The return
 *  value will be converted back to ruby.
 *  \param name the name of the method
 *  \param func the implementation of the function, either a function
 *  pointer or a member function pointer.
 *  \param args a list of Arg instance used to define default parameters (optional)
 *  \return *this
 */
template<typename Function_T, typename...Arg_Ts>
inline auto& define_singleton_function(std::string name, Function_T&& func, const Arg_Ts& ...args)
{
  MethodInfo* methodInfo = new MethodInfo(detail::method_traits<Function_T, false>::arity, args...);
  this->wrap_native_call<false>(rb_singleton_class(*this), name, std::forward<Function_T>(func), methodInfo);
  return *this;
}

//! Define a module function.
/*! A module function is a function that can be accessed either as a
 *  singleton method or as an instance method. It wrap a plain
 *  function, static member function or lambda.
 *  Rice will automatically convert method method from Ruby to C++ and
 *  then convert the return value from C++ to Ruby.
 *  \param name the name of the method
 *  \param func the implementation of the function, either a function
 *  pointer or a member function pointer.
 *  \param args a list of Arg instance used to define default parameters (optional)
 *  \return *this
 */
template<typename Function_T, typename...Arg_Ts>
inline auto& define_module_function(std::string name, Function_T&& func, const Arg_Ts& ...args)
{
  if (this->rb_type() != T_MODULE)
  {
    throw std::runtime_error("can only define module functions for modules");
  }

  define_function(name, std::forward<Function_T>(func), args...);
  define_singleton_function(name, std::forward<Function_T>(func), args...);
  return *this;
}

//! Define a constant
template<typename Constant_T>
inline auto& define_constant(std::string name, Constant_T value)
{
  using Base_T = detail::remove_cv_recursive_t<Constant_T>;
  detail::protect(rb_define_const, this->value(), name.c_str(), detail::To_Ruby<Base_T>().convert(value));
  return *this;
}
  };

  //! Define a new class in the namespace given by module.
  /*! \param module the Module in which to define the class.
   *  \param name the name of the class.
   *  \param superclass the base class to use.
   *  \return the new class.
   */
  Class define_class_under(Object parent, char const * name, const Class& superclass = rb_cObject);
  Class define_class_under(Object parent, Identifier id, const Class& superclass);

  //! Define a new class in the default namespace.
  /*! \param name the name of the class.
   *  \param superclass the base class to use.
   *  \return the new class.
   */
  Class define_class(char const * name, const Class& superclass = rb_cObject);

  //! Create a new anonymous class.
  /*! \return the new class.
   */
  Class anonymous_class();
} // namespace Rice


// =========   Class.ipp   =========

namespace Rice
{
  inline Class::Class(VALUE value) : Module(value)
  {
    detail::protect(rb_check_type, value, (int)T_CLASS);
  }

  inline Class& Class::undef_creation_funcs()
  {
    detail::protect(rb_undef_alloc_func, value());
    detail::protect(rb_undef_method, value(), "initialize");
    return *this;
  }

  template<typename ...Arg_Ts>
  inline Object Class::create(Arg_Ts ...args)
  {
    return this->call("new", args...);
  }

  inline const std::string Class::name() const
  {
    const char* buffer = rb_class2name(this->value());
    return std::string(buffer);
  }

  inline Class define_class_under(Object parent, Identifier id, const Class& superclass)
  {
    VALUE klass = detail::protect(rb_define_class_id_under, parent.value(), id, superclass.value());

    // We MUST reset the instance registry in case the user just redefined a class which resets it
    detail::Registries::instance.natives.reset(klass);

    return klass;
  }

  inline Class define_class_under(Object parent, char const* name, const Class& superclass)
  {
    Identifier id(name);
    return define_class_under(parent, id, superclass);
  }

  inline Class define_class(char const* name, const Class& superclass)
  {
    VALUE klass = detail::protect(rb_define_class, name, superclass.value());

    // We MUST reset the instance registry in case the user just redefined a class which resets it
    detail::Registries::instance.natives.reset(klass);

    return klass;
  }

  inline Class anonymous_class()
  {
    VALUE klass = detail::protect(rb_class_new, rb_cObject);
    VALUE singleton = detail::protect(rb_singleton_class, klass);

    // Ruby will reuse addresses previously assigned to other modules
    // that have subsequently been garbage collected
    detail::Registries::instance.natives.reset(klass);
    detail::Registries::instance.natives.reset(singleton);

    return klass;
  }
}

namespace Rice::detail
{
  template<>
  class To_Ruby<Class>
  {
  public:
    static VALUE convert(Class const& x)
    {
      return x.value();
    }
  };

  template<>
  class From_Ruby<Class>
  {
  public:
    Class convert(VALUE value)
    {
      return Class(value);
    }
  };
}

// =========   Struct.hpp   =========

namespace Rice
{
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
  class Struct : public Class
  {
  public:
    //! Define a new Struct member.
    /*! Defines a new member of the Struct.  Must be called before the
     *  Struct is initialized.
     *  \return *this
     */
    Struct& define_member(Identifier name);

    //! Initialize the Struct class.
    /*! Must be called after all Struct members have been defined.
     *  \param module the module under which to define the Struct.
     *  \param name the name of the Class at the ruby level.
     */
    Struct& initialize(Module module, Identifier name);

    //! Get the members in Struct.
    Array members() const;

    class Instance;
    friend class Instance;
    //friend Struct Rice::define_struct();

    //! Create a new instance of the Struct
    /*! \param args the arguments to the constructor.
     *  \return a new Struct::Instance
     */
    Instance new_instance(Array args = Array()) const;

  private:
    std::vector<Symbol> members_;
  };

  //! An instance of a Struct
  //! \sa Struct
  class Struct::Instance : public Builtin_Object<T_STRUCT>
  {
  public:
    //! Create a new Instance of a Struct.
    /*! \param type the Struct type to create.
     *  \param args the initial values for the objects of the instance.
     */
    Instance(Struct const& type,Array args = Array());

    //! Encapsulate an existing Struct instance.
    /*! \param type the Struct type to encapsulate.
     *  \param s the instance to encapsulate.
     */
    Instance(Struct const& type, Object s);

    //! Get a member, given its offset.
    /*! \param index the (integral) index into the Struct's internal
     *  array or its name (an Identifier or char const *)
     *  \return the member.
     */
    template<typename T>
    Object operator[](T index);

  private:
    Struct type_;
  };

  //! Define a new Struct
  Struct define_struct();

} // namespace Rice


// =========   Struct.ipp   =========

namespace Rice
{
  inline Struct& Struct::initialize(Module module, Identifier name)
  {
    Class struct_class(rb_cStruct);

    Object type = struct_class.vcall("new", this->members());

    set_value(type);
    module.const_set(name, type);

    return *this;
  }

  inline Struct& Struct::define_member(Identifier name)
  {
    if (value() != rb_cObject)
    {
      throw std::runtime_error("struct is already initialized");
    }

    members_.push_back(name.to_sym());

    return *this;
  }

  inline Array Struct::members() const
  {
    if (value() == rb_cObject)
    {
      // Struct is not yet defined
      return Array(members_.begin(), members_.end());
    }
    else
    {
      // Struct is defined, call Ruby API
      return rb_struct_s_members(this->value());
    }
  }

  inline Struct::Instance Struct::new_instance(Array args) const
  {
    Object instance = const_cast<Struct*>(this)->vcall("new", args);
    return Instance(*this, instance);
  }

  inline Struct::Instance::Instance(Struct const& type, Array args) :
    Builtin_Object<T_STRUCT>(type.new_instance(args)), type_(type)
  {
  }

  inline Struct::Instance::Instance(Struct const& type, Object s) :
    Builtin_Object<T_STRUCT>(s), type_(type)
  {
  }

  inline Struct define_struct()
  {
    return Struct();
  }

  template<typename T>
  inline Object Struct::Instance::operator[](T index)
  {
    return rb_struct_aref(value(), ULONG2NUM(index));
  }

  template<>
  inline Object Struct::Instance::operator[]<Identifier>(Identifier member)
  {
    return rb_struct_aref(value(), Symbol(member));
  }

  template<>
  inline Object Struct::Instance::operator[]<char const*>(char const* name)
  {
    return (*this)[Identifier(name)];
  }
}

namespace Rice::detail
{
  template<>
  struct Type<Struct>
  {
    static bool verify()
    {
      return true;
    }
  };
}

// =========   Address_Registration_Guard.hpp   =========

namespace Rice
{
  //! A guard to register a given address with the GC.
  /*! Calls rb_gc_register_address upon construction and
   *  rb_gc_unregister_address upon destruction.
   *  For example:
   *  \code
   *    Class Foo
   *    {
   *    public:
   *      Foo()
   *        : string_(rb_str_new2())
   *        , guard_(&string_);
   *
   *    private:
   *      VALUE string_;
   *      Address_Registration_Guard guard_;
   *    };
   *  \endcode
   */
  class Address_Registration_Guard
  {
  public:
    //! Register an address with the GC.
    /*  \param address The address to register with the GC.  The address
     *  must point to a valid ruby object (RObject).
     */
    Address_Registration_Guard(VALUE* address);

    //! Register an Object with the GC.
    /*! \param object The Object to register with the GC.  The object must
     *  not be destroyed before the Address_Registration_Guard is
     *  destroyed.
     */
    Address_Registration_Guard(Object* object);

    //! Unregister an address/Object with the GC.
    /*! Destruct an Address_Registration_Guard.  The address registered
     *  with the Address_Registration_Guard when it was constructed will
     *  be unregistered from the GC.
     */
    ~Address_Registration_Guard();

    // Disable copying
    Address_Registration_Guard(Address_Registration_Guard const& other) = delete;
    Address_Registration_Guard& operator=(Address_Registration_Guard const& other) = delete;

    // Enable moving
    Address_Registration_Guard(Address_Registration_Guard&& other);
    Address_Registration_Guard& operator=(Address_Registration_Guard&& other);

    //! Get the address that is registered with the GC.
    VALUE* address() const;

    /** Called during Ruby's exit process since we should not call
     * rb_gc unregister_address there
     */
    static void disable();

  private:
    inline static bool enabled = true;
    inline static bool exit_handler_registered = false;
    static void registerExitHandler();

  private:
    void registerAddress() const;
    void unregisterAddress();

    VALUE* address_ = nullptr;
  };
} // namespace Rice


// =========   Address_Registration_Guard.ipp   =========
namespace Rice
{
  inline Address_Registration_Guard::Address_Registration_Guard(VALUE* address) : address_(address)
  {
    registerExitHandler();
    registerAddress();
  }

  inline Address_Registration_Guard::Address_Registration_Guard(Object* object)
    : address_(const_cast<VALUE*>(&object->value()))
  {
    registerExitHandler();
    registerAddress();
  }

  inline Address_Registration_Guard::~Address_Registration_Guard()
  {
    unregisterAddress();
  }

  inline Address_Registration_Guard::Address_Registration_Guard(Address_Registration_Guard&& other)
  {
    // We don't use the constructor because we don't want to double register this address
    address_ = other.address_;
    other.address_ = nullptr;
  }

  inline Address_Registration_Guard& Address_Registration_Guard::operator=(Address_Registration_Guard&& other)
  {
    this->unregisterAddress();

    this->address_ = other.address_;
    other.address_ = nullptr;
    return *this;
  }

  inline void Address_Registration_Guard::registerAddress() const
  {
    if (enabled)
    {
      detail::protect(rb_gc_register_address, address_);
    }
  }

  inline void Address_Registration_Guard::unregisterAddress()
  {
    if (enabled && address_)
    {
      detail::protect(rb_gc_unregister_address, address_);
    }

    address_ = nullptr;
  }

  inline VALUE* Address_Registration_Guard::address() const
  {
    return address_;
  }

  static void disable_all_guards(VALUE)
  {
    Address_Registration_Guard::disable();
  }

  inline void Address_Registration_Guard::registerExitHandler()
  {
    if (exit_handler_registered)
    {
      return;
    }

    detail::protect(rb_set_end_proc, &disable_all_guards, Qnil);
    exit_handler_registered = true;
  }

  inline void Address_Registration_Guard::disable()
  {
    enabled = false;
  }
} // Rice
// =========   global_function.hpp   =========

namespace Rice
{
   //! Define an global function
   /*! The method's implementation can be any function or static member
    *  function.  A wrapper will be generated which will convert the arguments
    *  from ruby types to C++ types before calling the function.  The return
    *  value will be converted back to ruby.
    *  \param name the name of the method
    *  \param func the implementation of the function, either a function
    *  pointer or a member function pointer.
    *  \param args a list of Arg instance used to define default parameters (optional)
    *  \return *this
    */
  template<typename Function_T, typename...Arg_Ts>
  void define_global_function(char const * name, Function_T&& func, Arg_Ts const& ...args);
} // Rice


// =========   global_function.ipp   =========

template<typename Function_T, typename...Arg_Ts>
void Rice::define_global_function(char const * name, Function_T&& func, Arg_Ts const& ...args)
{
  Module(rb_mKernel).define_module_function(name, std::forward<Function_T>(func), args...);
}
// Code involved in creating custom DataTypes (ie, Ruby classes that wrap C++ classes)

// =========   ruby_mark.hpp   =========

//! Default function to call to mark a data object.
/*! This function can be specialized for a particular type to override
 *  the default behavior (which is to not mark any additional objects).
 */
namespace Rice
{
  template<typename T>
  void ruby_mark(T* data)
  {
  }
}


// =========   default_allocation_func.hpp   =========

namespace Rice::detail
{
  //! A default implementation of an allocate_func.  This function does no
  //! actual allocation; the initialize_func can later do the real
  //! allocation with: DATA_PTR(self) = new Type(arg1, arg2, ...)
  template<typename T>
  VALUE default_allocation_func(VALUE klass);
}

// =========   Director.hpp   =========

namespace Rice
{
  /**
   * A Director works exactly as a SWIG %director works (thus the name).
   * You use this class to help build proxy classes so that polymorphism
   * works from C++ into Ruby. See the main README for how this class works.
   */
  class Director
  {
    public:
      //! Construct new Director. Needs the Ruby object so that the
      //  proxy class can call methods on that object.
      Director(Object self) : self_(self)
      {
      }

      virtual ~Director() = default;

      //! Raise a ruby exception when a call comes through for a pure virtual method
      /*! If a Ruby script calls 'super' on a method that's otherwise a pure virtual
       *  method, use this method to throw an exception in this case.
       */
      void raisePureVirtual() const
      {
        rb_raise(rb_eNotImpError, "Cannot call super() into a pure-virtual C++ method");
      }

      //! Get the Ruby object linked to this C++ instance
      Object getSelf() const { return self_; }

    private:

      // Save the Ruby object related to the instance of this class
      Object self_;

  };
}

// =========   Data_Type.hpp   =========

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
    static_assert(std::is_same_v<detail::intrinsic_type<T>, T>);

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
    virtual Data_Type & operator=(Module const & klass);

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
    static bool is_defined(Object parent, const std::string& name);

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
    Data_Type<T>& define_attr(std::string name, Attribute_T attribute, AttrAccess access = AttrAccess::ReadWrite);
  
    template <typename Attribute_T>
    Data_Type<T>& define_singleton_attr(std::string name, Attribute_T attribute, AttrAccess access = AttrAccess::ReadWrite);

    // Include these methods to call methods from Module but return
// an instance of the current classes. This is an alternative to
// using CRTP.


//! Include a module.
/*! \param inc the module to be included.
*  \return *this
*/
inline auto& include_module(Module const& inc)
{
  detail::protect(rb_include_module, this->value(), inc.value());
  return *this;
}

//! Define an instance method.
/*! The method's implementation can be a member function, plain function
 *  or lambda. The easiest case is a member function where the Ruby
 *  method maps one-to-one to the C++ method. In the case of a
 *  plain function or lambda, the first argument must be SELF - ie,
 *  the current object. If the type of the first argument is VALUE, then
 *  the current Ruby object is passed. If the type is a C++ class,
 *  then the C++ object is passed. If you don't want to include the
 *  SELF argument see define_function.
 *  Rice will automatically convert method parameters from Ruby to C++ and
 *  convert the return value from C++ to Ruby.
 *  \param name the name of the method
 *  \param func the implementation of the function, either a function
 *  pointer or a member function pointer.
 *  \param args a list of Arg instance used to define default parameters.
 *  \return *this
 */
template<typename Function_T, typename...Arg_Ts>
inline auto& define_method(std::string name, Function_T&& func, const Arg_Ts&...args)
{
  MethodInfo* methodInfo = new MethodInfo(detail::method_traits<Function_T, true>::arity, args...);
  this->wrap_native_call<true>(this->value(), name, std::forward<Function_T>(func), methodInfo);
  return *this;
}

//! Define an instance function.
/*! The function implementation is a plain function or a static
 *  member function.
 *  Rice will automatically convert method method from Ruby to C++ and
 *  then convert the return value from C++ to Ruby.
 *  \param name the name of the method
 *  \param func the implementation of the function, either a function
 *  pointer or a member function pointer.
 *  \param args a list of Arg instance used to define default parameters (optional)
 *  \return *this
 */
template<typename Function_T, typename...Arg_Ts>
inline auto& define_function(std::string name, Function_T&& func, const Arg_Ts&...args)
{
  MethodInfo* methodInfo = new MethodInfo(detail::method_traits<Function_T, false>::arity, args...);
  this->wrap_native_call<false>(this->value(), name, std::forward<Function_T>(func), methodInfo);
  return *this;
}

//! Define a singleton method.
/*! The method's implementation can be a static member function,
*   plain function or lambda. In all cases the first argument
*   must be SELF - ie, the current object. If it is specified as a VALUE, then
 *  the current Ruby object is passed. If it is specified as a C++ class,
 *  then the C++ object is passed. If you don't want to include the
 *  SELF argument see define_singleton_function.
 *  Rice will automatically convert method method from Ruby to C++ and
 *  then convert the return value from C++ to Ruby.
 *  \param name the name of the method
 *  \param func the implementation of the function, either a function
 *  pointer or a member function pointer.
 *  \param args a list of Arg instance used to define default parameters (optional)
 *  \return *this
 */
template<typename Function_T, typename...Arg_Ts>
inline auto& define_singleton_method(std::string name, Function_T&& func, const Arg_Ts&...args)
{
  MethodInfo* methodInfo = new MethodInfo(detail::method_traits<Function_T, true>::arity, args...);
  this->wrap_native_call<true>(rb_singleton_class(*this), name, std::forward<Function_T>(func), methodInfo);
  return *this;
}

//! Define a singleton method.
/*! The method's implementation can be a static member function, plain
 *  function or lambda.
 . A wrapper will be
 * generated which will convert the method
 *  from ruby types to C++ types before calling the function.  The return
 *  value will be converted back to ruby.
 *  \param name the name of the method
 *  \param func the implementation of the function, either a function
 *  pointer or a member function pointer.
 *  \param args a list of Arg instance used to define default parameters (optional)
 *  \return *this
 */
template<typename Function_T, typename...Arg_Ts>
inline auto& define_singleton_function(std::string name, Function_T&& func, const Arg_Ts& ...args)
{
  MethodInfo* methodInfo = new MethodInfo(detail::method_traits<Function_T, false>::arity, args...);
  this->wrap_native_call<false>(rb_singleton_class(*this), name, std::forward<Function_T>(func), methodInfo);
  return *this;
}

//! Define a module function.
/*! A module function is a function that can be accessed either as a
 *  singleton method or as an instance method. It wrap a plain
 *  function, static member function or lambda.
 *  Rice will automatically convert method method from Ruby to C++ and
 *  then convert the return value from C++ to Ruby.
 *  \param name the name of the method
 *  \param func the implementation of the function, either a function
 *  pointer or a member function pointer.
 *  \param args a list of Arg instance used to define default parameters (optional)
 *  \return *this
 */
template<typename Function_T, typename...Arg_Ts>
inline auto& define_module_function(std::string name, Function_T&& func, const Arg_Ts& ...args)
{
  if (this->rb_type() != T_MODULE)
  {
    throw std::runtime_error("can only define module functions for modules");
  }

  define_function(name, std::forward<Function_T>(func), args...);
  define_singleton_function(name, std::forward<Function_T>(func), args...);
  return *this;
}

//! Define a constant
template<typename Constant_T>
inline auto& define_constant(std::string name, Constant_T value)
{
  using Base_T = detail::remove_cv_recursive_t<Constant_T>;
  detail::protect(rb_define_const, this->value(), name.c_str(), detail::To_Ruby<Base_T>().convert(value));
  return *this;
}
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

    template<bool IsMethod, typename Function_T>
    void wrap_native_call(VALUE klass, std::string name, Function_T&& function, MethodInfo* methodInfo);

  private:
    template<typename T_>
    friend class Data_Type;

    static inline VALUE klass_ = Qnil;

    // Typed Data support
    static inline rb_data_type_t* rb_data_type_ = nullptr;

    // Track unbound instances (ie, declared variables of type Data_Type<T>
    // before define_class is called)
    static inline std::set<Data_Type<T>*>unbound_instances_;
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


// =========   Data_Type.ipp   =========
#include <stdexcept>

namespace Rice
{
  template<typename T>
  inline void ruby_mark_internal(detail::Wrapper* wrapper)
  {
    // Tell the wrapper to mark the objects its keeping alive
    wrapper->ruby_mark();

    // Get the underlying data and call custom mark function (if any)
    T* data = static_cast<T*>(wrapper->get());
    ruby_mark<T>(data);
  }

  template<typename T>
  inline void ruby_free_internal(detail::Wrapper* wrapper)
  {
    delete wrapper;
  }

  template<typename T>
  inline size_t ruby_size_internal(const T* data)
  {
    return sizeof(T);
  }

  template<>
  inline size_t ruby_size_internal(const void* data)
  {
    return sizeof(void*);
  }

  template<typename T>
  template <typename Base_T>
  inline Data_Type<T> Data_Type<T>::bind(const Module& klass)
  {
    if (is_bound())
    {
      std::string message = "Type " + detail::typeName(typeid(T)) + " is already bound to a different type";
      throw std::runtime_error(message.c_str());
    }

    klass_ = klass;

    rb_data_type_ = new rb_data_type_t();
    rb_data_type_->wrap_struct_name = strdup(Rice::detail::protect(rb_class2name, klass_));
    rb_data_type_->function.dmark = reinterpret_cast<void(*)(void*)>(&Rice::ruby_mark_internal<T>);
    rb_data_type_->function.dfree = reinterpret_cast<void(*)(void*)>(&Rice::ruby_free_internal<T>);
    rb_data_type_->function.dsize = reinterpret_cast<size_t(*)(const void*)>(&Rice::ruby_size_internal<T>);
    rb_data_type_->data = nullptr;
    rb_data_type_->flags = RUBY_TYPED_FREE_IMMEDIATELY;

    if constexpr (!std::is_void_v<Base_T>)
    {
      rb_data_type_->parent = Data_Type<Base_T>::ruby_data_type();
    }

    // Now register with the type registry
    detail::Registries::instance.types.add<T>(klass_, rb_data_type_);

    auto iter = Data_Type<T>::unbound_instances_.begin();
    while (iter != Data_Type<T>::unbound_instances_.end())
    { 
      (*iter)->set_value(klass);
      iter = Data_Type<T>::unbound_instances_.erase(iter);
    }

    return Data_Type<T>();
  }

  template<typename T>
  inline void Data_Type<T>::unbind()
  {
    detail::Registries::instance.types.remove<T>();

    if (klass_ != Qnil)
    {
      klass_ = Qnil;
    }

    // There could be objects floating around using the existing rb_type so 
    // do not delete it. This is of course a memory leak.
    rb_data_type_ = nullptr;
  }

  template<typename T>
  inline Data_Type<T>::Data_Type() : Class(klass_ == Qnil ? rb_cObject : klass_)
  {
    if (!is_bound())
    {
      this->unbound_instances_.insert(this);
    }
  }

  template<typename T>
  inline Data_Type<T>::Data_Type(Module const& klass) : Class(klass)
  {
    this->bind(klass);
  }

  template<typename T>
  inline rb_data_type_t* Data_Type<T>::ruby_data_type()
  {
    check_is_bound();
    return rb_data_type_;
  }

  template<typename T>
  inline Class Data_Type<T>::klass()
  {
    check_is_bound();
    return klass_;
  }

  template<typename T>
  inline Data_Type<T>& Data_Type<T>::operator=(Module const& klass)
  {
    this->bind(klass);
    return *this;
  }

  template<typename T>
  template<typename Constructor_T, typename...Rice_Arg_Ts>
  inline Data_Type<T>& Data_Type<T>::define_constructor(Constructor_T constructor, Rice_Arg_Ts const& ...args)
  {
    check_is_bound();

    // Define a Ruby allocator which creates the Ruby object
    detail::protect(rb_define_alloc_func, static_cast<VALUE>(*this), detail::default_allocation_func<T>);

    // We can't do anything with move constructors so blow up
    static_assert(!Constructor_T::isMoveConstructor(), "Rice does not support move constructors");

    // If this is a copy constructor then use it to support Ruby's Object#dup and Object#clone methods.
    // Otherwise if a user calls #dup or #clone an error will occur because the newly cloned Ruby
    // object will have a NULL ptr because the C++ object is never copied. This also prevents having
    // very unlike Ruby code of:
    // 
    //    my_object_copy = MyObject.new(my_ojbect_original).

    if constexpr (Constructor_T::isCopyConstructor())
    {
      // Define initialize_copy that will copy the C++ object
      this->define_method("initialize_copy", &Constructor_T::initialize_copy, args...);
    }
    else
    {
      // Define an initialize function that will create the C++ object
      this->define_method("initialize", &Constructor_T::initialize, args...);
    }

    return *this;
  }

  template<typename T>
  template<typename Function_T>
  inline Data_Type<T>& Data_Type<T>::define(Function_T func)
  {
    // The passed in this pointer is an RValue, so we need to keep it alive by
    // assigning it to a const lvalue
    const auto& dummy = *this;
    func(*this);
    return *this;
  }

  template<typename T>
  template<typename Director_T>
  inline Data_Type<T>& Data_Type<T>::define_director()
  {
    if (!detail::Registries::instance.types.isDefined<Director_T>())
    {
      Data_Type<Director_T>::bind(*this);
    }

    // TODO - hack to fake Ruby into thinking that a Director is
    // the same as the base data type
    Data_Type<Director_T>::rb_data_type_ = Data_Type<T>::rb_data_type_;
    return *this;
  }

  template<typename T>
  inline bool Data_Type<T>::is_bound()
  {
    return klass_ != Qnil;
  }

  template<typename T>
  inline bool Data_Type<T>::is_descendant(VALUE value)
  {
    check_is_bound();
    return detail::protect(rb_obj_is_kind_of, value, klass_) == Qtrue;
  }

  template<typename T>
  inline void Data_Type<T>::check_is_bound()
  {
    if (!is_bound())
    {
      std::string message = "Type is not defined with Rice: " + detail::typeName(typeid(T));
      throw std::invalid_argument(message.c_str());
    }
  }

  template<typename T>
  inline bool Data_Type<T>::is_defined(Object parent, const std::string& name)
  {
    // Is the class already defined?
    if (detail::Registries::instance.types.isDefined<T>())
    {
      Data_Type<T> result = Data_Type<T>();

      // If this redefinition is a different name then create a new constant
      if (result.name() != name)
      {
        detail::protect(rb_define_const, parent, name.c_str(), result.klass());
      }

      return true;
    }
    return false;
  }
  
  template<typename Base_T>
  inline Class get_superklass()
  {
    Class result;

    if constexpr (std::is_void_v<Base_T>)
    {
      result = rb_cObject;
    }
    else
    {
      // This gives a chance for to auto-register classes such as std::exception
      detail::verifyType<Base_T>();
      result = Data_Type<Base_T>::klass();
    }

    return result;
  }

  template<typename T, typename Base_T>
  inline Data_Type<T> define_class_under(Object parent, Identifier id, Class superKlass)
  {
    if (Rice::Data_Type<T>::is_defined(parent, id.str()))
    {
      return Data_Type<T>();
    }

    Class klass = define_class_under(parent, id, superKlass);
    klass.undef_creation_funcs();
    return Data_Type<T>::template bind<Base_T>(klass);
  }

  template<typename T, typename Base_T>
  inline Data_Type<T> define_class_under(Object parent, char const* name)
  {
    Identifier id(name);
    Class superKlass = get_superklass<Base_T>();
    return define_class_under<T, Base_T>(parent, id, superKlass);
  }
  
  template<typename T, typename Base_T>
  inline Data_Type<T> define_class(char const* name)
  {
    std::string klassName(name);

    if (Rice::Data_Type<T>::is_defined(rb_cObject, klassName))
    {
      return Data_Type<T>();
    }

    Class superKlass = get_superklass<Base_T>();
    Class klass = define_class(name, superKlass);
    klass.undef_creation_funcs();
    return Data_Type<T>::template bind<Base_T>(klass);
  }

  template<typename T>
  template<typename Iterator_Func_T>
  inline Data_Type<T>& Data_Type<T>::define_iterator(Iterator_Func_T begin, Iterator_Func_T end, std::string name)
  {
    // Define a NativeIterator to bridge Ruby to C++
    detail::NativeIterator<T, Iterator_Func_T>::define(Data_Type<T>::klass(), name, begin, end);

    // Include enumerable support
    this->klass().include_module(rb_mEnumerable);

    return *this;
  }

  template <typename T>
  template <typename Attribute_T>
  inline Data_Type<T>& Data_Type<T>::define_attr(std::string name, Attribute_T attribute, AttrAccess access)
  {
    // Make sure the Attribute type has been previously seen by Rice
    detail::verifyType<typename detail::attribute_traits<Attribute_T>::attr_type>();

    // Define native attribute getter
    if (access == AttrAccess::ReadWrite || access == AttrAccess::Read)
      detail::NativeAttributeGet<Attribute_T>::define(klass_, name, std::forward<Attribute_T>(attribute));

    using Attr_T = typename detail::NativeAttributeSet<Attribute_T>::Attr_T;
    if constexpr (!std::is_const_v<Attr_T> &&
                  (std::is_fundamental_v<Attr_T> || std::is_assignable_v<Attr_T, Attr_T>))
        {
      // Define native attribute setter
      if (access == AttrAccess::ReadWrite || access == AttrAccess::Write)
        detail::NativeAttributeSet<Attribute_T>::define(klass_, name, std::forward<Attribute_T>(attribute));
    }

    return *this;
  }

  template <typename T>
  template <typename Attribute_T>
  inline Data_Type<T>& Data_Type<T>::define_singleton_attr(std::string name, Attribute_T attribute, AttrAccess access)
  {
    // Make sure the Attribute type has been previously seen by Rice
    detail::verifyType<typename detail::attribute_traits<Attribute_T>::attr_type>();

    // Define native attribute
    VALUE singleton = detail::protect(rb_singleton_class, this->value());

    // Define native attribute getter
    if (access == AttrAccess::ReadWrite || access == AttrAccess::Read)
      detail::NativeAttributeGet<Attribute_T>::define(singleton, name, std::forward<Attribute_T>(attribute));

    // Define native attribute setter
    if (access == AttrAccess::ReadWrite || access == AttrAccess::Write)
      detail::NativeAttributeSet<Attribute_T>::define(singleton, name, std::forward<Attribute_T>(attribute));

    return *this;
  }

  template <typename T>
  template<bool IsMethod, typename Function_T>
  inline void Data_Type<T>::wrap_native_call(VALUE klass, std::string name, Function_T&& function, MethodInfo* methodInfo)
  {
    // Make sure the return type and arguments have been previously seen by Rice
    using traits = detail::method_traits<Function_T, IsMethod>;
    detail::verifyType<typename traits::Return_T>();
    detail::verifyTypes<typename traits::Arg_Ts>();

    // Define a NativeFunction to bridge Ruby to C++
    detail::NativeFunction<T, Function_T, IsMethod>::define(klass, name, std::forward<Function_T>(function), methodInfo);
  }
}

// =========   default_allocation_func.ipp   =========
namespace Rice::detail
{
  template<typename T>
  VALUE default_allocation_func(VALUE klass)
  {
    // Create a new Ruby object but since we do not yet have a C++ object
    // just pass a nullptr. It will be set via the Constructor call
    return TypedData_Wrap_Struct(klass, Data_Type<T>::ruby_data_type(), nullptr);
  }
}
// =========   Constructor.hpp   =========

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
  template<typename T, typename...Arg_Ts>
  class Constructor;
}

// =========   Constructor.ipp   =========
namespace Rice
{
  template<typename T, typename...Arg_Ts>
  class Constructor
  {
  public:
    static constexpr std::size_t arity = sizeof...(Arg_Ts);

    static constexpr bool isCopyConstructor()
    {
      if constexpr (arity == 1)
      {
        using Arg_Types = std::tuple<Arg_Ts...>;
        using First_Arg_T = std::tuple_element_t<0, Arg_Types>;
        return (arity == 1 &&
                std::is_lvalue_reference_v<First_Arg_T> &&
                std::is_same_v<T, detail::intrinsic_type<First_Arg_T>>);
      }
      else
      {
        return false;
      }
    }

    static constexpr bool isMoveConstructor()
    {
      if constexpr (arity == 1)
      {
        using Arg_Types = std::tuple<Arg_Ts...>;
        using First_Arg_T = std::tuple_element_t<0, Arg_Types>;
        return (arity == 1 &&
          std::is_rvalue_reference_v<First_Arg_T> &&
          std::is_same_v<T, detail::intrinsic_type<First_Arg_T>>);
      }
      else
      {
        return false;
      }
    }

    static void initialize(VALUE self, Arg_Ts...args)
    {
      // Call C++ constructor
      T* data = new T(args...);
      detail::replace<T>(self, Data_Type<T>::ruby_data_type(), data, true);
    }

    static void initialize_copy(VALUE self, const T& other)
    {
      // Call C++ copy constructor
      T* data = new T(other);
      detail::replace<T>(self, Data_Type<T>::ruby_data_type(), data, true);
    }

  };

  //! Special-case Constructor used when defining Directors.
  template<typename T, typename...Arg_Ts>
  class Constructor<T, Object, Arg_Ts...>
  {
    public:
      static constexpr bool isCopyConstructor()
      {
        return false;
      }

      static constexpr bool isMoveConstructor()
      {
        return false;
      }

      static void initialize(Object self, Arg_Ts...args)
      {
        // Call C++ constructor
        T* data = new T(self, args...);
        detail::replace<T>(self.value(), Data_Type<T>::ruby_data_type(), data, true);
      }
  };
}
// =========   Data_Object.hpp   =========

/*! \file
 *  \brief Provides a helper class for wrapping and unwrapping C++
 *  objects as Ruby objects.
 */

namespace Rice
{
  //! A smartpointer-like wrapper for Ruby data objects.
  /*! A data object is a ruby object of type T_DATA, which is usually
   *  created by using the Data_Wrap_Struct or Data_Make_Struct macro.
   *  This class wraps creation of the data structure, providing a
   *  type-safe object-oriented interface to the underlying C interface.
   *  This class works in conjunction with the Data_Type class to ensure
   *  type safety.
   *
   *  Example:
   *  \code
   *    class Foo { };
   *    ...
   *    Data_Type<Foo> rb_cFoo = define_class("Foo");
   *    ...
   *    // Wrap:
   *    Data_Object<Foo> foo1(new Foo);
   *
   *    // Get value to return:
   *    VALUE v = foo1.value()
   *
   *    // Unwrap:
   *    Data_Object<Foo> foo2(v, rb_cFoo);
   *  \endcode
   */
  template<typename T>
  class Data_Object : public Object
  {
    static_assert(!std::is_pointer_v<T>);
    static_assert(!std::is_reference_v<T>);
    static_assert(!std::is_const_v<T>);
    static_assert(!std::is_volatile_v<T>);
    static_assert(!std::is_void_v<T>);

  public:
    static T* from_ruby(VALUE value, bool transferOwnership = false);

  public:
    //! Wrap a C++ object.
    /*! This constructor is analogous to calling Data_Wrap_Struct.  Be
     *  careful not to call this function more than once for the same
     *  pointer (in general, it should only be called for newly
     *  constructed objects that need to be managed by Ruby's garbage
     *  collector).
     *  \param obj the object to wrap.
     *  \param isOwner Should the Data_Object take ownership of the object?
     *  \param klass the Ruby class to use for the newly created Ruby
     *  object.
     */
    Data_Object(T* obj, bool isOwner = false, Class klass = Data_Type<T>::klass());
    Data_Object(T& obj, bool isOwner = false, Class klass = Data_Type<T>::klass());
    Data_Object(const T& obj, bool isOwner = false, Class klass = Data_Type<T>::klass());

    //! Unwrap a Ruby object.
    /*! This constructor is analogous to calling Data_Get_Struct.  Uses
     *  Data_Type<T>::klass as the class of the object.
     *  \param value the Ruby object to unwrap.
     */
    Data_Object(Object value);

    T& operator*() const; //!< Return a reference to obj_
    T* operator->() const; //!< Return a pointer to obj_
    T* get() const;        //!< Return a pointer to obj_

  private:
    static void check_ruby_type(VALUE value);
  };
} // namespace Rice


// =========   Data_Object.ipp   =========

#include <algorithm>

namespace Rice
{
  template <typename T>
  Exception create_type_exception(VALUE value)
  {
    return Exception(rb_eTypeError, "Wrong argument type. Expected: %s. Received: %s.",
      detail::protect(rb_class2name, Data_Type<T>::klass().value()),
      detail::protect(rb_obj_classname, value));
  }

  template<typename T>
  inline Data_Object<T>::Data_Object(T& data, bool isOwner, Class klass)
  {
    VALUE value = detail::wrap(klass, Data_Type<T>::ruby_data_type(), data, isOwner);
    this->set_value(value);
  }

  template<typename T>
  inline Data_Object<T>::Data_Object(const T& data, bool isOwner, Class klass)
  {
    VALUE value = detail::wrap(klass, Data_Type<T>::ruby_data_type(), data, isOwner);
    this->set_value(value);
  }

  template<typename T>
  inline Data_Object<T>::Data_Object(T* data, bool isOwner, Class klass)
  {
    VALUE value = detail::wrap(klass, Data_Type<T>::ruby_data_type(), data, isOwner);
    this->set_value(value);
  }

  template<typename T>
  inline Data_Object<T>::Data_Object(Object value) : Object(value)
  {
    check_ruby_type(value);
  }

  template<typename T>
  inline void Data_Object<T>::check_ruby_type(VALUE value)
  {
    if (rb_obj_is_kind_of(value, Data_Type<T>::klass()) == Qfalse)
    {
      throw create_type_exception<T>(value);
    }
  }

  template<typename T>
  inline T& Data_Object<T>::operator*() const
  {
    return *this->get();
  }

  template<typename T>
  inline T* Data_Object<T>::operator->() const
  {
    return this->get();
  }

  template<typename T>
  inline T* Data_Object<T>::get() const
  {
    if (this->value() == Qnil)
    {
      return nullptr;
    }
    else
    {
      return detail::unwrap<T>(this->value(), Data_Type<T>::ruby_data_type(), false);
    }
  }

  template<typename T>
  inline T* Data_Object<T>::from_ruby(VALUE value, bool transferOwnership)
  {
    if (Data_Type<T>::is_descendant(value))
    {
      return detail::unwrap<T>(value, Data_Type<T>::ruby_data_type(), transferOwnership);
    }
    else
    {
      throw create_type_exception<T>(value);
    }
  }
}

namespace Rice::detail
{
  template<typename T>
  class To_Ruby
  {
  public:
    VALUE convert(T& data)
    {
      // Get the ruby typeinfo
      std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::Registries::instance.types.figureType<T>(data);

      // We always take ownership of data passed by value (yes the parameter is T& but the template
      // matched <typename T> thus we have to tell wrap to copy the reference we are sending to it
      return detail::wrap(rubyTypeInfo.first, rubyTypeInfo.second, data, true);
    }

    VALUE convert(const T& data)
    {
      // Get the ruby typeinfo
      std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::Registries::instance.types.figureType<T>(data);

      // We always take ownership of data passed by value (yes the parameter is T& but the template
      // matched <typename T> thus we have to tell wrap to copy the reference we are sending to it
      return detail::wrap(rubyTypeInfo.first, rubyTypeInfo.second, data, true);
    }
  };

  template <typename T>
  class To_Ruby<T&>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Return * returnInfo) : returnInfo_(returnInfo)
    {
    }

    VALUE convert(T& data)
    {
      // Note that T could be a pointer or reference to a base class while data is in fact a
      // child class. Lookup the correct type so we return an instance of the correct Ruby class
      std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::Registries::instance.types.figureType<T>(data);

      bool isOwner = this->returnInfo_ && this->returnInfo_->isOwner();
      return detail::wrap(rubyTypeInfo.first, rubyTypeInfo.second, data, isOwner);
    }

    VALUE convert(const T& data)
    {
      // Note that T could be a pointer or reference to a base class while data is in fact a
      // child class. Lookup the correct type so we return an instance of the correct Ruby class
      std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::Registries::instance.types.figureType<T>(data);

      bool isOwner = this->returnInfo_ && this->returnInfo_->isOwner();
      return detail::wrap(rubyTypeInfo.first, rubyTypeInfo.second, data, isOwner);
    }

  private:
    Return* returnInfo_ = nullptr;
  };

  template <typename T>
  class To_Ruby<T*>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
    {
    }

    VALUE convert(T* data)
    {
      if (data)
      {
        // Note that T could be a pointer or reference to a base class while data is in fact a
        // child class. Lookup the correct type so we return an instance of the correct Ruby class
        std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::Registries::instance.types.figureType(*data);
        bool isOwner = this->returnInfo_ && this->returnInfo_->isOwner();
        return detail::wrap(rubyTypeInfo.first, rubyTypeInfo.second, data, isOwner);
      }
      else
      {
        return Qnil;
      }
    }

    VALUE convert(const T* data)
    {
      if (data)
      {
        // Note that T could be a pointer or reference to a base class while data is in fact a
        // child class. Lookup the correct type so we return an instance of the correct Ruby class
        std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::Registries::instance.types.figureType(*data);
        bool isOwner = this->returnInfo_ && this->returnInfo_->isOwner();
        return detail::wrap(rubyTypeInfo.first, rubyTypeInfo.second, data, isOwner);
      }
      else
      {
        return Qnil;
      }
    }

  private:
    Return* returnInfo_ = nullptr;
  };

  template <>
  class To_Ruby<void*>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
    {
    }

    VALUE convert(void* data)
    {
      if (data)
      {
        // Note that T could be a pointer or reference to a base class while data is in fact a
        // child class. Lookup the correct type so we return an instance of the correct Ruby class
        std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::Registries::instance.types.figureType(data);
        bool isOwner = this->returnInfo_ && this->returnInfo_->isOwner();
        return detail::wrap(rubyTypeInfo.first, rubyTypeInfo.second, data, isOwner);
      }
      else
      {
        return Qnil;
      }
    }

    VALUE convert(const void* data)
    {
      return convert((void*)data);
    }

  private:
    Return* returnInfo_ = nullptr;
  };

  template <typename T>
  class To_Ruby<T*&>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
    {
    }

    VALUE convert(T* data)
    {
      if (data)
      {
        // Note that T could be a pointer or reference to a base class while data is in fact a
        // child class. Lookup the correct type so we return an instance of the correct Ruby class
        std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::Registries::instance.types.figureType(*data);
        bool isOwner = this->returnInfo_ && this->returnInfo_->isOwner();
        return detail::wrap(rubyTypeInfo.first, rubyTypeInfo.second, data, isOwner);
      }
      else
      {
        return Qnil;
      }
    }

    VALUE convert(const T* data)
    {
      if (data)
      {
        // Note that T could be a pointer or reference to a base class while data is in fact a
        // child class. Lookup the correct type so we return an instance of the correct Ruby class
        std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::Registries::instance.types.figureType(*data);
        bool isOwner = this->returnInfo_ && this->returnInfo_->isOwner();
        return detail::wrap(rubyTypeInfo.first, rubyTypeInfo.second, data, isOwner);
      }
      else
      {
        return Qnil;
      }
    }

  private:
    Return* returnInfo_ = nullptr;
  };

  template <typename T>
  class To_Ruby<T**>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
    {
    }

    VALUE convert(T** data)
    {
      if (data)
      {
        // Note that T could be a pointer or reference to a base class while data is in fact a
        // child class. Lookup the correct type so we return an instance of the correct Ruby class
        std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::Registries::instance.types.figureType(**data);
        bool isOwner = this->returnInfo_ && this->returnInfo_->isOwner();
        return detail::wrap(rubyTypeInfo.first, rubyTypeInfo.second, data, isOwner);
      }
      else
      {
        return Qnil;
      }
    }

    VALUE convert(const T** data)
    {
      if (data)
      {
        // Note that T could be a pointer or reference to a base class while data is in fact a
        // child class. Lookup the correct type so we return an instance of the correct Ruby class
        std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::Registries::instance.types.figureType(*data);
        bool isOwner = this->returnInfo_ && this->returnInfo_->isOwner();
        return detail::wrap(rubyTypeInfo.first, rubyTypeInfo.second, data, isOwner);
      }
      else
      {
        return Qnil;
      }
    }

  private:
    Return* returnInfo_ = nullptr;
  };

  template<typename T>
  class To_Ruby<Data_Object<T>>
  {
  public:
    VALUE convert(const Object& x)
    {
      return x.value();
    }
  };

  template <typename T>
  class From_Ruby
  {
    static_assert(!std::is_fundamental_v<intrinsic_type<T>>,
                  "Data_Object cannot be used with fundamental types");
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg * arg) : arg_(arg)
    {
    }
    
    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
          return Data_Type<T>::is_descendant(value) ? Convertible::Exact : Convertible::None;
          break;
        default:
          return Convertible::None;
      }
    }

    T convert(VALUE value)
    {
      using Intrinsic_T = intrinsic_type<T>;

      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->template defaultValue<Intrinsic_T>();
      }
      else
      {
        return *Data_Object<Intrinsic_T>::from_ruby(value, this->arg_ && this->arg_->isTransfer());
      }
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<typename T>
  class From_Ruby<T&>
  {
    static_assert(!std::is_fundamental_v<intrinsic_type<T>>,
                  "Data_Object cannot be used with fundamental types");
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg * arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
          return Data_Type<T>::is_descendant(value) ? Convertible::Exact : Convertible::None;
          break;
        default:
          return Convertible::None;
      }
    }

    T& convert(VALUE value)
    {
      using Intrinsic_T = intrinsic_type<T>;

      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->template defaultValue<Intrinsic_T>();
      }
      else
      {
        return *Data_Object<Intrinsic_T>::from_ruby(value, this->arg_ && this->arg_->isTransfer());
      }
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<typename T>
  class From_Ruby<T&&>
  {
    static_assert(!std::is_fundamental_v<intrinsic_type<T>>,
                  "Data_Object cannot be used with fundamental types");
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg * arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
      case RUBY_T_DATA:
        return Data_Type<T>::is_descendant(value) ? Convertible::Exact : Convertible::None;
        break;
      default:
        return Convertible::None;
      }
    }

    T&& convert(VALUE value)
    {
      using Intrinsic_T = intrinsic_type<T>;

      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return std::move(this->arg_->template defaultValue<Intrinsic_T>());
      }
      else
      {
        return std::move(*Data_Object<Intrinsic_T>::from_ruby(value, this->arg_ && this->arg_->isTransfer()));
      }
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<typename T>
  class From_Ruby<T*>
  {
    static_assert(!std::is_fundamental_v<intrinsic_type<T>>,
                  "Data_Object cannot be used with fundamental types");
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
          return Data_Type<T>::is_descendant(value) ? Convertible::Exact : Convertible::None;
          break;
        case RUBY_T_NIL:
          return Convertible::Exact;
          break;
        default:
          return Convertible::None;
      }
    }

    T* convert(VALUE value)
    {
      using Intrinsic_T = intrinsic_type<T>;

      if (value == Qnil)
      {
        return nullptr;
      }
      else
      {
        return Data_Object<Intrinsic_T>::from_ruby(value, this->arg_ && this->arg_->isTransfer());
      }
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<typename T>
  class From_Ruby<T*&>
  {
    static_assert(!std::is_fundamental_v<intrinsic_type<T>>,
                  "Data_Object cannot be used with fundamental types");
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
          return Data_Type<T>::is_descendant(value) ? Convertible::Exact : Convertible::None;
          break;
        default:
          return Convertible::None;
      }
    }

    T* convert(VALUE value)
    {
      using Intrinsic_T = intrinsic_type<T>;

      if (value == Qnil)
      {
        return nullptr;
      }
      else
      {
        return Data_Object<Intrinsic_T>::from_ruby(value, this->arg_ && this->arg_->isTransfer());
      }
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<typename T>
  class From_Ruby<T**>
  {
    static_assert(!std::is_fundamental_v<intrinsic_type<T>>,
                  "Data_Object cannot be used with fundamental types");
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
      case RUBY_T_DATA:
        return Data_Type<T>::is_descendant(value) ? Convertible::Exact : Convertible::None;
        break;
      default:
        return Convertible::None;
      }
    }

    T** convert(VALUE value)
    {
      using Intrinsic_T = intrinsic_type<T>;

      if (value == Qnil)
      {
        return nullptr;
      }
      else
      {
        return detail::unwrap<Intrinsic_T*>(value, Data_Type<T>::ruby_data_type(), false);
      }
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<typename T>
  class From_Ruby<Data_Object<T>>
  {
    static_assert(!std::is_fundamental_v<intrinsic_type<T>>,
                  "Data_Object cannot be used with fundamental types");
  public:
    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
          return Data_Type<T>::is_descendant(value) ? Convertible::Exact : Convertible::None;
          break;
        default:
          return Convertible::None;
        }
    }

    static Data_Object<T> convert(VALUE value)
    {
      return Data_Object<T>(value);
    }
  };
}

// =========   Enum.hpp   =========

namespace Rice
{
  /*!
   *  \example enum/sample_enum.cpp
   */

   //! A wrapper for enumerated types.
   /*! Provides a simple type-safe wrapper for enumerated types.  At the
    *  ruby level, the class will have convenience methods for iterating
    *  over all the defined enum values, converting the values to strings,
    *  and more.
    *
    *  \param Enum_T the enumerated type
    *
    *  Example:
    *  \code
    *    enum Color { Red, Green, Blue };
    *    Enum<Color> rb_cColor = define_enum<Color>("Color")
    *      .define_value("Red", Red)
    *      .define_value("Green", Green)
    *      .define_value("Blue", Blue);
    *  \endcode
    */
  template<typename Enum_T>
  class Enum : public Data_Type<Enum_T>
  {
    using Underlying_T = std::underlying_type_t<Enum_T>;

  public:

    Enum() = default;

    //! Construct and initialize.
    Enum(char const* name, Module module = rb_cObject);

    //! Define a new enum value.
    /*! \param name the name of the enum value.
     *  \param value the value to associate with name.
     *  \return *this
     */
    Enum<Enum_T>& define_value(std::string name, Enum_T value);

    //! Maps an enum value to the correct Ruby object
    /*! \param klass The bound Ruby class
     *  \param enumValue The enum value
     *  \return Object - The Ruby wrapper */
    static Object from_enum(Class klass, Enum_T enumValue);

  private:
    void define_methods(Data_Type<Enum_T> klass);

    static inline std::map<Enum_T, std::string> valuesToNames_;
  };

  template<typename T>
  Enum<T> define_enum(char const* name);

  template<typename T>
  Enum<T> define_enum_under(char const* name, Module module );
} // namespace Rice


// =========   Enum.ipp   =========

#include <stdexcept>

namespace Rice
{
  template<typename Enum_T>
  Enum<Enum_T>::Enum(char const* name, Module module) : Data_Type<Enum_T>()
  {
    Data_Type<Enum_T> klass = define_class_under<Enum_T>(module, name);
    define_methods(klass);
  }

  template<typename Enum_T>
  inline Enum<Enum_T>& Enum<Enum_T>::define_value(std::string name, Enum_T value)
  {
    // Save mapping from value to name
    valuesToNames_[value] = name;

    // Store value as class constant available to Ruby
    Data_Object<Enum_T> object(value, true, Enum<Enum_T>::klass());
    this->const_set(name, object);

    return *this;
  }

  template<typename Enum_T>
  inline void Enum<Enum_T>::define_methods(Data_Type<Enum_T> klass)
  {
    // First we need a constructor
    klass.define_constructor(Constructor<Enum_T>());

    // Instance methods. The self parameter is confusing because it is really a Data_Object<Enum_T>.
    // However, if we make that the type then the From_Ruby code will consider it a 
    // Data_Type<Data_Object<Enum_T>>>. But in define class above it was actually bound as 
    // Data_Type<Enum_T>. Thus the static_casts in the methods below.
    klass.define_method("to_s", [](Enum_T& notSelf)
        {
          // We have to return string because we don't know if std::string support has
          // been included by the user
        Data_Object<Enum_T> self = static_cast<Data_Object<Enum_T>>(notSelf);
        return String(valuesToNames_[*self]);
        })
      .define_method("to_int", [](Enum_T& notSelf) ->  Underlying_T
        {
          Data_Object<Enum_T> self = static_cast<Data_Object<Enum_T>>(notSelf);
          return static_cast<Underlying_T>(*self);
        })
      .define_method("inspect", [](Enum_T& notSelf)
        {
          Data_Object<Enum_T> self = static_cast<Data_Object<Enum_T>>(notSelf);

          std::stringstream result;
          VALUE rubyKlass = Enum<Enum_T>::klass().value();
          result << "#<" << detail::protect(rb_class2name, rubyKlass)
            << "::" << Enum<Enum_T>::valuesToNames_[*self] << ">";

          // We have to return string because we don't know if std::string support has
          // been included by the user
          return String(result.str());
        })
      .define_method("hash", [](Enum_T& notSelf) ->  Underlying_T
        {
          Data_Object<Enum_T> self = static_cast<Data_Object<Enum_T>>(notSelf);
          return (Underlying_T)*self;
        })
      .define_method("eql?", [](Enum_T& notSelf, Enum_T& notOther)
        {
          Data_Object<Enum_T> self = static_cast<Data_Object<Enum_T>>(notSelf);
          Data_Object<Enum_T> other = static_cast<Data_Object<Enum_T>>(notOther);
          return self == other;
        });

    // Add aliases
    rb_define_alias(klass, "===", "eql?");

    // Add comparable support
    klass.include_module(rb_mComparable)
      .define_method("<=>", [](Enum_T& self, Enum_T& other)
    {
      if (self == other)
      {
        return 0;
      }
      else if (self < other)
      {
        return -1;
      }
      else
      {
        return 1;
      }
    });

    // Add enumerable support
    klass.include_module(rb_mEnumerable)
      .define_singleton_method("each", [](VALUE ruby_klass) -> VALUE
        {
          if (!rb_block_given_p())
          {
            return rb_enumeratorize_with_size(ruby_klass, Identifier("each").to_sym(),
                                      0, nullptr, 0);
          }

          for (auto& pair : valuesToNames_)
          {
            Enum_T enumValue = pair.first;
            VALUE value = detail::To_Ruby<Enum_T>().convert(enumValue);
            detail::protect(rb_yield, value);
          }

          return ruby_klass;
      }, Return().setValue())
      .define_singleton_method("from_int", [](VALUE ruby_klass, int32_t value) -> Object
      {
          auto iter = Enum<Enum_T>::valuesToNames_.find((Enum_T)value);
          if (iter == Enum<Enum_T>::valuesToNames_.end())
          {
            throw std::runtime_error("Unknown enum value: " + std::to_string(value));
          }

          std::string name = iter->second;
          return Object(ruby_klass).const_get(name);
      });
  }

  template<typename Enum_T>
  Enum<Enum_T> define_enum(char const* name)
  {
    return define_enum_under<Enum_T>(name, rb_cObject);
  }

  template<typename Enum_T>
  Enum<Enum_T> define_enum_under(char const* name, Module module)
  {
    if (detail::Registries::instance.types.isDefined<Enum_T>())
    {
      return Enum<Enum_T>();
    }

    return Enum<Enum_T>(name, module);
  }
}
// =========   MemoryView.hpp   =========

namespace Rice
{
  class MemoryView
  {
  };
}


// =========   MemoryView.ipp   =========
namespace Rice
{
}

namespace Rice::detail
{
  template<>
  class To_Ruby<unsigned char**>
  {
  public:
    VALUE convert(unsigned char** x)
    {
      std::runtime_error("To_Ruby unsigned char** is not implemented yet");
      return Qnil;
    }
  };

  template<>
  class From_Ruby<unsigned char**>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      std::runtime_error("From_Ruby unsigned char** is not implemented yet");
      return Convertible::None;
    }

    unsigned char** convert(VALUE value)
    {
      std::runtime_error("From_Ruby unsigned char** is not implemented yet");
      return nullptr;
    }

  private:
    Arg* arg_ = nullptr;
  };
}

// Dependent on Module, Class, Array and String

// =========   forward_declares.ipp   =========

namespace Rice
{
  // These methods cannot be defined where they are declared due to circular dependencies
  inline Class Object::class_of() const
  {
    return detail::protect(rb_class_of, value_);
  }

  inline String Object::to_s() const
  {
    return call("to_s");
  }

  inline String Object::class_name() const
  {
    return detail::protect(rb_obj_classname, this->value());
  }

  inline String Object::inspect() const
  {
    return call("inspect");
  }

  inline Object Object::instance_eval(String const& s)
  {
    const VALUE argv[] = { s.value() };
    return detail::protect(rb_obj_instance_eval, 1, &argv[0], this->value());
  }

  inline Object Object::vcall(Identifier id, Array args)
  {
    std::vector<VALUE> a(args.size());

    Array::const_iterator it = args.begin();
    Array::const_iterator end = args.end();

    for (int i = 0; it != end; i++, ++it)
    {
      a[i] = it->value();
    }

    return detail::protect(rb_funcall3, this->value(), id.id(), (int)args.size(), (const VALUE*)a.data());
  }

  inline std::ostream& operator<<(std::ostream& out, Object const& obj)
  {
    String s(obj.to_s());
    out << s.c_str();
    return out;
  }

  inline Identifier::Identifier(Symbol const& symbol) : id_(SYM2ID(symbol.value()))
  {
  }

  inline String Module::name() const
  {
    VALUE name = detail::protect(rb_mod_name, this->value());
    if (name == Qnil)
    {
      return String("");
    }
    else
    {
      return name;
    }
  }

  inline Array Module::ancestors() const
  {
    return detail::protect(rb_mod_ancestors, this->value());
  }

  inline Class Module::singleton_class() const
  {
    return CLASS_OF(value());
  }

  inline Object Module::module_eval(String const& s)
  {
    const VALUE argv[] = { s.value() };
    return detail::protect(rb_mod_module_eval, 1, &argv[0], this->value());
  }
}

// For now include libc support - maybe should be separate header file someday

// =========   file.hpp   =========

namespace Rice::libc
{
  extern Class rb_cLibcFile;
}


// ---------   file.ipp   ---------
#include <exception>

// Libraries sometime inherit custom exception objects from std::exception,
// so define it for Ruby if necessary
namespace Rice::libc
{
  inline Class rb_cLibcFile;

  inline void define_libc_file()
  {
    Module rb_mRice = define_module("Rice");
    Module rb_mLibc = define_module_under(rb_mRice, "libc");
    rb_cLibcFile = define_class_under<FILE>(rb_mLibc, "File");
  }
}

namespace Rice::detail
{
  template<>
  struct Type<FILE>
  {
    static bool verify()
    {
      if (!detail::Registries::instance.types.isDefined<FILE>())
      {
        libc::define_libc_file();
      }

      return true;
    }
  };
}


#endif // Rice__hpp_
