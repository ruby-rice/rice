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
#include <ruby/thread.h>

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





// C++ headers have to come after Ruby on MacOS for reasons I do not understand
#include <cstdio>
#include <cstring> // For std::memset
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
#include <tuple>
#include <type_traits>
#include <variant>
#include <vector>

namespace Rice
{
  namespace detail
  {
    // Get the base_type of T - without pointer, reference, const or volatile. We call remove_pointer_t twice 
    // for T**
    template<typename T>
    using intrinsic_type = typename std::remove_cv_t<std::remove_pointer_t<std::remove_pointer_t<std::remove_reference_t<T>>>>;

    template<typename T>
    constexpr bool is_const_any_v = std::is_const_v<std::remove_pointer_t<std::remove_pointer_t<std::remove_reference_t<T>>>>;

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

    template<typename T>
    struct remove_cv_recursive<T&&>
    {
      using type = typename remove_cv_recursive<T>::type&&;
    };

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

    template<typename>
    struct is_std_vector : std::false_type {};

    template<typename T, typename A>
    struct is_std_vector<std::vector<T, A>> : std::true_type {};

    template <typename T>
    constexpr bool is_std_vector_v = is_std_vector<T>::value;


    // -- Tuple Helpers ---
    template<typename T>
    struct tuple_shift;

    template<typename T, typename...Arg_Ts>
    struct tuple_shift<std::tuple<T, Arg_Ts...>>
    {
      using type = std::tuple<Arg_Ts...>;
    };

    template<typename T, typename...Arg_Ts>
    struct tuple_unshift;
      
    template<typename T, typename...Arg_Ts>
    struct tuple_unshift<T, std::tuple<Arg_Ts...>>
    {
      using type = std::tuple<T, Arg_Ts...>;
    };

    template<template<typename, typename...> typename T, typename...Arg_Ts>
    struct tuple_map;

    template<template<typename, typename...> typename T, typename...Arg_Ts>
    struct tuple_map<T, std::tuple<Arg_Ts...>>
    {
      using type = std::tuple<T<Arg_Ts>...>;
    };


    template<typename...Arg_Ts>
    struct tuple_to_variant;

    template<typename...Arg_Ts>
    struct tuple_to_variant<std::tuple<Arg_Ts...>>
    {
      using type = std::variant<Arg_Ts...>;
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

    // See https://www.cppstories.com/2022/tuple-iteration-apply/
    template <typename Tuple_T, typename Function_T>
    void for_each_tuple(Tuple_T&& tuple, Function_T&& callable)
    {
      std::apply([&callable](auto&& ...args)
        {
          (callable(std::forward<decltype(args)>(args)), ...);
        }, std::forward<Tuple_T>(tuple));
    }

    template<typename T, typename = void>
    struct is_wrapped : std::true_type {};

    template<typename T>
    struct is_wrapped<T, std::enable_if_t<std::is_fundamental_v<detail::intrinsic_type<T>> ||
                                          std::is_same_v<detail::intrinsic_type<T>, std::string>>
                     >: std::false_type {};

    template<typename T>
    constexpr bool is_wrapped_v = is_wrapped<T>::value;
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
    using nth_arg = typename std::tuple_element_t<N, arg_types>;

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
    static constexpr std::size_t arity = functor_t::arity;
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
  template<typename Function_T, typename = void>
  struct method_traits;

  /* Functions that have a self parameter and thus we treat them as free standing
    "methods" versus member functions. 
    
       doSomething(VALUE self, int a) */
  template<typename Function_T>
  struct method_traits<Function_T, std::enable_if_t<std::is_same_v<typename function_traits<Function_T>::class_type, std::nullptr_t>>>
  {
    using Return_T = typename function_traits<Function_T>::return_type;
    using Class_T = typename function_traits<Function_T>::template nth_arg<0>;
    using Arg_Ts = typename tuple_shift<typename function_traits<Function_T>::arg_types>::type;
    static constexpr std::size_t arity = std::tuple_size_v<Arg_Ts>;
  };

  // Member functions that have an implied self parameter of an object instance
  //   foo.doSomething(int a)
  template<typename Function_T>
  struct method_traits<Function_T, std::enable_if_t<!std::is_same_v<typename function_traits<Function_T>::class_type, std::nullptr_t>>>
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

  template<typename Attribute_T>
  struct attribute_traits<Attribute_T*>
  {
    using attr_type = Attribute_T;
    using class_type = std::nullptr_t;
  };

  template<typename Attribute_T, typename Class_T>
  struct attribute_traits<Attribute_T(Class_T::*)>
  {
    using attr_type = Attribute_T;
    using class_type = Class_T;
  };
}

// Wrap C++ objects as Ruby objects

// =========   Wrapper.hpp   =========

namespace Rice::detail
{
  class WrapperBase
  {
  public:
    WrapperBase() = default;
    virtual ~WrapperBase() = default;
    virtual void* get() = 0;
    bool isConst();

    void ruby_mark();
    void addKeepAlive(VALUE value);
    void setOwner(bool value);

  protected:
    bool isOwner_ = false;
    bool isConst_ = false;

  private:
    // We use a vector for speed and memory locality versus a set which does
    // not scale well when getting to tens of thousands of objects (not expecting
    // that to happen...but just in case)
    std::vector<VALUE> keepAlive_;
  };

  template <typename T>
  class Wrapper : public WrapperBase
  {
  public:
    Wrapper(T& data);
    Wrapper(T&& data);
    ~Wrapper();
    void* get() override;

  private:
    T data_;
  };

  template<typename T>
  class Wrapper<T&> : public WrapperBase
  {
  public:
    Wrapper(T& data);
    ~Wrapper();
    void* get() override;

  private:
    T& data_;
  };

  template <typename T>
  class Wrapper<T*> : public WrapperBase
  {
  public:
    Wrapper(T* data, bool isOwner);
    ~Wrapper();
    void* get() override;

  private:
    T* data_ = nullptr;
  };

  template <typename T>
  class Wrapper<T**> : public WrapperBase
  {
  public:
    Wrapper(T** data, bool isOwner);
    ~Wrapper();
    void* get() override;

  private:
    T** data_ = nullptr;
  };

  // ---- Helper Functions ---------
  template <typename T>
  void wrapConstructed(VALUE value, rb_data_type_t* rb_data_type, T* data, bool isOwner);

  template <typename T>
  VALUE wrap(VALUE klass, rb_data_type_t* rb_data_type, T& data, bool isOwner);

  template <typename T>
  VALUE wrap(VALUE klass, rb_data_type_t* rb_data_type, T* data, bool isOwner);

  template <typename T>
  T* unwrap(VALUE value, rb_data_type_t* rb_data_type, bool takeOwnership);

  template <typename Wrapper_T = WrapperBase>
  Wrapper_T* getWrapper(VALUE value, rb_data_type_t* rb_data_type);

  WrapperBase* getWrapper(VALUE value);
}
 
// =========   Type.hpp   =========

#include <regex>

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
  class TypeMapper
  {
  public:
    std::string name();
    std::string name(const std::type_index& typeIndex);
    std::string simplifiedName();
    std::string rubyName();
    VALUE rubyKlass();

    // public only for testing
    std::string findGroup(std::string& string, size_t start = 0);
  private:
    std::string demangle(char const* mangled_name);
    std::string rubyTypeName();
    void removeGroup(std::string& string, std::regex regex);
    void replaceGroup(std::string& string, std::regex regex, std::string replacement);
    void replaceAll(std::string& string, std::regex regex, std::string replacement);
    void capitalizeHelper(std::string& content, std::regex& regex);
  };

  template<typename T>
  void verifyType();

  template<typename Tuple_T>
  void verifyTypes();
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

// =========   NativeInvoker.hpp   =========

#include <optional>

namespace Rice::detail
{
  template<typename Return_T>
  class ResultWrapper
  {
  public:
    std::exception_ptr exception;
    Return_T getResult();
		void setResult(Return_T&& value);
  private:
    std::optional<Return_T> result_;
  };

  template<typename Return_T>
  class ResultWrapper<Return_T&>
  {
  public:
    std::exception_ptr exception;
    Return_T& getResult();
    void setResult(Return_T& value);
  private:
    std::optional<std::reference_wrapper<Return_T>> result_;
  };

  template<typename Return_T>
  class ResultWrapper<Return_T*>
  {
  public:
    std::exception_ptr exception;

    Return_T* getResult();
    void setResult(Return_T* value);
  private:
    Return_T* result_;
  };

  template<>
  class ResultWrapper<void>
  {
  public:
    std::exception_ptr exception;
  };

  template<typename Return_T, typename Function_T, typename Tuple_T>
  class NativeInvoker
  {
  public:
    const Function_T func;
    Tuple_T args;
  public:
    NativeInvoker(const Function_T func, Tuple_T&& args);
    void invoke();
    Return_T result();
    std::exception_ptr exception();
  private:
    ResultWrapper<Return_T> resultWrapper;
  };

  template<typename Function_T, typename ...Arg_Ts>
  auto protect(Function_T func, Arg_Ts...args);

  template<typename Function_T, typename ...Arg_Ts>
  typename function_traits<Function_T>::return_type no_gvl(Function_T func, Arg_Ts...args);

  template<typename Function_T, typename Tuple_T>
  typename function_traits<Function_T>::return_type no_gvl(Function_T func, Tuple_T&& args);
}


// =========   NativeInvoker.ipp   =========
namespace Rice::detail
{
  // ----- ResultWrapper -------
  template<typename Return_T>
  inline Return_T ResultWrapper<Return_T>::getResult()
  {
    if constexpr (!std::is_copy_constructible_v<Return_T> && std::is_move_constructible_v<Return_T>)
    {
      return std::move(this->result_.value());
    }
    // std::is_copy_constructible_v<std::vector<std::unique_ptr<T>>>> return true. Sigh.
    else if constexpr (detail::is_std_vector_v<Return_T> && std::is_copy_constructible_v<Return_T>)
    {
      if constexpr (!std::is_copy_constructible_v<typename Return_T::value_type>)
      {
        return std::move(this->result_.value());
      }
      else
      {
        return this->result_.value();
      }
    }
    else
    {
      return this->result_.value();
    }
	}

  template<typename Return_T>
  inline void ResultWrapper<Return_T>::setResult(Return_T&& value)
  {
    if constexpr(std::is_copy_assignable_v<Return_T> || std::is_move_assignable_v<Return_T>)
    {
      this->result_ = std::forward<Return_T>(value);
    }
    else if constexpr (std::is_copy_constructible_v<Return_T>)
    {
      this->result_.emplace(value);
    }
    else if constexpr (std::is_move_constructible_v<Return_T>)
    {
      this->result_.emplace(std::move(value));
    }
  }

  template<typename Return_T>
  inline Return_T& ResultWrapper<Return_T&>::getResult()
  {
    return this->result_.value().get();
  }

  template<typename Return_T>
  inline void ResultWrapper<Return_T&>::setResult(Return_T& value)
  {
    this->result_ = value;
  }

  template<typename Return_T>
  inline Return_T* ResultWrapper<Return_T*>::getResult()
  {
    return this->result_;
  }

  template<typename Return_T>
  inline void ResultWrapper<Return_T*>::setResult(Return_T* value)
  {
    this->result_ = value;
  }

  // ----- Invoker -------
  template<typename Return_T, typename Function_T, typename Tuple_T>
  inline NativeInvoker<Return_T, Function_T, Tuple_T>::NativeInvoker(const Function_T func, Tuple_T&& args)
    : func(func), args(std::forward<Tuple_T>(args))
  {
  }

  template<typename Return_T, typename Function_T, typename Tuple_T>
  inline void NativeInvoker<Return_T, Function_T, Tuple_T>::invoke()
  {
    try
    {
      if constexpr (std::is_void_v<Return_T>)
      {
        std::apply(this->func, std::forward<Tuple_T>(this->args));
      }
      else if constexpr (!std::is_pointer_v<Return_T> && !std::is_reference_v<Return_T> && !std::is_copy_assignable_v<Return_T> && std::is_move_assignable_v<Return_T>)
      {
        this->resultWrapper.setResult(std::move(std::apply(this->func, std::forward<Tuple_T>(this->args))));
      }
      else
      {
        this->resultWrapper.setResult(std::forward<Return_T>(std::apply(this->func, std::forward<Tuple_T>(this->args))));
      }
    }
    catch (...)
    {
      this->resultWrapper.exception = std::current_exception();
    }
  }

  template<typename Return_T, typename Function_T, typename Tuple_T>
  inline Return_T NativeInvoker<Return_T, Function_T, Tuple_T>::result()
  {
		return this->resultWrapper.getResult();
  }

  template<typename Return_T, typename Function_T, typename Tuple_T>
  inline std::exception_ptr NativeInvoker<Return_T, Function_T, Tuple_T>::exception()
  {
    return this->resultWrapper.exception;
  }

  // ------- Helper Methods --------
  // Create a functor for calling a Ruby function and define some aliases for readability.
  template<typename Function_T, typename ...Arg_Ts>
  auto protect(Function_T func, Arg_Ts...args)
  {
    using Return_T = typename function_traits<Function_T>::return_type;
    using Tuple_T = std::tuple<Arg_Ts...>;
    using Invoker_T = NativeInvoker<Return_T, Function_T, Tuple_T>;

    Tuple_T argsTuple = std::forward_as_tuple(args...);
    Invoker_T invoker(func, std::forward<Tuple_T>(argsTuple));

    auto trampoline = [](VALUE value) -> VALUE
      {
        Invoker_T* function = (Invoker_T*)value;

        function->invoke();
        return Qnil;
      };

    // Create Invoker and call it via ruby
    int state = (int)JumpException::RUBY_TAG_NONE;
    rb_protect(trampoline, (VALUE)(&invoker), &state);

    // Did anything go wrong?
    if (state == JumpException::RUBY_TAG_NONE)
    {
      if constexpr (!std::is_same_v<Return_T, void>)
      {
        return invoker.result();
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

  template<typename Function_T, typename ...Arg_Ts>
  typename function_traits<Function_T>::return_type no_gvl(Function_T func, Arg_Ts...args)
  {
    using Tuple_T = std::tuple<Arg_Ts...>;
    Tuple_T argsTuple = std::forward_as_tuple(args...);
    return no_gvl(func, argsTuple);
  }

  template<typename Function_T, typename Tuple_T>
  typename function_traits<Function_T>::return_type no_gvl(Function_T func, Tuple_T&& args)
  {
    using Return_T = typename function_traits<Function_T>::return_type;
    using Invoker_T = NativeInvoker<Return_T, Function_T, Tuple_T>;

    Invoker_T invoker(func, std::forward<Tuple_T>(args));

    auto trampoline = [](void* functor) -> void*
      {
        Invoker_T* function = (Invoker_T*)functor;
        function->invoke();

        if (function->exception())
        {
          std::rethrow_exception(function->exception());
        }

        return nullptr;
      };

    // Create Invoker and call it via ruby
    rb_thread_call_without_gvl(trampoline, &invoker, nullptr, nullptr);

    if constexpr (!std::is_same_v<Return_T, void>)
    {
      return invoker.result();
    }
  }
}
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


// Code for Ruby to call C++

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

    //! Specifies if the argument is opaque and Rice should not convert it from Ruby to C++ or vice versa.
    //! This is useful for callbacks and user provided data paramameters.
    virtual Arg& setOpaque();

    //! Returns if the argument should be treated as a value
    bool isOpaque() const;

    //! Specifies C++ will take ownership of this value and Ruby should not free it
    virtual Arg& takeOwnership();
    bool isOwner();

    //! Is the parameter a buffer
    virtual Arg& setBuffer();
    bool isBuffer();

  public:
    std::string name;
    int32_t position = -1;

  private:
    //! Our saved default value
    std::any defaultValue_;
    bool isValue_ = false;
    bool isKeepAlive_ = false;
    bool isOwner_ = false;
    bool isOpaque_ = false;
    bool isBuffer_ = false;

  };
} // Rice


// =========   Return.hpp   =========

namespace Rice
{
  //! Helper for defining Return argument of a method

  class Return: public Arg
  {
  public:
    Return();
    Return& keepAlive() override;
    Return& setValue() override;
    Return& setOpaque() override;
    Return& takeOwnership() override;
    Return& setBuffer() override;
  };
} // Rice


// =========   Function.hpp   =========

namespace Rice
{
  class Function
  {
  public:
    Function& setNoGvl();
    bool isNoGvl() const;

  private:
    bool isNoGvl_ = false;
  };
} // Rice


// =========   MethodInfo.hpp   =========

#include <vector>

namespace Rice
{
  class MethodInfo
  {
  public:
    MethodInfo() = default;

    template <typename...Arg_Ts>
    MethodInfo(size_t argCount, const Arg_Ts&...args);

    /**
      * Add a defined Arg to this list of Arguments
      */
    void addArg(const Arg& arg);

    /**
      * Get argument by position
      */
    Arg* arg(size_t pos);

    /**
      * Get argument by name
      */
    Arg* arg(std::string name);

    int argCount();
    Return* returnInfo();
    Function* function();

    // Iterator support
    std::vector<Arg>::iterator begin();
    std::vector<Arg>::iterator end();

  private:
    template <typename Arg_T>
    void processArg(const Arg_T& arg);

    std::vector<Arg> args_;
    Return returnInfo_;
    Function function_;
  };
}

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
      None   = 0b0000,
      Narrow = 0b0001,
      Cast   = 0b0011,
      Const  = 0b0111,
      Exact  = 0b1111
  };
}


// =========   RubyType.hpp   =========

#include <set>

namespace Rice::detail
{
  template <typename T>
  class RubyType
  {
  };
}


// =========   Parameter.hpp   =========

#include <optional>

namespace Rice::detail
{
  class ParameterAbstract
  {
  public:
    ParameterAbstract() = default;
    ParameterAbstract(Arg* arg);
    virtual ~ParameterAbstract() = default;

    ParameterAbstract(ParameterAbstract&& other) = default;
    ParameterAbstract& operator=(ParameterAbstract&& other) = default;

    virtual Convertible matches(std::optional<VALUE>& valueOpt) = 0;
    virtual std::string cppTypeName() = 0;
    virtual VALUE klass() = 0;

  public:
    Arg* arg = nullptr;
  };

  template<typename T>
  class Parameter: public ParameterAbstract
  {
   public:
     using Type = T;

     Parameter() = default;
     Parameter(Arg* arg);
     Parameter(Parameter&& other) = default;
     Parameter& operator=(Parameter&& other) = default;

     T convertToNative(std::optional<VALUE>& valueOpt);
     Convertible matches(std::optional<VALUE>& valueOpt) override;
     std::string cppTypeName() override;
     VALUE klass() override;

    // std::string typeName() override;
  private:
    From_Ruby<remove_cv_recursive_t<T>> fromRuby_;
  };
}

// C++ API declarations

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
  class String : public Builtin_Object<T_STRING>
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

    //! Return an array from a string by unpacking it
    template<typename T>
    Array unpack() const;

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
    Array(T (&a)[n]);

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
    String pack();

    // Join elements together
    String join(const char* separator);

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
    Object push(T&& obj, bool takeOwnership);

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
template<typename Method_T, typename...Arg_Ts>
inline auto& define_method(std::string name, Method_T&& method, const Arg_Ts&...args)
{
  MethodInfo* methodInfo = new MethodInfo(detail::method_traits<Method_T>::arity, args...);
  this->wrap_native_method(this->value(), name, std::forward<Method_T>(method), methodInfo);
  return *this;
}

//! Define a function.
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
  MethodInfo* methodInfo = new MethodInfo(detail::function_traits<Function_T>::arity, args...);
  this->wrap_native_function(this->value(), name, std::forward<Function_T>(func), methodInfo);
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
template<typename Method_T, typename...Arg_Ts>
inline auto& define_singleton_method(std::string name, Method_T&& method, const Arg_Ts&...args)
{
  MethodInfo* methodInfo = new MethodInfo(detail::method_traits<Method_T>::arity, args...);
  this->wrap_native_method(rb_singleton_class(*this), name, std::forward<Method_T>(method), methodInfo);
  return *this;
}

//! Define a singleton method.
/*! The method's implementation can be a static member function, plain
 *  function or lambda. A wrapper will be generated which will convert the method
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
  MethodInfo* methodInfo = new MethodInfo(detail::function_traits<Function_T>::arity, args...);
  this->wrap_native_function(rb_singleton_class(*this), name, std::forward<Function_T>(func), methodInfo);
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
    template<typename Function_T>
    void wrap_native_function(VALUE klass, std::string name, Function_T&& function, MethodInfo* methodInfo);

    template<typename Class_T = VALUE, typename Method_T>
    void wrap_native_method(VALUE klass, std::string name, Method_T&& method, MethodInfo* methodInfo);
  };

  //! Define a new module in the namespace given by module.
  /*! \param module the module in which to define the new module.
   *  \param name the name of the new module.
   */
  Module define_module_under(Object parent, char const * name);

  //! Define a new module in the default namespace.
  /*! \param name the name of the new module.
   */
  Module define_module(char const * name);

  //! Create a new anonymous module.
  /*! \return the new module.
   */
  Module anonymous_module();
}

// =========   Class.hpp   =========

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

    //! Base class name - does not include any parent modules
    /*! \return std::string.
     */
    const std::string base_name() const;

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
template<typename Method_T, typename...Arg_Ts>
inline auto& define_method(std::string name, Method_T&& method, const Arg_Ts&...args)
{
  MethodInfo* methodInfo = new MethodInfo(detail::method_traits<Method_T>::arity, args...);
  this->wrap_native_method(this->value(), name, std::forward<Method_T>(method), methodInfo);
  return *this;
}

//! Define a function.
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
  MethodInfo* methodInfo = new MethodInfo(detail::function_traits<Function_T>::arity, args...);
  this->wrap_native_function(this->value(), name, std::forward<Function_T>(func), methodInfo);
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
template<typename Method_T, typename...Arg_Ts>
inline auto& define_singleton_method(std::string name, Method_T&& method, const Arg_Ts&...args)
{
  MethodInfo* methodInfo = new MethodInfo(detail::method_traits<Method_T>::arity, args...);
  this->wrap_native_method(rb_singleton_class(*this), name, std::forward<Method_T>(method), methodInfo);
  return *this;
}

//! Define a singleton method.
/*! The method's implementation can be a static member function, plain
 *  function or lambda. A wrapper will be generated which will convert the method
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
  MethodInfo* methodInfo = new MethodInfo(detail::function_traits<Function_T>::arity, args...);
  this->wrap_native_function(rb_singleton_class(*this), name, std::forward<Function_T>(func), methodInfo);
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
  Class define_class_under(Object parent, Identifier id, const Class& superclass = rb_cObject);

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

  enum class NativeKind
  {
    Function,
    Method,
    Iterator,
    AttributeReader,
    AttributeWriter,
    Proc
  };

  class Native
  {
  public:
    static VALUE resolve(int argc, VALUE* argv, VALUE self);
  public:
    Native() = default;
    Native(std::vector<std::unique_ptr<ParameterAbstract>>&& parameters);
    virtual ~Native() = default;

    Native(const Native&) = delete;
    Native(Native&&) = delete;
    void operator=(const Native&) = delete;
    void operator=(Native&&) = delete;

    virtual Resolved matches(size_t argc, const VALUE* argv, VALUE self);
    virtual VALUE operator()(size_t argc, const VALUE* argv, VALUE self) = 0;
    virtual std::string toString() = 0;

    // Ruby API access
    virtual std::string name() = 0;
    virtual NativeKind kind() = 0;
    virtual VALUE returnKlass() = 0;
    std::vector<const ParameterAbstract*> parameters();

  protected:
    template<typename T>
    static void verify_type(bool isBuffer);

    template<typename Tuple_T, std::size_t ...Indices>
    static void verify_args(MethodInfo* methodInfo, std::index_sequence<Indices...> indices);

    std::vector<std::optional<VALUE>> getRubyValues(size_t argc, const VALUE* argv, bool validate);
    ParameterAbstract* getParameterByName(std::string name);
    Convertible matchParameters(std::vector<std::optional<VALUE>>& values);

    template<typename Tuple_T>
    static std::vector<std::unique_ptr<ParameterAbstract>> create_parameters(MethodInfo* methodInfo);

    template<typename Tuple_T, std::size_t ...Indices>
    static inline void create_parameters_impl(std::vector<std::unique_ptr<ParameterAbstract>>& parameters, MethodInfo* methodInfo, std::index_sequence<Indices...> indices);

  protected:
    std::vector<std::unique_ptr<ParameterAbstract>> parameters_;

  };
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

      using Attr_T = typename attribute_traits<Attribute_T>::attr_type;
      using Receiver_T = typename attribute_traits<Attribute_T>::class_type;
      using To_Ruby_T = remove_cv_recursive_t<Attr_T>;

    public:
      // Register attribute getter with Ruby
      static void define(VALUE klass, std::string name, Attribute_T attribute, Return returnInfo);

    public:
      // Disallow creating/copying/moving
      NativeAttributeGet() = delete;
      NativeAttributeGet(const NativeAttribute_T&) = delete;
      NativeAttributeGet(NativeAttribute_T&&) = delete;
      void operator=(const NativeAttribute_T&) = delete;
      void operator=(NativeAttribute_T&&) = delete;

      Resolved matches(size_t argc, const VALUE* argv, VALUE self) override;
      VALUE operator()(size_t argc, const VALUE* argv, VALUE self) override;
      std::string toString() override;

      std::string name() override;
      NativeKind kind() override;
      VALUE returnKlass() override;

    protected:
      NativeAttributeGet(VALUE klass, std::string name, Attribute_T attr, Return returnInfo);

    private:
      VALUE klass_;
      std::string name_;
      Attribute_T attribute_;
      Return return_;
    };
  } // detail
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

      Resolved matches(size_t argc, const VALUE* argv, VALUE self) override;
      VALUE operator()(size_t argc, const VALUE* argv, VALUE self) override;
      std::string toString() override;

      std::string name() override;
      NativeKind kind() override;
      VALUE returnKlass() override;

    protected:
      NativeAttributeSet(VALUE klass, std::string name, Attribute_T attr);

    private:
      VALUE klass_;
      std::string name_;
      Attribute_T attribute_;
    };
  } // detail
} // Rice


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
template<typename Method_T, typename...Arg_Ts>
inline auto& define_method(std::string name, Method_T&& method, const Arg_Ts&...args)
{
  MethodInfo* methodInfo = new MethodInfo(detail::method_traits<Method_T>::arity, args...);
  this->wrap_native_method(this->value(), name, std::forward<Method_T>(method), methodInfo);
  return *this;
}

//! Define a function.
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
  MethodInfo* methodInfo = new MethodInfo(detail::function_traits<Function_T>::arity, args...);
  this->wrap_native_function(this->value(), name, std::forward<Function_T>(func), methodInfo);
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
template<typename Method_T, typename...Arg_Ts>
inline auto& define_singleton_method(std::string name, Method_T&& method, const Arg_Ts&...args)
{
  MethodInfo* methodInfo = new MethodInfo(detail::method_traits<Method_T>::arity, args...);
  this->wrap_native_method(rb_singleton_class(*this), name, std::forward<Method_T>(method), methodInfo);
  return *this;
}

//! Define a singleton method.
/*! The method's implementation can be a static member function, plain
 *  function or lambda. A wrapper will be generated which will convert the method
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
  MethodInfo* methodInfo = new MethodInfo(detail::function_traits<Function_T>::arity, args...);
  this->wrap_native_function(rb_singleton_class(*this), name, std::forward<Function_T>(func), methodInfo);
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

    template<typename Method_T>
    void wrap_native_method(VALUE klass, std::string name, Method_T&& function, MethodInfo* methodInfo);

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
    Data_Object(T&& obj, Class klass = Data_Type<T>::klass());
    Data_Object(const T& obj, bool isOwner = false, Class klass = Data_Type<T>::klass());

    //! Unwrap a Ruby object.
    /*! This constructor is analogous to calling Data_Get_Struct.  Uses
     *  Data_Type<T>::klass as the class of the object.
     *  \param value the Ruby object to unwrap.
     */
    Data_Object(Object value);
    Data_Object(VALUE value);

    T& operator*() const; //!< Return a reference to obj_
    T* operator->() const; //!< Return a pointer to obj_
    T* get() const;        //!< Return a pointer to obj_

  private:
    static void check_ruby_type(VALUE value);
  };
} // namespace Rice


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
    static inline std::string name = "bool";
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
    static inline std::string name = "String";
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
    static inline std::string name = "String";
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
    static inline std::string name = "String";
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
    static inline std::string name = "Integer";
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
    static inline std::string name = "Integer";
  };

  template<>
  class RubyType<int>
  {
  public:
    using FromRuby_T = int(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2int_inline;
    static inline std::set<ruby_value_type> Exact = { RUBY_T_FIXNUM };
    static inline std::set<ruby_value_type> Castable = { };
    // We allow bignum to integer because Ruby switches to bignum at about 2 billion on 64 bit systems,
    // while int can go up to 4 billion
    static inline std::set<ruby_value_type> Narrowable = { RUBY_T_BIGNUM };
    static inline std::string packTemplate = "i*";
    static inline std::string name = "Integer";
  };

  template<>
  class RubyType<unsigned int>
  {
  public:
    using FromRuby_T = unsigned int(*)(VALUE);

    static inline FromRuby_T fromRuby = RB_NUM2UINT;
    static inline std::set<ruby_value_type> Exact = { RUBY_T_FIXNUM };
    static inline std::set<ruby_value_type> Castable = { };
    // We allow bignum to integer because Ruby switches to bignum at about 2 billion on 64 bit systems,
    // while int can go up to 4 billion
    static inline std::set<ruby_value_type> Narrowable = { RUBY_T_BIGNUM };
    static inline std::string packTemplate = "I*";
    static inline std::string name = "Integer";
  };

  template<>
  class RubyType<long>
  {
  public:
    using FromRuby_T = long(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2long_inline;
    static inline std::set<ruby_value_type> Exact = { RUBY_T_FIXNUM };
    static inline std::set<ruby_value_type> Castable = { };
    static inline std::set<ruby_value_type> Narrowable = { RUBY_T_BIGNUM };
    static inline std::string packTemplate = "l_*";
    static inline std::string name = "Integer";
  };

  template<>
  class RubyType<unsigned long>
  {
  public:
    using FromRuby_T = unsigned long(*)(VALUE);

    static inline FromRuby_T fromRuby = rb_num2ulong_inline;
    static inline std::set<ruby_value_type> Exact = { RUBY_T_FIXNUM  };
    static inline std::set<ruby_value_type> Castable = { };
    static inline std::set<ruby_value_type> Narrowable = { RUBY_T_BIGNUM};
    static inline std::string packTemplate = "L_*";
    static inline std::string name = "Integer";
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
    static inline std::string name = "Integer";
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
    static inline std::string name = "Integer";
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
    static inline std::string name = "Float";
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
    static inline std::string name = "Float";
  };

  template<>
  class RubyType<void>
  {
  public:
    static inline std::set<ruby_value_type> Exact = { };
    static inline std::set<ruby_value_type> Castable = { };
    static inline std::set<ruby_value_type> Narrowable = { };
    static inline std::string name = "void";
  };
}
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

    // API for access from Ruby
    VALUE klasses();

  private:
    std::optional<std::pair<VALUE, rb_data_type_t*>> lookup(const std::type_info& typeInfo);
    void raiseUnverifiedType(const std::string& typeName);

    std::unordered_map<std::type_index, std::pair<VALUE, rb_data_type_t*>> registry_{};
    std::set<std::type_index> unverified_{};
  };
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



// =========   DefaultHandler.hpp   =========

namespace Rice::detail
{
  class DefaultHandler
  {
  public:
    void operator()() const;
  };
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



// =========   ModuleRegistry.hpp   =========

namespace Rice::detail
{
  class ModuleRegistry
  {
  public:
    void add(VALUE module);
    // API for access from Ruby
    VALUE modules();

  private:
    std::set<VALUE> modules_{};
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
    // std::is_copy_constructible returns true for std::vector<std::unique_ptr> - so we need
    // to force the issue
    NativeRegistry() = default;
    NativeRegistry(const NativeRegistry& other) = delete;
    NativeRegistry& operator=(const NativeRegistry& other) = delete;

    void add(VALUE klass, ID methodId, std::unique_ptr<Native>& native);
    void reset(VALUE klass);

    const std::vector<Native*> lookup(VALUE klass);
    const std::vector<std::unique_ptr<Native>>& lookup(VALUE klass, ID methodId);

  private:
    // Key - Ruby klass/method
    // Value - Vector of Native pointers owned by the registry (thus wrapped in std::unique_ptr)
    std::map<std::pair<VALUE, ID>, std::vector<std::unique_ptr<Native>>> natives_ = {};
  };
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
    ModuleRegistry modules;
    NativeRegistry natives;
    TypeRegistry types;
  };
}


// To / From Ruby

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
  inline Arg_Type Arg::defaultValue()
  {
    return std::any_cast<Arg_Type>(this->defaultValue_);
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

  inline Arg& Arg::setOpaque()
  {
    isOpaque_ = true;
    return *this;
  }

  inline bool Arg::isOpaque() const
  {
    return isOpaque_;
  }

  inline Arg& Arg::takeOwnership()
  {
    this->isOwner_ = true;
    return *this;
  }

  inline bool Arg::isOwner()
  {
    return this->isOwner_;
  }

  inline Arg& Arg::setBuffer()
  {
    this->isBuffer_ = true;
    return *this;
  }

  inline bool Arg::isBuffer()
  {
    return this->isBuffer_;
  }
} // Rice
// =========   Parameter.ipp   =========
namespace Rice::detail
{
  // -----------  ParameterAbstract ----------------
  inline ParameterAbstract::ParameterAbstract(Arg* arg) : arg(arg)
  {
  }

  // -----------  Parameter ----------------
  template<typename T>
  inline Parameter<T>::Parameter(Arg* arg) : ParameterAbstract(arg), fromRuby_(arg)
  {
  }

  template<typename T>
  inline Convertible Parameter<T>::matches(std::optional<VALUE>& valueOpt)
  {
    Convertible result = Convertible::None;

    // Is a VALUE being passed directly to C++ ?
    if (valueOpt.has_value())
    {
      VALUE value = valueOpt.value();
      if (this->arg->isValue())
      {
        result = Convertible::Exact;
      }
      // If index is less than argc then check with FromRuby if the VALUE is convertible
      // to C++.
      else
      {
        result = this->fromRuby_.is_convertible(value);

        // If this is an exact match check if the const-ness of the value and the parameter match
        if (result == Convertible::Exact && rb_type(value) == RUBY_T_DATA)
        {
          // Check the constness of the Ruby wrapped value and the parameter
          WrapperBase* wrapper = getWrapper(value);

          // Do not send a const value to a non-const parameter
          if (wrapper->isConst() && !is_const_any_v<T>)
          {
            result = Convertible::None;
          }
          // It is ok to send a non-const value to a const parameter but
          // prefer non-const to non-const by slighly decreasing the convertible value
          else if (!wrapper->isConst() && is_const_any_v<T>)
          {
            result = Convertible::Const;
          }
        }
      }
    }
    // Last check if a default value has been set
    else if (this->arg->hasDefaultValue())
    {
      result = Convertible::Exact;
    }

    return result;
  }

  template<typename T>
  inline T Parameter<T>::convertToNative(std::optional<VALUE>& valueOpt)
  {
    /* In general the compiler will convert T to const T, but that does not work for converting
       T** to const T** (see see https://isocpp.org/wiki/faq/const-correctness#constptrptr-conversion)
       which comes up in the OpenCV bindings.

       An alternative solution is updating From_Ruby#convert to become a templated function that specifies
       the return type. That works but requires a lot more code changes for this one case and is not
       backwards compatible. */

    if constexpr (is_pointer_pointer_v<T> && !std::is_convertible_v<remove_cv_recursive_t<T>, T>)
    {
      return (T)this->fromRuby_.convert(valueOpt.value());
    }
    else if (valueOpt.has_value())
    {
      return this->fromRuby_.convert(valueOpt.value());
    }
    else if constexpr (std::is_constructible_v<std::remove_cv_t<T>, std::remove_cv_t<std::remove_reference_t<T>>&>)
    {
      if (this->arg->hasDefaultValue())
      {
        return this->arg->template defaultValue<T>();
      }
    }

    throw std::invalid_argument("Could not convert Rubyy value");
  }

  template<typename T>
  inline std::string Parameter<T>::cppTypeName()
  {
    detail::TypeMapper<T> typeMapper;
    return typeMapper.simplifiedName();
  }

  template<typename T>
  inline VALUE Parameter<T>::klass()
  {
    TypeMapper<T> typeMapper;
    return typeMapper.rubyKlass();
  }
}

// =========   Return.ipp   =========
#include <any>

namespace Rice
{
  inline Return::Return(): Arg("Return")
  {
  }

  inline Return& Return::keepAlive()
  {
    Arg::keepAlive();
    return *this;
  }

  inline Return& Return::setValue()
  {
    Arg::setValue();
    return *this;
  }

  inline Return& Return::setOpaque()
  {
    Arg::setOpaque();
    return *this;
  }

  inline Return& Return::takeOwnership()
  {
    Arg::takeOwnership();
    return *this;
  }

  inline Return& Return::setBuffer()
  {
    Arg::setBuffer();
    return *this;
  }
}  // Rice

// =========   Function.ipp   =========
namespace Rice
{
  inline Function& Function::setNoGvl()
  {
    this->isNoGvl_ = true;
    return *this;
  }

  inline bool Function::isNoGvl() const
  {
    return this->isNoGvl_;
  }
}  // Rice

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

// =========   Buffer.hpp   =========

namespace Rice
{
  template<typename T, typename = void>
  class Buffer;
    
  template<typename T>
  class Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>
  {
  public:
    Buffer(T* pointer);
    Buffer(T* pointer, size_t size);
    Buffer(VALUE value);
    Buffer(VALUE value, size_t size);

    ~Buffer();

    Buffer(const Buffer& other) = delete;
    Buffer(Buffer&& other);

    Buffer& operator=(const Buffer& other) = delete;
    Buffer& operator=(Buffer&& other);
    T& operator[](size_t index);

    T* ptr();
    T& reference();
    T* release();

    size_t size() const;

    // Ruby API
    VALUE toString() const;

    VALUE bytes() const;
    VALUE bytes(size_t count) const;

    Array toArray() const;
    Array toArray(size_t count) const;

    bool isOwner() const;
    void setOwner(bool value);

  private:
    void fromBuiltinType(VALUE value, size_t size);
    void fromWrappedType(VALUE value, size_t size);

    bool m_owner = false;
    size_t m_size = 0;
    // std::unique_ptr would be great but std::unique_ptr<void> isn't allowed. Mutable is needed to
    // support const T* buffers
    mutable T* m_buffer = nullptr;
  };

  template<typename T>
  class Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>
  {
  public:
    Buffer(T** pointer);
    Buffer(T** pointer, size_t size);
    Buffer(VALUE value);
    Buffer(VALUE value, size_t size);

    ~Buffer();

    Buffer(const Buffer& other) = delete;
    Buffer(Buffer&& other);

    Buffer& operator=(const Buffer& other) = delete;
    Buffer& operator=(Buffer&& other);

    T* operator[](size_t index);

    T** ptr();
    T** release();

    size_t size() const;

    // Ruby API
    VALUE toString() const;

    VALUE bytes() const;
    VALUE bytes(size_t count) const;

    Array toArray() const;
    Array toArray(size_t count) const;

    void setOwner(bool value);
    bool isOwner() const;

  private:
    bool m_owner = false;
    size_t m_size = 0;
    T** m_buffer = nullptr;
  };

  template<typename T>
  class Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>
  {
  public:
    Buffer(T** pointer);
    Buffer(T** pointer, size_t size);
    Buffer(VALUE value);
    Buffer(VALUE value, size_t size);

    ~Buffer();

    Buffer(const Buffer& other) = delete;
    Buffer(Buffer&& other);

    Buffer& operator=(const Buffer& other) = delete;
    Buffer& operator=(Buffer&& other);

    T* operator[](size_t index);

    T** ptr();
    T** release();

    size_t size() const;

    // Ruby API
    VALUE toString() const;

    VALUE bytes() const;
    VALUE bytes(size_t count) const;

    Array toArray() const;
    Array toArray(size_t count) const;

    void setOwner(bool value);
    bool isOwner() const;

  private:
    bool m_owner = false;
    size_t m_size = 0;
    T** m_buffer = nullptr;
  };

  template<typename T>
  class Buffer<T, std::enable_if_t<std::is_void_v<T>>>
  {
  public:
    Buffer(T* pointer);
    Buffer(VALUE value);
    Buffer(VALUE value, size_t size);

    Buffer(const Buffer& other) = delete;
    Buffer(Buffer&& other);

    Buffer& operator=(const Buffer& other) = delete;
    Buffer& operator=(Buffer&& other);

    size_t size() const;
      
    VALUE bytes(size_t count) const;
    VALUE bytes() const;

    T* ptr();
    T* release();

  private:
    bool m_owner = false;
    size_t m_size = 0;
    T* m_buffer = nullptr;
  };

  template<typename T>
  Data_Type<Buffer<T>> define_buffer(std::string klassName = "");
}


// =========   Pointer.hpp   =========

namespace Rice
{
  template<typename T>
  class Pointer
  {
  };

  template<typename T>
  Data_Type<Pointer<T>> define_pointer(std::string klassName = "");
}


// =========   Buffer.ipp   =========
namespace Rice
{
  // ----  Buffer<T> ------- 
  template<typename T>
  inline Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::Buffer(T* pointer) : m_buffer(pointer)
  {
  }

  template<typename T>
  inline Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::Buffer(T* pointer, size_t size) : m_size(size), m_buffer(pointer)
  {
  }

  template <typename T>
  inline Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::Buffer(VALUE value) : Buffer(value, 0)
  {
  }

  template <typename T>
  inline Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::Buffer(VALUE value, size_t size)
  {
    if constexpr (std::is_fundamental_v<T>)
    {
      this->fromBuiltinType(value, size);
    }
    else
    {
      this->fromWrappedType(value, size);
    }
  }

  template <typename T>
  inline Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::~Buffer()
  {
    if constexpr (std::is_destructible_v<T>)
    {
      if (this->m_owner)
      {
        delete[] this->m_buffer;
      }
    }
  }

  template <typename T>
  inline void Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::fromBuiltinType(VALUE value, size_t size)
  {
    using Intrinsic_T = typename detail::intrinsic_type<T>;
    using RubyType_T = typename detail::RubyType<Intrinsic_T>;

    ruby_value_type valueType = rb_type(value);
    switch (valueType)
    {
      case RUBY_T_ARRAY:
      {
        Array array(value);
        this->m_size = array.size();
        this->m_buffer = new T[this->m_size]();

        if constexpr (std::is_fundamental_v<T>)
        {
          String packed = array.pack<Intrinsic_T>();
          memcpy((void*)this->m_buffer, RSTRING_PTR(packed.value()), RSTRING_LEN(packed.value()));
        }
        else
        {
          detail::From_Ruby<Intrinsic_T> fromRuby;
          for (int i = 0; i < array.size(); i++)
          {
            this->m_buffer[0] = fromRuby.convert(array[i]);
          }
        }
        this->m_owner = true;
        break;
      }
      case RUBY_T_STRING:
      {
        this->m_size = RSTRING_LEN(value);
        if constexpr (std::is_same_v<T, char>)
        {
          // Add 2 for null characters (string and wstring)
          this->m_buffer = new T[this->m_size + 2]();
        }
        else
        {
          this->m_buffer = new T[this->m_size]();
        }
        memcpy((void*)this->m_buffer, RSTRING_PTR(value), this->m_size);

        this->m_owner = true;
        break;
      }
      case RUBY_T_DATA:
      {
        if (Data_Type<Pointer<T>>::is_descendant(value))
        {
          this->m_buffer = detail::unwrap<T>(value, Data_Type<Pointer<T>>::ruby_data_type(), false);
          this->m_owner = false;
          this->m_size = size;
          break;
        }
        [[fallthrough]];
      }
      default:
      {
        if (RubyType_T::Exact.find(valueType) != RubyType_T::Exact.end() ||
          RubyType_T::Castable.find(valueType) != RubyType_T::Castable.end() ||
          RubyType_T::Narrowable.find(valueType) != RubyType_T::Narrowable.end())
        {
          T data = detail::protect(RubyType_T::fromRuby, value);
          this->m_size = 1;
          this->m_buffer = new T[this->m_size]();
          memcpy((void*)this->m_buffer, &data, sizeof(T));
          this->m_owner = true;
          break;
        }
        else
        {
          detail::TypeMapper<T> typeMapper;
          std::string typeName = typeMapper.name();
          throw Exception(rb_eTypeError, "wrong argument type %s (expected % s*)",
            detail::protect(rb_obj_classname, value), typeName.c_str());
        }
      }
    }
  }

  template <typename T>
  inline void Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::fromWrappedType(VALUE value, size_t size)
  {
    using Intrinsic_T = typename detail::intrinsic_type<T>;

    switch (rb_type(value))
    {
      case RUBY_T_ARRAY:
      {
        Array array(value);
        this->m_size = array.size();

        // Use operator new[] to allocate memory but not call constructors.
        size_t size = sizeof(T) * this->m_size;
        this->m_buffer = static_cast<T*>(operator new[](size));

        detail::From_Ruby<Intrinsic_T> fromRuby;

        for (size_t i = 0; i < this->m_size; i++)
        {
          // Construct objects in allocated memory using move or copy construction
          if constexpr (std::is_move_constructible_v<T>)
          {
            new (&this->m_buffer[i]) T(std::move(fromRuby.convert(array[i].value())));
          }
          else if constexpr (std::is_copy_constructible_v<T>)
          {
            new (&this->m_buffer[i]) T(fromRuby.convert(array[i].value()));
          }
          else
          {
            throw Exception(rb_eTypeError, "Cannot construct object of type %s - type is not move or copy constructible",
              detail::TypeMapper<Intrinsic_T>().name().c_str());
          }
        }
        break;
      }
      case RUBY_T_DATA:
      {
        if (Data_Type<Pointer<Intrinsic_T>>::is_descendant(value))
        {
          this->m_buffer = detail::unwrap<T>(value, Data_Type<Pointer<Intrinsic_T>>::ruby_data_type(), false);
          this->m_owner = false;
          this->m_size = size;
          break;
        }
        else if (Data_Type<Intrinsic_T>::is_descendant(value))
        {
          this->m_buffer = detail::unwrap<T>(value, Data_Type<Intrinsic_T>::ruby_data_type(), false);
          this->m_owner = false;
          this->m_size = size;
          break;
        }
      }
      case RUBY_T_STRING:
      {
        // This special case is a bit ugly...
        if constexpr (std::is_same_v<detail::intrinsic_type<T>, std::string>)
        {
          // FromRuby owns the converted string so we need to keep it alive until we get to the copy constructor
          // two lines down
          detail::From_Ruby<T*> fromRuby;
          T* converted = fromRuby.convert(value);
          this->m_buffer = new T[1]{ *converted };
          this->m_owner = true;
          this->m_size = 1;
          break;
        }
        [[fallthrough]];
      }
      default:
      {
        detail::TypeMapper<T> typeMapper;
        std::string typeName = typeMapper.name();
        throw Exception(rb_eTypeError, "wrong argument type %s (expected % s*)",
          detail::protect(rb_obj_classname, value), typeName.c_str());
      }
    }
  }

  template <typename T>
  inline Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::Buffer(Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>&& other) : m_owner(other.m_owner), m_size(other.m_size), m_buffer(other.m_buffer)
  {
    other.m_buffer = nullptr;
    other.m_size = 0;
    other.m_owner = false;
  }

  template <typename T>
  inline Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>& Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::operator=(Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>&& other)
  {
    this->m_buffer = other.m_buffer;
    other.m_buffer = nullptr;

    this->m_size = other.m_size;
    other.m_size = 0;

    this->m_owner = other.m_owner;
    other.m_owner = false;

    return *this;
  }

  template <typename T>
  inline size_t Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::size() const
  {
    return this->m_size;
  }

  template <typename T>
  inline T* Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::ptr()
  {
    return this->m_buffer;
  }

  template <typename T>
  inline T& Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::reference()
  {
    return *this->m_buffer;
  }

  template <typename T>
  inline T* Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::release()
  {
    this->m_owner = false;
    return this->m_buffer;
  }

  template <typename T>
  inline bool Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::isOwner() const
  {
    return this->m_owner;
  }

  template <typename T>
  inline void Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::setOwner(bool value)
  {
    this->m_owner = value;
  }

  template<typename T>
  inline VALUE Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::toString() const
  {
    detail::TypeMapper<T*> typeMapper;
    std::string description = "Buffer<type: " + typeMapper.simplifiedName() + ", size: " + std::to_string(this->m_size) + ">";

    // We can't use To_Ruby because To_Ruby depends on Buffer - ie a circular reference
    return detail::protect(rb_utf8_str_new_cstr, description.c_str());
  }

  template<typename T>
  inline VALUE Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::bytes(size_t count) const
  {
    if (!this->m_buffer)
    {
      return Qnil;
    }
    else
    {
      long length = (long)(count * sizeof(T));
      return detail::protect(rb_str_new_static, (const char*)this->m_buffer, length);
    }
  }

  template<typename T>
  inline VALUE Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::bytes() const
  {
    return this->bytes(this->m_size);
  }

  template<typename T>
  inline Array Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::toArray(size_t count) const
  {
    if (!this->m_buffer)
    {
      return Qnil;
    }
    else if constexpr (std::is_fundamental_v<T>)
    {
      VALUE string = this->bytes(count);
      return String(string).unpack<T>();
    }
    else
    {
      Array result;

      T* ptr = this->m_buffer;
      T* end = this->m_buffer + count;

      for (; ptr < end; ptr++)
      {
        result.push(*ptr, false);
      }
      return result;
    }
  }

  template<typename T>
  inline Array Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::toArray() const
  {
    return this->toArray(this->m_size);
  }

  template<typename T>
  inline T& Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::operator[](size_t index)
  {
    if (index >= this->m_size)
    {
      throw Exception(rb_eIndexError, "index %ld outside of bounds: 0..%ld", index, this->m_size);
    }

    return this->m_buffer[index];
  }

  // ----  Buffer<T*> - Builtin ------- 
  template<typename T>
  inline Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::Buffer(T** pointer) : m_buffer(pointer)
  {
  }

  template<typename T>
  inline Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::Buffer(T** pointer, size_t size) : m_buffer(pointer), m_size(size)
  {
  }

  template <typename T>
  inline Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::Buffer(VALUE value) : Buffer(value, 0)
  {
  }
 
  template <typename T>
  inline Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::Buffer(VALUE value, size_t size)
  {
    using Intrinsic_T = typename detail::intrinsic_type<T>;

    ruby_value_type valueType = rb_type(value);
    switch (valueType)
    {
      case RUBY_T_ARRAY:
      {
        Array outer(value);

        // Allocate outer buffer
        this->m_size = outer.size();
        this->m_buffer = new T*[this->m_size]();

        for (size_t i = 0; i < this->m_size; i++)
        {
          // Check the inner value is also an array
          Array inner(outer[i].value());

          // Allocate inner buffer
          this->m_buffer[i] = new T[inner.size()]();

          if constexpr (std::is_fundamental_v<Intrinsic_T>)
          {
            String packed = inner.pack<Intrinsic_T>();
            memcpy((void*)this->m_buffer[i], RSTRING_PTR(packed.value()), RSTRING_LEN(packed.value()));
          }
          else
          {
            detail::From_Ruby<Intrinsic_T*> fromRuby;
            for (int i = 0; i < inner.size(); i++)
            {
              this->m_buffer[0] = fromRuby.convert(inner[i].value());
            }
          }
        }

        this->m_owner = true;
        break;
      }
      case RUBY_T_DATA:
      {
        if (Data_Type<Pointer<T*>>::is_descendant(value))
        {
          this->m_buffer = detail::unwrap<T*>(value, Data_Type<Pointer<T*>>::ruby_data_type(), false);
          this->m_owner = false;
          this->m_size = size;
          break;
        }
        [[fallthrough]];
      }
      default:
      {
        detail::TypeMapper<T> typeMapper;
        std::string typeName = typeMapper.name();
        throw Exception(rb_eTypeError, "wrong argument type %s (expected % s*)",
          detail::protect(rb_obj_classname, value), typeName.c_str());
      }
    }
  }

  template <typename T>
  inline Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::~Buffer()
  {
    if (this->m_owner)
    {
      for (size_t i = 0; i < this->m_size; i++)
      {
        delete this->m_buffer[i];
      }

      delete[] this->m_buffer;
    }
  }

  template <typename T>
  inline Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::Buffer(Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>&& other) : m_owner(other.m_owner), m_size(other.m_size),
                                                  m_buffer(other.m_buffer)
  {
    other.m_buffer = nullptr;
    other.m_size = 0;
    other.m_owner = false;
  }

  template <typename T>
  inline Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>& Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::operator=(Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>&& other)
  {
    this->m_buffer = other.m_buffer;
    other.m_buffer = nullptr;

    this->m_size = other.m_size;
    other.m_size = 0;

    this->m_owner = other.m_owner;
    other.m_owner = false;

    return *this;
  }

  template <typename T>
  inline T* Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::operator[](size_t index)
  {
    return this->m_buffer[index];
  }

  template <typename T>
  inline size_t Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::size() const
  {
    return this->m_size;
  }

  template <typename T>
  inline T** Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::ptr()
  {
    return this->m_buffer;
  }

  template <typename T>
  inline T** Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::release()
  {
    this->m_owner = false;
    return this->m_buffer;
  }

  template <typename T>
  inline bool Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::isOwner() const
  {
    return this->m_owner;
  }

  template <typename T>
  inline void Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::setOwner(bool value)
  {
    this->m_owner = value;
  }

  template<typename T>
  inline VALUE Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::toString() const
  {
    detail::TypeMapper<T*> typeMapper;
    std::string description = "Buffer<type: " + typeMapper.simplifiedName() + ", size: " + std::to_string(this->m_size) + ">";

    // We can't use To_Ruby because To_Ruby depends on Buffer - ie a circular reference
    return detail::protect(rb_utf8_str_new_cstr, description.c_str());
  }

  template<typename T>
  inline VALUE Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::bytes(size_t count) const
  {
    if (!this->m_buffer)
    {
      return Qnil;
    }
    else
    {
      T** begin = this->m_buffer;
      long length = (long)(count * sizeof(T*));
      return detail::protect(rb_str_new_static, (const char*)*begin, length);
    }
  }

  template<typename T>
  inline VALUE Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::bytes() const
  {
    return this->bytes(this->m_size);
  }

  template<typename T>
  inline Array Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::toArray(size_t count) const
  {
    if (!this->m_buffer)
    {
      return Qnil;
    }
    else
    {
      Array result;

      T** ptr = this->m_buffer;
      T** end = this->m_buffer + count;

      for (; ptr < end; ptr++)
      {
        Buffer<T> buffer(*ptr);
        result.push(std::move(buffer), true);
      }
      return result;
    }
  }

  template<typename T>
  inline Array Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::toArray() const
  {
    return this->toArray(this->m_size);
  }

  // ----  Buffer<T*> - Wrapped ------- 
  template<typename T>
  inline Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::Buffer(T** pointer) : m_buffer(pointer)
  {
  }

  template<typename T>
  inline Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::Buffer(T** pointer, size_t size) : m_buffer(pointer), m_size(size)
  {
  }

  template <typename T>
  inline Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::Buffer(VALUE value) : Buffer(value, 0)
  {
  }
  
  template <typename T>
  inline Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::Buffer(VALUE value, size_t size)
  {
    ruby_value_type valueType = rb_type(value);
    switch (valueType)
    {
      case RUBY_T_ARRAY:
      {
        Array array(value);
        this->m_size = array.size();
        this->m_buffer = new T * [this->m_size]();

        detail::From_Ruby<T*> fromRuby;

        for (size_t i = 0; i < this->m_size; i++)
        {
          this->m_buffer[i] = fromRuby.convert(array[i].value());
        }

        this->m_owner = true;
        break;
      }
      case RUBY_T_DATA:
      {
        if (Data_Type<Pointer<T*>>::is_descendant(value))
        {
          this->m_buffer = detail::unwrap<T*>(value, Data_Type<Pointer<T*>>::ruby_data_type(), false);
          this->m_owner = false;
          this->m_size = size;
          break;
        }
      }
      default:
      {
        detail::TypeMapper<T> typeMapper;
        std::string typeName = typeMapper.name();
        throw Exception(rb_eTypeError, "wrong argument type %s (expected % s*)",
          detail::protect(rb_obj_classname, value), typeName.c_str());
      }
    }
  }

  template <typename T>
  inline Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::~Buffer()
  {
    if (this->m_owner)
    {
      delete[] this->m_buffer;
    }
  }

  template <typename T>
  inline Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::Buffer(Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>&& other) : m_owner(other.m_owner), m_size(other.m_size),
    m_buffer(other.m_buffer)
  {
    other.m_buffer = nullptr;
    other.m_size = 0;
    other.m_owner = false;
  }

  template <typename T>
  inline Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>& Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::operator=(Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>&& other)
  {
    this->m_buffer = other.m_buffer;
    other.m_buffer = nullptr;

    this->m_size = other.m_size;
    other.m_size = 0;

    this->m_owner = other.m_owner;
    other.m_owner = false;

    return *this;
  }

  template <typename T>
  inline T* Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::operator[](size_t index)
  {
    if (index >= this->m_size)
    {
      throw Exception(rb_eIndexError, "index %ld outside of bounds: 0..%ld", index, this->m_size);
    }
    return this->m_buffer[index];
  }

  template <typename T>
  inline size_t Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::size() const
  {
    return this->m_size;
  }

  template <typename T>
  inline T** Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::ptr()
  {
    return this->m_buffer;
  }

  template <typename T>
  inline T** Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::release()
  {
    this->m_owner = false;
    return this->m_buffer;
  }

  template <typename T>
  inline bool Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::isOwner() const
  {
    return this->m_owner;
  }

  template <typename T>
  inline void Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::setOwner(bool value)
  {
    this->m_owner = value;
  }

  template<typename T>
  inline VALUE Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::toString() const
  {
    detail::TypeMapper<T*> typeMapper;
    std::string description = "Buffer<type: " + typeMapper.simplifiedName() + ", size: " + std::to_string(this->m_size) + ">";

    // We can't use To_Ruby because To_Ruby depends on Buffer - ie a circular reference
    return detail::protect(rb_utf8_str_new_cstr, description.c_str());
  }

  template<typename T>
  inline VALUE Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::bytes(size_t count) const
  {
    if (!this->m_buffer)
    {
      return Qnil;
    }
    else
    {
      T** begin = this->m_buffer;
      long length = (long)(count * sizeof(T*));
      return detail::protect(rb_str_new_static, (const char*)*begin, length);
    }
  }

  template<typename T>
  inline VALUE Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::bytes() const
  {
    return this->bytes(this->m_size);
  }

  template<typename T>
  inline Array Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::toArray(size_t count) const
  {
    if (!this->m_buffer)
    {
      return Qnil;
    }
    else
    {
      Array result;

      T** ptr = this->m_buffer;
      T** end = this->m_buffer + count;

      for (; ptr < end; ptr++)
      {
        result.push(*ptr, false);
      }
      return result;
    }
  }

  template<typename T>
  inline Array Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::toArray() const
  {
    return this->toArray(this->m_size);
  }

  // ----  Buffer<void> ------- 
  template<typename T>
  inline Buffer<T, std::enable_if_t<std::is_void_v<T>>>::Buffer(VALUE value) : Buffer(value, 0)
  {
  }
  
  template<typename T>
  inline Buffer<T, std::enable_if_t<std::is_void_v<T>>>::Buffer(VALUE value, size_t size)
  {
    ruby_value_type valueType = rb_type(value);

    switch (valueType)
    {
      case RUBY_T_STRING:
      {
        this->m_size = RSTRING_LEN(value);
        this->m_buffer = ::operator new(this->m_size);
        memcpy((void*)this->m_buffer, RSTRING_PTR(value), this->m_size);

        this->m_owner = true;
        break;
      }
      default:
      {
        detail::TypeMapper<void> typeMapper;
        std::string typeName = typeMapper.name();
        throw Exception(rb_eTypeError, "wrong argument type %s (expected % s*)",
          detail::protect(rb_obj_classname, value), typeName.c_str());
      }
    }
  }

  template<typename T>
  inline Buffer<T, std::enable_if_t<std::is_void_v<T>>>::Buffer(T* pointer) : m_buffer(pointer)
  {
  }

  template<typename T>
  inline Buffer<T, std::enable_if_t<std::is_void_v<T>>>::Buffer(Buffer<T, std::enable_if_t<std::is_void_v<T>>>&& other) : m_owner(other.m_owner), m_size(other.m_size), m_buffer(other.m_buffer)
  {
    other.m_buffer = nullptr;
    other.m_size = 0;
    other.m_owner = false;
  }

  template<typename T>
  inline Buffer<T, std::enable_if_t<std::is_void_v<T>>>& Buffer<T, std::enable_if_t<std::is_void_v<T>>>::operator=(Buffer<T, std::enable_if_t<std::is_void_v<T>>>&& other)
  {
    this->m_buffer = other.m_buffer;
    other.m_buffer = nullptr;

    return *this;
  }

  template<typename T>
  inline size_t Buffer<T, std::enable_if_t<std::is_void_v<T>>>::size() const
  {
    return this->m_size;
  }

  template<typename T>
  inline T* Buffer<T, std::enable_if_t<std::is_void_v<T>>>::ptr()
  {
    return this->m_buffer;
  }

  template <typename T>
  inline T* Buffer<T, std::enable_if_t<std::is_void_v<T>>>::release()
  {
    this->m_owner = false;
    return this->m_buffer;
  }

  template<typename T>
  inline VALUE Buffer<T, std::enable_if_t<std::is_void_v<T>>>::bytes(size_t count) const
  {
    if (!this->m_buffer)
    {
      return Qnil;
    }
    else
    {
      return detail::protect(rb_usascii_str_new, (const char*)this->m_buffer, (long)count);
    }
  }

  template<typename T>
  inline VALUE Buffer<T, std::enable_if_t<std::is_void_v<T>>>::bytes() const
  {
    return this->bytes(this->m_size);
  }

  // ------  define_buffer ----------
  template<typename T>
  inline Data_Type<Buffer<T>> define_buffer(std::string klassName)
  {
    using Buffer_T = Buffer<T>;
    using Data_Type_T = Data_Type<Buffer_T>;

    if (klassName.empty())
    {
      detail::TypeMapper<Buffer_T> typeMapper;
      klassName = typeMapper.rubyName();
    }

    Module rb_mRice = define_module("Rice");

    if (Data_Type_T::check_defined(klassName, rb_mRice))
    {
      return Data_Type_T();
    }

    if constexpr (std::is_void_v<T>)
    {
      return define_class_under<Buffer_T>(rb_mRice, klassName).
        define_constructor(Constructor<Buffer_T, VALUE>(), Arg("value").setValue()).
        define_constructor(Constructor<Buffer_T, VALUE, size_t>(), Arg("value").setValue(), Arg("size")).
        define_method("size", &Buffer_T::size).
        template define_method<VALUE(Buffer_T::*)(size_t) const>("bytes", &Buffer_T::bytes, Return().setValue()).
        template define_method<VALUE(Buffer_T::*)() const>("bytes", &Buffer_T::bytes, Return().setValue()).
        define_method("data", &Buffer_T::ptr, Return().setBuffer()).
        define_method("release", &Buffer_T::release, Return().setBuffer());
    }
    else
    {
      Data_Type<Buffer_T> klass = define_class_under<Buffer_T>(rb_mRice, klassName).
        define_constructor(Constructor<Buffer_T, VALUE>(), Arg("value").setValue()).
        define_constructor(Constructor<Buffer_T, VALUE, size_t>(), Arg("value").setValue(), Arg("size")).
        define_method("size", &Buffer_T::size).
        template define_method<VALUE(Buffer_T::*)() const>("to_s", &Buffer_T::toString, Return().setValue()).
        template define_method<VALUE(Buffer_T::*)(size_t) const>("bytes", &Buffer_T::bytes, Return().setValue()).
        template define_method<VALUE(Buffer_T::*)() const>("bytes", &Buffer_T::bytes, Return().setValue()).
        template define_method<Array(Buffer_T::*)(size_t) const>("to_ary", &Buffer_T::toArray, Return().setValue()).
        template define_method<Array(Buffer_T::*)() const>("to_ary", &Buffer_T::toArray, Return().setValue()).
        define_method("[]", &Buffer_T::operator[], Arg("index")).
        define_method("data", &Buffer_T::ptr, Return().setBuffer()).
        define_method("release", &Buffer_T::release, Return().setBuffer());

      if constexpr (!std::is_pointer_v<T> && !std::is_void_v<T> && !std::is_const_v<T> && std::is_copy_assignable_v<T>)
      {
        klass.define_method("[]=", [](Buffer_T& self, size_t index, T& value) -> void
        {
          self[index] = value;
        });
      }
      else if constexpr (std::is_pointer_v<T> && !std::is_const_v<std::remove_pointer_t<T>> && std::is_copy_assignable_v<std::remove_pointer_t<T>>)
      {
        klass.define_method("[]=", [](Buffer_T& self, size_t index, T value) -> void
        {
          *self[index] = *value;
        });
      }

      return klass;
    }
  }
}

namespace Rice::detail
{
  template<typename T>
  struct Type<Buffer<T>>
  {
    static bool verify()
    {
      detail::verifyType<T>();
      define_buffer<T>();
      return true;
    }
  };
}

// =========   Pointer.ipp   =========
namespace Rice
{
  template<typename T>
  inline Data_Type<Pointer<T>> define_pointer(std::string klassName)
  {
    using Pointer_T = Pointer<T>;
    using Data_Type_T = Data_Type<Pointer_T>;

    if (klassName.empty())
    {
      detail::TypeMapper<Pointer_T> typeMapper;
      klassName = typeMapper.rubyName();
    }

    Module rb_mRice = define_module("Rice");

    if (Data_Type_T::check_defined(klassName, rb_mRice))
    {
      return Data_Type_T();
    }

    Data_Type<Pointer<T>> result = define_class_under<Pointer_T>(rb_mRice, klassName).
      define_method("buffer", [](VALUE self) -> Buffer<T>
      {
        T* ptr = detail::unwrap<T>(self, Data_Type<Pointer<T>>::ruby_data_type(), false);
        Buffer<T> buffer(ptr);
        return buffer;
      }, Arg("self").setValue());

    // Define a buffer to read the pointer's data
    define_buffer<T>();

    return result;
  }
}

namespace Rice::detail
{
  template<typename T>
  struct Type<Pointer<T>>
  {
    static bool verify()
    {
      detail::verifyType<T>();
      define_pointer<T>();
      return true;
    }
  };
}

// =========   Types.ipp   =========
namespace Rice::detail
{
  template<>
  struct Type<bool>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cTrueClass;
    }
  };

  template<>
  struct Type<char>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };

  template<int N>
  struct Type<char[N]>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };

  template<>
  struct Type<char*>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };

  template<>
  struct Type<signed char>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };
  
  template<int N>
  struct Type<signed char[N]>
  {
    static bool verify()
    {
      define_buffer<signed char>();
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };

  template<>
  struct Type<unsigned char>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };

  template<int N>
  struct Type<unsigned char[N]>
  {
    static bool verify()
    {
      define_buffer<unsigned char>();
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };
  
  template<>
  struct Type<short>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cInteger;
    }
  };

  template<int N>
  struct Type<short[N]>
  {
    static bool verify()
    {
      define_buffer<short>();
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };
  
  template<>
  struct Type<unsigned short>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cInteger;
    }
  };

  template<int N>
  struct Type<unsigned short[N]>
  {
    static bool verify()
    {
      define_buffer<unsigned short>();
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };

  template<>
  struct Type<int>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cInteger;
    }
  };

  template<int N>
  struct Type<int[N]>
  {
    static bool verify()
    {
      define_buffer<int>();
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };

  template<>
  struct Type<unsigned int>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cInteger;
    }
  };

  template<int N>
  struct Type<unsigned int[N]>
  {
    static bool verify()
    {
      define_buffer<unsigned int>();
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };

  template<>
  struct Type<long>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cInteger;
    }
  };

  template<int N>
  struct Type<long[N]>
  {
    static bool verify()
    {
      define_buffer<long>();
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };

  template<>
  struct Type<unsigned long>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cInteger;
    }
  };

  template<int N>
  struct Type<unsigned long[N]>
  {
    static bool verify()
    {
      define_buffer<unsigned long>();
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };
  
  template<>
  struct Type<long long>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cInteger;
    }
  };

  template<int N>
  struct Type<long long[N]>
  {
    static bool verify()
    {
      define_buffer<long long>();
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };

  template<>
  struct Type<unsigned long long>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cInteger;
    }
  };

  template<int N>
  struct Type<unsigned long long[N]>
  {
    static bool verify()
    {
      define_buffer<unsigned long long>();
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };

  template<>
  struct Type<float>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cFloat;
    }
  };

  template<int N>
  struct Type<float[N]>
  {
    static bool verify()
    {
      define_buffer<float>();
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };

  template<>
  struct Type<double>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cFloat;
    }
  };

  template<int N>
  struct Type<double[N]>
  {
    static bool verify()
    {
      define_buffer<double>();
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };

  template<>
  struct Type<void>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cNilClass;
    }
  };

  template<>
  struct Type<void*>
  {
    static bool verify()
    {
      Type<Pointer<void>>::verify();
      return true;
    }
  };
}
// =========   to_ruby.ipp   =========

namespace Rice
{
  namespace detail
  {
    // ===========  bool  ============
    template<>
    class To_Ruby<bool>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const bool& native)
      {
        return native ? Qtrue : Qfalse;
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<bool&>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const bool& native)
      {
        return native ? Qtrue : Qfalse;
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<bool N>
    class To_Ruby<bool[N]>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(bool data[N])
      {
        Buffer<bool> buffer(data, N);
        Data_Object<Buffer<bool>> dataObject(std::move(buffer));
        return dataObject.value();
      }
    private:
      Arg* arg_ = nullptr;
    };

    // ===========  int  ============
    template<>
    class To_Ruby<int>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

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

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<int&>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const int& native)
      {
#ifdef rb_int2num_inline
        return protect(rb_int2num_inline, (int)native);
#else
        return RB_INT2NUM(native);
#endif
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<int N>
    class To_Ruby<int[N]>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(int data[N])
      {
        Buffer<int> buffer(data, N);
        Data_Object<Buffer<int>> dataObject(std::move(buffer));
        return dataObject.value();
      }
    private:
      Arg* arg_ = nullptr;
    };

    // ===========  unsigned int  ============
    template<>
    class To_Ruby<unsigned int>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const unsigned int& native)
      {
#ifdef rb_int2num_inline
        return protect(rb_uint2num_inline, (unsigned int)native);
#else
        return RB_UINT2NUM(native);
#endif
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<unsigned int&>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const unsigned int& native)
      {
#ifdef rb_int2num_inline
        return protect(rb_uint2num_inline, (unsigned int)native);
#else
        return RB_UINT2NUM(native);
#endif
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<int N>
    class To_Ruby<unsigned int[N]>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(unsigned int data[N])
      {
        Buffer<unsigned int> buffer(data, N);
        Data_Object<Buffer<unsigned int>> dataObject(std::move(buffer));
        return dataObject.value();
      }
    private:
      Arg* arg_ = nullptr;
    };

    // ===========  char  ============
    template<>
    class To_Ruby<char>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const char& native)
      {
        return To_Ruby<int>().convert(native);
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<char&>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const char& native)
      {
        return To_Ruby<int>().convert(native);
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<char*>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const char* data)
      {
        if (!data)
        {
          return Qnil;
        }
        else if (strlen(data) > 0 && data[0] == ':')
        {
          size_t symbolLength = strlen(data) - 1;
          char* symbol = new char[symbolLength];
          strncpy(symbol, data + 1, symbolLength);
          ID id = protect(rb_intern2, symbol, (long)symbolLength);
          delete[] symbol;
          return protect(rb_id2sym, id);
        }
        else if (this->arg_ && this->arg_->isOwner())
        {
          // This copies the buffer but does not free it. So Ruby is not really
          // taking ownership of it. But there isn't a Ruby API for creating a string
          // from an existing buffer and later freeing it.
          return protect(rb_usascii_str_new_cstr, data);
        }
        else
        {
          // Does NOT copy the passed in buffer and does NOT free it when the string is GCed
          long size = (long)strlen(data);
          VALUE result = protect(rb_str_new_static, data, size);
          // Freeze the object so Ruby can't modify the C string
          return rb_obj_freeze(result);
        }
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<int N>
    class To_Ruby<char[N]>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

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

    private:
      Arg* arg_ = nullptr;
    };

    // ===========  unsigned char  ============
    template<>
    class To_Ruby<unsigned char>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const unsigned char& native)
      {
        return To_Ruby<unsigned int>().convert(native);
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<unsigned char&>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const unsigned char& native)
      {
        return To_Ruby<unsigned int>().convert(native);
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<int N>
    class To_Ruby<unsigned char[N]>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(unsigned char data[N])
      {
        Buffer<unsigned char> buffer(data, N);
        Data_Object<Buffer<unsigned char>> dataObject(std::move(buffer));
        return dataObject.value();
      }
    private:
      Arg* arg_ = nullptr;
    };

    // ===========  signed char  ============
    template<>
    class To_Ruby<signed char>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const signed char& native)
      {
        return To_Ruby<signed int>().convert(native);
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<signed char&>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const signed char& native)
      {
        return To_Ruby<signed int>().convert(native);
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<int N>
    class To_Ruby<signed char[N]>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(signed char data[N])
      {
        Buffer<signed char> buffer(data, N);
        Data_Object<Buffer<signed char>> dataObject(std::move(buffer));
        return dataObject.value();
      }
    private:
      Arg* arg_ = nullptr;
    };

    // ===========  double  ============
    template<>
    class To_Ruby<double>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const double& native)
      {
        return protect(rb_float_new, native);
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<double&>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const double& native)
      {
        return protect(rb_float_new, native);
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<int N>
    class To_Ruby<double[N]>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(double data[N])
      {
        Buffer<double> buffer(data, N);
        Data_Object<Buffer<double>> dataObject(std::move(buffer));
        return dataObject.value();
      }
    private:
      Arg* arg_ = nullptr;
    };

    // ===========  float  ============
    template<>
    class To_Ruby<float>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const float& native)
      {
        return protect(rb_float_new, (double)native);
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<float&>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const float& native)
      {
        return protect(rb_float_new, (double)native);
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<int N>
    class To_Ruby<float[N]>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(float data[N])
      {
        Buffer<float> buffer(data, N);
        Data_Object<Buffer<float>> dataObject(std::move(buffer));
        return dataObject.value();
      }
    private:
      Arg* arg_ = nullptr;
    };

    // ===========  long  ============
    template<>
    class To_Ruby<long>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const long& native)
      {
        return protect(rb_long2num_inline, native);
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<long&>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const long& native)
      {
        return protect(rb_long2num_inline, native);
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<int N>
    class To_Ruby<long[N]>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(long data[N])
      {
        Buffer<long> buffer(data, N);
        Data_Object<Buffer<long>> dataObject(std::move(buffer));
        return dataObject.value();
      }
    private:
      Arg* arg_ = nullptr;
    };

    // ===========  unsigned long  ============
    template<>
    class To_Ruby<unsigned long>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const unsigned long& native)
      {
        if (this->arg_ && this->arg_->isValue())
        {
          return native;
        }
        else
        {
          return protect(rb_ulong2num_inline, native);
        }
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<unsigned long&>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const unsigned long& native)
      {
        if (this->arg_ && this->arg_->isValue())
        {
          return native;
        }
        else
        {
          return protect(rb_ulong2num_inline, native);
        }
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<int N>
    class To_Ruby<unsigned long[N]>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(unsigned long data[N])
      {
        Buffer<unsigned long> buffer(data, N);
        Data_Object<Buffer<unsigned long>> dataObject(std::move(buffer));
        return dataObject.value();
      }
    private:
      Arg* arg_ = nullptr;
    };

    // ===========  long long  ============
    template<>
    class To_Ruby<long long>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const long long& native)
      {
        return protect(rb_ll2inum, native);
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<long long&>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const long long& native)
      {
        return protect(rb_ll2inum, native);
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<int N>
    class To_Ruby<long long[N]>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(long long data[N])
      {
        Buffer<long long> buffer(data, N);
        Data_Object<Buffer<long long>> dataObject(std::move(buffer));
        return dataObject.value();
      }
    private:
      Arg* arg_ = nullptr;
    };

    // ===========  unsigned long long  ============
    template<>
    class To_Ruby<unsigned long long>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const unsigned long long& native)
      {
        if (this->arg_ && this->arg_->isValue())
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
        if (this->arg_ && this->arg_->isValue())
        {
          return native;
        }
        else
        {
          return protect(rb_ull2inum, (unsigned long long)native);
        }
      }
    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<unsigned long long&>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const unsigned long long& native)
      {
        if (this->arg_ && this->arg_->isValue())
        {
          return native;
        }
        else
        {
          return protect(rb_ull2inum, (unsigned long long)native);
        }
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<int N>
    class To_Ruby<unsigned long long[N]>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(unsigned long long data[N])
      {
        Buffer<unsigned long long> buffer(data, N);
        Data_Object<Buffer<unsigned long long>> dataObject(std::move(buffer));
        return dataObject.value();
      }
    private:
      Arg* arg_ = nullptr;
    };

    // ===========  short  ============
    template<>
    class To_Ruby<short>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const short& native)
      {
#ifdef rb_int2num_inline
        return protect(rb_int2num_inline, (int)native);
#else
        return RB_INT2NUM(native);
#endif
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<short&>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const short& native)
      {
#ifdef rb_int2num_inline
        return protect(rb_int2num_inline, (int)native);
#else
        return RB_INT2NUM(native);
#endif
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<int N>
    class To_Ruby<short[N]>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(short data[N])
      {
        Buffer<short> buffer(data, N);
        Data_Object<Buffer<short>> dataObject(std::move(buffer));
        return dataObject.value();
      }
    private:
      Arg* arg_ = nullptr;
    };

    // ===========  unsigned short  ============
    template<>
    class To_Ruby<unsigned short>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const unsigned short& native)
      {
#ifdef rb_int2num_inline
        return protect(rb_uint2num_inline, (unsigned int)native);
#else
        return RB_UINT2NUM(native);
#endif
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<unsigned short&>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const unsigned short& native)
      {
#ifdef rb_int2num_inline
        return protect(rb_uint2num_inline, (unsigned int)native);
#else
        return RB_UINT2NUM(native);
#endif
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<int N>
    class To_Ruby<unsigned short[N]>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(unsigned short data[N])
      {
        Buffer<unsigned short> buffer(data, N);
        Data_Object<Buffer<unsigned short>> dataObject(std::move(buffer));
        return dataObject.value();
      }
    private:
      Arg* arg_ = nullptr;
    };

    // ===========  std::nullptr_t  ============
    template<>
    class To_Ruby<std::nullptr_t>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(std::nullptr_t const)
      {
        return Qnil;
      }

    private:
      Arg* arg_ = nullptr;
    };

    // ===========  void  ============
    template<>
    class To_Ruby<void>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const void*)
      {
        throw std::runtime_error("Converting from void pointer is not implemented");
        return Qnil;
      }

    private:
      Arg* arg_ = nullptr;
    };
 
    template <>
    class To_Ruby<void*>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(void* data)
      {
        if (data == nullptr)
        {
          return Qnil;
        }
        else if (this->arg_ && this->arg_->isOpaque())
        {
          return VALUE(data);
        }
        else
        {
          bool isOwner = this->arg_ && this->arg_->isOwner();
          return detail::wrap(Data_Type<Pointer<void>>::klass(), Data_Type<Pointer<void>>::ruby_data_type(), data, isOwner);
        }
      }

      VALUE convert(const void* data)
      {
        return convert((void*)data);
      }

    private:
      Arg* arg_ = nullptr;
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
        return Convertible::Narrow;
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
    static Convertible is_convertible(VALUE value)
    {
      ruby_value_type valueType = rb_type(value);

      switch (valueType)
      {
        case RUBY_T_NIL:
        {
          return Convertible::Exact;
          break;
        }
        case RUBY_T_DATA:
        {
          if (Data_Type<Pointer<T>>::is_descendant(value))
          {
            return Convertible::Exact;
          }
          [[fallthrough]];
        }
        default:
        {
          return Convertible::None;
        }
      }
    }

    static T* convert(VALUE value)
    {
      ruby_value_type valueType = rb_type(value);

      switch (valueType)
      {
        case RUBY_T_NIL:
        {
          return nullptr;
        }
        case RUBY_T_DATA:
        {
          if (Data_Type<Pointer<T>>::is_descendant(value))
          {
            return unwrap<T>(value, Data_Type<Pointer<T>>::ruby_data_type(), false);
          }
          [[fallthrough]];
        }
        default:
        {
          detail::TypeMapper<Pointer<T>> typeMapper;
          std::string expected = typeMapper.rubyName();
          throw Exception(rb_eTypeError, "wrong argument type %s (expected % s)",
            detail::protect(rb_obj_classname, value), expected.c_str());
        }
      }
    }
  };

  template<typename T>
  class FromRubyFundamental<T**>
  {
  public:
    static Convertible is_convertible(VALUE value)
    {
      ruby_value_type valueType = rb_type(value);

      switch (valueType)
      {
        case RUBY_T_NIL:
        {
          return Convertible::Exact;
          break;
        }
        case RUBY_T_DATA:
        {
          if (Data_Type<Pointer<T*>>::is_descendant(value))
          {
            return Convertible::Exact;
          }
          [[fallthrough]];
        }
        default:
        {
          return Convertible::None;
        }
      }
    }

    static T** convert(VALUE value)
    {
      ruby_value_type valueType = rb_type(value);

      switch (valueType)
      {
        case RUBY_T_NIL:
        {
          return nullptr;
        }
        case RUBY_T_DATA:
        {
          if (Data_Type<Pointer<T*>>::is_descendant(value))
          {
            return unwrap<T*>(value, Data_Type<Pointer<T*>>::ruby_data_type(), false);
          }
          [[fallthrough]];
        }
        default:
        {
          detail::TypeMapper<Pointer<T*>> typeMapper;
          std::string expected = typeMapper.rubyName();
          throw Exception(rb_eTypeError, "wrong argument type %s (expected % s)",
            detail::protect(rb_obj_classname, value), expected.c_str());
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
      return FromRubyFundamental<bool>::convert(value);
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<bool&>
  {
  public:
    using Pointer_T = Pointer<bool>;

    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Pointer_T>::is_descendant(value))
          {
            return Convertible::Exact;
          }
          [[fallthrough]];
        }
        default:
        {
          return FromRubyFundamental<bool>::is_convertible(value);
        }
      }
    }

    bool& convert(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Pointer_T>::is_descendant(value))
          {
            return (bool&)*unwrap<Pointer_T>(value, Data_Type<Pointer_T>::ruby_data_type(), false);
          }
          [[fallthrough]];
        }
        default:
        {
          this->converted_ = FromRubyFundamental<bool>::convert(value);
          return this->converted_;
        }
      }
    }

  private:
    Arg* arg_ = nullptr;
    bool converted_ = false;
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
      return FromRubyFundamental<char>::convert(value);
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<char&>
  {
  public:
    using Pointer_T = Pointer<char>;

    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Pointer_T>::is_descendant(value))
          {
            return Convertible::Exact;
          }
          [[fallthrough]];
        }
        default:
        {
          return FromRubyFundamental<char>::is_convertible(value);
        }
      }
    }

    char& convert(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Pointer_T>::is_descendant(value))
          {
            return (char&)*unwrap<Pointer_T>(value, Data_Type<Pointer_T>::ruby_data_type(), false);
          }
          [[fallthrough]];
        }
        default:
        {
          this->converted_ = FromRubyFundamental<char>::convert(value);
          return this->converted_;
        }
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

    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
      case RUBY_T_NIL:
      {
        return Convertible::Exact;
      }
      case RUBY_T_STRING:
      {
        return Convertible::Exact;
      }
      default:
      {
        return FromRubyFundamental<char*>::is_convertible(value);
      }
      }
    }

    char* convert(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_NIL:
        {
          return nullptr;
        }
        case RUBY_T_STRING:
        {
          // WARNING - this shares the Ruby string memory directly with C++. value really should be frozen.
          // Maybe we should enforce that? Note the user can always create a Buffer to allocate new memory.
          return rb_string_value_cstr(&value);
        }
        default:
        {
          char* rb_string_value_cstr(volatile VALUE * ptr);
          return FromRubyFundamental<char*>::convert(value);
        }
      }
    }

  private:
    Arg* arg_ = nullptr;
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
      return FromRubyFundamental<unsigned char>::convert(value);
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<unsigned char&>
  {
  public:
    using Pointer_T = Pointer<unsigned char>;

    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Pointer_T>::is_descendant(value))
          {
            return Convertible::Exact;
          }
          [[fallthrough]];
        }
        default:
        {
          return FromRubyFundamental<unsigned char>::is_convertible(value);
        }
      }
    }

    unsigned char& convert(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Pointer_T>::is_descendant(value))
          {
            return (unsigned char&)*unwrap<Pointer_T>(value, Data_Type<Pointer_T>::ruby_data_type(), false);
          }
          [[fallthrough]];
        }
        default:
        {
          this->converted_ = FromRubyFundamental<unsigned char>::convert(value);
          return this->converted_;
        }
      }
    }

  private:
    Arg* arg_ = nullptr;
    unsigned char converted_ = 0;
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
      return FromRubyFundamental<signed char>::convert(value);
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<signed char&>
  {
  public:
    using Pointer_T = Pointer<signed char>;

    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Pointer_T>::is_descendant(value))
          {
            return Convertible::Exact;
          }
          [[fallthrough]];
        }
        default:
        {
          return FromRubyFundamental<signed char>::is_convertible(value);
        }
      }
    }

    signed char& convert(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Pointer_T>::is_descendant(value))
          {
            return (signed char&)*unwrap<Pointer_T>(value, Data_Type<Pointer_T>::ruby_data_type(), false);
          }
          [[fallthrough]];
        }
        default:
        {
          this->converted_ = FromRubyFundamental<signed char>::convert(value);
          return this->converted_;
        }
      }
    }

  private:
    Arg* arg_ = nullptr;
    signed char converted_ = 0;
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
      return FromRubyFundamental<double>::convert(value);
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<double&>
  {
  public:
    using Pointer_T = Pointer<double>;

    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Pointer_T>::is_descendant(value))
          {
            return Convertible::Exact;
          }
          [[fallthrough]];
        }
        default:
        {
          return FromRubyFundamental<double>::is_convertible(value);
        }
      }
    }

    double& convert(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Pointer_T>::is_descendant(value))
          {
            return (double&)*unwrap<Pointer_T>(value, Data_Type<Pointer_T>::ruby_data_type(), false);
          }
          [[fallthrough]];
        }
        default:
        {
          this->converted_ = FromRubyFundamental<double>::convert(value);
          return this->converted_;
        }
      }
    }

  private:
    Arg* arg_ = nullptr;
    double converted_;
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
      return FromRubyFundamental<float>::convert(value);
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<float&>
  {
  public:
    using Pointer_T = Pointer<float>;

    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Pointer_T>::is_descendant(value))
          {
            return Convertible::Exact;
          }
          [[fallthrough]];
        }
        default:
        {
          return FromRubyFundamental<float>::is_convertible(value);
        }
      }
    }

    float& convert(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Pointer_T>::is_descendant(value))
          {
            return (float&)*unwrap<Pointer_T>(value, Data_Type<Pointer_T>::ruby_data_type(), false);
          }
          [[fallthrough]];
        }
        default:
        {
          this->converted_ = FromRubyFundamental<float>::convert(value);
          return this->converted_;
        }
      }
    }

  private:
    Arg* arg_ = nullptr;
    float converted_;
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
      return FromRubyFundamental<int>::convert(value);
    }
  
  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<int&>
  {
  public:
    using Pointer_T = Pointer<int>;

    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Pointer_T>::is_descendant(value))
          {
            return Convertible::Exact;
          }
          [[fallthrough]];
        }
        default:
        {
          return FromRubyFundamental<int>::is_convertible(value);
        }
      }
    }

    int& convert(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Pointer_T>::is_descendant(value))
          {
            return (int&)*unwrap<Pointer_T>(value, Data_Type<Pointer_T>::ruby_data_type(), false);
          }
          [[fallthrough]];
        }
        default:
        {
          this->converted_ = FromRubyFundamental<int>::convert(value);
          return this->converted_;
        }
      }
    }

  private:
    Arg* arg_ = nullptr;
    int converted_ = 0;
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
      return FromRubyFundamental<unsigned int>::convert(value);
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<unsigned int&>
  {
  public:
    using Pointer_T = Pointer<unsigned int>;

    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Pointer_T>::is_descendant(value))
          {
            return Convertible::Exact;
          }
          [[fallthrough]];
        }
        default:
        {
          return FromRubyFundamental<unsigned int>::is_convertible(value);
        }
      }
    }

    unsigned int& convert(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Pointer_T>::is_descendant(value))
          {
            return (unsigned int&)*unwrap<Pointer_T>(value, Data_Type<Pointer_T>::ruby_data_type(), false);
          }
          [[fallthrough]];
        }
        default:
        {
          this->converted_ = FromRubyFundamental<unsigned int>::convert(value);
          return this->converted_;
        }
      }
    }

  private:
    Arg* arg_ = nullptr;
    unsigned int converted_ = 0;
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
      return FromRubyFundamental<long>::convert(value);
    }
  
  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<long&>
  {
  public:
    using Pointer_T = Pointer<long>;

    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Pointer_T>::is_descendant(value))
          {
            return Convertible::Exact;
          }
          [[fallthrough]];
        }
        default:
        {
          return FromRubyFundamental<long>::is_convertible(value);
        }
      }
    }

    long& convert(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Pointer_T>::is_descendant(value))
          {
            return (long&)*unwrap<Pointer_T>(value, Data_Type<Pointer_T>::ruby_data_type(), false);
          }
          [[fallthrough]];
        }
        default:
        {
          this->converted_ = FromRubyFundamental<long>::convert(value);
          return this->converted_;
        }
      }
    }

  private:
    Arg* arg_ = nullptr;
    long converted_ = 0;
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
    using Pointer_T = Pointer<unsigned long>;

    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Pointer_T>::is_descendant(value))
          {
            return Convertible::Exact;
          }
          [[fallthrough]];
        }
        default:
        {
          return FromRubyFundamental<unsigned long>::is_convertible(value);
        }
      }
    }

    unsigned long& convert(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Pointer_T>::is_descendant(value))
          {
            return (unsigned long&)*unwrap<Pointer_T>(value, Data_Type<Pointer_T>::ruby_data_type(), false);
          }
          [[fallthrough]];
        }
        default:
        {
          this->converted_ = FromRubyFundamental<unsigned long>::convert(value);
          return this->converted_;
        }
      }
    }

  private:
    Arg* arg_ = nullptr;
    unsigned long converted_ = 0;
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
    using Pointer_T = Pointer<unsigned long long>;

    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Pointer_T>::is_descendant(value))
          {
            return Convertible::Exact;
          }
          [[fallthrough]];
        }
        default:
        {
          return FromRubyFundamental<unsigned long long>::is_convertible(value);
        }
      }
    }

    unsigned long long& convert(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Pointer_T>::is_descendant(value))
          {
            return (unsigned long long&)*unwrap<Pointer_T>(value, Data_Type<Pointer_T>::ruby_data_type(), false);
          }
          [[fallthrough]];
        }
        default:
        {
          this->converted_ = FromRubyFundamental<unsigned long long>::convert(value);
          return this->converted_;
        }
      }
    }

  private:
    Arg* arg_ = nullptr;
    unsigned long long converted_ = 0;
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
      return FromRubyFundamental<long long>::convert(value);
    }
  
  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<long long&>
  {
  public:
    using Pointer_T = Pointer<long long>;

    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Pointer_T>::is_descendant(value))
          {
            return Convertible::Exact;
          }
          [[fallthrough]];
        }
        default:
        {
          return FromRubyFundamental<long long>::is_convertible(value);
        }
      }
    }

    long long& convert(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Pointer_T>::is_descendant(value))
          {
            return (long long&)*unwrap<Pointer_T>(value, Data_Type<Pointer_T>::ruby_data_type(), false);
          }
          [[fallthrough]];
        }
        default:
        {
          this->converted_ = FromRubyFundamental<long long>::convert(value);
          return this->converted_;
        }
      }
    }

  private:
    Arg* arg_ = nullptr;
    long long converted_ = 0;
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
      return FromRubyFundamental<short>::convert(value);
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<short&>
  {
  public:
    using Pointer_T = Pointer<short>;

    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Pointer_T>::is_descendant(value))
          {
            return Convertible::Exact;
          }
          [[fallthrough]];
        }
        default:
        {
          return FromRubyFundamental<short>::is_convertible(value);
        }
      }
    }

    short& convert(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Pointer_T>::is_descendant(value))
          {
            return (short&)*unwrap<Pointer_T>(value, Data_Type<Pointer_T>::ruby_data_type(), false);
          }
          [[fallthrough]];
        }
        default:
        {
          this->converted_ = FromRubyFundamental<short>::convert(value);
          return this->converted_;
        }
      }
    }

  private:
    Arg* arg_ = nullptr;
    short converted_ = 0;
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
      return FromRubyFundamental<unsigned short>::convert(value);
    }
  
  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<unsigned short&>
  {
  public:
    using Pointer_T = Pointer<unsigned short>;

    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Pointer_T>::is_descendant(value))
          {
            return Convertible::Exact;
          }
          [[fallthrough]];
        }
        default:
        {
          return FromRubyFundamental<unsigned short>::is_convertible(value);
        }
      }
    }

    unsigned short& convert(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Pointer_T>::is_descendant(value))
          {
            return (unsigned short&)*unwrap<Pointer_T>(value, Data_Type<Pointer_T>::ruby_data_type(), false);
          }
          [[fallthrough]];
        }
        default:
        {
          this->converted_ = FromRubyFundamental<unsigned short>::convert(value);
          return this->converted_;
        }
      }
    }

  private:
    Arg* arg_ = nullptr;
    unsigned short converted_ = 0;
  };

  // ===========  std::nullptr_t  ============
  template<>
  class From_Ruby<std::nullptr_t>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
      if (this->arg_->isOwner())
      {
        throw Exception(rb_eTypeError, "Cannot transfer ownership of C++ void pointer");
      }
    }

    Convertible is_convertible(VALUE value)
    {
      if (this->arg_ && this->arg_->isOpaque())
      {
        return Convertible::Exact;
      }

      switch (rb_type(value))
      {
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
      if (value == Qnil)
      {
        return nullptr;
      }

      if (this->arg_ && this->arg_->isOpaque())
      {
        return (void*)value;
      }

      switch (rb_type(value))
      {
        case RUBY_T_NIL:
        {
          return nullptr;
          break;
        }
        default:
        {
          throw Exception(rb_eTypeError, "wrong argument type %s (expected % s)",
            detail::protect(rb_obj_classname, value), "nil");
        }
      }
    }
  private:
    Arg* arg_ = nullptr;
  };

  // ===========  void  ============
  template<>
  class From_Ruby<void*>
  {
  public:
    From_Ruby()
    {
    };

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
      if (this->arg_->isOwner())
      {
        throw Exception(rb_eTypeError, "Cannot transfer ownership of C++ void pointer");
      }
    }

    Convertible is_convertible(VALUE value)
    {
      if (this->arg_ && this->arg_->isOpaque())
      {
        return Convertible::Exact;
      }

      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          return Convertible::Cast;
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
      if (value == Qnil)
      {
        return nullptr;
      }

      if (this->arg_ && this->arg_->isOpaque())
      {
        return (void*)value;
      }

      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          // Since C++ is not telling us type information, we need to extract it
          // from the Ruby object.
          const rb_data_type_t* rb_type = RTYPEDDATA_TYPE(value);

          if (Data_Type<Pointer<void>>::is_descendant(value))
          {
            return unwrap<void>(value, Data_Type<Pointer<void>>::ruby_data_type(), false);
          }
          else 
          {
            return detail::unwrap<void>(value, (rb_data_type_t*)rb_type, this->arg_ && this->arg_->isOwner());
          }

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
        {
          throw Exception(rb_eTypeError, "wrong argument type %s (expected % s)",
            detail::protect(rb_obj_classname, value), "pointer");
        }
      }
    }
  private:
    Arg* arg_ = nullptr;
  };
}
// =========   Proc.hpp   =========


// Registries

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

    detail::TypeMapper<T> typeMapper;
    raiseUnverifiedType(typeMapper.name());
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
      detail::TypeMapper<int> typeMapper;
      stream << "  " << typeMapper.name(typeIndex) << "\n";
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

  inline VALUE TypeRegistry::klasses()
  {
    Array result;

    for (auto& pair : this->registry_)
    {
      std::pair<VALUE, rb_data_type_t*>& value = pair.second;
      Class klass = value.first;
      result.push(klass, false);
    }
    return result;
  }
}
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

// =========   DefaultHandler.ipp   =========
namespace Rice::detail
{
  inline void Rice::detail::DefaultHandler::operator()() const
  {
    // This handler does nothing, it just rethrows the exception so it can be handled
    throw;
  }
}
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
// =========   ModuleRegistry.ipp   =========
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <typeindex>


namespace Rice::detail
{
  inline void ModuleRegistry::add(VALUE module)
  {
    this->modules_.insert(module);
  }

  inline VALUE ModuleRegistry::modules()
  {
    Array result;

    for (const VALUE& value : this->modules_)
    {
      Module module(value);
      result.push(module, false);
    }
    return result;
  }
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
  
  inline const std::vector<Native*> NativeRegistry::lookup(VALUE klass)
  {
    std::vector<Native*> result;

    if (rb_type(klass) == T_ICLASS)
    {
      klass = detail::protect(rb_class_of, klass);
    }

    for (auto& pair : this->natives_)
    {
      const std::pair<VALUE, ID>& key = pair.first;

      if (klass == key.first)
      {
        const std::vector<std::unique_ptr<Native>>& value = pair.second;
        for (auto& native : value)
        {
          result.push_back(native.get());
        }
      }
    }

    return result;
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

// Rice saves types either as the intrinsic type (MyObject) or pointer (MyObject*).
// It strips out references, const and volatile to avoid an explosion of template classes.
// Pointers are used for C function pointers used in callbacks and for the Buffer class.
namespace Rice::detail
{
  // ------ Type ----------------
  template<typename T>
  inline bool Type<T>::verify()
  {
    return Registries::instance.types.verify<intrinsic_type<T>>();
  }

  template<typename T>
  inline bool Type<T&>::verify()
  {
    return Type<T>::verify();
  }

  template<typename T>
  inline bool Type<T&&>::verify()
  {
    return Type<T>::verify();
  }

  template<typename T>
  inline bool Type<T*>::verify()
  {
    return Type<T>::verify();
  }

  template<typename T>
  inline bool Type<T**>::verify()
  {
    return Type<T>::verify();
  }

  template<typename T>
  void verifyType()
  {
    Type<remove_cv_recursive_t<T>>::verify();
  }

  template<typename Tuple_T, size_t...Is>
  void verifyTypesImpl(std::index_sequence<Is...> indexes)
  {
    (verifyType<std::tuple_element_t<Is, Tuple_T>>(), ...);
  }

  template<typename Tuple_T>
  void verifyTypes()
  {
    std::make_index_sequence<std::tuple_size_v<Tuple_T>> indexes;
    verifyTypesImpl<Tuple_T>(indexes);
  }

  // ---------- RubyKlass ------------
  // Helper template to see if the method rubyKlass is defined on a Type specialization
  template<typename, typename = std::void_t<>>
  struct has_ruby_klass : std::false_type
  {
  };

  template<typename T>
  struct has_ruby_klass<T, std::void_t<decltype(T::rubyKlass())>> : std::true_type
  {
  };

  // ---------- TypeMapper ------------
  template<typename T>
  inline std::string TypeMapper<T>::demangle(char const* mangled_name)
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

  template<typename T>
  inline std::string TypeMapper<T>::name()
  {
    const std::type_index& typeIndex = typeid(T);
    return demangle(typeIndex.name());
  }

  template<typename T>
  inline std::string TypeMapper<T>::name(const std::type_index& typeIndex)
  {
    return demangle(typeIndex.name());
  }

  // Find text inside of < > taking into account nested groups.
  // 
  // Example:
  //  
  //   std::vector<std::vector<int>, std::allocator<std::vector, std::allocator<int>>>
  template<typename T>
  inline std::string TypeMapper<T>::findGroup(std::string& string, size_t offset)
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

  template<typename T>
  inline void TypeMapper<T>::replaceAll(std::string& string, std::regex regex, std::string replacement)
  {
    std::smatch match;
    while (std::regex_search(string, match, regex))
    {
      string = std::regex_replace(string, regex, replacement);
    }
  }

  template<typename T>
  inline void TypeMapper<T>::removeGroup(std::string& string, std::regex regex)
  {
    std::smatch match;
    while (std::regex_search(string, match, regex))
    {
      std::string group = findGroup(string, match.position());
      group = match.str() + group;
      string.erase(match.position(), group.length());
    }
  }

  template<typename T>
  inline void TypeMapper<T>::replaceGroup(std::string& string, std::regex regex, std::string replacement)
  {
    std::smatch match;
    while (std::regex_search(string, match, regex))
    {
      std::string group = findGroup(string, match.position());
      group = match.str() + group;
      string.replace(match.position(), group.length(), replacement);
    }
  }

  template<typename T>
  inline std::string TypeMapper<T>::simplifiedName()
  {
    std::string base = this->name();

    // Remove void from Buffer<T, void> - the void comes from SFINAE
    std::regex fixBuffer = std::regex("(Buffer<.*),\\s?void>");
    base = std::regex_replace(base, fixBuffer, "$1>");

    // Remove class keyword
    std::regex classRegex = std::regex("class +");
    base = std::regex_replace(base, classRegex, "");

    // Remove struct keyword
    std::regex structRegex = std::regex("struct +");
    base = std::regex_replace(base, structRegex, "");

    // Remove std::__[^:]*::
    std::regex stdClangRegex = std::regex("std::__[^:]+::");
    base = std::regex_replace(base, stdClangRegex, "std::");

    // Remove allocators
    std::regex allocatorRegex(R"(,\s*std::allocator)");
    removeGroup(base, allocatorRegex);

    // Remove char_traits
    std::regex charTraitsRegex(R"(,\s*std::char_traits)");
    removeGroup(base, charTraitsRegex);

    // Remove less (std::map)
    std::regex lessRegex(R"(,\s*std::less)");
    removeGroup(base, lessRegex);

    // Remove hash (std::unordered_map)
    std::regex hashRegex(R"(,\s*std::hash)");
    removeGroup(base, hashRegex);

    // Remove equal_to (std::unordered_map)
    std::regex equalRegex(R"(,\s*std::equal_to)");
    removeGroup(base, equalRegex);

    // Remove spaces before pointers
    std::regex ptrRegex = std::regex(R"(\s+\*)");
    base = std::regex_replace(base, ptrRegex, "*");

    // Remove __ptr64
    std::regex ptr64Regex(R"(\s*__ptr64\s*)");
    base = std::regex_replace(base, ptr64Regex, "");

    // Replace " >" with ">"
    std::regex trailingAngleBracketSpaceRegex = std::regex(R"(\s+>)");
    replaceAll(base, trailingAngleBracketSpaceRegex, ">");

    // One space after a comma (MSVC has no spaces, GCC one space)
    std::regex commaSpaceRegex = std::regex(R"(,(\S))");
    replaceAll(base, commaSpaceRegex, ", $1");

    // Fix strings
    std::regex stringRegex = std::regex(R"(basic_string<char>)");
    replaceAll(base, stringRegex, "string");

    std::regex wstringRegex = std::regex(R"(basic_string<wchar_t>)");
    replaceAll(base, wstringRegex, "wstring");

    // Normalize Anonymous namespace
    std::regex anonymousNamespaceGcc = std::regex(R"(\(anonymous namespace\))");
    replaceAll(base, anonymousNamespaceGcc, "AnonymousNamespace");
    std::regex anonymousNamespaceMsvc = std::regex(R"(`anonymous namespace')");
    replaceAll(base, anonymousNamespaceMsvc, "AnonymousNamespace");

    return base;
  }

  template<typename T>
  inline void TypeMapper<T>::capitalizeHelper(std::string& content, std::regex& regex)
  {
    std::smatch match;
    while (std::regex_search(content, match, regex))
    {
      std::string replacement = match[1];
      std::transform(replacement.begin(), replacement.end(), replacement.begin(), ::toupper);
      content.replace(match.position(), match.length(), replacement);
    }
  }

  template<typename T>
  inline std::string TypeMapper<T>::rubyTypeName()
  {
    using Intrinsic_T = detail::intrinsic_type<T>;

    if constexpr (std::is_fundamental_v<T>)
    {
      return RubyType<Intrinsic_T>::name;
    }
    else if constexpr (std::is_same_v<std::remove_cv_t<T>, char*>)
    {
      return "String";
    }
    else
    {
      detail::TypeMapper<Intrinsic_T> typeIntrinsicMapper;
      return typeIntrinsicMapper.simplifiedName();
    }
  }

  template<typename T>
  inline std::string TypeMapper<T>::rubyName()
  {
    std::string base = this->rubyTypeName();

    // Remove std:: these could be embedded in template types
    auto stdRegex = std::regex("std::");
    base = std::regex_replace(base, stdRegex, "");

    // Remove leading namespaces. This will not remove namespaces
    // embedded in template types like std::vector<mynamespace::foo>
    auto leadingNamespacesRegex = std::regex("^[^<]*::");
    base = std::regex_replace(base, leadingNamespacesRegex, "");

    // Capitalize first letter
    base[0] = std::toupper(base[0]);

    // Replace :: with unicode U+u02F8 (Modified Letter raised colon)
    auto colonRegex = std::regex(R"(:)");
    replaceAll(base, colonRegex, "\uA789");

    // Replace _ and capitalize the next letter
    std::regex underscoreRegex(R"(_(\w))");
    capitalizeHelper(base, underscoreRegex);

    if constexpr (std::is_fundamental_v<intrinsic_type<T>>)
    {
      // Replace space and capitalize the next letter
      std::regex spaceRegex(R"(\s+(\w))");
      capitalizeHelper(base, spaceRegex);
    }
    else
    {
      // Replace spaces with unicode U+u00A0 (Non breaking Space)
      std::regex spaceRegex = std::regex(R"(\s+)");
      replaceAll(base, spaceRegex, "\u00A0");
    }

    // Replace < with unicode U+227A (Precedes)
    auto lessThanRegex = std::regex("<");
    //replaceAll(base, lessThanRegex, "≺");
    replaceAll(base, lessThanRegex, "\u227A");

    // Replace > with unicode U+227B (Succeeds)
    auto greaterThanRegex = std::regex(">");
    //replaceAll(base, greaterThanRegex, "≻");
    replaceAll(base, greaterThanRegex, "\u227B");

    // Replace , with Unicode Character (U+066C) - Arabic Thousands Separator
    auto commaRegex = std::regex(R"(,\s*)");
    replaceAll(base, commaRegex, "\u201A");

    // Replace * with Unicode Character (U+2217) -	Asterisk Operator
    auto asteriskRegex = std::regex(R"(\*)");
    replaceAll(base, asteriskRegex, "\u2217");

    return base;
  }

  template<typename T>
  inline VALUE TypeMapper<T>::rubyKlass()
  {
    using Type_T = Type<std::remove_reference_t<detail::remove_cv_recursive_t<T>>>;
    using Intrinsic_T = detail::intrinsic_type<T>;

    if constexpr (has_ruby_klass<Type_T>::value)
    {
      return Type_T::rubyKlass();
    }
    else if constexpr (std::is_fundamental_v<Intrinsic_T> && std::is_pointer_v<T>)
    {
			using Pointer_T = Pointer<std::remove_pointer_t<remove_cv_recursive_t<T>>>;
      std::pair<VALUE, rb_data_type_t*> pair = Registries::instance.types.getType<Pointer_T>();
      return pair.first;
    }
    else
    {
      std::pair<VALUE, rb_data_type_t*> pair = Registries::instance.types.getType<Intrinsic_T>();
      return pair.first;
    }
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
  inline bool WrapperBase::isConst()
  {
    return this->isConst_;
  }

  inline void WrapperBase::ruby_mark()
  {
    for (VALUE value : this->keepAlive_)
    {
      rb_gc_mark(value);
    }
  }

  inline void WrapperBase::addKeepAlive(VALUE value)
  {
    this->keepAlive_.push_back(value);
  }

  inline void WrapperBase::setOwner(bool value)
  {
    this->isOwner_ = value;
  }

  // ----  Wrapper -----
  template <typename T>
  inline Wrapper<T>::Wrapper(T& data): data_(data)
  {
  }

  template <typename T>
  inline Wrapper<T>::Wrapper(T&& data) : data_(std::move(data))
  {
  }

  template <typename T>
  inline Wrapper<T>::~Wrapper()
  {
    Registries::instance.instances.remove(this->get());
  }

  template <typename T>
  inline void* Wrapper<T>::Wrapper::get()
  {
    return (void*)&this->data_;
  }

  // ----  Wrapper& -----
  template <typename T>
  inline Wrapper<T&>::Wrapper(T& data): data_(data)
  {
    this->isConst_ = std::is_const_v<std::remove_reference_t<T>>;
  }

  template <typename T>
  inline Wrapper<T&>::~Wrapper()
  {
    Registries::instance.instances.remove(this->get());
  }

  template <typename T>
  inline void* Wrapper<T&>::get()
  {
    return (void*)&this->data_;
  }

  // ----  Wrapper* -----
  template <typename T>
  inline Wrapper<T*>::Wrapper(T* data, bool isOwner) : data_(data)
  {
    this->isOwner_ = isOwner;
    this->isConst_ = std::is_const_v<std::remove_pointer_t<T>>;
  }

  template <typename T>
  inline Wrapper<T*>::~Wrapper()
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

  template <typename T>
  inline void* Wrapper<T*>::get()
  {
    return (void*)this->data_;
  }

  // ----  Wrapper** -----
  template <typename T>
  inline Wrapper<T**>::Wrapper(T** data, bool isOwner) : data_(data)
  {
    this->isOwner_ = isOwner;
    this->isConst_ = std::is_const_v<std::remove_pointer_t<std::remove_pointer_t<T>>>;
  }

  template <typename T>
  inline Wrapper<T**>::~Wrapper()
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

  template <typename T>
  inline void* Wrapper<T**>::get()
  {
    return (void*)this->data_;
  }

  // ---- Helper Functions -------
  template <typename T>
  inline VALUE wrap(VALUE klass, rb_data_type_t* rb_data_type, T& data, bool isOwner)
  {
    VALUE result = Registries::instance.instances.lookup(&data);

    if (result != Qnil)
      return result;

    WrapperBase* wrapper = nullptr;

    // If Ruby is not the owner then wrap the reference
    if (!isOwner)
    {
      wrapper = new Wrapper<T&>(data);
      result = TypedData_Wrap_Struct(klass, rb_data_type, wrapper);
    }

    // std::is_copy_constructible_v<std::vector<std::unique_ptr<T>>>> return true. Sigh.
    else if constexpr (detail::is_std_vector_v<T>)
    {
      if constexpr (std::is_copy_constructible_v<typename T::value_type>)
      {
        wrapper = new Wrapper<T>(data);
        result = TypedData_Wrap_Struct(klass, rb_data_type, wrapper);
      }
      else
      {
        wrapper = new Wrapper<T>(std::move(data));
        result = TypedData_Wrap_Struct(klass, rb_data_type, wrapper);
      }
    }

    // Ruby is the owner so copy data
    else if constexpr (std::is_copy_constructible_v<T>)
    {
      wrapper = new Wrapper<T>(data);
      result = TypedData_Wrap_Struct(klass, rb_data_type, wrapper);
    }

    // Ruby is the owner so move data
    else if constexpr (std::is_move_constructible_v<T>)
    {
      wrapper = new Wrapper<T>(std::move(data));
      result = TypedData_Wrap_Struct(klass, rb_data_type, wrapper);
    }

    else
    {
      detail::TypeMapper<T> typeMapper;
      std::string message = "Rice was directed to take ownership of a C++ object but it does not have an accessible copy or move constructor. Type: " +
        typeMapper.name();
      throw std::runtime_error(message);
    }

    Registries::instance.instances.add(wrapper->get(), result);

    return result;
  };

  template <typename T>
  inline VALUE wrap(VALUE klass, rb_data_type_t* rb_data_type, T* data, bool isOwner)
  {
    VALUE result = Registries::instance.instances.lookup(data);

    if (result != Qnil)
      return result;

    WrapperBase* wrapper = new Wrapper<T*>(data, isOwner);
    result = TypedData_Wrap_Struct(klass, rb_data_type, wrapper);

    Registries::instance.instances.add(wrapper->get(), result);
    return result;
  };

  template <typename T>
  inline T* unwrap(VALUE value, rb_data_type_t* rb_data_type, bool takeOwnership)
  {
    if (rb_type(value) != RUBY_T_DATA)
    {
      std::string message = "The provided Ruby object does not wrap a C++ object";
      throw std::runtime_error(message);
    }

    WrapperBase* wrapper = getWrapper(value, rb_data_type);

    if (wrapper == nullptr)
    {
      std::string message = "Wrapped C++ object is nil. Did you override " + 
                            std::string(detail::protect(rb_obj_classname, value)) + 
                            "#initialize and forget to call super?";

      throw std::runtime_error(message);
    }

    if (takeOwnership)
      wrapper->setOwner(false);

    return static_cast<T*>(wrapper->get());
  }
    
  template <typename Wrapper_T>
  inline Wrapper_T* getWrapper(VALUE value, rb_data_type_t* rb_data_type)
  {
    WrapperBase* wrapper = nullptr;
    TypedData_Get_Struct(value, WrapperBase, rb_data_type, wrapper);
    return dynamic_cast<Wrapper_T*>(wrapper);
  }

  inline WrapperBase* getWrapper(VALUE value)
  {
    // Turn off spurious warning on g++ 12
    #if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Warray-bounds"
    #endif

    return RTYPEDDATA_P(value) ? static_cast<WrapperBase*>(RTYPEDDATA_DATA(value)) : nullptr;
    
    #if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic pop
    #endif
  }

  template <typename T>
  inline void wrapConstructed(VALUE value, rb_data_type_t* rb_data_type, T* data, bool isOwner)
  {
    using Wrapper_T = Wrapper<T*>;
    
    Wrapper_T* wrapper = nullptr;
    TypedData_Get_Struct(value, Wrapper_T, rb_data_type, wrapper);
    if (wrapper)
    {
      Registries::instance.instances.remove(wrapper->get());
      delete wrapper;
    }

    wrapper = new Wrapper_T(data, true);
    RTYPEDDATA_DATA(value) = wrapper;

    Registries::instance.instances.add(data, value);
  }
} // namespace

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
    if constexpr (std::is_same_v<Arg_T, Return>)
    {
      this->returnInfo_ = arg;
    }
    else if constexpr (std::is_same_v<Arg_T, Function>)
    {
      this->function_ = arg;
    }
    else if constexpr (std::is_same_v<Arg_T, Arg>)
    {
      this->addArg(arg);
    }
    else
    {
      static_assert(true, "Unknown argument type");
    }
  }

  inline void MethodInfo::addArg(const Arg& arg)
  {
    this->args_.push_back(arg);
  }

  inline int MethodInfo::argCount()
  {
    return this->args_.size();
  }

  inline Arg* MethodInfo::arg(size_t pos)
  {
    if (pos < this->args_.size())
    {
      return &this->args_[pos];
    }
    else
    {
      return nullptr;
    }
  }

  inline Arg* MethodInfo::arg(std::string name)
  {
    for (Arg& arg : this->args_)
    {
      if (arg.name == name)
      {
        return &arg;
      }
    }
    return nullptr;
  }

  inline Return* MethodInfo::returnInfo()
  {
    return &this->returnInfo_;
  }
  
  inline Function* MethodInfo::function()
  {
    return &this->function_;
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
        rb_enc_raise(rb_utf8_encoding(), rb_eArgError, "Could not find method call for %s#%s", rb_class2name(klass), identifier.c_str());
      }
      else
      {
        Identifier identifier(methodId);

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

        // Get the first one
        Resolved resolved = resolves.front();

        // Was there more than one match?
        /*size_t count = std::count_if(resolves.begin(), resolves.end(),
          [&resolved](Resolved& element)
          {
            return resolved.convertible == element.convertible;
          });

        if (count > 1)
        {
          std::ostringstream message;
          message << "Could not resolve method call for %s#%s" << "\n"
                  << "  %d overloaded functions matched based on the types of Ruby parameters provided:";

          for (int i = 0; i < count; i++)
          {
            message << "\n     " << resolves[i].native->toString();
          }
          
          rb_raise(rb_eArgError, message.str().c_str(), rb_class2name(klass), identifier.c_str(), count);
        }*/

        // Did it match?
        if (resolved.convertible != Convertible::None)
        {
          native = resolved.native;
        }
        else
        {
          // Special case == to make the RubyMine debugger work. It calls == with a Module as
          // the other argument, thus breaking if C++ operator== is implemented.
          if (identifier.str() == "==")
          {
            return detail::protect(rb_call_super, argc, argv);
          }
          else
          {
            std::ostringstream message;
            message << "Could not resolve method call for %s#%s" << "\n"
                    << "  %d overload(s) were evaluated based on the types of Ruby parameters provided:";

            for (Resolved& resolve: resolves)
            {
              message << "\n     " << resolve.native->toString();
            }

            rb_enc_raise(rb_utf8_encoding(), rb_eArgError, message.str().c_str(), rb_class2name(klass), identifier.c_str(), natives.size());
          }
        }
      }

      // Call the C++ function
      return (*native)(argc, argv, self);
    });
  }

  inline Native::Native(std::vector<std::unique_ptr<ParameterAbstract>>&& parameters) : parameters_(std::move(parameters))
  {
  }

  inline ParameterAbstract* Native::getParameterByName(std::string name)
  {
    for (std::unique_ptr<ParameterAbstract>& parameter : this->parameters_)
    {
      if (parameter->arg->name == name)
      {
        return parameter.get();
      }
    }

    return nullptr;
  }

  // -----------  Helpers ----------------
  template<typename T>
  inline void Native::verify_type(bool isBuffer)
  {
    using Base_T = std::remove_pointer_t<remove_cv_recursive_t<T>>;

    detail::verifyType<T>();

    if constexpr (std::is_pointer_v<T> && std::is_fundamental_v<std::remove_pointer_t<T>>)
    {
      Type<Pointer<std::remove_pointer_t<T>>>::verify();
      Type<Buffer<std::remove_pointer_t<T>>>::verify();
    }
    if constexpr (std::is_array_v<T>)
    {
      Type<Pointer<std::remove_extent_t<remove_cv_recursive_t<T>>>>::verify();
      Type<Buffer<std::remove_extent_t<remove_cv_recursive_t<T>>>>::verify();
    }
    else if (isBuffer)
    {
      if constexpr (std::is_pointer_v<T> && !std::is_function_v<Base_T> && !std::is_abstract_v<Base_T>)
      {
        Type<Pointer<Base_T>>::verify();
        Type<Buffer<Base_T>>::verify();
      }
      else
      {
        static_assert(true, "Only pointer types can be marked as buffers");
      }
    }
  }

  template<typename Tuple_T, std::size_t ...Indices>
  inline void Native::verify_args(MethodInfo* methodInfo, std::index_sequence<Indices...> indices)
  {
    (Native::verify_type<std::tuple_element_t<Indices, Tuple_T>>(methodInfo->arg(Indices)->isBuffer()), ...);
  }

  template<typename Tuple_T, std::size_t ...Indices>
  inline void Native::create_parameters_impl(std::vector<std::unique_ptr<ParameterAbstract>>& parameters, MethodInfo* methodInfo, std::index_sequence<Indices...> indices)
  {
    (parameters.push_back(std::move(std::make_unique<Parameter<std::tuple_element_t<Indices, Tuple_T>>>(methodInfo->arg(Indices)))), ...);
  }

  template<typename Tuple_T>
  inline std::vector<std::unique_ptr<ParameterAbstract>> Native::create_parameters(MethodInfo* methodInfo)
  {
    std::vector<std::unique_ptr<ParameterAbstract>> result;
    auto indices = std::make_index_sequence<std::tuple_size_v<Tuple_T>>{};
    Native::create_parameters_impl<Tuple_T>(result, methodInfo, indices);
    return result;
  }

  inline std::vector<std::optional<VALUE>> Native::getRubyValues(size_t argc, const VALUE* argv, bool validate)
  {
#undef max
    int size = std::max(this->parameters_.size(), argc);
    std::vector<std::optional<VALUE>> result(size);

    // Keyword handling
    if (rb_keyword_given_p())
    {
      // Keywords are stored in the last element in a hash
      int actualArgc = argc - 1;

      VALUE value = argv[actualArgc];
      Hash keywords(value);

      // Copy over leading non-keyword arguments
      for (int i = 0; i < actualArgc; i++)
      {
        result[i] = argv[i];
      }

      // Copy over keyword arguments
      for (auto pair : keywords)
      {
        Symbol key(pair.first);
        ParameterAbstract* parameter = this->getParameterByName(key.str());
        if (!parameter)
        {
          throw std::invalid_argument("Unknown keyword: " + key.str());
        }

        const Arg* arg = parameter->arg;

        result[arg->position] = pair.second.value();
      }
    }
    else
    {
      std::copy(argv, argv + argc, result.begin());
    }

    // Block handling. If we find a block and the last parameter is missing then
    // set it to the block
    if (rb_block_given_p() && result.size() > 0 && !result.back().has_value())
    {
      VALUE proc = rb_block_proc();
      result.back() = proc;
    }

    if (validate)
    {
      // Protect against user sending too many arguments
      if (argc > this->parameters_.size())
      {
        std::string message = "wrong number of arguments (given " +
          std::to_string(argc) + ", expected " + std::to_string(this->parameters_.size()) + ")";
        throw std::invalid_argument(message);
      }

      for (size_t i = 0; i < result.size(); i++)
      {
        std::optional<VALUE> value = result[i];
        ParameterAbstract* parameter = this->parameters_[i].get();

        if (!parameter->arg->hasDefaultValue() && !value.has_value())
        {
          std::string message;
          message = "Missing argument. Name: " + parameter->arg->name + ". Index: " + std::to_string(parameter->arg->position) + ".";
          throw std::invalid_argument(message);
        }
      }
    }

    return result;
  }

  inline Convertible Native::matchParameters(std::vector<std::optional<VALUE>>& values)
  {
    Convertible result = Convertible::Exact;
    for (size_t i = 0; i < this->parameters_.size(); i++)
    {
      ParameterAbstract* parameter = this->parameters_[i].get();
      std::optional<VALUE>& value = values[i];
      result = result & parameter->matches(value);
    }
    return result;
  }

  inline Resolved Native::matches(size_t argc, const VALUE* argv, VALUE self)
  {
    // Return false if Ruby provided more arguments than the C++ method takes
    if (argc > this->parameters_.size())
      return Resolved{ Convertible::None, 0, this };

    Resolved result{ Convertible::Exact, 1, this };

    std::vector<std::optional<VALUE>> rubyValues = this->getRubyValues(argc, argv, false);
    result.convertible = this->matchParameters(rubyValues);

    if (this->parameters_.size() > 0)
    {
      int providedValues = std::count_if(rubyValues.begin(), rubyValues.end(), [](std::optional<VALUE>& value)
        {
          return value.has_value();
        });

      result.parameterMatch = providedValues / (double)this->parameters_.size();
    }
    return result;
  }

  inline std::vector<const ParameterAbstract*> Native::parameters()
  {
    std::vector<const ParameterAbstract*> result;

    std::transform(this->parameters_.begin(), this->parameters_.end(), std::back_inserter(result),
      [](std::unique_ptr<ParameterAbstract>& parameter) -> ParameterAbstract*
      {
        return parameter.get();
      });
      
    return result;
  }
}

// =========   NativeAttributeGet.ipp   =========
#include <array>
#include <algorithm>

namespace Rice::detail
{
  template<typename Attribute_T>
  void NativeAttributeGet<Attribute_T>::define(VALUE klass, std::string name, Attribute_T attribute, Return returnInfo)
  {
    // Verify attribute type
    Native::verify_type<Attr_T>(returnInfo.isBuffer());

    // Create a NativeAttributeGet that Ruby will call to read/write C++ variables
    NativeAttribute_T* nativeAttribute = new NativeAttribute_T(klass, name, std::forward<Attribute_T>(attribute), returnInfo);
    std::unique_ptr<Native> native(nativeAttribute);

    detail::protect(rb_define_method, klass, name.c_str(), (RUBY_METHOD_FUNC)&Native::resolve, -1);

    // Add to native registry. Since attributes cannot be overridden, there is no need to set the
    // matches or calls function pointer. Instead Ruby can call the static call method defined on
    // this class (&NativeAttribute_T::get).
    Identifier identifier(name);
    detail::Registries::instance.natives.add(klass, identifier.id(), native);
  }

  template<typename Attribute_T>
  inline Resolved NativeAttributeGet<Attribute_T>::matches(size_t argc, const VALUE* argv, VALUE self)
  {
    if (argc == 0)
      return Resolved { Convertible::Exact, 1, this };
    else
      return Resolved{ Convertible::None, 0, this };
  }
  
  template<typename Attribute_T>
  NativeAttributeGet<Attribute_T>::NativeAttributeGet(VALUE klass, std::string name, Attribute_T attribute, Return returnInfo)
    : klass_(klass), name_(name), attribute_(attribute), return_(returnInfo)
  {
  }

  template<typename Attribute_T>
  inline VALUE NativeAttributeGet<Attribute_T>::operator()(size_t argc, const VALUE* argv, VALUE self)
  {
    if constexpr (std::is_member_object_pointer_v<Attribute_T>)
    {
      Receiver_T* nativeSelf = From_Ruby<Receiver_T*>().convert(self);

      if constexpr (std::is_fundamental_v<detail::intrinsic_type<To_Ruby_T>>)
      {
        return To_Ruby<To_Ruby_T>(&this->return_).convert(nativeSelf->*attribute_);
      }
      else if constexpr (std::is_array_v<To_Ruby_T>)
      {
        return To_Ruby<To_Ruby_T>(&this->return_).convert(nativeSelf->*attribute_);
      }
      else if constexpr (std::is_pointer_v<To_Ruby_T>)
      {
        return To_Ruby<To_Ruby_T>(&this->return_).convert(nativeSelf->*attribute_);
      }
      else
      {
        // If the attribute is an object return a reference to avoid a copy (and avoid issues with
        // attributes that are not assignable, copy constructible or move constructible)
        return To_Ruby<To_Ruby_T&>(&this->return_).convert(nativeSelf->*attribute_);
      }
    }
    else
    {
      if constexpr (std::is_fundamental_v<detail::intrinsic_type<To_Ruby_T>>)
      {
        return To_Ruby<To_Ruby_T>(&this->return_).convert(*attribute_);
      }
      else if constexpr (std::is_array_v<To_Ruby_T>)
      {
        return To_Ruby<To_Ruby_T>(&this->return_).convert(*attribute_);
      }
      else if constexpr (std::is_pointer_v<To_Ruby_T>)
      {
        return To_Ruby<To_Ruby_T>(&this->return_).convert(*attribute_);
      }
      else
      {
        // If the attribute is an object return a reference to avoid a copy (and avoid issues with
        // attributes that are not assignable, copy constructible or move constructible)
        return To_Ruby<To_Ruby_T&>(&this->return_).convert(*attribute_);
      }
    }
  }

  template<typename Attribute_T>
  inline std::string NativeAttributeGet<Attribute_T>::toString()
  {
    return "";
  }

  template<typename Attribute_T>
  inline std::string NativeAttributeGet<Attribute_T>::name()
  {
    return this->name_;
  }

  template<typename Attribute_T>
  inline NativeKind NativeAttributeGet<Attribute_T>::kind()
  {
    return NativeKind::AttributeReader;
  }

  template<typename Attribute_T>
  inline VALUE NativeAttributeGet<Attribute_T>::returnKlass()
  {
    // Check if an array is being returned
    if (this->return_.isBuffer())
    {
      TypeMapper<Pointer<Attr_T>> typeMapper;
      return typeMapper.rubyKlass();
    }
    else
    {
      TypeMapper<Attr_T> typeMapper;
      return typeMapper.rubyKlass();
    }
  }
}
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
  inline Resolved NativeAttributeSet<Attribute_T>::matches(size_t argc, const VALUE* argv, VALUE self)
  {
    if (argc == 1)
      return Resolved{ Convertible::Exact, 1, this };
    else
      return Resolved{ Convertible::None, 0, this };
  }

  template<typename Attribute_T>
  inline VALUE NativeAttributeSet<Attribute_T>::operator()(size_t argc, const VALUE* argv, VALUE self)
  {
    if (argc != 1)
    {
      throw std::runtime_error("Incorrect number of parameters for setting attribute. Attribute: " + this->name_);
    }

    VALUE value = argv[0];

    if constexpr (!std::is_null_pointer_v<Receiver_T>)
    {
      Receiver_T* nativeSelf = From_Ruby<Receiver_T*>().convert(self);

      // Deal with pointers to pointes, see Parameter::convertToNative commment
      if constexpr (is_pointer_pointer_v<Attr_T> && !std::is_convertible_v<remove_cv_recursive_t<Attr_T>, Attr_T>)
      {
        nativeSelf->*attribute_ = (Attr_T)From_Ruby<T_Unqualified>().convert(value);
      }
      else
      {
        nativeSelf->*attribute_ = From_Ruby<T_Unqualified>().convert(value);
      }
    }
    else
    {
      *attribute_ = From_Ruby<T_Unqualified>().convert(value);
    }

    return value;
  }

  template<typename Attribute_T>
  inline std::string NativeAttributeSet<Attribute_T>::toString()
  {
    return "";
  }

  template<typename Attribute_T>
  inline std::string NativeAttributeSet<Attribute_T>::name()
  {
    return this->name_;
  }

  template<typename Attribute_T>
  inline NativeKind NativeAttributeSet<Attribute_T>::kind()
  {
    return NativeKind::AttributeWriter;
  }

  template<typename Attribute_T>
  inline VALUE NativeAttributeSet<Attribute_T>::returnKlass()
  {
    TypeMapper<Attr_T> typeMapper;
    return typeMapper.rubyKlass();
  }
}

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

  template<typename Function_T>
  class NativeFunction: Native
  {
  public:
    using NativeFunction_T = NativeFunction<Function_T>;

    // We remove const to avoid an explosion of To_Ruby specializations and Ruby doesn't
    // have the concept of constants anyways
    using Return_T = typename function_traits<Function_T>::return_type;
    using Class_T = typename function_traits<Function_T>::class_type;
    using Arg_Ts = typename function_traits<Function_T>::arg_types;
    using To_Ruby_T = remove_cv_recursive_t<Return_T>;

    // Register function with Ruby
    static void define(VALUE klass, std::string method_name, Function_T function, MethodInfo* methodInfo);

  public:
    NativeFunction(VALUE klass, std::string method_name, Function_T function, MethodInfo* methodInfo);

    VALUE operator()(size_t argc, const VALUE* argv, VALUE self) override;
    std::string toString() override;

    std::string name() override;
    NativeKind kind() override;
    VALUE returnKlass() override;

  private:
    template<std::size_t...I>
    std::vector<std::string> argTypeNames(std::ostringstream& stream, std::index_sequence<I...>& indices);

    // Convert Ruby values to C++ values
    template<typename std::size_t...I>
    Arg_Ts getNativeValues(std::vector<std::optional<VALUE>>& values, std::index_sequence<I...>& indices);

    // Throw an exception when wrapper cannot be extracted
    [[noreturn]] void noWrapper(const VALUE klass, const std::string& wrapper);

    // Do we need to keep alive any arguments?
    void checkKeepAlive(VALUE self, VALUE returnValue, std::vector<std::optional<VALUE>>& rubyValues);

    // Call the underlying C++ function
    VALUE invoke(Arg_Ts&& nativeArgs);
    VALUE invokeNoGVL(Arg_Ts&& nativeArgs);

  private:
    VALUE klass_;
    std::string method_name_;
    Function_T function_;
    std::unique_ptr<MethodInfo> methodInfo_;
    To_Ruby<To_Ruby_T> toRuby_;
  };
}


// =========   NativeFunction.ipp   =========
#include <algorithm>
#include <array>
#include <stdexcept>
#include <sstream>
#include <tuple>

namespace Rice::detail
{
  template<typename Function_T>
  void NativeFunction<Function_T>::define(VALUE klass, std::string method_name, Function_T function, MethodInfo* methodInfo)
  {
    // Verify return and argument types
    Native::verify_type<Return_T>(methodInfo->returnInfo()->isBuffer());
    auto indices = std::make_index_sequence<std::tuple_size_v<Arg_Ts>>{};
    Native::verify_args<Arg_Ts>(methodInfo, indices);

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

  template<typename Function_T>
  NativeFunction<Function_T>::NativeFunction(VALUE klass, std::string method_name, Function_T function, MethodInfo* methodInfo)
    : Native(Native::create_parameters<Arg_Ts>(methodInfo)),
      klass_(klass), method_name_(method_name), function_(function), methodInfo_(methodInfo),
      toRuby_(methodInfo->returnInfo())
  {
  }

  template<typename Function_T>
  template<std::size_t... I>
  std::vector<std::string> NativeFunction<Function_T>::argTypeNames(std::ostringstream& stream, std::index_sequence<I...>& indices)
  {
    std::vector<std::string> result;
    for (std::unique_ptr<ParameterAbstract>& parameter : this->parameters_)
    {
      result.push_back(parameter->cppTypeName());
    }
    return result;
  }

  template<typename Function_T>
  std::string NativeFunction<Function_T>::toString()
  {
    std::ostringstream result;

    detail::TypeMapper<Return_T> typeMapper;
    result << typeMapper.simplifiedName() << " ";
    result << this->method_name_;

    result << "(";

    auto indices = std::make_index_sequence<std::tuple_size_v<Arg_Ts>>{};
    std::vector<std::string> argTypeNames = this->argTypeNames(result, indices);
    for (size_t i = 0; i < argTypeNames.size(); i++)
    {
      result << argTypeNames[i];
      if (i < argTypeNames.size() - 1)
        result << ", ";
    }
    result << ")";
    return result.str();
  }
    
  template<typename Function_T>
  template<std::size_t... I>
  typename NativeFunction<Function_T>::Arg_Ts NativeFunction<Function_T>::getNativeValues(std::vector<std::optional<VALUE>>& values,
     std::index_sequence<I...>& indices)
  {
    /* Loop over each value returned from Ruby and convert it to the appropriate C++ type based
       on the arguments (Arg_Ts) required by the C++ function. Arg_T may have const/volatile while
       the associated From_Ruby<T> template parameter will not. Thus From_Ruby produces non-const values 
       which we let the compiler convert to const values as needed. This works except for 
       T** -> const T**, see comment in convertToNative method. */
    //return std::forward_as_tuple(this->getNativeValue<std::tuple_element_t<I, Arg_Ts>, I>(values)...);
    return std::forward_as_tuple(
      (dynamic_cast<Parameter<std::tuple_element_t<I, Arg_Ts>>*>(this->parameters_[I].get()))->
               convertToNative(values[I])...);
  }

  template<typename Function_T>
  VALUE NativeFunction<Function_T>::invoke(Arg_Ts&& nativeArgs)
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

  template<typename Function_T>
  VALUE NativeFunction<Function_T>::invokeNoGVL(Arg_Ts&& nativeArgs)
  {
    if constexpr (std::is_void_v<Return_T>)
    {
      no_gvl(this->function_, std::forward<Arg_Ts>(nativeArgs));
      return Qnil;
    }
    else
    {
      // Call the native method and get the result
      Return_T nativeResult = no_gvl(this->function_, std::forward<Arg_Ts>(nativeArgs));

      // Return the result
      return this->toRuby_.convert(nativeResult);
    }
  }

  template<typename Function_T>
  void NativeFunction<Function_T>::noWrapper(const VALUE klass, const std::string& wrapper)
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

  template<typename Function_T>
  void NativeFunction<Function_T>::checkKeepAlive(VALUE self, VALUE returnValue, std::vector<std::optional<VALUE>>& rubyValues)
  {
    // Self will be Qnil for wrapped procs
    if (self == Qnil)
      return;

    // selfWrapper will be nullptr if this(self) is a builtin type and not an external(wrapped) type
    // it is highly unlikely that keepAlive is used in this case but we check anyway
    WrapperBase* selfWrapper = getWrapper(self);

    // Check function arguments
    for (const Arg& arg : (*this->methodInfo_))
    {
      if (arg.isKeepAlive())
      {
        if (selfWrapper == nullptr)
        {
          noWrapper(self, "self");
        }
        selfWrapper->addKeepAlive(rubyValues[arg.position].value());
      }
    }

    // Check return value
    if (this->methodInfo_->returnInfo()->isKeepAlive())
    {
      if (selfWrapper == nullptr)
      {
        noWrapper(self, "self");
      }

      // returnWrapper will be nullptr if returnValue is a built-in type and not an external(wrapped) type
      WrapperBase* returnWrapper = getWrapper(returnValue);
      if (returnWrapper == nullptr)
      {
        noWrapper(returnValue, "return");
      }
      returnWrapper->addKeepAlive(self);
    }
  }

  template<typename Function_T>
  VALUE NativeFunction<Function_T>::operator()(size_t argc, const VALUE* argv, VALUE self)
  {
    // Get the ruby values and make sure we have the correct number
    std::vector<std::optional<VALUE>> rubyValues = this->getRubyValues(argc, argv, true);

    auto indices = std::make_index_sequence<std::tuple_size_v<Arg_Ts>>{};

    // Convert the Ruby values to native values
    Arg_Ts nativeValues = this->getNativeValues(rubyValues, indices);

    bool noGvl = this->methodInfo_->function()->isNoGvl();

    VALUE result = Qnil;

    if (noGvl)
    {
      result = this->invokeNoGVL(std::forward<Arg_Ts>(nativeValues));
    }
    else
    {
      result = this->invoke(std::forward<Arg_Ts>(nativeValues));
    }

    // Check if any function arguments or return values need to have their lifetimes tied to the receiver
    this->checkKeepAlive(self, result, rubyValues);

    return result;
  }

  template<typename Function_T>
  inline std::string NativeFunction<Function_T>::name()
  {
    return this->method_name_;
  }

  template<typename Function_T>
  inline NativeKind NativeFunction<Function_T>::kind()
  {
    return NativeKind::Function;
  }

  template<typename Function_T>
  inline VALUE NativeFunction<Function_T>::returnKlass()
  {
    // Check if an array is being returned
    if (this->methodInfo_->returnInfo()->isBuffer())
    {
      TypeMapper<Pointer<Return_T>> typeMapper;
      return typeMapper.rubyKlass();
    }
    else
    {
      TypeMapper<Return_T> typeMapper;
      return typeMapper.rubyKlass();
    }
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

    Resolved matches(size_t argc, const VALUE* argv, VALUE self) override;
    VALUE operator()(size_t argc, const VALUE* argv, VALUE self) override;
    std::string toString() override;

    std::string name() override;
    NativeKind kind() override;
    VALUE returnKlass() override;

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
    // Tell Ruby to invoke the resolveIterator static method defined in Native super class.
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
  inline Resolved NativeIterator<T, Iterator_Func_T>::matches(size_t argc, const VALUE* argv, VALUE self)
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
  inline VALUE NativeIterator<T, Iterator_Func_T>::operator()(size_t argc, const VALUE* argv, VALUE self)
  {
    if (!protect(rb_block_given_p))
    {
      return createRubyEnumerator(self);
    }
    else
    {
      detail::From_Ruby<T*> fromRuby;
      T* receiver = fromRuby.convert(self);

      Iterator_T it = std::invoke(this->begin_, *receiver);
      Iterator_T end = std::invoke(this->end_, *receiver);

      detail::To_Ruby<To_Ruby_T> toRuby;
      for (; it != end; ++it)
      {
        protect(rb_yield, toRuby.convert(*it));
      }

      return self;
    }
  }

  template<typename T, typename Iterator_Func_T>
  inline std::string NativeIterator<T, Iterator_Func_T>::toString()
  {
    return "";
  }

  template<typename T, typename Iterator_Func_T>
  inline std::string NativeIterator<T, Iterator_Func_T>::name()
  {
    return this->method_name_;
  }

  template<typename T, typename Iterator_Func_T>
  inline NativeKind NativeIterator<T, Iterator_Func_T>::kind()
  {
    return NativeKind::Iterator;
  }

  template<typename T, typename Iterator_Func_T>
  inline VALUE NativeIterator<T, Iterator_Func_T>::returnKlass()
  {
    TypeMapper<Value_T> typeMapper;
    return typeMapper.rubyKlass();
  }
}
// =========   NativeMethod.hpp   =========

namespace Rice::detail
{
  //! The NativeMethod class calls C++ functions/methods/lambdas on behalf of Ruby
  /*! The NativeMethod class is an intermediate between Ruby and C++. Every method
   *  defined in Rice is associated with a NativeFuntion instance that is stored in
   *  a unordered_map maintained by the MethodData class. The key is the Ruby class
   *  and method.
   * 
   *  When Ruby calls into C++ it invokes the static NativeMethod.call method. This
   *  method then looks up the NativeMethod instance and calls its ->() operator.
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
   *  @tparam Receiver_T - The type of C++ class wrapped by Ruby. Althought NativeMethod
   *    can derive the C++ class (Receiver_T), it can differ per member function. For example,
   *    std::map has a size() method but that is actually implemented on an ancestor class _Tree.
   *    Thus Receiver_T is std::map but Method_T::Receiver_T is _Tree. This breaks Rice in two ways. 
   *    First, _Tree is not a registered type. Second, Rice would return a _Tree instance back to
   *    C++ and not a std::map.
   *  @tparam Method_T - A template that represents the C++ function
   *    to call. This typename is automatically deduced by the compiler.
   *  @tparam IsMethod - A boolean specifying whether the function has
   *    a self parameter or not. Rice differentiates these two cases by
   *    calling them methods (self) or functions (no self).
   */

  template<typename Class_T, typename Method_T>
  class NativeMethod: Native
  {
  public:
    using NativeMethod_T = NativeMethod<Class_T, Method_T>;

    // We remove const to avoid an explosion of To_Ruby specializations and Ruby doesn't
    // have the concept of constants anyways
    using Return_T = typename method_traits<Method_T>::Return_T;
    using Receiver_T = typename method_traits<Method_T>::Class_T;
    using Arg_Ts = typename method_traits<Method_T>::Arg_Ts;
    using Apply_Args_T = typename tuple_unshift<Receiver_T, Arg_Ts>::type;

    using To_Ruby_T = remove_cv_recursive_t<Return_T>;

    // Register method with Ruby
    static void define(VALUE klass, std::string method_name, Method_T method, MethodInfo* methodInfo);

  public:
    NativeMethod(VALUE klass, std::string method_name, Method_T method, MethodInfo* methodInfo);

    VALUE operator()(size_t argc, const VALUE* argv, VALUE self) override;
    std::string toString() override;

    std::string name() override;
    NativeKind kind() override;
    VALUE returnKlass() override;

  private:

    template<std::size_t...I>
    std::vector<std::string> argTypeNames(std::ostringstream& stream, std::index_sequence<I...>& indices);

    // Convert Ruby values to C++ values
    template<typename std::size_t...I>
    Apply_Args_T getNativeValues(VALUE self, std::vector<std::optional<VALUE>>& values, std::index_sequence<I...>& indices);

    // Figure out what self is
    Receiver_T getReceiver(VALUE self);

    // Throw an exception when wrapper cannot be extracted
    [[noreturn]] void noWrapper(const VALUE klass, const std::string& wrapper);

    // Do we need to keep alive any arguments?
    void checkKeepAlive(VALUE self, VALUE returnValue, std::vector<std::optional<VALUE>>& rubyValues);

    // Call the underlying C++ method
    VALUE invoke(VALUE self, Apply_Args_T&& nativeArgs);
    VALUE invokeNoGVL(VALUE self, Apply_Args_T&& nativeArgs);

  private:
    VALUE klass_;
    std::string method_name_;
    Method_T method_;
    std::unique_ptr<MethodInfo> methodInfo_;
    To_Ruby<To_Ruby_T> toRuby_;
  };
}


// =========   NativeMethod.ipp   =========
#include <algorithm>
#include <array>
#include <stdexcept>
#include <sstream>
#include <tuple>

namespace Rice::detail
{
  template<typename Class_T, typename Method_T>
  void NativeMethod<Class_T, Method_T>::define(VALUE klass, std::string method_name, Method_T method, MethodInfo* methodInfo)
  {
    // Verify return and argument types
    Native::verify_type<Return_T>(methodInfo->returnInfo()->isBuffer());
    auto indices = std::make_index_sequence<std::tuple_size_v<Arg_Ts>>{};
    Native::verify_args<Arg_Ts>(methodInfo, indices);

    // Have we defined this method yet in Ruby?
    Identifier identifier(method_name);
    const std::vector<std::unique_ptr<Native>>& natives = Registries::instance.natives.lookup(klass, identifier.id());
    if (natives.empty())
    {
      // Tell Ruby to invoke the static resolved method defined above
      detail::protect(rb_define_method, klass, method_name.c_str(), (RUBY_METHOD_FUNC)&Native::resolve, -1);
    }

    // Create a NativeMethod instance and save it to the NativeRegistry. There may be multiple
    // NativeMethod instances for a specific method because C++ supports method overloading.
    NativeMethod_T* NativeMethod = new NativeMethod_T(klass, method_name, std::forward<Method_T>(method), methodInfo);
    std::unique_ptr<Native> native(NativeMethod);
    detail::Registries::instance.natives.add(klass, identifier.id(), native);
  }

  template<typename Class_T, typename Method_T>
  NativeMethod<Class_T, Method_T>::NativeMethod(VALUE klass, std::string method_name, Method_T method, MethodInfo* methodInfo)
    : Native(Native::create_parameters<Arg_Ts>(methodInfo)),
      klass_(klass), method_name_(method_name), method_(method), methodInfo_(methodInfo),
      toRuby_(methodInfo->returnInfo())
  {
  }

  template<typename Class_T, typename Method_T>
  template<std::size_t... I>
  std::vector<std::string> NativeMethod<Class_T, Method_T>::argTypeNames(std::ostringstream& stream, std::index_sequence<I...>& indices)
  {
    std::vector<std::string> result;
    for (std::unique_ptr<ParameterAbstract>& parameter : this->parameters_)
    {
      result.push_back(parameter->cppTypeName());
    }
    return result;
  }

  template<typename Class_T, typename Method_T>
  std::string NativeMethod<Class_T, Method_T>::toString()
  {
    std::ostringstream result;

    detail::TypeMapper<Return_T> typeReturnMapper;
    result << typeReturnMapper.simplifiedName() << " ";
    
    if (!std::is_null_pointer_v<Receiver_T>)
    {
      detail::TypeMapper<Receiver_T> typeReceiverMapper;
      result << typeReceiverMapper.simplifiedName() << "::";
    }
    
    result << this->method_name_;

    result << "(";

    auto indices = std::make_index_sequence<std::tuple_size_v<Arg_Ts>>{};
    std::vector<std::string> argTypeNames = this->argTypeNames(result, indices);
    for (size_t i = 0; i < argTypeNames.size(); i++)
    {
      result << argTypeNames[i];
      if (i < argTypeNames.size() - 1)
        result << ", ";
    }
    result << ")";
    return result.str();
  }
    
  template<typename Class_T, typename Method_T>
  template<std::size_t... I>
  typename NativeMethod<Class_T, Method_T>::Apply_Args_T NativeMethod<Class_T, Method_T>::getNativeValues(VALUE self, std::vector<std::optional<VALUE>>& values, std::index_sequence<I...>& indices)
  {
    /* Loop over each value returned from Ruby and convert it to the appropriate C++ type based
       on the arguments (Arg_Ts) required by the C++ method. Arg_T may have const/volatile while
       the associated From_Ruby<T> template parameter will not. Thus From_Ruby produces non-const values 
       which we let the compiler convert to const values as needed. This works except for 
       T** -> const T**, see comment in convertToNative method. */
    return std::forward_as_tuple(this->getReceiver(self),
      (dynamic_cast<Parameter<std::tuple_element_t<I, Arg_Ts>>*>(this->parameters_[I].get()))->
               convertToNative(values[I])...);
  }

  template<typename Class_T, typename Method_T>
  typename NativeMethod<Class_T, Method_T>::Receiver_T NativeMethod<Class_T, Method_T>::getReceiver(VALUE self)
  {
    // Self parameter is a Ruby VALUE so no conversion is needed
    if constexpr (std::is_same_v<Receiver_T, VALUE>)
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

  template<typename Class_T, typename Method_T>
  inline VALUE NativeMethod<Class_T, Method_T>::invoke(VALUE self, Apply_Args_T&& nativeArgs)
  {
    if constexpr (std::is_void_v<Return_T>)
    {
      std::apply(this->method_, std::forward<Apply_Args_T>(nativeArgs));
      return Qnil;
    }
    else
    {
      Return_T nativeResult = std::apply(this->method_, std::forward<Apply_Args_T>(nativeArgs));

      // Special handling if the method returns self. If so we do not want
      // to create a new Ruby wrapper object and instead return self.
      if constexpr (std::is_same_v<intrinsic_type<Return_T>, intrinsic_type<Receiver_T>>)
      {
				Receiver_T receiver = std::get<0>(nativeArgs);

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

  template<typename Class_T, typename Method_T>
  inline VALUE NativeMethod<Class_T, Method_T>::invokeNoGVL(VALUE self, Apply_Args_T&& nativeArgs)
  {
    if constexpr (std::is_void_v<Return_T>)
    {
      no_gvl(this->method_, std::forward<Apply_Args_T>(nativeArgs));
      return Qnil;
    }
    else
    {
      Return_T nativeResult = no_gvl(this->method_, std::forward<Apply_Args_T>(nativeArgs));

      // Special handling if the method returns self. If so we do not want
      // to create a new Ruby wrapper object and instead return self.
      if constexpr (std::is_same_v<intrinsic_type<Return_T>, intrinsic_type<Receiver_T>>)
      {
        Receiver_T receiver = std::get<0>(nativeArgs);

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

  template<typename Class_T, typename Method_T>
  inline void NativeMethod<Class_T, Method_T>::noWrapper(const VALUE klass, const std::string& wrapper)
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

  template<typename Class_T, typename Method_T>
  void NativeMethod<Class_T, Method_T>::checkKeepAlive(VALUE self, VALUE returnValue, std::vector<std::optional<VALUE>>& rubyValues)
  {
    // Self will be Qnil for wrapped procs
    if (self == Qnil)
      return;

    // selfWrapper will be nullptr if this(self) is a builtin type and not an external(wrapped) type
    // it is highly unlikely that keepAlive is used in this case but we check anyway
    WrapperBase* selfWrapper = getWrapper(self);

    // Check method arguments
    for (const Arg& arg : (*this->methodInfo_))
    {
      if (arg.isKeepAlive())
      {
        if (selfWrapper == nullptr)
        {
          noWrapper(self, "self");
        }
        selfWrapper->addKeepAlive(rubyValues[arg.position].value());
      }
    }

    // Check return value
    if (this->methodInfo_->returnInfo()->isKeepAlive())
    {
      if (selfWrapper == nullptr)
      {
        noWrapper(self, "self");
      }

      // returnWrapper will be nullptr if returnValue is a built-in type and not an external(wrapped) type
      WrapperBase* returnWrapper = getWrapper(returnValue);
      if (returnWrapper == nullptr)
      {
        noWrapper(returnValue, "return");
      }
      returnWrapper->addKeepAlive(self);
    }
  }

  template<typename Class_T, typename Method_T>
  VALUE NativeMethod<Class_T, Method_T>::operator()(size_t argc, const VALUE* argv, VALUE self)
  {
    // Get the ruby values and make sure we have the correct number
    std::vector<std::optional<VALUE>> rubyValues = this->getRubyValues(argc, argv, true);
    auto indices = std::make_index_sequence<std::tuple_size_v<Arg_Ts>>{};
    Apply_Args_T nativeArgs = this->getNativeValues(self, rubyValues, indices);

    bool noGvl = this->methodInfo_->function()->isNoGvl();

    VALUE result = Qnil;

    if (noGvl)
    {
      result = this->invokeNoGVL(self, std::forward<Apply_Args_T>(nativeArgs));
    }
    else
    {
      result = this->invoke(self, std::forward<Apply_Args_T>(nativeArgs));
    }

    // Check if any method arguments or return values need to have their lifetimes tied to the receiver
    this->checkKeepAlive(self, result, rubyValues);

    return result;
  }

  template<typename Class_T, typename Method_T>
  inline std::string NativeMethod<Class_T, Method_T>::name()
  {
    return this->method_name_;
  }

  template<typename Class_T, typename Method_T>
  inline NativeKind NativeMethod<Class_T, Method_T>::kind()
  {
    return NativeKind::Method;
  }

  template<typename Class_T, typename Method_T>
  inline VALUE NativeMethod<Class_T, Method_T>::returnKlass()
  {
    // Check if an array is being returned
    if (this->methodInfo_->returnInfo()->isBuffer())
    {
      TypeMapper<Pointer<Return_T>> typeMapper;
      return typeMapper.rubyKlass();
    }
    else
    {
      TypeMapper<Return_T> typeMapper;
      return typeMapper.rubyKlass();
    }
  }
}

// =========   NativeProc.hpp   =========

namespace Rice::detail
{
  template<typename Proc_T>
  class NativeProc: Native
  {
  public:
    using NativeProc_T = NativeProc<Proc_T>;

    // We remove const to avoid an explosion of To_Ruby specializations and Ruby doesn't
    // have the concept of constants anyways
    using Return_T = typename function_traits<Proc_T>::return_type;
    using Arg_Ts = typename function_traits<Proc_T>::arg_types;
    using To_Ruby_T = remove_cv_recursive_t<Return_T>;

    // Define a new Ruby Proc to wrap a C++ function
    static VALUE createRubyProc(Proc_T proc);
    static NativeProc<Proc_T>* define(Proc_T proc);

    // This is the method Ruby calls when invoking the proc
    static VALUE resolve(VALUE yielded_arg, VALUE callback_arg, int argc, const VALUE* argv, VALUE blockarg);
 
  public:
    NativeProc(Proc_T proc, MethodInfo* methodInfo);
    VALUE operator()(size_t argc, const VALUE* argv, VALUE self) override;
    std::string toString() override;
    
    std::string name() override;
    NativeKind kind() override;
    VALUE returnKlass() override;

  private:
    static VALUE finalizerCallback(VALUE yielded_arg, VALUE callback_arg, int argc, const VALUE* argv, VALUE blockarg);

    // Convert Ruby values to C++ values
    template<typename std::size_t...I>
    Arg_Ts getNativeValues(std::vector<std::optional<VALUE>>& values, std::index_sequence<I...>& indices);

    // Call the underlying C++ function
    VALUE invoke(Arg_Ts&& nativeArgs);

  private:
    Proc_T proc_;
    std::unique_ptr<MethodInfo> methodInfo_;
    To_Ruby<To_Ruby_T> toRuby_;
  };
}


// =========   NativeProc.ipp   =========
#include <algorithm>
#include <array>
#include <stdexcept>
#include <sstream>
#include <tuple>

namespace Rice::detail
{
  template<typename Proc_T>
  NativeProc<Proc_T>* NativeProc<Proc_T>::define(Proc_T proc)
  {
    MethodInfo* methodInfo = new MethodInfo(detail::function_traits<Proc_T>::arity);
    return new NativeProc_T(std::forward<Proc_T>(proc), methodInfo);
  }

  template<typename Proc_T>
  VALUE NativeProc<Proc_T>::createRubyProc(Proc_T proc)
  {
    NativeProc_T* nativeProc = NativeProc_T::define(std::forward<Proc_T>(proc));

    // Create a Ruby proc to wrap it and pass the NativeProc as a callback parameter
    VALUE result = rb_proc_new(NativeProc_T::resolve, (VALUE)nativeProc);

    // Tie the lifetime of the NativeProc to the Ruby Proc
    VALUE finalizer = rb_proc_new(NativeProc_T::finalizerCallback, (VALUE)nativeProc);
    rb_define_finalizer(result, finalizer);

    return result;
  }

  // Ruby calls this method when invoking a proc that was defined as a C++ function
  template<typename Proc_T>
  VALUE NativeProc<Proc_T>::resolve(VALUE yielded_arg, VALUE callback_arg, int argc, const VALUE* argv, VALUE blockarg)
  {
    return cpp_protect([&]
    {
      NativeProc_T * native = (NativeProc_T*)callback_arg;
      return (*native)(argc, argv, Qnil);
    });
  }

  // Ruby calls this method if an instance of a NativeProc is owned by a Ruby proc. That happens when C++
  // returns a function back to Ruby
  template<typename Proc_T>
  VALUE NativeProc<Proc_T>::finalizerCallback(VALUE yielded_arg, VALUE callback_arg, int argc, const VALUE* argv, VALUE blockarg)
  {
    NativeProc_T* native = (NativeProc_T*)callback_arg;
    delete native;
    return Qnil;
  }

  template<typename Proc_T>
  NativeProc<Proc_T>::NativeProc(Proc_T proc, MethodInfo* methodInfo) : Native(Native::create_parameters<Arg_Ts>(methodInfo)),
                                                                        proc_(proc), methodInfo_(methodInfo)
  {
  }

  template<typename Proc_T>
  std::string NativeProc<Proc_T>::toString()
  {
    return "Proc";
  }

  template<typename Proc_T>
  template<std::size_t... I>
  typename NativeProc<Proc_T>::Arg_Ts NativeProc<Proc_T>::getNativeValues(std::vector<std::optional<VALUE>>& values,
     std::index_sequence<I...>& indices)
  {
    /* Loop over each value returned from Ruby and convert it to the appropriate C++ type based
       on the arguments (Arg_Ts) required by the C++ function. Arg_T may have const/volatile while
       the associated From_Ruby<T> template parameter will not. Thus From_Ruby produces non-const values 
       which we let the compiler convert to const values as needed. This works except for 
       T** -> const T**, see comment in convertToNative method. */
    //return std::forward_as_tuple(this->getNativeValue<std::tuple_element_t<I, Arg_Ts>, I>(values)...);
    return std::forward_as_tuple(
      (dynamic_cast<Parameter<std::tuple_element_t<I, Arg_Ts>>*>(this->parameters_[I].get()))->
               convertToNative(values[I])...);
  }

  template<typename Proc_T>
  VALUE NativeProc<Proc_T>::invoke(Arg_Ts&& nativeArgs)
  {
    if constexpr (std::is_void_v<Return_T>)
    {
      std::apply(this->proc_, std::forward<Arg_Ts>(nativeArgs));
      return Qnil;
    }
    else
    {
      // Call the native method and get the result
      Return_T nativeResult = std::apply(this->proc_, std::forward<Arg_Ts>(nativeArgs));

      // Return the result
      return this->toRuby_.convert(std::forward<Return_T>(nativeResult));
    }
  }

  template<typename Proc_T>
  VALUE NativeProc<Proc_T>::operator()(size_t argc, const VALUE* argv, VALUE self)
  {
    // Get the ruby values and make sure we have the correct number
    std::vector<std::optional<VALUE>> rubyValues = this->getRubyValues(argc, argv, true);

    auto indices = std::make_index_sequence<std::tuple_size_v<Arg_Ts>>{};

    // Convert the Ruby values to native values
    Arg_Ts nativeValues = this->getNativeValues(rubyValues, indices);

    // Now call the native method
    VALUE result = this->invoke(std::forward<Arg_Ts>(nativeValues));

    return result;
  }

  template<typename Proc_T>
  inline std::string NativeProc< Proc_T>::name()
  {
    return "proc";
  }

  template<typename Proc_T>
  inline NativeKind NativeProc< Proc_T>::kind()
  {
    return NativeKind::Proc;
  }

  template<typename Proc_T>
  inline VALUE NativeProc<Proc_T>::returnKlass()
  {
    TypeMapper<Return_T> typeMapper;
    return typeMapper.rubyKlass();
  }
}
// =========   NativeCallbackFFI.hpp   =========

#ifdef HAVE_LIBFFI

#include <ffi.h>

namespace Rice::detail
{
  template<typename Callback_T>
  class NativeCallbackFFI;

  template<typename Return_T, typename ...Arg_Ts>
  class NativeCallbackFFI<Return_T(*)(Arg_Ts...)>
  {
  public:
    using Callback_T = Return_T(Arg_Ts...);
    using Tuple_T = std::tuple<Arg_Ts...>;
    static void ffiCallback(ffi_cif* cif, void* ret, void* args[], void* instance);
    static VALUE finalizerCallback(VALUE yielded_arg, VALUE callback_arg, int argc, const VALUE* argv, VALUE blockarg);
    static void setMethodInfo(MethodInfo* methodInfo);

  public:
    NativeCallbackFFI(VALUE proc);
    ~NativeCallbackFFI();
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
    static inline std::unique_ptr<MethodInfo> methodInfo_ = std::make_unique<MethodInfo>();

    template<std::size_t... I>
    Return_T callRuby(std::index_sequence<I...>& indices, Arg_Ts...args);
  private:
    VALUE proc_;
  };
}
#endif // HAVE_LIBFFI


// =========   NativeCallbackFFI.ipp   =========
#ifdef HAVE_LIBFFI
#include <tuple>
#include <ffi.h>

namespace Rice::detail
{
  template<typename Return_T, typename ...Arg_Ts>
  template<typename Arg_T>
  ffi_type* NativeCallbackFFI<Return_T(*)(Arg_Ts...)>::ffiType()
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
  typename NativeCallbackFFI<Return_T(*)(Arg_Ts...)>::Tuple_T NativeCallbackFFI<Return_T(*)(Arg_Ts...)>::convertArgsToTuple(void* args[], std::index_sequence<I...>& indices)
  {
    /* Loop over each value returned from Ruby and convert it to the appropriate C++ type based
       on the arguments (Arg_Ts) required by the C++ function. Arg_T may have const/volatile while
       the associated From_Ruby<T> template parameter will not. Thus From_Ruby produces non-const values
       which we let the compiler convert to const values as needed. This works except for
       T** -> const T**, see comment in convertToNative method. */
    return std::forward_as_tuple(*(std::tuple_element_t<I, Tuple_T>*)(args[I])...);
  }

  template<typename Return_T, typename ...Arg_Ts>
  void NativeCallbackFFI<Return_T(*)(Arg_Ts...)>::ffiCallback(ffi_cif* cif, void* ret, void* args[], void* instance)
  {
    using Self_T = NativeCallbackFFI<Return_T(*)(Arg_Ts...)>;
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
  void NativeCallbackFFI<Return_T(*)(Arg_Ts...)>::setMethodInfo(MethodInfo* methodInfo)
  {
    methodInfo_.reset(methodInfo);
  }

  template<typename Return_T, typename ...Arg_Ts>
  NativeCallbackFFI<Return_T(*)(Arg_Ts...)>::~NativeCallbackFFI()
  {
    this->proc_ = Qnil;
  }

  template<typename Return_T, typename ...Arg_Ts>
  VALUE NativeCallbackFFI<Return_T(*)(Arg_Ts...)>::finalizerCallback(VALUE yielded_arg, VALUE callback_arg, int argc, const VALUE* argv, VALUE blockarg)
  {
    using NativeCallback_T = NativeCallbackFFI<Return_T(*)(Arg_Ts...)>;
    NativeCallback_T* nativeCallback = (NativeCallback_T*)callback_arg;
    delete nativeCallback;
    return Qnil;
  }

  template<typename Return_T, typename ...Arg_Ts>
  NativeCallbackFFI<Return_T(*)(Arg_Ts...)>::NativeCallbackFFI(VALUE proc) : proc_(proc)
  {
    // First setup desccription of callback
    if (cif_.bytes == 0)
    {
      ffi_prep_cif(&cif_, FFI_DEFAULT_ABI, sizeof...(Arg_Ts), &ffi_type_pointer, args_.data());
    }

    // Create FFI closure
    this->closure_ = (ffi_closure *)ffi_closure_alloc(sizeof(ffi_closure) + sizeof(void*), (void**)(&this->callback_));
    ffi_prep_closure_loc(this->closure_, &cif_, ffiCallback, (void*)this, (void*)this->callback_);
  }

  template<typename Return_T, typename ...Arg_Ts>
  typename NativeCallbackFFI<Return_T(*)(Arg_Ts...)>::Callback_T* NativeCallbackFFI<Return_T(*)(Arg_Ts...)>::callback()
  {
    return (Callback_T*)this->callback_;
  }

  template<typename Return_T, typename ...Arg_Ts>
  template<std::size_t... I>
  Return_T NativeCallbackFFI<Return_T(*)(Arg_Ts...)>::callRuby(std::index_sequence<I...>& indices, Arg_Ts...args)
  {
    static Identifier id("call");
    std::array<VALUE, sizeof...(Arg_Ts)> values = { detail::To_Ruby<detail::remove_cv_recursive_t<Arg_Ts>>(methodInfo_->arg(I)).convert(args)... };
    VALUE result = detail::protect(rb_funcallv, this->proc_, id.id(), (int)sizeof...(Arg_Ts), values.data());
    if constexpr (!std::is_void_v<Return_T>)
    {
      static From_Ruby<Return_T> fromRuby(dynamic_cast<Arg*>(methodInfo_->returnInfo()));
      return fromRuby.convert(result);
    }
  }

  template<typename Return_T, typename ...Arg_Ts>
  Return_T NativeCallbackFFI<Return_T(*)(Arg_Ts...)>::operator()(Arg_Ts...args)
  {
    auto indices = std::make_index_sequence<sizeof...(Arg_Ts)>{};
    return NativeCallbackFFI<Return_T(*)(Arg_Ts...)>::callRuby(indices, args...);
  }
}
#endif // HAVE_LIBFFI

// =========   NativeCallbackSimple.hpp   =========

namespace Rice::detail
{
  template<typename Callback_T>
  class NativeCallbackSimple;

  template<typename Return_T, typename ...Arg_Ts>
  class NativeCallbackSimple<Return_T(*)(Arg_Ts...)>
  {
  public:
    static Return_T callback(Arg_Ts...args);
    static inline VALUE proc = Qnil;
    static void setMethodInfo(MethodInfo* methodInfo);

  public:
    NativeCallbackSimple()  = delete;
    NativeCallbackSimple(const NativeCallbackSimple&) = delete;
    NativeCallbackSimple(NativeCallbackSimple&&) = delete;
    void operator=(const NativeCallbackSimple&) = delete;
    void operator=(NativeCallbackSimple&&) = delete;

  private:
    template<std::size_t... I>
    static Return_T callRuby(std::index_sequence<I...>& indices, Arg_Ts...args);
    static inline std::unique_ptr<MethodInfo> methodInfo_ = std::make_unique<MethodInfo>();
  };
}

// =========   NativeCallbackSimple.ipp   =========
namespace Rice::detail
{
  template<typename Return_T, typename ...Arg_Ts>
  void NativeCallbackSimple<Return_T(*)(Arg_Ts...)>::setMethodInfo(MethodInfo* methodInfo)
  {
    methodInfo_.reset(methodInfo);
  }

  template<typename Return_T, typename ...Arg_Ts>
  template<std::size_t... I>
  Return_T NativeCallbackSimple<Return_T(*)(Arg_Ts...)>::callRuby(std::index_sequence<I...>& indices, Arg_Ts...args)
  {
    static Identifier id("call");
    std::array<VALUE, sizeof...(Arg_Ts)> values = { detail::To_Ruby<detail::remove_cv_recursive_t<Arg_Ts>>(methodInfo_->arg(I)).convert(args)... };
    VALUE result = detail::protect(rb_funcallv, proc, id.id(), (int)sizeof...(Arg_Ts), values.data());
    if constexpr (!std::is_void_v<Return_T>)
    {
      static From_Ruby<Return_T> fromRuby(dynamic_cast<Arg*>(methodInfo_->returnInfo()));
      return fromRuby.convert(result);
    }
  }

  template<typename Return_T, typename ...Arg_Ts>
  Return_T NativeCallbackSimple<Return_T(*)(Arg_Ts...)>::callback(Arg_Ts...args)
  {
    auto indices = std::make_index_sequence<sizeof...(Arg_Ts)>{};
    return NativeCallbackSimple<Return_T(*)(Arg_Ts...)>::callRuby(indices, args...);
  }
}
// =========   Proc.ipp   =========
namespace Rice::detail
{
  template<typename Return_T, typename ...Arg_Ts>
  struct Type<Return_T(*)(Arg_Ts...)>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cProc;
    }
  };

  // Wraps a C++ function as a Ruby proc
  template<typename Return_T, typename ...Arg_Ts>
  class To_Ruby<Return_T(*)(Arg_Ts...)>
  {
  public:
    using Proc_T = Return_T(*)(Arg_Ts...);

    To_Ruby() = default;

    explicit To_Ruby(Arg* arg)
    {
    }

    VALUE convert(Proc_T proc)
    {
      // Wrap the C+++ function pointer as a Ruby Proc
      return NativeProc<Proc_T>::createRubyProc(std::forward<Proc_T>(proc));
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
      using NativeCallback_T = NativeCallbackFFI<Return_T(*)(Arg_Ts...)>;
      NativeCallback_T* nativeCallback = new NativeCallback_T(value);

      // Tie the lifetime of the NativeCallback C++ instance to the lifetime of the Ruby proc object
      VALUE finalizer = rb_proc_new(NativeCallback_T::finalizerCallback, (VALUE)nativeCallback);
      rb_define_finalizer(value, finalizer);

      return nativeCallback->callback();
    }
#else
    Callback_T convert(VALUE value)
    {
      using NativeCallback_T = NativeCallbackSimple<Return_T(*)(Arg_Ts...)>;
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
    std::array<VALUE, sizeof...(Arg_Ts)> values = { detail::To_Ruby<detail::remove_cv_recursive_t<Arg_Ts>>().convert(std::forward<Arg_Ts>(args))... };
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

    static VALUE rubyKlass()
    {
      return rb_cObject;
    }
  };

  template<>
  class To_Ruby<Object>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
    {
    }

    VALUE convert(Object const& x)
    {
      return x.value();
    }

  private:
    Return* returnInfo_ = nullptr;
  };

  template<>
  class To_Ruby<Object&>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
    {
    }

    VALUE convert(Object const& x)
    {
      return x.value();
    }

  private:
    Return* returnInfo_ = nullptr;
  };

  template<>
  class From_Ruby<Object>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg)
    {
    }

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

  template<typename T>
  inline Array String::unpack() const
  {
    return this->call("unpack", detail::RubyType<detail::remove_cv_recursive_t<T>>::packTemplate.c_str());
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

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };
  
  template<>
  class To_Ruby<String>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
    {
    }

    VALUE convert(String const& x)
    {
      return x.value();
    }

  private:
    Return* returnInfo_ = nullptr;
  };

  template<>
  class From_Ruby<String>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg)
    {
    }

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
      push(*it, false);
    }
  }

  template<typename T, long n>
  inline Array::Array(T (&a)[n]) : Builtin_Object<T_ARRAY>(detail::protect(rb_ary_new))
  {
    for (long j = 0; j < n; ++j)
    {
      push(a[j], false);
    }
  }

  inline long Array::size() const
  {
    return RARRAY_LEN(this->value());
  }

  inline String Array::join(const char* separator)
  {
    return this->call("join", separator);
  }

  template<typename T>
  String Array::pack()
  {
    // Use .c_str so that the stl.hpp header is not required
    return this->call("pack", detail::RubyType<T>::packTemplate.c_str());
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
  inline Object Array::push(T&& obj, bool takeOwnership)
  {
    Return returnInfo;
    if (takeOwnership)
    {
      returnInfo.takeOwnership();
    }

    detail::To_Ruby<detail::remove_cv_recursive_t<T>> toRuby(&returnInfo);
    return detail::protect(rb_ary_push, value(), toRuby.convert(std::forward<T>(obj)));
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
    std::vector<T> result;

    long size = this->size();
    result.reserve(size);

    detail::From_Ruby<T> fromRuby;
    for (long i = 0; i < size; i++)
    {
      VALUE element = detail::protect(rb_ary_entry, this->value(), i);
      result.push_back(fromRuby.convert(element));
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

    static VALUE rubyKlass()
    {
      return rb_cArray;
    }
  };

  template<>
  class To_Ruby<Array>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
    {
    }

    VALUE convert(Array const& x)
    {
      return x.value();
    }

  private:
    Return* returnInfo_ = nullptr;
  };

  template<>
  class To_Ruby<Array&>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
    {
    }

    VALUE convert(Array const& x)
    {
      return x.value();
    }

  private:
    Return* returnInfo_ = nullptr;
  };

  template<>
  class To_Ruby<Array*>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
    {
    }

    VALUE convert(Array const* x)
    {
      return x->value();
    }

  private:
    Return* returnInfo_ = nullptr;
  };

  template<>
  class From_Ruby<Array>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg)
    {
    }

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

    static VALUE rubyKlass()
    {
      return rb_cHash;
    }
  };

  template<>
  class To_Ruby<Hash>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
    {
    }

    VALUE convert(Hash const& x)
    {
      return x.value();
    }

  private:
    Return* returnInfo_ = nullptr;
  };

  template<>
  class From_Ruby<Hash>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg)
    {
    }

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

    static VALUE rubyKlass()
    {
      return rb_cSymbol;
    }
  };
  
  template<>
  class To_Ruby<Symbol>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
    {
    }

    VALUE convert(Symbol const& x)
    {
      return x.value();
    }

  private:
    Return* returnInfo_ = nullptr;
  };


  template<>
  class To_Ruby<Symbol&>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
    {
    }

    VALUE convert(Symbol const& x)
    {
      return x.value();
    }

  private:
    Return* returnInfo_ = nullptr;
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

  template<typename Function_T>
  inline void Module::wrap_native_function(VALUE klass, std::string name, Function_T&& function, MethodInfo* methodInfo)
  {
    detail::NativeFunction<Function_T>::define(klass, name, std::forward<Function_T>(function), methodInfo);
  }

  template<typename Class_T, typename Method_T>
  inline void Module::wrap_native_method(VALUE klass, std::string name, Method_T&& method, MethodInfo* methodInfo)
  {
    detail::NativeMethod<Class_T, Method_T>::define(klass, name, std::forward<Method_T>(method), methodInfo);
  }

  inline Module define_module_under(Object parent, char const* name)
  {
    VALUE module = detail::protect(rb_define_module_under, parent.value(), name);
    detail::Registries::instance.modules.add(module);
    return module;
  }

  inline Module define_module(char const* name)
  {
    VALUE module = detail::protect(rb_define_module, name);
    detail::Registries::instance.modules.add(module);
    return module;
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

    static VALUE rubyKlass()
    {
      return rb_cModule;
    }
  };

  template<>
  class To_Ruby<Module>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
    {
    }

    VALUE convert(Module const& x)
    {
      return x.value();
    }

  private:
    Return* returnInfo_ = nullptr;
  };


  template<>
  class To_Ruby<Module&>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
    {
    }

    VALUE convert(Module const& x)
    {
      return x.value();
    }

  private:
    Return* returnInfo_ = nullptr;
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

  inline const std::string Class::base_name() const
  {
    std::string name = this->name();
    auto regex = std::regex("^.*::");
    std::string result = std::regex_replace(name, regex, "");
    return result;
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
  class To_Ruby<Class&>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
    {
    }

    VALUE convert(Class const& x)
    {
      return x.value();
    }

  private:
    Return* returnInfo_ = nullptr;
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

    static VALUE rubyKlass()
    {
      return rb_cStruct;
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

// =========   Data_Type.ipp   =========
#include <stdexcept>

namespace Rice
{
  template<typename T>
  inline void ruby_mark_internal(detail::WrapperBase* wrapper)
  {
    // Tell the wrapper to mark the objects its keeping alive
    wrapper->ruby_mark();

    // Get the underlying data and call custom mark function (if any)
    T* data = static_cast<T*>(wrapper->get());
    ruby_mark<T>(data);
  }

  template<typename T>
  inline void ruby_free_internal(detail::WrapperBase* wrapper)
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
      detail::TypeMapper<T> typeMapper;
      std::string message = "Type " + typeMapper.name() + " is already bound to a different type";
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

    auto instances = unbound_instances();
    for (auto instance: instances)
    {
      instance->set_value(klass);
    }
    instances.clear();

    // Register with the type registry
    detail::Registries::instance.types.add<T>(klass_, rb_data_type_);

    // Add a method to get the source C++ class name from Ruby
    Data_Type<T> dataType;
    dataType.define_singleton_method("cpp_class", [](VALUE klass) -> VALUE
    {
      detail::TypeMapper<T> typeMapper;
      std::string cppClassName = typeMapper.simplifiedName();
      Return returnInfo;
      returnInfo.takeOwnership();
      return detail::To_Ruby<char*>(&returnInfo).convert(cppClassName.c_str());
    }, Arg("klass").setValue(), Return().setValue());

    return dataType;
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

  // Track unbound instances (ie, declared variables of type Data_Type<T>
  // before define_class is called). We can't simply use a static inline
  // member because it sometimes crashes clang and gcc (msvc seems fine)
  template<typename T>
  inline std::set<Data_Type<T>*>& Data_Type<T>::unbound_instances()
  {
    static std::set<Data_Type<T>*> unbound_instances;
    return unbound_instances;
  }

  template<typename T>
  inline Data_Type<T>::Data_Type()
  {
    if (is_bound())
    {
      this->set_value(klass_);
    }
    else
    {
      unbound_instances().insert(this);
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
    else if constexpr (Constructor_T::isMoveConstructor())
    {
      throw std::runtime_error("Rice does not support move constructors");
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
    if (!Data_Type<Director_T>::is_defined())
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
    if (is_bound())
    {
      return detail::protect(rb_obj_is_kind_of, value, klass_) == Qtrue;
    }
    else
    {
      return false;
    }
  }

  template<typename T>
  inline void Data_Type<T>::check_is_bound()
  {
    if (!is_bound())
    {
      detail::TypeMapper<T> typeMapper;
      std::string message = "Type is not defined with Rice: " + typeMapper.name();
      throw std::invalid_argument(message.c_str());
    }
  }

  template<typename T>
  inline bool Data_Type<T>::is_defined()
  {
    return detail::Registries::instance.types.isDefined<T>();
  }

  template<typename T>
  inline bool Data_Type<T>::check_defined(const std::string& name, Object parent)
  {
    if (Data_Type<T>::is_defined())
    {
      Data_Type<T> dataType;
      parent.const_set_maybe(name, dataType.klass());
      return true;
    }
    else
    {
      return false;
    }
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
    if (Rice::Data_Type<T>::check_defined(id.str(), parent))
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

    if (Rice::Data_Type<T>::check_defined(klassName))
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
  inline Data_Type<T>& Data_Type<T>::define_attr(std::string name, Attribute_T attribute, AttrAccess access, Return returnInfo)
  {
    return this->define_attr_internal<Attribute_T>(this->klass_, name, std::forward<Attribute_T>(attribute), access, returnInfo);
  }

  template <typename T>
  template <typename Attribute_T>
  inline Data_Type<T>& Data_Type<T>::define_singleton_attr(std::string name, Attribute_T attribute, AttrAccess access, Return returnInfo)
  {
    VALUE singleton = detail::protect(rb_singleton_class, this->value());
    return this->define_attr_internal<Attribute_T>(singleton, name, std::forward<Attribute_T>(attribute), access, returnInfo);
  }

  template <typename T>
  template <typename Attribute_T>
  inline Data_Type<T>& Data_Type<T>::define_attr_internal(VALUE klass, std::string name, Attribute_T attribute, AttrAccess access, Return returnInfo)
  {
    using Attr_T = typename detail::attribute_traits<Attribute_T>::attr_type;

    // Define attribute getter
    if (access == AttrAccess::ReadWrite || access == AttrAccess::Read)
    {
      detail::NativeAttributeGet<Attribute_T>::define(klass, name, std::forward<Attribute_T>(attribute), returnInfo);
    }

    // Define attribute setter
    // Define attribute setter
    if (access == AttrAccess::ReadWrite || access == AttrAccess::Write)
    {
      // This seems super hacky - must be a better way?
      constexpr bool checkWriteAccess = !std::is_reference_v<Attr_T> && 
                                        !std::is_pointer_v<Attr_T> &&
                                        !std::is_fundamental_v<Attr_T> &&
                                        !std::is_enum_v<Attr_T>;
      
      if constexpr (std::is_const_v<Attr_T>)
      {
        throw std::runtime_error("Cannot define attribute writer for a const attribute: " + name);
      }
      // Attributes are set using assignment operator like this:
      //   myInstance.attribute = newvalue
      else if constexpr (checkWriteAccess && !std::is_assignable_v<Attr_T, Attr_T>)
      {
        throw std::runtime_error("Cannot define attribute writer for a non assignable attribute: " + name);
      }
      // From_Ruby returns a copy of the value for non-reference and non-pointers, thus needs to be copy constructable
      else if constexpr (checkWriteAccess && !std::is_copy_constructible_v<Attr_T>)
      {
        throw std::runtime_error("Cannot define attribute writer for a non copy constructible attribute: " + name);
      }
      else
      {
        detail::NativeAttributeSet<Attribute_T>::define(klass, name, std::forward<Attribute_T>(attribute));
      }
    }

    return *this;
  }

  template <typename T>
  template<typename Method_T>
  inline void Data_Type<T>::wrap_native_method(VALUE klass, std::string name, Method_T&& method, MethodInfo* methodInfo)
  {
    Module::wrap_native_method<T, Method_T>(klass, name, std::forward<Method_T>(method), methodInfo);
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
      detail::wrapConstructed<T>(self, Data_Type<T>::ruby_data_type(), data, true);
    }

    static void initialize_copy(VALUE self, const T& other)
    {
      // Call C++ copy constructor
      T* data = new T(other);
      detail::wrapConstructed<T>(self, Data_Type<T>::ruby_data_type(), data, true);
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
        detail::wrapConstructed<T>(self.value(), Data_Type<T>::ruby_data_type(), data, true);
      }
  };
}
// =========   Callback.hpp   =========

namespace Rice
{
  //! Define a callback.
  /*! When C++ invokes a C style callback, Rice automatically converts the C++ arguments
   *  to Ruby. However, there may be cases where you need to specify how individual arguments
   *  should be handled. For example, callbacks often have a user data parameter which is 
   *  defined as a void pointer (void*). In this case, you need to tell Ruby that the parameter
   *  is opaque and should not be convered. For example:
   * 
   *  define_callback<void(*)(void*)>(Arg("user_data").setOpaque());
   *
   *  \param args a list of Arg instance used to define default parameters (optional)
   *  \return nothing
   */
  template<typename Callback_T, typename...Arg_Ts>
  void define_callback(const Arg_Ts&...args);
}

// =========   Callback.ipp   =========
namespace Rice
{
  template<typename Callback_T, typename...Arg_Ts>
  void define_callback(const Arg_Ts&...args)
  {
    MethodInfo* methodInfo = new MethodInfo(detail::function_traits<Callback_T>::arity, args...);
  #ifdef HAVE_LIBFFI
    detail::NativeCallbackFFI<Callback_T>::setMethodInfo(methodInfo);
  #else
    detail::NativeCallbackSimple<Callback_T>::setMethodInfo(methodInfo);
  #endif
  }
}
// =========   Data_Object.ipp   =========

#include <algorithm>

namespace Rice
{
  template <typename T>
  Exception create_type_exception(VALUE value)
  {
    if constexpr (std::is_pointer_v<T>)
    {
      return Exception(rb_eTypeError, "Wrong argument type. Expected: %s. Received: %s.",
        detail::protect(rb_class2name, Data_Type<Pointer<std::remove_cv_t<std::remove_pointer_t<T>>>>::klass().value()),
        detail::protect(rb_obj_classname, value));
    }
    else
    {
      return Exception(rb_eTypeError, "Wrong argument type. Expected: %s. Received: %s.",
        detail::protect(rb_class2name, Data_Type<detail::intrinsic_type<T>>::klass().value()),
        detail::protect(rb_obj_classname, value));
    }
  }

  template<typename T>
  inline Data_Object<T>::Data_Object(T& data, bool isOwner, Class klass)
  {
    VALUE value = detail::wrap(klass, Data_Type<T>::ruby_data_type(), data, isOwner);
    this->set_value(value);
  }

  template<typename T>
  inline Data_Object<T>::Data_Object(T&& data, Class klass)
  {
    VALUE value = detail::wrap(klass, Data_Type<T>::ruby_data_type(), data, true);
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
  inline Data_Object<T>::Data_Object(VALUE value) : Object(value)
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
}

namespace Rice::detail
{
  template<typename T>
  class To_Ruby
  {
    static_assert(!std::is_fundamental_v<intrinsic_type<T>>,
                  "Data_Object cannot be used with fundamental types");

    static_assert(!std::is_same_v<T, std::map<T, T>> && !std::is_same_v<T, std::unordered_map<T, T>> &&
                  !std::is_same_v<T, std::monostate> && !std::is_same_v<T, std::multimap<T, T>> &&
                  !std::is_same_v<T, std::optional<T>> && !std::is_same_v<T, std::pair<T, T>> &&
                  !std::is_same_v<T, std::set<T>> && !std::is_same_v<T, std::string> &&
                  !std::is_same_v<T, std::vector<T>>,
                  "Please include rice/stl.hpp header for STL support");

  public:
    To_Ruby() = default;

    explicit To_Ruby(Arg* arg) : arg_(arg)
    {
    }

    template<typename U>
    VALUE convert(U& data)
    {
      // Get the ruby typeinfo
      std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::Registries::instance.types.figureType<T>(data);

      // We always take ownership of data passed by value (yes the parameter is T& but the template
      // matched <typename T> thus we have to tell wrap to copy the reference we are sending to it
      return detail::wrap(rubyTypeInfo.first, rubyTypeInfo.second, data, true);
    }

    template<typename U>
    VALUE convert(U&& data)
    {
      // Get the ruby typeinfo
      std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::Registries::instance.types.figureType<T>(data);

      // We always take ownership of data passed by value (yes the parameter is T& but the template
      // matched <typename T> thus we have to tell wrap to copy the reference we are sending to it
      return detail::wrap(rubyTypeInfo.first, rubyTypeInfo.second, data, true);
    }

  private:
    Arg* arg_ = nullptr;
  };

  template <typename T>
  class To_Ruby<T&>
  {
    static_assert(!std::is_fundamental_v<intrinsic_type<T>>,
                  "Data_Object cannot be used with fundamental types");

    static_assert(!std::is_same_v<T, std::map<T, T>> && !std::is_same_v<T, std::unordered_map<T, T>> &&
                  !std::is_same_v<T, std::monostate> && !std::is_same_v<T, std::multimap<T, T>> &&
                  !std::is_same_v<T, std::optional<T>> && !std::is_same_v<T, std::pair<T, T>> &&
                  !std::is_same_v<T, std::set<T>> && !std::is_same_v<T, std::string> &&
                  !std::is_same_v<T, std::vector<T>>,
                  "Please include rice/stl.hpp header for STL support");

  public:
    To_Ruby() = default;

    explicit To_Ruby(Arg* arg) : arg_(arg)
    {
    }

    template<typename U>
    VALUE convert(U& data)
    {
      // Note that T could be a pointer or reference to a base class while data is in fact a
      // child class. Lookup the correct type so we return an instance of the correct Ruby class
      std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::Registries::instance.types.figureType<T>(data);

      bool isOwner = (this->arg_ && this->arg_->isOwner());
      return detail::wrap(rubyTypeInfo.first, rubyTypeInfo.second, data, isOwner);
    }
  private:
    Arg* arg_ = nullptr;
  };

  template <typename T, int N>
  class To_Ruby<T[N]>
  {
    static_assert(!std::is_same_v<T, std::map<T, T>> && !std::is_same_v<T, std::unordered_map<T, T>> &&
      !std::is_same_v<T, std::monostate> && !std::is_same_v<T, std::multimap<T, T>> &&
      !std::is_same_v<T, std::optional<T>> && !std::is_same_v<T, std::pair<T, T>> &&
      !std::is_same_v<T, std::set<T>> && !std::is_same_v<T, std::string> &&
      !std::is_same_v<T, std::vector<T>>,
      "Please include rice/stl.hpp header for STL support");

  public:
    To_Ruby() = default;

    explicit To_Ruby(Arg* arg) : arg_(arg)
    {
    }

    template<typename U>
    VALUE convert(U data[N])
    {
      Buffer<T> buffer(data, N);
      Data_Object<Buffer<T>> dataObject(std::move(buffer));
      return dataObject.value();
    }

  private:
    Arg* arg_ = nullptr;
  };

  template <typename T>
  class To_Ruby<T*>
  {
    static_assert(!std::is_same_v<T, std::map<T, T>> && !std::is_same_v<T, std::unordered_map<T, T>> &&
                  !std::is_same_v<T, std::monostate> && !std::is_same_v<T, std::multimap<T, T>> &&
                  !std::is_same_v<T, std::optional<T>> && !std::is_same_v<T, std::pair<T, T>> &&
                  !std::is_same_v<T, std::set<T>> && !std::is_same_v<T, std::string> &&
                  !std::is_same_v<T, std::vector<T>>,
                  "Please include rice/stl.hpp header for STL support");

  public:
    To_Ruby() = default;

    explicit To_Ruby(Arg* arg) : arg_(arg)
    {
    }

    template<typename U>
    VALUE convert(U* data)
    {
      bool isOwner = this->arg_ && this->arg_->isOwner();
      bool isBuffer = this->arg_ && this->arg_->isBuffer();

      if (data == nullptr)
      {
        return Qnil;
      }
      else if (std::is_fundamental_v<std::remove_pointer_t<T>> || isBuffer)
      {
        using Pointer_T = Pointer<remove_cv_recursive_t<U>>;
        return detail::wrap(Data_Type<Pointer_T>::klass(), Data_Type<Pointer_T>::ruby_data_type(), data, isOwner);
      }
      else
      {
        // Note that T could be a pointer or reference to a base class while data is in fact a
        // child class. Lookup the correct type so we return an instance of the correct Ruby class
        std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::Registries::instance.types.figureType(*data);
        return detail::wrap(rubyTypeInfo.first, rubyTypeInfo.second, data, isOwner);
      }
    }
    
  private:
    Arg* arg_ = nullptr;
  };

  template <typename T>
    class To_Ruby<T*&>
  {
    static_assert(!std::is_same_v<T, std::map<T, T>> && !std::is_same_v<T, std::unordered_map<T, T>> &&
                  !std::is_same_v<T, std::monostate> && !std::is_same_v<T, std::multimap<T, T>> &&
                  !std::is_same_v<T, std::optional<T>> && !std::is_same_v<T, std::pair<T, T>> &&
                  !std::is_same_v<T, std::set<T>> && !std::is_same_v<T, std::string> &&
                  !std::is_same_v<T, std::vector<T>>,
                  "Please include rice/stl.hpp header for STL support");

  public:
    To_Ruby() = default;

    explicit To_Ruby(Arg* arg) : arg_(arg)
    {
    }

    template<typename U>
    VALUE convert(U* data)
    {
      bool isOwner = this->arg_ && this->arg_->isOwner();
      bool isBuffer = this->arg_ && this->arg_->isBuffer();
 
      if (data == nullptr)
      {
        return Qnil;
      }
      else if constexpr (std::is_const_v<U>)
      {
        std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::Registries::instance.types.figureType(*data);
        return detail::wrap(rubyTypeInfo.first, rubyTypeInfo.second, data, isOwner);
      }
      else if (std::is_fundamental_v<T> || isBuffer)
      {
        using Pointer_T = Pointer<remove_cv_recursive_t<U>>;
        return detail::wrap(Data_Type<Pointer_T>::klass(), Data_Type<Pointer_T>::ruby_data_type(), data, isOwner);
      }
      else
      {
        // Note that T could be a pointer or reference to a base class while data is in fact a
        // child class. Lookup the correct type so we return an instance of the correct Ruby class
        std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::Registries::instance.types.figureType(*data);
        return detail::wrap(rubyTypeInfo.first, rubyTypeInfo.second, data, isOwner);
      }
    }

  private:
    Arg* arg_ = nullptr;
  };

  template <typename T>
  class To_Ruby<T**>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Arg* arg) : arg_(arg)
    {
    }

    template<typename U>
    VALUE convert(U** data)
    {
      if (data)
      {
        bool isOwner = this->arg_ && this->arg_->isOwner();
        using Pointer_T = Pointer<remove_cv_recursive_t<U>*>;
        return detail::wrap(Data_Type<Pointer_T>::klass(), Data_Type<Pointer_T>::ruby_data_type(), data, isOwner);
      }
      else
      {
        return Qnil;
      }
    }

  private:
    Arg* arg_ = nullptr;
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

    static_assert(!std::is_same_v<T, std::map<T, T>> || !std::is_same_v<T, std::unordered_map<T, T>> ||
                  !std::is_same_v<T, std::monostate> || !std::is_same_v<T, std::multimap<T, T>> ||
                  !std::is_same_v<T, std::optional<T>> || !std::is_same_v<T, std::pair<T, T>> ||
                  !std::is_same_v<T, std::set<T>> || !std::is_same_v<T, std::string> ||
                  !std::is_same_v<T, std::vector<T>>,
                  "Please include rice/stl.hpp header for STL support");

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
      Intrinsic_T* instance = detail::unwrap<Intrinsic_T>(value, Data_Type<Intrinsic_T>::ruby_data_type(), this->arg_ && this->arg_->isOwner());

      if constexpr (std::is_constructible_v<T, T> && !std::is_convertible_v<T, T>)
      {
        return T(*instance);
      }
      else
      {
        return *instance;
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

    static_assert(!std::is_same_v<T, std::map<T, T>>   && !std::is_same_v<T, std::unordered_map<T, T>> &&
                  !std::is_same_v<T, std::monostate>   && !std::is_same_v<T, std::multimap<T, T>>      &&
                  !std::is_same_v<T, std::optional<T>> && !std::is_same_v<T, std::pair<T, T>>          &&
                  !std::is_same_v<T, std::set<T>>      && !std::is_same_v<T, std::string>              &&
                  !std::is_same_v<T, std::vector<T>>,
                  "Please include rice/stl.hpp header for STL support");

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

      return *detail::unwrap<Intrinsic_T>(value, Data_Type<Intrinsic_T>::ruby_data_type(), this->arg_ && this->arg_->isOwner()); 
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<typename T>
  class From_Ruby<T&&>
  {
    static_assert(!std::is_fundamental_v<intrinsic_type<T>>,
                  "Data_Object cannot be used with fundamental types");

    static_assert(!std::is_same_v<T, std::map<T, T>> && !std::is_same_v<T, std::unordered_map<T, T>> &&
                  !std::is_same_v<T, std::monostate> && !std::is_same_v<T, std::multimap<T, T>> &&
                  !std::is_same_v<T, std::optional<T>> && !std::is_same_v<T, std::pair<T, T>> &&
                  !std::is_same_v<T, std::set<T>> && !std::is_same_v<T, std::string> &&
                  !std::is_same_v<T, std::vector<T>>,
                  "Please include rice/stl.hpp header for STL support");

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

      Intrinsic_T* object = detail::unwrap<Intrinsic_T>(value, Data_Type<Intrinsic_T>::ruby_data_type(), this->arg_ && this->arg_->isOwner());
      return std::move(*object);
    }

  private:
    Arg* arg_ = nullptr;
  };

  // 99% of the time a T* represents a wrapped C++ object that we want to call methods on. However, T* 
  // could also be a pointer to an array of T objects, so T[]. OpenCV for example has API calls like this.
  template<typename T>
  class From_Ruby<T*>
  {
    static_assert(!std::is_same_v<T, std::map<T, T>> && !std::is_same_v<T, std::unordered_map<T, T>> &&
                  !std::is_same_v<T, std::monostate> && !std::is_same_v<T, std::multimap<T, T>> &&
                  !std::is_same_v<T, std::optional<T>> && !std::is_same_v<T, std::pair<T, T>> &&
                  !std::is_same_v<T, std::set<T>> && !std::is_same_v<T, std::string> &&
                  !std::is_same_v<T, std::vector<T>>,
                  "Please include rice/stl.hpp header for STL support");

    using Intrinsic_T = intrinsic_type<T>;

  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_NIL:
          return Convertible::Exact;
          break;
        case RUBY_T_DATA:
          if (Data_Type<T>::is_descendant(value))
          {
            return Convertible::Exact;
          }
          else if (Data_Type<Pointer<T>>::is_descendant(value))
          {
            return Convertible::Exact;
          }
          [[fallthrough]];
        default:
          return Convertible::None;
      }
    }

    T* convert(VALUE value)
    {
      bool isOwner = this->arg_ && this->arg_->isOwner();

      switch (rb_type(value))
      {
        case RUBY_T_NIL:
        {
          return nullptr;
          break;
        }
        case RUBY_T_DATA:
        {
          if (Data_Type<T>::is_descendant(value))
          {
            return detail::unwrap<Intrinsic_T>(value, Data_Type<Intrinsic_T>::ruby_data_type(), isOwner);
          }
          else if (Data_Type<Pointer<T>>::is_descendant(value))
          {
            return detail::unwrap<T>(value, Data_Type<Pointer<T>>::ruby_data_type(), isOwner);
          }
          [[fallthrough]];
        }
        default:
        {
          throw create_type_exception<T*>(value);
        }
      }
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<typename T>
  class From_Ruby<T*&>
  {
    static_assert(!std::is_same_v<T, std::map<T, T>> && !std::is_same_v<T, std::unordered_map<T, T>> &&
                  !std::is_same_v<T, std::monostate> && !std::is_same_v<T, std::multimap<T, T>> &&
                  !std::is_same_v<T, std::optional<T>> && !std::is_same_v<T, std::pair<T, T>> &&
                  !std::is_same_v<T, std::set<T>> && !std::is_same_v<T, std::string> &&
                  !std::is_same_v<T, std::vector<T>>,
                  "Please include rice/stl.hpp header for STL support");

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
        return detail::unwrap<Intrinsic_T>(value, Data_Type<Intrinsic_T>::ruby_data_type(), this->arg_ && this->arg_->isOwner());
      }
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<typename T>
  class From_Ruby<T**>
  {
    static_assert(!std::is_same_v<T, std::map<T, T>> && !std::is_same_v<T, std::unordered_map<T, T>> &&
                  !std::is_same_v<T, std::monostate> && !std::is_same_v<T, std::multimap<T, T>> &&
                  !std::is_same_v<T, std::optional<T>> && !std::is_same_v<T, std::pair<T, T>> &&
                  !std::is_same_v<T, std::set<T>> && !std::is_same_v<T, std::string> &&
                  !std::is_same_v<T, std::vector<T>>,
                  "Please include rice/stl.hpp header for STL support");

    using Intrinsic_T = intrinsic_type<T>;
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
          return Data_Type<Pointer<T*>>::is_descendant(value) ? Convertible::Exact : Convertible::None;
          break;
        case RUBY_T_NIL:
          return Convertible::Exact;
          break;
        default:
          return Convertible::None;
      }
    }

    T** convert(VALUE value)
    {
      bool isOwner = this->arg_ && this->arg_->isOwner();

      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          T** result = detail::unwrap<Intrinsic_T*>(value, Data_Type<Pointer<T*>>::ruby_data_type(), isOwner);
          return result;
          break;
        }
        case RUBY_T_NIL:
        {
          return nullptr;
          break;
        }
        default:
        {
          throw create_type_exception<T**>(value);
        }
      }
    }

  private:
    Arg* arg_ = nullptr;
    std::vector<Intrinsic_T*> vector_;
  };

  template<typename T>
  class From_Ruby<Data_Object<T>>
  {
    static_assert(!std::is_fundamental_v<intrinsic_type<T>>,
                  "Data_Object cannot be used with fundamental types");

    static_assert(!std::is_same_v<T, std::map<T, T>> && !std::is_same_v<T, std::unordered_map<T, T>> &&
                  !std::is_same_v<T, std::monostate> && !std::is_same_v<T, std::multimap<T, T>> &&
                  !std::is_same_v<T, std::optional<T>> && !std::is_same_v<T, std::pair<T, T>> &&
                  !std::is_same_v<T, std::set<T>> && !std::is_same_v<T, std::string> &&
                  !std::is_same_v<T, std::vector<T>>,
                  "Please include rice/stl.hpp header for STL support");

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

    klass.define_method("to_s", [](Enum_T& self) -> String
      {
        return String(valuesToNames_[self]);
      })
      .define_method("to_int", [](Enum_T& self) ->  Underlying_T
      {
        return (Underlying_T)(self);
      })
      .define_method("coerce", [](Enum_T& self, Underlying_T& other) -> Array
      {
        /* Other will be a numeric value that matches the underlying type of the enum, for example an int.
           Convert that to the enum type and then create new Ruby object to wrap it. This then enables code
           like this:
        
           Colors::Red | Colors:Blue | Colors:Green

        Colors::Red | Colors:Blue returns an integer. Then this method converts the integer back into an Enum
        instance so that Colors:Blue | Colors:Green works. */
        Enum_T otherEnum = (Enum_T)other;

        Array result;
        result.push(self, false);
        result.push(otherEnum, true);
        return result;
      })
      .define_method("inspect", [](Enum_T& self)
      {
        std::stringstream result;
        VALUE rubyKlass = Enum<Enum_T>::klass().value();
        result << "#<" << detail::protect(rb_class2name, rubyKlass)
          << "::" << Enum<Enum_T>::valuesToNames_[self] << ">";

        // We have to return string because we don't know if std::string support has
        // been included by the user
        return String(result.str());
      })
      .define_method("hash", [](Enum_T& self) ->  Underlying_T
      {
        return (Underlying_T)self;
      })
      .define_method("eql?", [](Enum_T& self, Enum_T& other)
      {
        return self == other;
    });

    // Add aliases
    rb_define_alias(klass, "===", "eql?");
    rb_define_alias(klass, "to_i", "to_int");

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

    // Add ability to get enum values
    klass.define_singleton_method("values", [](VALUE ruby_klass) -> VALUE
    {
      Array result;

      for (auto& pair : valuesToNames_)
      {
        Object enumValue = Class(ruby_klass).const_get(pair.second);
        result.push(enumValue, false);
      }

      return result;
    }, Return().setValue());

    // Add bitwise operators
    klass.define_method("&", [](Enum_T& self, Enum_T& other) -> Underlying_T
      {
        return (Underlying_T)self & (Underlying_T)other;
      })
      .define_method("|", [](Enum_T& self, Enum_T& other) -> Underlying_T
      {
        return (Underlying_T)self | (Underlying_T)other;
      })
      .define_method("^", [](Enum_T& self, Enum_T& other) -> Underlying_T
      {
        return (Underlying_T)self ^ (Underlying_T)other;
      })
      .define_method("~", [](Enum_T& self) -> Underlying_T
      {
        return ~(Underlying_T)self;
      });

    // Add shift operators
    klass.define_method("<<", [](Enum_T& self, int shift) -> Underlying_T
      {
        return (Underlying_T)self << shift;
      })
      .define_method(">>", [](Enum_T& self, int shift) -> Underlying_T
      {
        return (Underlying_T)self >> shift;
      });

    // Add conversions from int
    klass.define_singleton_method("from_int", [](VALUE ruby_klass, int32_t value) -> Object
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

namespace Rice::Libc
{
  inline Class rb_cLibcFile;

  inline void define_libc_file()
  {
    Module rb_mLibc = define_module("Libc");
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
      if (!Data_Type<FILE>::is_defined())
      {
        Libc::define_libc_file();
      }

      return true;
    }
  };
}


#endif // Rice__hpp_
