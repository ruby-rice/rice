
// =========   ruby.hpp   =========

/*! \file
 * \brief Hacks for addressing incompatibilities between various Ruby
 * versions.
 */

#include <cmath>

// missing.h that comes with the one-click installer doesn't properly
// check for double-definition of isinf
#ifdef isinf
#define HAVE_ISINF
#endif

#include <ruby.h>

// ruby.h has a few defines that conflict with Visual Studio's STL
#if defined(_MSC_VER)
#undef write
#undef read
#undef bind
#endif

// And some c library conflicts
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



// =========   rice_traits.hpp   =========

#include <string>
#include <type_traits>

namespace Rice
{
  namespace detail
  {
    // Get the base_type of T - without pointer, reference, const or volatile
    template<typename T>
    using base_type = typename std::remove_cv_t<std::remove_reference_t<std::remove_pointer_t<T>>>;

    // Helper to determine if we have to keep a local copy of a value converted from Ruby.
    // This is needed for primitive types (numeric, char, bool and we include strings)
    template <typename T, typename = void>
    struct is_primitive : public std::false_type {};

    template <typename T>
    struct is_primitive<T, std::enable_if_t<std::is_fundamental_v<base_type<T>> ||
      std::is_same_v<std::string, base_type<T>>>> : public std::true_type {};

    template <typename T>
    constexpr bool is_primitive_v = is_primitive<T>::value;

  } // detail
} // Rice


// =========   Arguments.hpp   =========

#include <vector>

namespace Rice
{

// TODO - forward declaration
class Arg;

class Arguments
{
public:
  template <typename...Arg_Ts>
  Arguments(Arg_Ts...args);

  /**
    * Get the full argument count of this
    * list of arguments.
    * Returns -1 no defined arguments
    */
  int count();

  /**
    * Get the rb_scan_args format string for this
    * list of arguments.
    * In the case of no Args (default case), this
    * method uses the passed in full argument count
    */
  std::string formatString(size_t fullArgCount);

  /**
    * Add a defined Arg to this list of Arguments
    */
  void add(const Arg& arg);

  /**
    * Is the argument at the request location an optional
    * argument?
    */
  bool isOptional(unsigned int pos);

  /**
    * Given a position, a type, and a ruby VALUE, figure out
    * what argument value we need to return according to
    * defaults and if that VALUE is nil or not
    */
  template<typename Arg_T>
  Arg_T& defaultValue(int pos);

private:
  std::vector<Arg> args_;

  /** Keep counts of required and optional parameters */
  int required_ = 0;
  int optional_ = 0;
};

} // rice


// =========   default_allocation_func.hpp   =========

namespace Rice
{

namespace detail
{

//! A default implementation of an allocate_func.  This function does no
//! actual allocation; the initialize_func can later do the real
//! allocation with: DATA_PTR(self) = new Type(arg1, arg2, ...)
template<typename T>
VALUE default_allocation_func(VALUE klass);

} // detail

} // Rice



// =========   demangle.hpp   =========

#include <string>

namespace Rice
{

namespace detail
{

std::string demangle(char const * mangled_name);
std::string demangle(std::string const & mangled_name);

} // detail

} // Rice


// ---------   demangle.ipp   ---------
#ifdef __GNUC__
#include <cxxabi.h>
#include <cstdlib>
#include <cstring>
#endif

inline std::string
Rice::detail::
demangle(char const * mangled_name)
{
#ifdef __GNUC__
    struct Helper
  {
    Helper(
        char const * mangled_name)
      : name_(0)
    {
      int status = 0;
      name_ = abi::__cxa_demangle(mangled_name, 0, 0, &status);
    }

    ~Helper()
    {
      std::free(name_);
    }

    char * name_;

  private:
    Helper(Helper const &);
    void operator=(Helper const &);
  };

  Helper helper(mangled_name);
  if(helper.name_)
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

inline std::string
Rice::detail::
demangle(std::string const & mangled_name)
{
  return demangle(mangled_name.c_str());
}


// =========   from_ruby.hpp   =========


// ---------   from_ruby_defn.hpp   ---------
#ifndef Rice__detail__from_ruby_defn__hpp_
#define Rice__detail__from_ruby_defn__hpp_

#include <type_traits>


namespace Rice
{
  namespace detail
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
    struct From_Ruby;

  } // detail
} // Rice

#endif // Rice__detail__From_Ruby2_defn__hpp_

// ---------   from_ruby.ipp   ---------
#ifndef Rice__detail__from_ruby__ipp_
#define Rice__detail__from_ruby__ipp_

#include <stdexcept>

/* This file implements conversions from Ruby to native values fo fundamental types 
   such as bool, int, float, etc. It also includes conversions for chars and strings */
namespace Rice
{
  namespace detail
  {
    template<>
    struct From_Ruby<short>
    {
      static short convert(VALUE x)
      {
        return NUM2SHORT(x);
      }
    };

    template<>
    struct From_Ruby<int>
    {
      static int convert(VALUE x)
      {
        return NUM2INT(x);
      }
    };

    template<>
    struct From_Ruby<long>
    {
      static long convert(VALUE x)
      {
        return (long)NUM2LONG(x);
      }
    };

    template<>
    struct From_Ruby<long long>
    {
      static long long convert(VALUE x)
      {
        return RB_NUM2LL(x);
      }
    };

    template<>
    struct From_Ruby<unsigned short>
    {
      static unsigned short convert(VALUE x)
      {
        return NUM2USHORT(x);
      }
    };

    template<>
    struct From_Ruby<unsigned int>
    {
      static unsigned int convert(VALUE x)
      {
        return NUM2UINT(x);
      }
    };

    template<>
    struct From_Ruby<unsigned long>
    {
      static unsigned long convert(VALUE x)
      {
        return (unsigned long)RB_NUM2ULONG(x);
      }
    };

    template<>
    struct From_Ruby<unsigned long long>
    {
      static unsigned long long convert(VALUE x)
      {
        return RB_NUM2ULL(x);
      }
    };

    template<>
    struct From_Ruby<bool>
    {
      static bool convert(VALUE x)
      {
        return RTEST(x);
      }
    };

    template<>
    struct From_Ruby<char>
    {
      static char convert(VALUE x)
      {
        switch (rb_type(x))
        {
          case T_STRING:
          {
            if (RSTRING_LEN(x) == 1)
            {
              return RSTRING_PTR(x)[0];
            }
            else
            {
              throw std::invalid_argument("from_ruby<char>: string must have length 1");
            }
            break;
          }
          case T_FIXNUM:
          {
            return From_Ruby<long>::convert(x) & 0xff;
            break;
          }
          default:
          {
            throw std::invalid_argument("Cannot convert Ruby value to char");
          }
        }
      }
    };

    template<>
    struct From_Ruby<float>
    {
      static float convert(VALUE x)
      {
        return (float)rb_num2dbl(x);
      }
    };

    template<>
    struct From_Ruby<double>
    {
      static double convert(VALUE x)
      {
        return rb_num2dbl(x);
      }
    };

    template<>
    struct From_Ruby<char const*>
    {
      static char const* convert(VALUE x)
      {
        return RSTRING_PTR(x);
      }
    };

    template<>
    struct From_Ruby<std::string>
    {
      static std::string convert(VALUE x)
      {
        return std::string(RSTRING_PTR(x), RSTRING_LEN(x));
      }
    };
  }
}
#endif // Rice__detail__from_ruby__ipp_


// =========   Exception_Handler.hpp   =========


// ---------   Exception_Handler_defn.hpp   ---------
#ifndef Rice__detail__Exception_Handler_defn__hpp_
#define Rice__detail__Exception_Handler_defn__hpp_

#include <memory>

namespace Rice
{

namespace detail
{

/* An abstract class for converting C++ exceptions to ruby exceptions.  It's used
   like this:

   try
   {
   }
   catch(...)
   {
     handler->handle_exception();
   }

 If an exception is thrown the handler will pass the exception up the
 chain, then the last handler in the chain will throw the exception
 down the chain until a lower handler can handle it, e.g.:

 try
 {
   return call_next_exception_handler();
 }
 catch(MyException const & ex)
 {
   throw Rice::Exception(rb_cMyException, "%s", ex.what());
  } 
  
  Memory management. Handlers are created by the ModuleBase constructor. When the
  module defines a new Ruby method, metadata  is stored on the Ruby klass including 
  the exception handler. Since the metadata outlives the module, handlers are stored
  using std::shared_ptr. Thus the Module (or its inherited children) can be destroyed
  without corrupting the metadata references to the shared exception handler. */

class Exception_Handler
{
public:
  Exception_Handler() = default;
  virtual ~Exception_Handler() = default;

  // Don't allow copying or assignment
  Exception_Handler(const Exception_Handler& other) = delete;
  Exception_Handler& operator=(const Exception_Handler& other) = delete;

  virtual VALUE handle_exception() const = 0;
};

// The default exception handler just rethrows the exception.  If there
// are other handlers in the chain, they will try to handle the rethrown
// exception.
class Default_Exception_Handler
  : public Exception_Handler
{
public:
  virtual VALUE handle_exception() const override;
};

// An exception handler that takes a functor as an argument.  The
// functor should throw a Rice::Exception to handle the exception.  If
// the functor does not handle the exception, the exception will be
// re-thrown.
template <typename Exception_T, typename Functor_T>
class Functor_Exception_Handler
  : public Exception_Handler
{
public:
  Functor_Exception_Handler(Functor_T handler,
                            std::shared_ptr<Exception_Handler> next_exception_handler);

  virtual VALUE handle_exception() const override;

private:
  Functor_T handler_;
  std::shared_ptr<Exception_Handler> next_exception_handler_;
};

} // namespace detail

} // namespace Rice

#endif // Rice__detail__Exception_Handler_defn__hpp_


// ---------   Exception_Handler.ipp   ---------
inline
VALUE
Rice::detail::Default_Exception_Handler::
handle_exception() const
{
  throw;
}

template <typename Exception_T, typename Functor_T>
inline
Rice::detail::Functor_Exception_Handler<Exception_T, Functor_T>::
Functor_Exception_Handler(
    Functor_T handler,
    std::shared_ptr<Exception_Handler> next_exception_handler)
  : handler_(handler)
  , next_exception_handler_(next_exception_handler)
{
}

template <typename Exception_T, typename Functor_T>
inline
VALUE
Rice::detail::Functor_Exception_Handler<Exception_T, Functor_T>::
handle_exception() const
{
  try
  {
    return this->next_exception_handler_->handle_exception();
  }
  catch(Exception_T const & ex)
  {
    handler_(ex);
    throw;
  }
}


// =========   Iterator.hpp   =========
#ifndef Rice_Iterator__hpp_
#define Rice_Iterator__hpp_

namespace Rice
{
  namespace detail
  {

    template<typename T, typename Iterator_T>
    class Iterator
    {
    public:
      static VALUE call(VALUE self);

    public:
      Iterator(Iterator_T(T::* begin)(), Iterator_T(T::* end)());
      virtual ~Iterator() = default;
      VALUE operator()(VALUE self);

    private:
      Iterator_T(T::* begin_)();
      Iterator_T(T::* end_)();
    };

  } // detail
} // Rice

#endif // Rice_Iterator__hpp_


// =========   Jump_Tag.hpp   =========

namespace Rice
{

//! A placeholder for Ruby longjmp data.
/*! When a Ruby exception is caught, the tag used for the longjmp is stored in
 *  a Jump_Tag, then later passed to rb_jump_tag() when there is no more
 *  C++ code to pass over.
 */
struct Jump_Tag
{
  //! Construct a Jump_Tag with tag t.
  Jump_Tag(int t) : tag(t) { }

  //! The tag being held.
  int tag;
};

} // namespace Rice



// =========   method_data.hpp   =========

#include <unordered_map>
#include <any>


namespace Rice
{
  namespace detail
  {
    class MethodData
    {
    public:
      // Defines a new Ruby method and stores the Rice metadata about it
      static void define_method(VALUE klass, ID id, VALUE(*cfunc)(ANYARGS), int arity, std::any data);

      // Returns the Rice data for the currently active Ruby method
      template <typename Return_T>
      static Return_T data();

    private:
      static size_t key(VALUE klass, ID id);
      inline static std::unordered_map<size_t, std::any> methodWrappers_ = {};
    };
  }
} 

// ---------   method_data.ipp   ---------

// Ruby 2.7 now includes a similarly named macro that uses templates to
// pick the right overload for the underlying function. That doesn't work
// for our cases because we are using this method dynamically and get a
// compilation error otherwise. This removes the macro and lets us fall
// back to the C-API underneath again.
#undef rb_define_method_id

namespace Rice
{
  namespace detail
  {
    // Effective Java (2nd edition)
    // https://stackoverflow.com/a/2634715
    inline size_t MethodData::key(VALUE klass, ID id)
    {
      if (rb_type(klass) == T_ICLASS)
      {
        klass = rb_class_of(klass);
      }

      uint32_t prime = 53;
      return (prime + klass) * prime + id;
    }

    template <typename Return_T>
    inline Return_T MethodData::data()
    {
      ID id;
      VALUE klass;
      if (!rb_frame_method_id_and_class(&id, &klass))
      {
        rb_raise(rb_eRuntimeError, "Cannot get method id and class for function");
      }

      auto iter = methodWrappers_.find(key(klass, id));
      if (iter == methodWrappers_.end())
      {
        rb_raise(rb_eRuntimeError, "Could not find data for klass and method id");
      }

      std::any data = iter->second;
      return std::any_cast<Return_T>(data);
    }

    inline void
    MethodData::define_method(VALUE klass, ID id, VALUE(*cfunc)(ANYARGS), int arity, std::any data)
    {
      // Define the method
      rb_define_method_id(klass, id, cfunc, arity);

      // Now store data about it
      methodWrappers_[key(klass, id)] = data;
    }
  }
}


// =========   NativeArg.hpp   =========


namespace Rice
{
  namespace detail
  {
    // Primary Template for NativeArg
    template <typename T, typename Enable = void>
    class NativeArg;

    // NativeArg implementation that works on fundamental types. It keeps a copy of the
    // converted value so that a reference or a pointer to the value can be passed to 
    // the native function.
    template <typename T>
    class NativeArg<T, typename std::enable_if_t<is_primitive_v<T>>>
    {
    public:
      using Base_T = std::decay_t<std::remove_pointer_t<T>>;

      T nativeValue(VALUE value)
      {
        this->native_ = From_Ruby<Base_T>::convert(value);

        if constexpr (std::is_pointer_v<T>)
        {
          return &this->native_;
        }
        else
        {
          return this->native_;
        }
      }

    private:
      Base_T native_;
    };

    // NativeArg implementation that works on all other types. The primary use is for 
    // pointers wrapped by Data_Object where there is no reason to store a local copy.
    // It is also used for converting to various Rice C++ wrappers such as Rice::Hash,
    // Rice::String, etc.
    template <typename T>
    class NativeArg<T, typename std::enable_if_t<!is_primitive_v<T>>>
    {
    public:
      T nativeValue(VALUE value)
      {
        return From_Ruby<T>::convert(value);
      }
    };

  } // detail
} // Rice


// =========   protect.hpp   =========


/*! \file
 *  \brief Functions for making exception-safe calls into Ruby code.
 *  These are the building blocks for building other exception-safe
 *  helper functions.
 */

namespace Rice
{

namespace detail
{

//! Call the given function, converting Ruby exceptions to C++
//! exceptions.
/*! Call the function f with the parameter arg  If f raises a Ruby
 * exception, then the exception is re-thrown as an Exception.  If f
 * exits with any other non-zero tag (e.g. a Symbol is thrown), then the
 * tag is re-thrown as a Jump_Tag.
 */
VALUE protect(
    RUBY_VALUE_FUNC f,
    VALUE arg);

} // namespace detail

} // namespace Rice



// =========   to_ruby.hpp   =========


// ---------   to_ruby_defn.hpp   ---------
#ifndef Rice__detail__to_ruby_defn__hpp_
#define Rice__detail__to_ruby_defn__hpp_


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
    template <typename T, typename = void>
    struct To_Ruby;
   
    // Supports int*, bool*, std::string*, etc but *NOT* any C++ structs/classes
    template<typename T>
    struct To_Ruby<T*, std::enable_if_t<is_primitive_v<T>>>
    {
      static VALUE convert(T const* x)
      {
        return To_Ruby<base_type<T>>::convert(*x);
      }
    };

    template<typename T>
    struct To_Ruby<T&>
    {
      static VALUE convert(T const& x)
      {
        return To_Ruby<base_type<T>>::convert(x);
      }
    };

    // Helper template function that let's users avoid having to specify the template type - its deduced
    template <typename T>
    VALUE to_ruby(T&& x)
    {
      return To_Ruby<T>::convert(std::forward<T>(x));
    }

    // Helper template function that let's users avoid having to specify the template type - its deduced
    template <typename T>
    VALUE to_ruby(T* x)
    {
      return To_Ruby<T*>::convert(x);
    }

  } // detail
} // Rice

#endif // Rice__detail__to_ruby_defn__hpp_

// ---------   to_ruby.ipp   ---------
namespace Rice
{
  namespace detail
  {
    // This template is never called but needs to exist for MSVC to successfully compile
    // this line from Wrapped_Function.ipp:
    // 
    //   return To_Ruby<Return_T>::convert(result);
    // 
    // Note there is a if constexpr check for Return_T being void. Sigh.
    template<>
    struct To_Ruby<void>
    {
      static VALUE convert()
      {
        return Qnil;
      }
    };

    template<>
    struct To_Ruby<short>
    {
      static VALUE convert(short const& x)
      {
        return INT2NUM(x);
      }
    };

    template<>
    struct To_Ruby<int>
    {
      static VALUE convert(int const& x)
      {
        return INT2NUM(x);
      }
    };

    template<>
    struct To_Ruby<long>
    {
      static VALUE convert(long const& x)
      {
        return LONG2NUM(x);
      }
    };

    template<>
    struct To_Ruby<long long>
    {
      static VALUE convert(long long const& x)
      {
        return LL2NUM(x);
      }
    };

    template<>
    struct To_Ruby<unsigned short>
    {
      static VALUE convert(unsigned short const& x)
      {
        return UINT2NUM(x);
      }
    };

    template<>
    struct To_Ruby<unsigned int>
    {
      static VALUE convert(unsigned int const& x)
      {
        return UINT2NUM(x);
      }
    };

    template<>
    struct To_Ruby<unsigned long>
    {
      static VALUE convert(unsigned long const& x)
      {
        return ULONG2NUM(x);
      }
    };

    template<>
    struct To_Ruby<unsigned long long>
    {
      static VALUE convert(unsigned long long const& x)
      {
        return ULL2NUM(x);
      }
    };

    template<>
    struct To_Ruby<float>
    {
      static VALUE convert(float const& x)
      {
        return rb_float_new(x);
      }
    };

    template<>
    struct To_Ruby<double>
    {
      static VALUE convert(double const& x)
      {
        return rb_float_new(x);
      }
    };

    template<>
    struct To_Ruby<bool>
    {
      static VALUE convert(bool const& x)
      {
        return x ? Qtrue : Qfalse;
      }
    };

    template<>
    struct To_Ruby<char>
    {
      static VALUE convert(char const& x)
      {
        return To_Ruby<int>::convert(x);
      }
    };

    template<>
    struct To_Ruby<unsigned char>
    {
      static VALUE convert(unsigned char const& x)
      {
        return To_Ruby<unsigned int>::convert(x);
      }
    };

    template<>
    struct To_Ruby<const char*>
    {
      static VALUE convert(const char* x)
      {
        return rb_str_new2(x);
      }
    };

    template<>
    struct To_Ruby<std::string>
    {
      static VALUE convert(std::string const& x)
      {
        return rb_str_new(x.data(), (long)x.size());
      }
    };
  }
}


// =========   Ruby_Function.hpp   =========


namespace Rice
{

namespace detail
{
  /* This is functor class that wraps calls to a Ruby C API method. It is needed because
     rb_protect only supports calling methods that take one argument. Thus 
     we invoke rb_protect telling it to invoke Ruby_Function::call with an 
     instance of a Ruby_Function. That instance then in turn calls the original
     Ruby method passing along its required arguments. */

  template<typename Function_T, typename Return_T, typename...Arg_Ts>
  class Ruby_Function
  {
  public:
    static VALUE call(Ruby_Function* rubyFunction);

  public:
    Ruby_Function(Function_T func, const Arg_Ts&... args);
    Return_T operator()();

  private:
    Function_T func_;
    std::tuple<Arg_Ts...> args_;
  };

  template<typename Function_T, typename Return_T, typename...Arg_Ts>
  inline VALUE Ruby_Function<Function_T, Return_T, Arg_Ts...>::
    call(Ruby_Function* rubyFunction)
  {
    if constexpr (std::is_void_v<Return_T>)
    {
      rubyFunction->operator()();
      return Qnil;
    }
    else
    {
      return rubyFunction->operator()();
    }
  }

  template<typename Function_T, typename Return_T, typename...Arg_Ts>
  Ruby_Function<Function_T, Return_T, Arg_Ts...>::
    Ruby_Function(Function_T func, const Arg_Ts&... args)
    : func_(func), args_(std::forward_as_tuple(args...))
  {
  }

  template<typename Function_T, typename Return_T, typename...Arg_Ts>
  Return_T Ruby_Function<Function_T, Return_T, Arg_Ts...>::
    operator()()
  {
    return std::apply(this->func_, this->args_);
  }
} // namespace detail


namespace detail
{
    template<typename Function_T, typename... Arg_Ts>
    decltype(auto) ruby_function(Function_T func, Arg_Ts&&... args)
    {
      using Return_T = std::invoke_result_t<decltype(func), Arg_Ts...>;
      return Ruby_Function<Function_T, Return_T, Arg_Ts...>(func, std::forward<Arg_Ts>(args)...);
    }

} // namespace detail

} // namespace rice




// =========   Wrapped_Function.hpp   =========


namespace Rice
{

namespace detail
{
/* This class wraps a native function call. The most important template parameters
   are Receiver_T and Return_T.

   Receiver_T can have one of the following values:
   
   * std::null_ptr - There is no reciever, thus this is a function or static member function
   * Object - This is used for calling constructors.
   * Class - This is used for translating enums values
   * C++ Object - This is the most common value and is used to invoke a member function 
     on a wrapped instance of a C++ object
   
   Return_T which specifies the return value. It can either be:

   * void - Meaning there is no return value. This is mapped to Qnil in Ruby
   * any other possible value */
  
template<typename Function_T, typename Return_T, typename Receiver_T, typename... Arg_Ts>
class Wrapped_Function
{
// NativeTypes are the types that we pass to the native function. They may or may not include
// the receiver as the first argument. This alias makes it possible to have just one
// implemenation of invokeNative
using NativeTypes = typename std::conditional_t<std::is_same_v<Receiver_T, std::nullptr_t>,
                                                std::tuple<Arg_Ts...>, 
                                                std::tuple<Receiver_T, Arg_Ts...>>;

public:
  // Static member function that Ruby calls
  static VALUE call(int argc, VALUE* argv, VALUE self);

public:
  Wrapped_Function(Function_T func, std::shared_ptr<Exception_Handler> handler, Arguments* arguments);

  // Invokes the wrapped function
  VALUE operator()(int argc, VALUE* argv, VALUE self);

private:
  // Convert Ruby argv pointer to Ruby values
  std::vector<VALUE> getRubyValues(int argc, VALUE* argv);

  // Convert Ruby values to C++ values
  template<typename std::size_t... I>
  std::tuple<Arg_Ts...> getNativeValues(std::vector<VALUE>& values, std::tuple<NativeArg<Arg_Ts>...>& nativeArgs, std::index_sequence<I...>& indices);

  // Figure out the receiver of the function call
  Receiver_T getReceiver(VALUE receiver);

  // Call the underlying C++ function
  VALUE invokeNative(NativeTypes& nativeArgs);
  
private:
  Function_T func_;
  std::unique_ptr<Arguments> arguments_;
  std::shared_ptr<Exception_Handler> handler_;
};

} // detail

} // Rice



// =========   wrap_function.hpp   =========


namespace Rice
{

// TODO - forward declarations
class Class;
class Object;

namespace detail
{

// Used for calling Constructor
template<typename Return_T, typename ...Arg_T>
auto* wrap_function(Return_T(*func)(Object, Arg_T...),
  std::shared_ptr<Exception_Handler> handler,
  Arguments* arguments)
{
  using Function_T = Return_T(*)(Object, Arg_T...);
  return new Wrapped_Function<Function_T, Return_T, Object, Arg_T...>(func, handler, arguments);
}

// Used by Enums
template<typename Return_T, typename ...Arg_T>
auto* wrap_function(Return_T(*func)(Class, Arg_T...),
  std::shared_ptr<Exception_Handler> handler,
  Arguments* arguments)
{
  using Function_T = Return_T(*)(Class, Arg_T...);
  return new Wrapped_Function<Function_T, Return_T, Class, Arg_T...>(func, handler, arguments);
}

// A plain function or static member call
template<typename Return_T, typename ...Arg_T>
auto* wrap_function(Return_T (*func)(Arg_T...),
    std::shared_ptr<Exception_Handler> handler,
    Arguments* arguments)
{
  using Function_T = Return_T(*)(Arg_T...);
  return new Wrapped_Function<Function_T, Return_T, std::nullptr_t, Arg_T...>(func, handler, arguments);
}

// Call a member function on a C++ object
template<typename Return_T, typename Self_T, typename ...Arg_T>
auto* wrap_function(Return_T (Self_T::*func)(Arg_T...),
    std::shared_ptr<Exception_Handler> handler,
    Arguments* arguments)
{
  using Function_T = Return_T(Self_T::*)(Arg_T...);
  return new Wrapped_Function<Function_T, Return_T, Self_T*, Arg_T...>(func, handler, arguments);
}

// Call a const member function on a C++ object
template<typename Return_T, typename Self_T, typename ...Arg_T>
auto* wrap_function(Return_T (Self_T::*func)(Arg_T...) const,
    std::shared_ptr<Exception_Handler> handler,
    Arguments* arguments)
{
  using Function_T = Return_T(Self_T::*)(Arg_T...) const;
  return new Wrapped_Function<Function_T, Return_T, Self_T, Arg_T...>(func, handler, arguments);
}

} // namespace detail

} // namespace Rice



// =========   Arg.hpp   =========

#include <any>

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

    //! Get the name of this Arg
    const std::string name() const;

  private:
    //! Name of the argument
    const std::string name_;

    //! Our saved default value
    std::any defaultValue_;
  };
} // Rice


// ---------   Arg.ipp   ---------
#include <any>
#include <string>

namespace Rice {

inline Arg::Arg(std::string name)
  : name_(name)
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

//! Get the name of this Arg
inline const std::string Arg::name() const
{
  return name_;
}

}  // Rice


// =========   Arguments.ipp   =========
#include <sstream>

namespace Rice
{

template <typename...Arg_Ts>
inline Arguments::Arguments(Arg_Ts...args)
{
  (this->add(args), ...);
}

inline int Arguments::count()
{
  if (required_ == 0 && optional_ == 0)
  {
    return -1;
  }
  else
  {
    return required_ + optional_;
  }
}

inline std::string Arguments::formatString(size_t fullArgCount)
{
  std::stringstream s;
  if (required_ == 0 && optional_ == 0)
  {
    s << fullArgCount << 0;
  }
  else
  {
    s << required_ << optional_;
  }

  return s.str();
}

inline void Arguments::add(const Arg& arg)
{
  args_.push_back(arg);

  if (arg.hasDefaultValue())
  {
    optional_++;
  }
  else
  {
    required_++;
  }
}

inline bool Arguments::isOptional(unsigned int pos)
{
  if (required_ == 0 && optional_ == 0)
  {
    return false;
  }
  if (pos >= args_.size())
  {
    return false;
  }
  return args_[pos].hasDefaultValue();
}

template<typename Arg_T>
inline Arg_T& Arguments::defaultValue(int pos)
{
  return args_[pos].defaultValue<Arg_T>();
}

} // Rice

// =========   Arg_operators.hpp   =========

namespace Rice
{

  //! Build the list of Arg objects into an Arguments object
  /*! Take a list of Arg objects and build up a single Argument
   *  object used later in method dispatch
   */
  [[deprecated("You should no longer enclose multiple Args in parentheses.")]]
  inline Arguments* operator,(Arg arg1, Arg arg2)
  {
    Arguments* a = new Arguments();
    a->add(arg1);
    a->add(arg2);
    return a;
  }

  /*! @see operator,(Arg, Arg)
   */
  [[deprecated("You should no longer enclose multiple Args in parentheses.")]]
  inline Arguments* operator,(Arguments* arguments, Arg arg)
  {
    arguments->add(arg);
    return arguments;
  }

}


// =========   Exception.hpp   =========


// ---------   Exception_defn.hpp   ---------
#ifndef Rice__Exception_defn__hpp_
#define Rice__Exception_defn__hpp_

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
  Exception(const Exception& other, char const * fmt, Arg_Ts&&...args);

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

#endif // Rice__Exception_defn__hpp_
// ---------   Exception.ipp   ---------
#ifndef Rice__Exception__ipp_
#define Rice__Exception__ipp_


inline Rice::Exception::
Exception(VALUE exception): exception_(exception)
{
}

template <typename... Arg_Ts>
inline Rice::Exception::
Exception(const Exception& other, char const* fmt, Arg_Ts&&...args)
: Exception(other.class_of(), fmt, std::forward<Arg_Ts>(args)...)
{
}

template <typename... Arg_Ts>
inline Rice::Exception::
Exception(const VALUE exceptionClass, char const* fmt, Arg_Ts&&...args)
{
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
#endif

  size_t size = std::snprintf(nullptr, 0, fmt, std::forward<Arg_Ts>(args)...);
  this->message_ = std::string(size, '\0');

  // size+1 avoids trunctaing the string. Otherwise snprintf writes n - 1 characters
  // to allow space for null character but we don't need that since std::string
  // will add a null character internally at n + 1
  std::snprintf(&this->message_[0], size + 1, fmt, std::forward<Arg_Ts>(args)...);

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

  // Now create the Ruby exception
  this->exception_ = rb_exc_new2(exceptionClass, this->message_.c_str());
}

inline char const* Rice::Exception::
what() const noexcept
{
  if (this->message_.empty())
  {
    // Cache the message - this allows the returned pointer to be valid for the
    // lifetime of this exception instance.
    VALUE rubyMessage = rb_funcall(this->exception_, rb_intern("message"), 0);
    this->message_ = detail::From_Ruby<std::string>::convert(rubyMessage);
  }
  return this->message_.c_str();
}

inline VALUE Rice::Exception::
class_of() const
{
  return rb_class_of(this->exception_);
}

inline VALUE Rice::Exception::
value() const
{
  return this->exception_;
}
#endif // Rice__Exception__ipp_

// =========   protect.hpp   =========

// This causes problems with certain C++ libraries
#undef TYPE


// ---------   protect.ipp   ---------
// TODO - don't like pulling in Rice headers

namespace Rice
{
  namespace detail
  {
    const int TAG_RAISE = 0x6;

    inline VALUE
    protect(
        RUBY_VALUE_FUNC f,
        VALUE arg)
    {
      int state = 0;
      VALUE retval = rb_protect(f, arg, &state);
      if (state != 0)
      {
        VALUE err = rb_errinfo();
        if (state == TAG_RAISE && RTEST(err))
        {
          // TODO: convert NoMemoryError into bad_alloc?
          rb_set_errinfo(Qnil);
          throw Rice::Exception(err);
        }
        else
        {
          throw Jump_Tag(state);
        }
      }
      return retval;
    }
  }
}
namespace Rice
{

/*! \file
 *  \brief A collection of functions (overloaded on number of
 *  arguments) for calling C functions that might raise Ruby exceptions.
 */

//! Call the C function f with arguments (arg1, arg2, ...).
/*! E.g.:
 *  \code
 *    VALUE x = protect(rb_ary_new);
 *    protect(rb_ary_push, x, INT2NUM(42));
 *  \endcode
 *
 *  Note that this function makes copies of all of its arguments; it
 *  does not take anything by reference.  All of the copies are const so
 *  that protect will not work if f takes a non-const
 *  reference to any of its arguments (though you can use non-const
 *  pointers).
 */
template<typename Func_T, typename ...Arg_Ts>
VALUE protect(Func_T func, const Arg_Ts&... args)
{
  // Create a functor for calling a Ruby fucntion and define some aliases for readability.
  auto rubyFunction = detail::ruby_function(func, args...);
  using RubyFunctionType = decltype(rubyFunction);
  VALUE rubyFunctionAsFakeValue = reinterpret_cast<VALUE>(&rubyFunction);

  // Now call rb_protect sending it the functor we created above - it's call method
  // will invoke the underyling Ruby fuction.
  return detail::protect((RUBY_VALUE_FUNC)&RubyFunctionType::call, rubyFunctionAsFakeValue);
}

} // namespace Rice


// =========   ruby_mark.hpp   =========
#ifndef ruby_mark__hpp
#define ruby_mark__hpp

//! Default function to call to mark a data object.
/*! This function can be specialized for a particular type to override
 *  the default behavior (which is to not mark any additional objects).
 */
template<typename T>
void ruby_mark(T * /* obj */)
{
}

#endif // ruby_mark__hpp

// =========   ruby_try_catch.hpp   =========

#include <stdexcept>


/*! \def RUBY_TRY
 *  \brief Start a block to catch Ruby exceptions and rethrow them.
 */
// Goto is used here to avoid having to use a second try/catch block (we
// can't rb_exc_raise directly out of the catch blocks below, since the
// exceptions will not get properly cleaned up).
// The labels are located before the try and not after it so the function can't
// "fall through" into the exception-handling code accidentally.
#define RUBY_TRY \
  VALUE Rice__ruby_exc = Qnil; \
  int Rice__ruby_jump_tag = 0; \
  \
  goto start_of_RUBY_TRY; \
  \
  Rice__ruby_exception: \
    rb_exc_raise(Rice__ruby_exc); \
  Rice__ruby_jump_tag: \
    rb_jump_tag(Rice__ruby_jump_tag); \
  \
  start_of_RUBY_TRY: \
  try

/*! \def RUBY_RETHROW(ex)
 *  \brief Given a Ruby exception as a VALUE, safely raise the exception as a
 *         Ruby exception.  This should be used inside a RUBY_TRY/RUBY_CATCH
 *         block.
 */
#define RUBY_RETHROW(ex) \
  Rice__ruby_exc = ex; \
  goto Rice__ruby_exception;

/*! \def RUBY_CATCH
 *  \brief End a RUBY_TRY block.
 */
#define RUBY_CATCH \
  catch(::Rice::Exception const & ex) \
  { \
    RUBY_RETHROW(ex.value()); \
  } \
  catch(::Rice::Jump_Tag const & ex) \
  { \
    Rice__ruby_jump_tag = ex.tag; \
    goto Rice__ruby_jump_tag; \
  } \
  catch(std::bad_alloc const & ex) \
  { \
    /* This won't work quite right if the rb_exc_new2 fails; not    */ \
    /* much we can do about that, since Ruby doesn't give us access */ \
    /* to a pre-allocated NoMemoryError object                      */ \
    RUBY_RETHROW(rb_exc_new2(rb_eNoMemError, ex.what())); \
  } \
  catch(std::invalid_argument const & ex) \
  { \
    /* This can raise a NoMemoryError in VERY rare circumstances    */ \
    RUBY_RETHROW(rb_exc_new2(rb_eArgError, ex.what())); \
  } \
  catch(std::domain_error const & ex) \
  { \
    /* This can raise a NoMemoryError in VERY rare circumstances    */ \
    RUBY_RETHROW(rb_exc_new2(rb_eFloatDomainError, ex.what())); \
  } \
  catch(std::out_of_range const & ex) \
  { \
    /* This can raise a NoMemoryError in VERY rare circumstances    */ \
    RUBY_RETHROW(rb_exc_new2(rb_eRangeError, ex.what())); \
  } \
  catch(std::exception const & ex) \
  { \
    /* This can raise a NoMemoryError in VERY rare circumstances    */ \
    RUBY_RETHROW(rb_exc_new2(rb_eRuntimeError, ex.what())); \
  } \
  catch(...) \
  { \
    RUBY_RETHROW(rb_exc_new2(rb_eRuntimeError, "unknown C++ exception thrown")); \
  } \



// =========   Identifier.hpp   =========

#include <string>

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
  Identifier(Symbol const & symbol);

  //! Construct a new Identifier from a c string.
  Identifier(char const * s);

  //! Construct a new Identifier from a string.
  Identifier(std::string const string);

  //! Return a string representation of the Identifier.
  char const * c_str() const;

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


// ---------   Identifier.ipp   ---------
inline Rice::Identifier::
Identifier(ID id)
  : id_(id)
{
}

inline Rice::Identifier::
Identifier(char const * s)
  : id_(rb_intern(s))
{
}

inline Rice::Identifier::
Identifier(std::string const s)
  : id_(rb_intern(s.c_str()))
{
}

inline char const *
Rice::Identifier::
c_str() const
{
  return rb_id2name(id_);
}

inline std::string
Rice::Identifier::
str() const
{
  return c_str();
}

inline VALUE
Rice::Identifier::
to_sym() const
{
  return ID2SYM(id_);
}



// =========   Object.hpp   =========


// ---------   Object_defn.hpp   ---------
#ifndef Rice__Object_defn__hpp_
#define Rice__Object_defn__hpp_

/*! \file Object.hpp
 */


#include <iosfwd>
#include <vector>

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
  Object(VALUE value = Qnil) : value_(value) { }

  //! Destructor
  virtual ~Object() = default;

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
  VALUE const volatile & value() const { return value_; }

  //! Get the class of an object.
  /*! \return the object's Class.
   */
  Class class_of() const;

  //! Compare this object to another object.
  /*! Gets the result of self <=> other and returns the result.  The
   *  result will be less than zero if self < other, greater than zero
   *  if self > other, and equal to zero if self == other.
   */
  int compare(Object const & other) const;

  //! Return a string representation of an object.
  /*! \return the result of calling to_s on the object.  A String is not
   *  returned, because it is not possible to return an instance of a
   *  derived class.
   */
  String to_s() const;

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
  Object instance_eval(String const & s);

  //! Return the type of the underlying C object.
  /*! This is equivalent to calling rb_type(obj).
   * \return the type of the underlying C object (e.g. T_DATA, T_ARRAY,
   * etc.).
   */
  int rb_type() const;

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
  void iv_set(
      Identifier name,
      T const & value);

  //! Get the value of an instance variable.
  /*! \param name the name of the instance variable to get
   *  \return the value of the instance variable
   */
  Object iv_get(
      Identifier name) const;

  //! Get the value of an instance variable, but don't warn if it is
  //unset.
  /*! \param name the name of the instance variable to get
   *  \return the value of the instance variable
   */
  Object attr_get(
      Identifier name) const;

  //! Call the Ruby method specified by 'id' on object 'obj'.
  /*! Pass in arguments (arg1, arg2, ...).  The arguments will be converted to
  *  Ruby objects with to_ruby<>.
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
  template<typename ...ArgT>
  Object call(Identifier id, ArgT... args) const;

  //! Vectorized call.
  /*! Calls the method identified by id with the list of arguments
   *  identified by args.
   *  \param id the name of the method to call
   *  \param args the arguments to the method
   *  \return the return value of the method call
   */
  Object vcall(Identifier id, Array args);

  //! Mark the object with the garbage collector.
  void mark() const;

protected:
  //! Set the encapsulated value.
  void set_value(VALUE v);

  //! Unpack the provided arguments and convert them all to Ruby types.
  template<typename ...ArgT>
  std::vector<VALUE> convert_args(ArgT&... args) const;

private:
  volatile VALUE value_;
};

std::ostream & operator<<(std::ostream & out, Object const & obj);

bool operator==(Object const & lhs, Object const & rhs);
bool operator!=(Object const & lhs, Object const & rhs);
bool operator<(Object const & lhs, Object const & rhs);
bool operator>(Object const & lhs, Object const & rhs);

extern Object const Nil;
extern Object const True;
extern Object const False;
extern Object const Undef;

} // namespace Rice

namespace Rice
{
  namespace detail
  {
    template <typename T>
    constexpr bool is_kind_of_object = std::is_base_of_v<Rice::Object, T>;
  }
}

template<>
struct Rice::detail::From_Ruby<Rice::Object>
{
  static Rice::Object convert(VALUE value)
  {
    return Rice::Object(value);
  }
};

template<typename T>
struct Rice::detail::To_Ruby<T, std::enable_if_t<Rice::detail::is_kind_of_object<T>>>
{
  static VALUE convert(Rice::Object const& x)
  {
    return x.value();
  }
};

template<typename T>
struct Rice::detail::To_Ruby<T*, std::enable_if_t<std::is_base_of_v<Rice::Object, T>>>
{
  static VALUE convert(Rice::Object const* x)
  {
    return x->value();
  }
};

#endif // Rice__Object_defn__hpp_


// ---------   Object.ipp   ---------
#ifndef Rice__Object__ipp_
#define Rice__Object__ipp_


namespace Rice
{
  inline const Object Nil(Qnil);
  inline const Object True(Qtrue);
  inline const Object False(Qfalse);
  inline const Object Undef(Qundef);
}

inline Rice::Object::
Object(Object&& other)
{
  this->value_ = other.value_;
  other.value_ = Qnil;
}

inline Rice::Object& Rice::Object::
operator=(Object&& other)
{
  this->value_ = other.value_;
  other.value_ = Qnil;
  return *this;
}

template<typename ...ArgT>
inline Rice::Object Rice::Object::
call(Identifier id, ArgT... args) const
{
  auto asList = this->convert_args<ArgT...>(args...);
  return protect(rb_funcall2, value(), id, (int)asList.size(), asList.data());
}

template<typename ...ArgT>
std::vector<VALUE> Rice::Object::convert_args(ArgT&... args) const {
  return std::vector<VALUE>{ detail::To_Ruby<ArgT>::convert(args)... };
}

template<typename T>
inline void Rice::Object::
iv_set(
    Identifier name,
    T const & value)
{
  protect(rb_ivar_set, *this, name.id(), detail::To_Ruby<T>::convert(value));
}

inline int Rice::Object::
compare(Object const& other) const
{
  Object result = call("<=>", other);
  return Rice::detail::From_Ruby<int>::convert(result);
}

inline bool Rice::Object::
is_equal(const Object& other) const
{
  return rb_equal(this->value_, other.value_);
}

inline bool Rice::Object::
is_eql(const Object& other) const
{
  return rb_eql(this->value_, other.value_);
}

inline void Rice::Object::
freeze()
{
  protect(rb_obj_freeze, value());
}

inline bool Rice::Object::
is_frozen() const
{
  return bool(OBJ_FROZEN(value()));
}

inline int Rice::Object::
rb_type() const
{
  return ::rb_type(*this);
}

inline bool Rice::Object::
is_a(Object klass) const
{
  Object result = protect(rb_obj_is_kind_of, *this, klass);
  return result.test();
}

inline bool Rice::Object::
respond_to(Identifier id) const
{
  return bool(rb_respond_to(*this, id));
}

inline bool Rice::Object::
is_instance_of(Object klass) const
{
  Object result = protect(rb_obj_is_instance_of, *this, klass);
  return result.test();
}

inline Rice::Object Rice::Object::
iv_get(
  Identifier name) const
{
  return protect(rb_ivar_get, *this, name.id());
}

inline Rice::Object Rice::Object::
attr_get(
  Identifier name) const
{
  return protect(rb_attr_get, *this, name.id());
}

inline void Rice::Object::
mark() const
{
  rb_gc_mark(*this);
}

inline void Rice::Object::
set_value(VALUE v)
{
  value_ = v;
}

inline bool Rice::
operator==(Rice::Object const& lhs, Rice::Object const& rhs)
{
  Object result = lhs.call("==", rhs);
  return result.test();
}

inline bool Rice::
operator!=(Rice::Object const& lhs, Rice::Object const& rhs)
{
  return !(lhs == rhs);
}

inline bool Rice::
operator<(Rice::Object const& lhs, Rice::Object const& rhs)
{
  Object result = lhs.call("<", rhs);
  return result.test();
}

inline bool Rice::
operator>(Rice::Object const& lhs, Rice::Object const& rhs)
{
  Object result = lhs.call(">", rhs);
  return result.test();
}

#endif // Rice__Object__ipp_




// =========   Builtin_Object.hpp   =========


// ---------   Builtin_Object_defn.hpp   ---------
#ifndef Rice__Builtin_Object_defn__hpp_
#define Rice__Builtin_Object_defn__hpp_


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

#endif // Rice__Builtin_Object_defn__hpp_


// ---------   Builtin_Object.ipp   ---------
#ifndef Rice__Builtin_Object__ipp_
#define Rice__Builtin_Object__ipp_


#include <algorithm>

namespace Rice
{

namespace detail
{
  inline VALUE check_type(Object value, int type)
  {
    rb_check_type(value, type);
    return Qnil;
  }
}

template<int Builtin_Type>
inline Builtin_Object<Builtin_Type>::
Builtin_Object(Object value)
  : Object(value)
{
  protect(detail::check_type, value, Builtin_Type);
}

template<int Builtin_Type>
inline RObject& Builtin_Object<Builtin_Type>::
operator*() const
{
  return *ROBJECT(this->value());
}

template<int Builtin_Type>
inline RObject* Builtin_Object<Builtin_Type>::
operator->() const
{
  return ROBJECT(this->value());
}

template<int Builtin_Type>
inline RObject* Builtin_Object<Builtin_Type>::
get() const
{
  return ROBJECT(this->value());
}

} // namespace Rice

#endif // Rice__Builtin_Object__ipp_




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
  String(char const * s);

  //! Construct a String from an std::string.
  String(std::string const & s);

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
  char const * c_str() const;

  //! Return a copy of the string as an std::string.
  std::string str() const;

  //! Create an Identifier from the String.
  /*! Calls rb_intern to create an ID.
   *  \return an Identifier holding the ID returned from rb_intern.
   */
  Identifier intern() const;
};

} // namespace Rice

template<>
struct Rice::detail::From_Ruby<Rice::String>
{
  static Rice::String convert(VALUE value)
  {
    return Rice::String(value);
  }
};


// ---------   String.ipp   ---------

inline Rice::String::
String()
  : Builtin_Object<T_STRING>(protect(rb_str_new2, ""))
{
}

inline Rice::String::
String(VALUE v)
  : Builtin_Object<T_STRING>(v)
{
}

inline Rice::String::
String(Object v)
  : Builtin_Object<T_STRING>(v)
{
}

inline Rice::String::
String(char const * s)
  : Builtin_Object<T_STRING>(protect(rb_str_new2, s))
{
}

inline Rice::String::
String(std::string const & s)
  : Builtin_Object<T_STRING>(protect(rb_str_new, s.data(), (long)s.length()))
{
}

inline Rice::String::
String(Identifier id)
  : Builtin_Object<T_STRING>(protect(rb_str_new2, id.c_str()))
{
}

template <typename... Arg_Ts>
inline Rice::String Rice::String::
format(char const * fmt, Arg_Ts&&...args)
{
  size_t size = std::snprintf(nullptr, 0, fmt, std::forward<Arg_Ts>(args)...);
  std::string temp(size, '\0');

  // size+1 avoids trunctaing the string. Otherwise snprintf writes n - 1 characters
  // to allow space for null character but we don't need that since std::string
  // will add a null character internally at n + 1
  std::snprintf(&temp[0], size + 1, fmt, std::forward<Arg_Ts>(args)...);

  String s = String(temp.c_str());
  return s;
}

inline size_t Rice::String::
length() const
{
  return RSTRING_LEN(value());
}

inline char Rice::String::
operator[](ptrdiff_t index) const
{
  return RSTRING_PTR(value())[index];
}

inline char const * Rice::String::
c_str() const
{
  return RSTRING_PTR(value());
}

inline std::string Rice::String::
str() const
{
  return std::string(RSTRING_PTR(value()), length());
}

inline Rice::Identifier Rice::String::
intern() const
{
  return rb_intern(c_str());
}


// =========   Array.hpp   =========

#include <iterator>

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
  Array(T const (& a)[n]);

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
  Object push(T const & obj);

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
  Object unshift(T const & obj);

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
  typedef Iterator<Array *, Proxy> iterator;

  //! A const iterator.
  typedef Iterator<Array const *, Object> const_iterator;

  //! Return an iterator to the beginning of the array.
  iterator begin();

  //! Return a const iterator to the beginning of the array.
  const_iterator begin() const;

  //! Return an iterator to the end of the array.
  iterator end();

  //! Return a const iterator to the end of the array.
  const_iterator end() const;
};

//! A helper class so array[index]=value can work.
class Array::Proxy
{
public:
  //! Construct a new Proxy
  Proxy(Array array, long index);

  //! Implicit conversion to Object.
  operator Object() const;

  //! Explicit conversion to VALUE.
  VALUE value() const;

  //! Assignment operator.
  template<typename T>
  Object operator=(T const & value);

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
  Iterator(Iterator<Array_Ptr_T_, Value_T_> const & rhs);

  template<typename Array_Ptr_T_, typename Value_T_>
  Iterator & operator=(Iterator<Array_Ptr_T_, Value_T_> const & rhs);

  Iterator & operator++();
  Iterator operator++(int);
  Value_T operator*();
  Object * operator->();

  template<typename Array_Ptr_T_, typename Value_T_>
  bool operator==(Iterator<Array_Ptr_T_, Value_T_> const & rhs) const;

  template<typename Array_Ptr_T_, typename Value_T_>
  bool operator!=(Iterator<Array_Ptr_T_, Value_T_> const & rhs) const;

  Array_Ptr_T array() const;
  long index() const;

private:
  Array_Ptr_T array_;
  long index_;

  Object tmp_;
};

} // namespace Rice

template<>
struct Rice::detail::From_Ruby<Rice::Array>
{
  static Rice::Array convert(VALUE value)
  {
    return Rice::Array(value);
  }
};


// ---------   Array.ipp   ---------
#ifndef Rice__Array__ipp_
#define Rice__Array__ipp_


inline Rice::Array::
Array()
  : Builtin_Object<T_ARRAY>(protect(rb_ary_new))
{
}

inline Rice::Array::
Array(Object v)
  : Builtin_Object<T_ARRAY>(v)
{
}

inline Rice::Array::
Array(VALUE v)
  : Builtin_Object<T_ARRAY>(v)
{
}

template<typename Iter_T>
inline Rice::Array::
Array(Iter_T it, Iter_T end)
  : Builtin_Object<T_ARRAY>(protect(rb_ary_new))
{
  for(; it != end; ++it)
  {
    push(*it);
  }
}

template<typename T, long n>
inline Rice::Array::
Array(T const (& a)[n])
  : Builtin_Object<T_ARRAY>(protect(rb_ary_new))
{
  for(long j = 0; j < n; ++j)
  {
    push(a[j]);
  }
}

inline long Rice::Array::
size() const
{
  return RARRAY_LEN(this->value());
}

inline Rice::Object Rice::Array::
operator[](long index) const
{
  return protect(rb_ary_entry, value(), position_of(index));
}

inline Rice::Array::Proxy Rice::Array::
operator[](long index)
{
  return Proxy(*this, position_of(index));
}

template<typename T>
inline Rice::Object Rice::Array::
push(T const & obj)
{
  return protect(rb_ary_push, value(), detail::To_Ruby<T>::convert(obj));
}

inline Rice::Object Rice::Array::
pop()
{
  return protect(rb_ary_pop, value());
}

template<typename T>
inline Rice::Object Rice::Array::
unshift(T const & obj)
{
  return protect(rb_ary_unshift, value(), detail::To_Ruby<T>::convert(obj));
}

inline Rice::Object Rice::Array::
shift()
{
  return protect(rb_ary_shift, value());
}

inline long Rice::Array::
position_of(long index) const
{
  if(index < 0)
  {
    return size() + index;
  }
  else
  {
    return static_cast<long>(index);
  }
}

inline Rice::Array::Proxy::
Proxy(Array array, long index)
  : array_(array)
  , index_(index)
{
}

inline Rice::Array::Proxy::
operator Rice::Object() const
{
  return protect(rb_ary_entry, array_.value(), index_);
}

inline VALUE Rice::Array::Proxy::
value() const
{
  return protect(rb_ary_entry, array_.value(), index_);
}

template<typename T>
Rice::Object Rice::Array::Proxy::
operator=(T const & value)
{
  Object o = detail::To_Ruby<T>::convert(value);
  rb_ary_store(array_.value(), index_, o.value());
  return o;
}

template<typename Array_Ptr_T, typename Value_T>
inline Rice::Array::Iterator<Array_Ptr_T, Value_T>::
Iterator(Array_Ptr_T array, long index)
  : array_(array)
  , index_(index)
{
}

template<typename Array_Ptr_T, typename Value_T>
template<typename Array_Ptr_T_, typename Value_T_>
inline
Rice::Array::Iterator<Array_Ptr_T, Value_T>::
Iterator(Iterator<Array_Ptr_T_, Value_T_> const & rhs)
  : array_(rhs.array())
  , index_(rhs.index())
  , tmp_()
{
}

template<typename Array_Ptr_T, typename Value_T>
template<typename Array_Ptr_T_, typename Value_T_>
inline Rice::Array::Iterator<Array_Ptr_T, Value_T> &
Rice::Array::Iterator<Array_Ptr_T, Value_T>::
operator=(Iterator<Array_Ptr_T_, Value_T_> const & rhs)
{
  array_ = rhs.array_;
  index_ = rhs.index_;
  return *this;
}

template<typename Array_Ptr_T, typename Value_T>
inline Rice::Array::Iterator<Array_Ptr_T, Value_T> &
Rice::Array::Iterator<Array_Ptr_T, Value_T>::
operator++()
{
  ++index_;
  return *this;
}

template<typename Array_Ptr_T, typename Value_T>
inline Rice::Array::Iterator<Array_Ptr_T, Value_T>
Rice::Array::Iterator<Array_Ptr_T, Value_T>::
operator++(int)
{
  Array copy(*this);
  ++(*this);
  return *this;
}

template<typename Array_Ptr_T, typename Value_T>
inline Value_T
Rice::Array::Iterator<Array_Ptr_T, Value_T>::
operator*()
{
  return (*array_)[index_];
}

template<typename Array_Ptr_T, typename Value_T>
inline Rice::Object *
Rice::Array::Iterator<Array_Ptr_T, Value_T>::
operator->()
{
  tmp_ = (*array_)[index_];
  return &tmp_;
}

template<typename Array_Ptr_T, typename Value_T>
template<typename Array_Ptr_T_, typename Value_T_>
inline bool
Rice::Array::Iterator<Array_Ptr_T, Value_T>::
operator==(Iterator<Array_Ptr_T_, Value_T_> const & rhs) const
{
  return array_->value() == rhs.array_->value() && index_ == rhs.index_;
}

template<typename Array_Ptr_T, typename Value_T>
template<typename Array_Ptr_T_, typename Value_T_>
inline bool
Rice::Array::Iterator<Array_Ptr_T, Value_T>::
operator!=(Iterator<Array_Ptr_T_, Value_T_> const & rhs) const
{
  return !(*this == rhs);
}

template<typename Array_Ptr_T, typename Value_T>
Array_Ptr_T
Rice::Array::Iterator<Array_Ptr_T, Value_T>::
array() const
{
  return array_;
}

template<typename Array_Ptr_T, typename Value_T>
long
Rice::Array::Iterator<Array_Ptr_T, Value_T>::
index() const
{
  return index_;
}

inline Rice::Array::iterator Rice::Array::
begin()
{
  return iterator(this, 0);
}

inline Rice::Array::const_iterator Rice::Array::
begin() const
{
  return const_iterator(this, 0);
}

inline Rice::Array::iterator Rice::Array::
end()
{
  return iterator(this, size());
}

inline Rice::Array::const_iterator Rice::Array::
end() const
{
  return const_iterator(this, size());
}

#endif // Rice__Array__ipp_




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
class Hash
  : public Builtin_Object<T_HASH>
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
  Proxy const operator[](Key_T const & key) const;

  //! Get the value for the given key.
  /*! \param key the key whose value should be retrieved from the hash.
   *  \return the value associated with the given key.
   */
  template<typename Key_T>
  Proxy operator[](Key_T const & key);

  //! Get the value for the given key
  /*! The returned value is converted to the type given by Value_T.
   *  \param key the key whose value should be retrieved from the hash.
   *  \return the value associated with the given key, converted to C++
   *  type Value_T.
   */
  template<typename Value_T, typename Key_T>
  Value_T get(Key_T const & key);

  //! A helper class for dereferencing iterators
  class Entry;

  //! A helper class for implementing iterators for a Hash.
  template<typename Hash_Ptr_T, typename Value_T>
  class Iterator;

public:
  //! An iterator.
  typedef Iterator<Hash *, Entry> iterator;

  //! A const iterator.
  typedef Iterator<Hash const *, Entry const> const_iterator;

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
  Object operator=(T const & value);

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
  Entry(Entry const & entry);

  Object const key;          //!< The key
  Object const & first;      //!< An alias for the key

  Proxy value;              //!< The value
  Proxy & second;           //!< An alias for the value

  Entry & operator=(Entry const & rhs);

  friend bool operator<(Entry const & lhs, Entry const & rhs);
};

bool operator<(Hash::Entry const & lhs, Hash::Entry const & rhs);

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
  Iterator(Iterator_T const & iterator);

  //! Preincrement operator.
  Iterator & operator++();

  //! Postincrement operator.
  Iterator operator++(int);

  //! Dereference operator.
  Value_T operator*();

  //! Dereference operator.
  Value_T * operator->();

  //! Equality operator.
  bool operator==(Iterator const & rhs) const;

  //! Inequality operator.
  bool operator!=(Iterator const & rhs) const;

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

template<>
struct Rice::detail::From_Ruby<Rice::Hash>
{
  static Rice::Hash convert(VALUE value)
  {
    return Rice::Hash(value);
  }
};


// ---------   Hash.ipp   ---------
#ifndef Rice__Hash__ipp_
#define Rice__Hash__ipp_

#include <algorithm>

inline Rice::Hash::
Hash()
  : Builtin_Object<T_HASH>(protect(rb_hash_new))
{
}

inline Rice::Hash::
Hash(Object v)
  : Builtin_Object<T_HASH>(v)
{
}

inline size_t Rice::Hash::
size() const
{
  return RHASH_SIZE(this->value());
}

inline Rice::Hash::Proxy::
Proxy(Hash* hash, Object key)
  : hash_(hash)
  , key_(key)
{
}

/*
inline Rice::Hash::Proxy::
operator VALUE() const
{
  return value();
}
*/

inline Rice::Hash::Proxy::
operator Rice::Object() const
{
  return value();
}

inline VALUE Rice::Hash::Proxy::
value() const
{
  return protect(rb_hash_aref, hash_->value(), key_);
}

template<typename T>
inline Rice::Object Rice::Hash::Proxy::
operator=(T const & value)
{
  return protect(rb_hash_aset, hash_->value(), key_, detail::To_Ruby<T>::convert(value));
}

template<typename Key_T>
inline Rice::Hash::Proxy const Rice::Hash::
operator[](Key_T const & key) const
{
  return Proxy(*this, detail::To_Ruby<Key_T>::convert(key));
}

template<typename Key_T>
inline Rice::Hash::Proxy Rice::Hash::
operator[](Key_T const & key)
{
  return Proxy(this, detail::To_Ruby<Key_T>::convert(key));
}

template<typename Value_T, typename Key_T>
inline Value_T Rice::Hash::
get(Key_T const & key)
{
  Object ruby_key(detail::To_Ruby<Key_T>::convert(key));
  Object value = operator[](ruby_key);
  try
  {
    return Rice::detail::From_Ruby<Value_T>::convert(value);
  }
  catch(Exception const & ex)
  {
    String s_key(ruby_key.to_s());
    throw Exception(
        ex,
        "%s while converting value for key %s",
        ex.what(),
        s_key.c_str());
  }
}

inline Rice::Hash::Entry::
Entry(Hash* hash, Object key)
  : key(key)
  , first(Hash::Entry::key)
  , value(hash, key)
  , second(Hash::Entry::value)
{
}

inline Rice::Hash::Entry::
Entry(Entry const & entry)
  : key(entry.key)
  , first(Hash::Entry::key)
  , value(entry.value)
  , second(Hash::Entry::value)
{
}

inline Rice::Hash::Entry & Rice::Hash::Entry::
operator=(Rice::Hash::Entry const & other)
{
  const_cast<Object&>(key) = const_cast<Object&>(other.key);

  this->value = other.value;
  this->second = this->value;

  return *this;
}

template<typename Hash_Ptr_T, typename Value_T>
inline Rice::Hash::Iterator<Hash_Ptr_T, Value_T>::
Iterator(Hash_Ptr_T hash)
  : hash_(hash)
  , current_index_(0)
  , keys_(Qnil)
  , tmp_(const_cast<Hash*>(hash), Qnil)
{
}

template<typename Hash_Ptr_T, typename Value_T>
inline Rice::Hash::Iterator<Hash_Ptr_T, Value_T>::
Iterator(Hash_Ptr_T hash, int start_at)
  : hash_(hash)
  , current_index_(start_at)
  , keys_(Qnil)
  , tmp_(const_cast<Hash*>(hash), Qnil)
{
}

template<typename Hash_Ptr_T, typename Value_T>
template<typename Iterator_T>
inline Rice::Hash::Iterator<Hash_Ptr_T, Value_T>::
Iterator(Iterator_T const & iterator)
  : hash_(iterator.hash_)
  , current_index_(iterator.current_index_)
  , keys_(Qnil)
  , tmp_(iterator.hash_, Qnil)
{
}

template<typename Hash_Ptr_T, typename Value_T>
inline Rice::Hash::Iterator<Hash_Ptr_T, Value_T> &
Rice::Hash::Iterator<Hash_Ptr_T, Value_T>::
operator++()
{
  // Ensure we're within the range
  if(current_index_ < hash_keys().size()) {
    current_index_++;
  }

  return *this;
}

template<typename Hash_Ptr_T, typename Value_T>
inline Rice::Hash::Iterator<Hash_Ptr_T, Value_T>
Rice::Hash::Iterator<Hash_Ptr_T, Value_T>::
operator++(int)
{
  Iterator copy(*this);
  ++(*this);
  return copy;
}

template<typename Hash_Ptr_T, typename Value_T>
inline Value_T
Rice::Hash::Iterator<Hash_Ptr_T, Value_T>::
operator*()
{
  return Value_T(const_cast<Hash*>(hash_), current_key());
}

template<typename Hash_Ptr_T, typename Value_T>
inline Value_T *
Rice::Hash::Iterator<Hash_Ptr_T, Value_T>::
operator->()
{
  this->tmp_ = Entry(const_cast<Hash*>(hash_), current_key());
  return &tmp_;
}

template<typename Hash_Ptr_T, typename Value_T>
inline bool Rice::Hash::Iterator<Hash_Ptr_T, Value_T>::
operator==(Iterator const & rhs) const
{
  return hash_->value() == rhs.hash_->value()
    && current_index_ == rhs.current_index_;
}

template<typename Hash_Ptr_T, typename Value_T>
inline bool Rice::Hash::Iterator<Hash_Ptr_T, Value_T>::
operator!=(Iterator const & rhs) const
{
  return !(*this == rhs);
}

template<typename Hash_Ptr_T, typename Value_T>
inline Rice::Object
Rice::Hash::Iterator<Hash_Ptr_T, Value_T>::
current_key()
{
  return hash_keys()[current_index_];
}

template<typename Hash_Ptr_T, typename Value_T>
inline Rice::Array
Rice::Hash::Iterator<Hash_Ptr_T, Value_T>::
hash_keys()
{
  if(NIL_P(keys_)) {
    keys_ = rb_funcall(hash_->value(), rb_intern("keys"), 0, 0);
  }

  return Rice::Array(keys_);
}

inline Rice::Hash::iterator Rice::Hash::
begin()
{
  return iterator(this);
}

inline Rice::Hash::const_iterator Rice::Hash::
begin() const
{
  return const_iterator(this);
}

inline Rice::Hash::iterator Rice::Hash::
end()
{
  return iterator(this, (int)size());
}

inline Rice::Hash::const_iterator Rice::Hash::
end() const
{
  return const_iterator(this, (int)size());
}

inline bool Rice::
operator<(
    Hash::Entry const & lhs, Hash::Entry const & rhs)
{
  Object lhs_key(lhs.key);
  Object rhs_key(rhs.key);
  if(lhs_key < rhs_key)
  {
    return true;
  }
  else if(lhs_key > rhs_key)
  {
    return false;
  }
  else if(Object(lhs.value.value()) < Object(rhs.value.value()))
  {
    return true;
  }
  else
  {
    return false;
  }
}

#endif // Rice__Hash__ipp_




// =========   Symbol.hpp   =========

#include <string>

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
  Symbol(char const * s = "");

  //! Construct a Symbol from an std::string.
  Symbol(std::string const & s);

  //! Return a string representation of the Symbol.
  char const * c_str() const;

  //! Return a string representation of the Symbol.
  std::string str() const;

  //! Return the Symbol as an Identifier.
  Identifier to_id() const;
};

} // namespace Rice

template<>
struct Rice::detail::From_Ruby<Rice::Symbol>
{
  static Rice::Symbol convert(VALUE value)
  {
    return Rice::Symbol(value);
  }
};


// ---------   Symbol.ipp   ---------

inline Rice::Symbol::
Symbol(VALUE v)
  : Object(v)
{
  protect(rb_check_type, v, T_SYMBOL);
}

inline Rice::Symbol::
Symbol(Object v)
  : Object(v)
{
  protect(rb_check_type, v, T_SYMBOL);
}

inline Rice::Symbol::
Symbol(char const * s)
  : Object(ID2SYM(rb_intern(s)))
{
}

inline Rice::Symbol::
Symbol(std::string const & s)
  : Object(ID2SYM(rb_intern(s.c_str())))
{
}

inline Rice::Symbol::
Symbol(Identifier id)
  : Object(ID2SYM(id))
{
}

inline char const * Rice::Symbol::
c_str() const
{
  return to_id().c_str();
}

inline std::string Rice::Symbol::
str() const
{
  return to_id().str();
}

inline Rice::Identifier Rice::Symbol::
to_id() const
{
  return rb_to_id(value());
}



// =========   Address_Registration_Guard.hpp   =========


// ---------   Address_Registration_Guard_defn.hpp   ---------
#ifndef Rice__Address_Registration_Guard_defn__hpp_
#define Rice__Address_Registration_Guard_defn__hpp_


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
  Address_Registration_Guard(VALUE * address);

  //! Register an Object with the GC.
  /*! \param object The Object to register with the GC.  The object must
   *  not be destroyed before the Address_Registration_Guard is
   *  destroyed.
   */
  Address_Registration_Guard(Object * object);

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
  VALUE * address() const;

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

#endif // Rice__Address_Registration_Guard_defn__hpp_

// ---------   Address_Registration_Guard.ipp   ---------
#include <algorithm>

namespace Rice
{

inline Address_Registration_Guard::
Address_Registration_Guard(VALUE * address)
  : address_(address)
{
  registerExitHandler();
  registerAddress();
}

inline Address_Registration_Guard::
Address_Registration_Guard(Object * object)
  : address_(const_cast<VALUE*>(&object->value()))
{
  registerExitHandler();
  registerAddress();
}

inline Address_Registration_Guard::
~Address_Registration_Guard()
{
  unregisterAddress();
}

inline Address_Registration_Guard::
Address_Registration_Guard(Address_Registration_Guard&& other)
{
  // We don't use the constructor because we don't want to double register this address
  address_ = other.address_;
  other.address_ = nullptr;
}

inline Address_Registration_Guard & Address_Registration_Guard::
operator=(Address_Registration_Guard && other)
{
  this->unregisterAddress();

  this->address_ = other.address_;
  other.address_ = nullptr;
  return *this;
}

inline void Address_Registration_Guard::
registerAddress() const
{
  if (enabled)
    rb_gc_register_address(address_);
}

inline void Address_Registration_Guard::
unregisterAddress()
{
  if (enabled && address_)
    rb_gc_unregister_address(address_);

  address_ = nullptr;
}

inline VALUE * Address_Registration_Guard::
address() const
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
    return;

  rb_set_end_proc(&disable_all_guards, Qnil);
  exit_handler_registered = true;
}

inline void Address_Registration_Guard::disable()
{
  enabled = false;
}

} // Rice

// =========   Module.hpp   =========


// ---------   Module_defn.hpp   ---------
#ifndef Rice__Module_defn__hpp_
#define Rice__Module_defn__hpp_


namespace Rice
{
template<typename T>
class Data_Type;

//! A helper for defining a Module and its methods.
/*! This class provides a C++-style interface to ruby's Module class and
 *  for defining methods on that module.
 *
 *  Many of the methods are defined in Module_impl.hpp so that they can
 *  return a reference to the most derived type.
 */
class Module
  // TODO: we can't inherit from Builtin_Object, because Class needs
  // type T_CLASS and Module needs type T_MODULE
  : public Object
{
public:
  //! Default construct a Module and initialize it to rb_cObject.
  Module();

  //! Construct a Module from an existing Module object.
  Module(VALUE v);

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

   //! Define a class under this module.
  /*! \param name the name of the class.
   *  \param superclass the base class to use.
   *  \return the new class.
   */
  Class define_class(char const* name, Object superclass = rb_cObject);

  //! Define an exception handler.
  /*! Whenever an exception of type Exception_T is thrown from a
   *  function defined on this class, functor will be called to
   *  translate the exception into a ruby exception.
   *  \param Exception_T a template parameter indicating the type of
   *  exception to be translated.
   *  \param functor a functor to be called to translate the exception
   *  into a ruby exception.  This functor should re-throw the exception
   *  as an Exception.
   *  Example:
   *  \code
   *    class MyException : public std::exception { };
   *    Data_Type<MyException> rb_cMyException;
   *    Class rb_cFoo;
   *
   *    void translate_my_exception(MyException const & ex)
   *    {
   *      Data_Object<MyException> ex_(
   *          new MyException(ex),
   *          rb_cMyException);
   *      throw Exception(ex_);
   *    }
   *
   *    extern "C"
   *    void Init_MyExtension()
   *    {
   *      rb_cMyException = define_class("MyException");
   *      rb_cFoo = define_class("Foo")
   *        .add_handler<MyException>(translate_my_exception);
   *    }
   *  \endcode
   */
  template<typename Exception_T, typename Functor_T>
  Module& add_handler(
    Functor_T functor);

  //! Include a module.
  /*! \param inc the module to be included.
  *  \return *this
  */
  Module& include_module(Module const& inc);

  template<typename Func_T>
  [[deprecated("Please call define_method with Arg parameters")]]
  Module& define_method(
    Identifier name,
    Func_T func,
    Arguments* arguments);

  //! Define an instance method.
  /*! The method's implementation can be any function or member
   *  function.  A wrapper will be generated which will convert the arguments
   *  from ruby types to C++ types before calling the function.  The return
   *  value will be converted back to ruby.
   *  \param name the name of the method
   *  \param func the implementation of the function, either a function
   *  pointer or a member function pointer.
   *  \param args a list of Arg instance used to define default parameters (optional)
   *  \return *this
   */
  template<typename Func_T, typename...Arg_Ts>
  Module& define_method(
    Identifier name,
    Func_T func,
    Arg_Ts const& ...args);

  template<typename Func_T>
  [[deprecated("Please call define_singleton_method with Arg parameters")]]
  Module& define_singleton_method(
    Identifier name,
    Func_T func,
    Arguments* arguments);

  //! Define a singleton method.
  /*! The method's implementation can be any function or member
   *  function.  A wrapper will be generated which will convert the arguments
   *  from ruby types to C++ types before calling the function.  The return
   *  value will be converted back to ruby.
   *  \param name the name of the method
   *  \param func the implementation of the function, either a function
   *  pointer or a member function pointer.
   *  \param args a list of Arg instance used to define default parameters (optional)
   *  \return *this
   */
  template<typename Func_T, typename...Arg_Ts>
  Module& define_singleton_method(
    Identifier name,
    Func_T func,
    Arg_Ts const& ...args);

  template<typename Func_T>
  [[deprecated("Please call define_module_function with Arg parameters")]]
  Module& define_module_function(
    Identifier name,
    Func_T func,
    Arguments* arguments);

  //! Define a module function.
  /*! A module function is a function that can be accessed either as a
   *  singleton method or as an instance method.
   *  The method's implementation can be any function or member
   *  function.  A wrapper will be generated which will convert the arguments
   *  from ruby types to C++ types before calling the function.  The return
   *  value will be converted back to ruby.
   *  \param name the name of the method
   *  \param func the implementation of the function, either a function
   *  pointer or a member function pointer.
   *  \param args a list of Arg instance used to define default parameters (optional)
   *  \return *this
   */
  template<typename Func_T, typename...Arg_Ts>
  Module& define_module_function(
    Identifier name,
    Func_T func,
    Arg_Ts const& ...args);

  //! Set a constant.
  /*! \param name the name of the constant to set.
   *  \param value the value of the constant.
   *  \return *this
   */
  Module& const_set(
    Identifier name,
    Object value);

  //! Get a constant.
  /*! \param name the name of the constant to get.
   *  \return the value of the constant.
   */
  Object const_get(
    Identifier name) const;

  //! Determine whether a constant is defined.
  /*! \param name the name of the constant to check.
   *  \return true if the constant is defined in this module or false
   *  otherwise.
   */
  bool const_defined(
    Identifier name) const;

  //! Remove a constant.
  /*! \param name the name of the constant to remove.
   */
  void remove_const(
    Identifier name);

  //! Define a new data class under this module.
  /*! The class will have a base class of Object.
   *  \param T the C++ type of the wrapped class.
   *  \return the new class.
   */
  template<typename T>
  Data_Type<T>
    define_class(
      char const* name);

  //! Define a new data class under this module.
  /*! The class with have a base class determined by Base_T (specifically,
   *  Data_Type<Base_T>::klass).  Therefore, the type Base_T must already
   *  have been registered using define_class<> or define_class_under<>.
   *  \param T the C++ type of the wrapped class.
   *  \return the new class.
   */
  template<typename T, typename T_Base_T>
  Data_Type<T>
    define_class(
      char const* name);

protected:
  std::shared_ptr<detail::Exception_Handler> handler() const;

private:
  template<typename T>
  Data_Type<T> define_class_with_object_as_base(char const* name);

  template<typename Fun_T>
  void define_method_and_auto_wrap(VALUE klass, Identifier name, Fun_T function,
                                   std::shared_ptr<detail::Exception_Handler> handler, Arguments* arguments = 0);

  mutable std::shared_ptr<detail::Exception_Handler> handler_ = std::make_shared<Rice::detail::Default_Exception_Handler>();
};

//! Define a new module in the namespace given by module.
/*! \param module the module in which to define the new module.
 *  \param name the name of the new module.
 */
Module define_module_under(
    Object module,
    char const * name);

//! Define a new module in the default namespace.
/*! \param name the name of the new module.
 */
Module define_module(
    char const * name);

//! Create a new anonymous module.
/*! \return the new module.
 */
Module anonymous_module();

} // namespace Rice

template<>
struct Rice::detail::From_Ruby<Rice::Module>
{
  static Rice::Module convert(VALUE value)
  {
    return Rice::Module(value);
  }
};

#endif // Rice__Module_defn__hpp_


// ---------   Module.ipp   ---------
#ifndef Rice__Module__ipp_
#define Rice__Module__ipp_


inline
Rice::Module::
Module()
  : Object(rb_cObject)
{
}

inline
Rice::Module::
Module(VALUE v)
  : Object(v)
{
  if (::rb_type(v) != T_CLASS && ::rb_type(v) != T_MODULE)
  {
    throw Exception(
      rb_eTypeError,
      "Expected a Module but got a %s",
      rb_class2name(CLASS_OF(v))); // TODO: might raise an exception
  }
}

inline
Rice::Module Rice::
define_module_under(
  Rice::Object module,
  char const* name)
{
  VALUE v = rb_define_module_under(module, name);
  return Module(v);
}

inline
Rice::Module Rice::
define_module(
  char const* name)
{
  VALUE v = rb_define_module(name);
  return Module(v);
}

inline
Rice::Module Rice::
anonymous_module()
{
  return Module(protect(rb_module_new));
}

template<typename Exception_T, typename Functor_T>
inline
Rice::Module&
Rice::Module::
add_handler(
  Functor_T functor)
{
  // Create a new exception handler and pass ownership of the current handler to it (they
  // get chained together). Then take ownership of the new handler.
  this->handler_ = std::make_shared<detail::Functor_Exception_Handler<Exception_T, Functor_T>>(
    functor, std::move(this->handler_));

  return *this;
}

inline
std::shared_ptr<Rice::detail::Exception_Handler>
Rice::Module::
handler() const
{
  return this->handler_;
}

inline
Rice::Module&
Rice::Module::
include_module(Rice::Module const& inc)
{
  Rice::protect(rb_include_module, *this, inc);
  return *this;
}

template<typename Func_T>
inline
Rice::Module&
Rice::Module::
define_method(
  Identifier name,
  Func_T func,
  Arguments* arguments)
{
  this->define_method_and_auto_wrap(*this, name, func, this->handler(), arguments);
  return *this;
}

template<typename Func_T, typename...Arg_Ts>
inline
Rice::Module&
Rice::Module::
define_method(
  Identifier name,
  Func_T func,
  Arg_Ts const& ...args)
{
  Arguments* arguments = new Arguments(args...);
  this->define_method_and_auto_wrap(*this, name, func, this->handler(), arguments);
  return *this;
}

template<typename Func_T>
inline
Rice::Module&
Rice::Module::
define_singleton_method(
  Identifier name,
  Func_T func,
  Arguments* arguments)
{
  this->define_method_and_auto_wrap(rb_singleton_class(*this), name, func, this->handler(), arguments);
  return *this;
}

template<typename Func_T, typename...Arg_Ts>
inline
Rice::Module&
Rice::Module::
define_singleton_method(
  Identifier name,
  Func_T func,
  Arg_Ts const& ...args)
{
  Arguments* arguments = new Arguments(args...);
  this->define_method_and_auto_wrap(rb_singleton_class(*this), name, func, this->handler(), arguments);
  return *this;
}

template<typename Func_T>
inline
Rice::Module&
Rice::Module::
define_module_function(
  Identifier name,
  Func_T func,
  Arguments* arguments)
{
  if (this->rb_type() != T_MODULE)
  {
    throw std::runtime_error("can only define module functions for modules");
  }

  define_method(name, func, arguments);
  define_singleton_method(name, func, arguments);
  return *this;
}

template<typename Func_T, typename...Arg_Ts>
inline
Rice::Module&
Rice::Module::
define_module_function(
  Identifier name,
  Func_T func,
  Arg_Ts const& ...args)
{
  if (this->rb_type() != T_MODULE)
  {
    throw std::runtime_error("can only define module functions for modules");
  }

  define_method(name, func, args...);
  define_singleton_method(name, func, args...);
  return *this;
}

namespace Rice
{

  namespace detail
  {


    inline VALUE
      const_set(VALUE mod, ID name, VALUE value)
    {
      rb_const_set(mod, name, value);
      return Qnil;
    }

  } // namespace detail

} // namespace Rice

inline
Rice::Module&
Rice::Module::
const_set(
  Identifier name,
  Object value)
{
  protect(detail::const_set, *this, name, value);
  return *this;
}

inline
Rice::Object
Rice::Module::
const_get(
  Identifier name) const
{
  return protect(rb_const_get, *this, name);
}

inline
bool
Rice::Module::
const_defined(Identifier name) const
{
  size_t result = protect(rb_const_defined, *this, name);
  return bool(result);
}

inline
void
Rice::Module::
remove_const(Identifier name)
{
  protect(rb_mod_remove_const, *this, name.to_sym());
}

template<typename T>
Rice::Data_Type<T>
Rice::Module::
define_class(
  char const* name)
{
  return this->define_class_with_object_as_base<T>(name);
}

#endif // Rice__Module__ipp_



// =========   global_function.hpp   =========


namespace Rice
{

  template<typename Func_T>
  [[deprecated("Please call define_global_function with Arg parameters")]]
  void define_global_function(
      char const * name,
      Func_T func,
      Arguments* arguments);

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
  template<typename Func_T, typename...Arg_Ts>
  void define_global_function(
      char const * name,
      Func_T func,
      Arg_Ts const& ...args);
  
} // Rice


// ---------   global_function.ipp   ---------

template<typename Func_T>
void Rice::define_global_function(
    char const * name,
    Func_T func,
    Arguments* arguments)
{
  Module(rb_mKernel).define_module_function(name, func, arguments);
}

template<typename Func_T, typename...Arg_Ts>
void Rice::define_global_function(
    char const * name,
    Func_T func,
    Arg_Ts const& ...args)
{
  Module(rb_mKernel).define_module_function(name, func, args...);
}



// =========   Caster.hpp   =========

#include <stdexcept>

namespace Rice
{

namespace detail
{

class Abstract_Caster
{
public:
  virtual void * cast_to_base(void * derived, Module type) const = 0;
  virtual ~Abstract_Caster() { }
};

template<typename Derived_T, typename Base_T>
class Caster
  : public Abstract_Caster
{
public:
  Caster(Abstract_Caster * base_caster, Module type)
    : base_caster_(base_caster)
    , type_(type)
  {
  }

protected:
  virtual void * cast_to_base(void * derived, Module type) const
  {
    if(type.value() == type_.value())
    {
      Derived_T * d(static_cast<Derived_T *>(derived));
      return static_cast<Base_T *>(d);
    }
    else
    {
      if(base_caster_)
      {
        return base_caster_->cast_to_base(derived, type);
      }
      else
      {
        std::string s = "bad cast. No caster found for ";
        s += type_.name().str();
        throw std::runtime_error(s);
      }
    }
  }

private:
  Abstract_Caster * base_caster_;
  Module type_;
};

template<typename To_T, typename From_T>
class Implicit_Caster
  : public Abstract_Caster
{
public:
  Implicit_Caster(Abstract_Caster * base_caster, Module type)
    : base_caster_(base_caster)
    , type_(type)
  {
  }

protected:
  virtual void * cast_to_base(void * derived, Module type) const
  {
    if(type.value() == type_.value())
    {
      return new To_T( *static_cast<From_T*>(derived) );
    }
    else
    {
      if(base_caster_)
      {
        return base_caster_->cast_to_base(derived, type);
      }
      else
      {
        std::string s = "bad cast. No implicit caster found for ";
        s += type_.name().str();
        throw std::runtime_error(s);
      }
    }
  }

private:
  Abstract_Caster * base_caster_;
  Module type_;
};



} // detail

} // Rice


// =========   Class.hpp   =========


// ---------   Class_defn.hpp   ---------
#ifndef Rice__Class_defn__hpp_
#define Rice__Class_defn__hpp_


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
class Class
  : public Module
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

  // Include these methods to call methods from Module but return
// an instance of the current classes. This is an alternative to
// using CRTP.

auto& include_module(Module const& inc)
{
  protect(rb_include_module, *this, inc);
  return *this;
}

auto& const_set(Identifier name, Object value)
{
  return dynamic_cast<decltype(*this)>(Module::const_set(name, value));
}

template<typename Func_T>
[[deprecated("Please call define_method with Arg parameters")]]
auto& define_method(Identifier name, Func_T func, Arguments* arguments)
{
  return dynamic_cast<decltype(*this)>(Module::define_method(name, func, arguments));
}

template<typename Func_T, typename...Arg_Ts>
auto& define_method(Identifier name, Func_T func, Arg_Ts const& ...args)
{
  return dynamic_cast<decltype(*this)>(Module::define_method(name, func, args...));
}

template<typename Func_T>
[[deprecated("Please call define_singleton_method with Arg parameters")]]
auto& define_singleton_method(Identifier name, Func_T func, Arguments* arguments)
{
  return dynamic_cast<decltype(*this)>(Module::define_singleton_method(name, func, arguments));
}

template<typename Func_T, typename...Arg_Ts>
auto& define_singleton_method(Identifier name, Func_T func, Arg_Ts const& ...args)
{
  return dynamic_cast<decltype(*this)>(Module::define_singleton_method(name, func, args...));
}

};

//! Define a new class in the namespace given by module.
/*! \param module the Module in which to define the class.
 *  \param name the name of the class.
 *  \param superclass the base class to use.
 *  \return the new class.
 */
Class define_class_under(
    Object module,
    char const * name,
    Object superclass = rb_cObject);

//! Define a new class in the default namespace.
/*! \param name the name of the class.
 *  \param superclass the base class to use.
 *  \return the new class.
 */
Class define_class(
    char const * name,
    Object superclass = rb_cObject);

//! Create a new anonymous class.
/*! \param superclass the base class to use.
 *  \return the new class.
 */
Class anonymous_class(
    Object superclass = rb_cObject);

} // namespace Rice

template<>
struct Rice::detail::From_Ruby<Rice::Class>
{
  static Rice::Class convert(VALUE value)
  {
    return Rice::Class(value);
  }
};

#endif // Rice__Class_defn__hpp_


// ---------   Class.ipp   ---------
#ifndef Rice__Class__ipp_
#define Rice__Class__ipp_


inline
Rice::Class::
Class(VALUE v)
  : Module(v)
{
  if (::rb_type(v) != T_CLASS)
  {
    throw Exception(
      rb_eTypeError,
      "Expected a Class but got a %s",
      rb_class2name(CLASS_OF(v))); // TODO: might raise an exception
  }
}

inline
Rice::Class& Rice::Class::
undef_creation_funcs()
{
  rb_undef_alloc_func(value());
  rb_undef_method(value(), "initialize");
  return *this;
}

inline
Rice::Class Rice::
define_class_under(
  Rice::Object module,
  char const* name,
  Rice::Object superclass)
{
  VALUE v = rb_define_class_under(module, name, superclass);
  return Class(v);
}

inline
Rice::Class Rice::
define_class(
  char const* name,
  Rice::Object superclass)
{
  VALUE v = rb_define_class(name, superclass);
  return Class(v);
}

inline
Rice::Class Rice::
anonymous_class(
  Rice::Object superclass)
{
  VALUE v = Class(rb_cClass).call("new");
  return Class(v);
}

#endif // Rice__Class__ipp_



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
  *  The first template type must be the type being wrapped.
  *  Afterwards any extra types must match the appropriate constructor
  *  to be used in C++ when constructing the object.
  *
  *  For more information, see Rice::Data_Type::define_constructor.
  */
  template<typename T, typename ...Arg_T>
  class Constructor
  {
  public:
    static void construct(Object self, Arg_T... args)
    {
      DATA_PTR(self.value()) = new T(args...);
    }
  };

  //! Special-case Constructor used when defining Directors.
  template<typename T, typename ...Arg_T>
  class Constructor<T, Object, Arg_T...>
  {
    public:
      static void construct(Object self, Arg_T... args)
      {
        DATA_PTR(self.value()) = new T(self, args...);
      }
  };
}



// =========   Director.hpp   =========


namespace Rice {

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


// ---------   Data_Type_defn.hpp   ---------
#ifndef Rice__Data_Type_defn__hpp_
#define Rice__Data_Type_defn__hpp_

#include <memory>
#include <map>
#include <set>

/*!
 *  \example map/map.cpp
 */

namespace Rice
{

namespace detail
{
  // TODO - forward declaration
  class Abstract_Caster;
}

class Module;

//! The base class for all instantiations of Data_Type.
class Data_Type_Base
  : public Class
{
public:

  //! Default constructor.
  Data_Type_Base() = default;

  //! Constructor.
  Data_Type_Base(VALUE v);

  //! Destructor.
  virtual ~Data_Type_Base() = default;

  virtual detail::Abstract_Caster * caster() const = 0;

  static inline std::map<VALUE, detail::Abstract_Caster*> casters;
};

//! Define a new data class in the namespace given by module.
/*! The class will have a base class of Object.
 *  \param T the C++ type of the wrapped class.
 *  \param module the the Module in which to define the class.
 *  \return the new class.
 */
template<typename T>
Rice::Data_Type<T> define_class_under(
    Object module,
    char const * name);

//! Define a new data class in the namespace given by module.
/*! The class with have a base class determined by Base_T (specifically,
 *  Data_Type<Base_T>::klass).  Therefore, the type Base_T must already
 *  have been registered using define_class<> or define_class_under<>.
 *  \param T the C++ type of the wrapped class.
 *  \param module the the Module in which to define the class.
 *  \return the new class.
 */
template<typename T, typename Base_T>
Rice::Data_Type<T> define_class_under(
    Object module,
    char const * name);

//! Define a new data class in the default namespace.
/*! The class will have a base class of Object.
 *  \param T the C++ type of the wrapped class.
 *  \return the new class.
 */
template<typename T>
Rice::Data_Type<T> define_class(
    char const * name);

//! Define a new data class in the default namespace.
/*! The class with have a base class determined by Base_T (specifically,
 *  Data_Type<Base_T>::klass).  Therefore, the type Base_T must already
 *  have been registered using define_class<> or define_class_under<>.
 *  \param T the C++ type of the wrapped class.
 *  \param module the the Module in which to define the class.
 *  \return the new class.
 */
template<typename T, typename Base_T>
Rice::Data_Type<T> define_class(
    char const * name);

//! Define an implicit conversion rule between two types.
/*! Given two types, which can be custom types already 
 *  wrapped into Rice or fundamental C++ types, this
 *  tells Rice that the two types can be used interchangably.
 *  \param From_T The type to convert from
 *  \param To_T The type to convert to
 */
template<typename From_T, typename To_T>
void define_implicit_cast();

//! A mechanism for binding ruby types to C++ types.
/*! This class binds run-time types (Ruby VALUEs) to compile-time types
 *  (C++ types).  The binding can occur only once.
 */
template<typename T>
class Data_Type
  : public Data_Type_Base
{
public:
  //! The C++ type being held.
  typedef T Type;

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

  //! Destructor.
  virtual ~Data_Type();
 
  //! Explictly return the Ruby type.
  /*! \return the ruby class to which the type is bound.
   */
  static Module klass();

  //! Assignment operator which takes a Module
  /*! \param klass must be the class to which this data type is already
   *  bound.
   *  \return *this
   */
  virtual Data_Type & operator=(Module const & klass);

  //! Define a constructor for the class.
  template<typename Constructor_T>
  [[deprecated("Please call define_constructor with Arg parameters")]]
  Data_Type<T> & define_constructor(
      Constructor_T constructor,
      Arguments * arguments);

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
  template<typename Constructor_T, typename...Arg_Ts>
  Data_Type<T> & define_constructor(
      Constructor_T constructor,
      Arg_Ts const& ...args);

  //! Register a Director class for this class.
  /*! For any class that uses Rice::Director to enable polymorphism
   *  across the languages, you need to register that director proxy
   *  class with this method. Not doing so will cause the resulting 
   *  library to die at run time when it tries to convert the base
   *  type into the Director proxy type, and cannot find an appropriate Caster.
   *
   *  This method takes no arguments, just needs the type of the
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

  //! Convert ruby object x to type T.
  /*! \param x the object to convert.
   *  \return the C++ object wrapped inside object x.
   */
  static T * from_ruby(Object x);

  //! Determine if the type is bound.
  /*! \return true if the object is bound, false otherwise.
   */
  static bool is_bound();
  static void check_is_bound();

  virtual detail::Abstract_Caster * caster() const;

  static inline std::unique_ptr<detail::Abstract_Caster> caster_ = nullptr;

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

  template<typename U = T, typename Iterator_Return_T>
  Data_Type<T>& define_iterator(Iterator_Return_T(U::* begin)(), Iterator_Return_T(U::* end)(), Identifier name = "each");

  // Include these methods to call methods from Module but return
// an instance of the current classes. This is an alternative to
// using CRTP.

auto& include_module(Module const& inc)
{
  protect(rb_include_module, *this, inc);
  return *this;
}

auto& const_set(Identifier name, Object value)
{
  return dynamic_cast<decltype(*this)>(Module::const_set(name, value));
}

template<typename Func_T>
[[deprecated("Please call define_method with Arg parameters")]]
auto& define_method(Identifier name, Func_T func, Arguments* arguments)
{
  return dynamic_cast<decltype(*this)>(Module::define_method(name, func, arguments));
}

template<typename Func_T, typename...Arg_Ts>
auto& define_method(Identifier name, Func_T func, Arg_Ts const& ...args)
{
  return dynamic_cast<decltype(*this)>(Module::define_method(name, func, args...));
}

template<typename Func_T>
[[deprecated("Please call define_singleton_method with Arg parameters")]]
auto& define_singleton_method(Identifier name, Func_T func, Arguments* arguments)
{
  return dynamic_cast<decltype(*this)>(Module::define_singleton_method(name, func, arguments));
}

template<typename Func_T, typename...Arg_Ts>
auto& define_singleton_method(Identifier name, Func_T func, Arg_Ts const& ...args)
{
  return dynamic_cast<decltype(*this)>(Module::define_singleton_method(name, func, args...));
}


protected:
  //! Bind a Data_Type to a VALUE.
  /*! Throws an exception if the Data_Type is already bound to a
   *  different class.  Any existing instances of the Data_Type will be
   *  bound after this function returns.
   *  \param klass the ruby type to which to bind.
   *  \return *this
   */
  template<typename Base_T>
  static Data_Type bind(Module const & klass);

  template<typename T_>
  friend Rice::Data_Type<T_> define_class_under(
      Object module,
      char const * name);

  template<typename T_, typename Base_T_>
  friend Rice::Data_Type<T_> define_class_under(
      Object module,
      char const * name);

  template<typename T_>
  friend Rice::Data_Type<T_> Rice::define_class(
      char const * name);

  template<typename T_, typename Base_T_>
  friend Rice::Data_Type<T_> define_class(
      char const * name);

private:
  template<typename T_>
  friend class Data_Type;

  static inline VALUE klass_ = Qnil;

  typedef std::set<Data_Type<T> *> Instances;

  static Instances & unbound_instances()
  {
    static Instances unbound_instances;
    return unbound_instances;
  }
};

} // namespace Rice

template<typename T>
struct Rice::detail::From_Ruby
{
  static T& convert(VALUE value)
  {
    using Base_T = std::decay_t<T>;
    return *Data_Type<Base_T>::from_ruby(value);
  }
};

template<typename T>
struct Rice::detail::From_Ruby<T&>
{
  static T& convert(VALUE value)
  {
    using Base_T = std::decay_t<T>;
    return *Data_Type<Base_T>::from_ruby(value);
  }
};

template<typename T>
struct Rice::detail::From_Ruby<T*>
{
  static T* convert(VALUE value)
  {
    using Base_T = std::decay_t<T>;
    return Data_Type<Base_T>::from_ruby(value);
  }
};

namespace Rice
{
  // Forward declaration
  template<typename T>
  class Data_Object;
}

template<typename T, typename>
struct Rice::detail::To_Ruby
{
  static VALUE convert(T const& x)
  {
    using Base_T = base_type<T>;
    Data_Type<Base_T>::check_is_bound();
    return Rice::Data_Object<Base_T>(new Base_T(x));
  }
};

template <typename T>
struct Rice::detail::To_Ruby<T*, std::enable_if_t<!Rice::detail::is_primitive_v<T> &&
                                                  !Rice::detail::is_kind_of_object<T>>>
{
  static VALUE convert(T* x)
  {
    using Base_T = base_type<T>;
    Data_Type<Base_T>::check_is_bound();
    return Data_Object<Base_T>(x);
  }
};


#endif // Rice__Data_Type_defn__hpp_


// ---------   Data_Type.ipp   ---------
#ifndef Rice__Data_Type__ipp_
#define Rice__Data_Type__ipp_



#include <stdexcept>
#include <typeinfo>

inline Rice::Data_Type_Base::
Data_Type_Base(VALUE v)
  : Class(v)
{
}

template<typename T>
template<typename Base_T>
inline Rice::Data_Type<T> Rice::Data_Type<T>::
bind(Module const & klass)
{
  if(klass.value() == klass_)
  {
    return Data_Type<T>();
  }

  if(is_bound())
  {
    std::string s;
    s = "Data type ";
    s += detail::demangle(typeid(T).name());
    s += " is already bound to a different type";
    throw std::runtime_error(s.c_str());
  }

  // TODO: Make sure base type is bound; throw an exception otherwise.
  // We can't do this just yet, because we don't have a specialization
  // for binding to void.
  klass_ = klass;

  // TODO: do we need to unregister when the program exits?  we have to
  // be careful if we do, because the ruby interpreter might have
  // already shut down.  The correct behavior is probably to register an
  // exit proc with the interpreter, so the proc gets called before the
  // GC shuts down.
  rb_gc_register_address(&klass_);

  for(typename Instances::iterator it = unbound_instances().begin(),
      end = unbound_instances().end();
      it != end;
      unbound_instances().erase(it++))
  {
    (*it)->set_value(klass);
  }

  detail::Abstract_Caster * base_caster = Data_Type<Base_T>().caster();
  caster_.reset(new detail::Caster<T, Base_T>(base_caster, klass));
  Data_Type_Base::casters.insert(std::make_pair(klass, caster_.get()));
  return Data_Type<T>();
}

template<typename T>
inline Rice::Data_Type<T>::
Data_Type()
  : Data_Type_Base(
      klass_ == Qnil ? rb_cData : klass_)
{
  if(!is_bound())
  {
    unbound_instances().insert(this);
  }
}

template<typename T>
inline Rice::Data_Type<T>::
Data_Type(Module const & klass)
  : Data_Type_Base(
      klass)
{
  this->bind<void>(klass);
}

template<typename T>
inline Rice::Data_Type<T>::
~Data_Type()
{
  unbound_instances().erase(this);
}

template<typename T>
Rice::Module
Rice::Data_Type<T>::
klass() {
  if(is_bound())
  {
    return klass_;
  }
  else
  {
    std::string s;
    s += detail::demangle(typeid(T *).name());
    s += " is unbound";
    throw std::runtime_error(s.c_str());
  }
}

template<typename T>
Rice::Data_Type<T> & Rice::Data_Type<T>::
operator=(Module const & klass)
{
  this->bind<void>(klass);
  return *this;
}

template<typename T>
template<typename Constructor_T>
inline Rice::Data_Type<T> & Rice::Data_Type<T>::
define_constructor(
    Constructor_T /* constructor */,
    Arguments* arguments)
{
  check_is_bound();

  // Normal constructor pattern with new/initialize
  rb_define_alloc_func(
      static_cast<VALUE>(*this),
      detail::default_allocation_func<T>);
  this->define_method(
      "initialize",
      &Constructor_T::construct,
      arguments
      );

  return *this;
}

template<typename T>
template<typename Constructor_T, typename...Arg_Ts>
inline Rice::Data_Type<T> & Rice::Data_Type<T>::
define_constructor(
    Constructor_T constructor,
    Arg_Ts const& ...args)
{
  check_is_bound();

  // Define a Ruby allocator which creates the Ruby object
  rb_define_alloc_func(static_cast<VALUE>(*this), detail::default_allocation_func<T>);

  // Define an initialize function that will create the C++ object
  this->define_method("initialize", &Constructor_T::construct, args...);

  return *this;
}

template<typename T>
template<typename Director_T>
inline Rice::Data_Type<T>& Rice::Data_Type<T>::
define_director()
{
  Rice::Data_Type<Director_T>::template bind<T>(*this);
  return *this;
}

template<typename T>
inline T * Rice::Data_Type<T>::
from_ruby(Object x)
{
  check_is_bound();

  void * v = DATA_PTR(x.value());
  Class klass = x.class_of();

  if(klass.value() == klass_)
  {
    // Great, not converting to a base/derived type
    Data_Type<T> data_klass;
    Data_Object<T> obj(x, data_klass);
    return obj.get();
  }

  // Finding the bound type that relates to the given klass is
  // a two step process. We iterate over the list of known type casters,
  // looking for:
  //
  // 1) casters that handle this direct type
  // 2) casters that handle types that are ancestors of klass
  //
  // Step 2 allows us to handle the case where a Rice-wrapped class
  // is subclassed in Ruby but then an instance of that class is passed
  // back into C++ (say, in a Listener / callback construction)
  //

  VALUE ancestors = rb_mod_ancestors(klass.value());
  long earliest = RARRAY_LEN(ancestors) + 1;

  int index;
  VALUE indexFound;

  std::pair<VALUE, detail::Abstract_Caster*> toUse{ Qnil, nullptr };

  for (const auto& pair: casters)
  {
    // Do we match directly?
    if (klass.value() == pair.first)
    {
      toUse = pair;
      break;
    }

    // Check for ancestors. Trick is, we need to find the lowest
    // ancestor that does have a Caster to make sure that we're casting
    // to the closest C++ type that the Ruby class is subclassing. 
    // There might be multiple ancestors that are also wrapped in
    // the extension, so find the earliest in the list and use that one.
    indexFound = rb_funcall(ancestors, rb_intern("index"), 1, pair.first);

    if(indexFound != Qnil)
    {
      index = NUM2INT(indexFound);

      if(index < earliest)
      {
        earliest = index;
        toUse = pair;
      }
    }
  }
  
  if (toUse.first == Qnil)
  {
    std::string s = "Class ";
    s += klass.name().str();
    s += " is not registered/bound in Rice";
    throw std::runtime_error(s);
  }

  detail::Abstract_Caster* caster = toUse.second;
  if (caster)
  {
    T * result = static_cast<T *>(caster->cast_to_base(v, klass_));
    return result;
  }
  else
  {
    return static_cast<T *>(v);
  }
}

template<typename T>
inline bool Rice::Data_Type<T>::
is_bound()
{
  return klass_ != Qnil;
}

template<typename T>
inline Rice::detail::Abstract_Caster * Rice::Data_Type<T>::
caster() const
{
  check_is_bound();
  return caster_.get();
}

namespace Rice
{

template<>
inline detail::Abstract_Caster * Data_Type<void>::
caster() const
{
  return 0;
}

template<typename T>
void Data_Type<T>::
check_is_bound()
{
  if(!is_bound())
  {
    std::string s;
    s = "Data type ";
    s += detail::demangle(typeid(T).name());
    s += " is not bound";
    throw std::runtime_error(s.c_str());
  }
}

} // Rice

template<typename T>
inline Rice::Data_Type<T> Rice::
define_class_under(
    Object module,
    char const * name)
{
  Class c(define_class_under(module, name, rb_cData));
  c.undef_creation_funcs();
  return Data_Type<T>::template bind<void>(c);
}

template<typename T, typename Base_T>
inline Rice::Data_Type<T> Rice::
define_class_under(
    Object module,
    char const * name)
{
  Data_Type<Base_T> base_dt;
  Class c(define_class_under(module, name, base_dt));
  c.undef_creation_funcs();
  return Data_Type<T>::template bind<Base_T>(c);
}

template<typename T>
inline Rice::Data_Type<T> Rice::
define_class(
    char const * name)
{
  Class c(define_class(name, rb_cData));
  c.undef_creation_funcs();
  return Data_Type<T>::template bind<void>(c);
}

template<typename T, typename Base_T>
inline Rice::Data_Type<T> Rice::
define_class(
    char const * name)
{
  Data_Type<Base_T> base_dt;
  Class c(define_class(name, base_dt));
  c.undef_creation_funcs();
  return Data_Type<T>::template bind<Base_T>(c);
}

template<typename T>
template<typename U, typename Iterator_T>
inline Rice::Data_Type<T>& Rice::Data_Type<T>::
define_iterator(Iterator_T(U::* begin)(), Iterator_T(U::* end)(), Identifier name)
{
  using Iter_T = detail::Iterator<U, Iterator_T>;
  Iter_T* iterator = new Iter_T(begin, end);
  detail::MethodData::define_method(Data_Type<T>::klass(), name, 
    (RUBY_METHOD_FUNC)iterator->call, 0, iterator);

  return *this;
}

template<typename From_T, typename To_T>
inline void 
Rice::define_implicit_cast()
{
  // As Rice currently expects only one entry into
  // this list for a given klass VALUE, we need to get
  // the current caster for From_T and insert in our
  // new caster as the head of the caster list

  Class from_class = Data_Type<From_T>::klass().value();
  Class to_class = Data_Type<To_T>::klass().value();

  detail::Abstract_Caster* from_caster = 
    Data_Type<From_T>::caster_.release();

  detail::Abstract_Caster* new_caster = 
    new detail::Implicit_Caster<To_T, From_T>(from_caster, to_class);

  // Insert our new caster into the list for the from class
  Data_Type_Base::casters.erase(from_class);
  Data_Type_Base::casters.insert(
    std::make_pair(
      from_class,
      new_caster
    )
  );

  // And make sure the from_class has direct access to the
  // updated caster list
  Data_Type<From_T>::caster_.reset(new_caster);
}

#endif // Rice__Data_Type__ipp_



// =========   Data_Object.hpp   =========


// ---------   Data_Object_defn.hpp   ---------
#ifndef Rice__Data_Object_defn__hpp_
#define Rice__Data_Object_defn__hpp_


/*! \file
 *  \brief Provides a helper class for wrapping and unwrapping C++
 *  objects as Ruby objects.
 */

namespace Rice
{
// Forward declaration
template<typename T>
class Data_Type;

template<typename T>
struct Default_Mark_Function
{
  typedef void (*Ruby_Data_Func)(T * obj);
  static const Ruby_Data_Func mark;
};

template<typename T>
struct Default_Free_Function
{
  static void free(T * obj) { delete obj; }
};


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
class Data_Object
  : public Object
{
public:
  //! A function that takes a T* and returns void.
  typedef void (*Ruby_Data_Func)(T * obj);

  //! Wrap a C++ object.
  /*! This constructor is analogous to calling Data_Wrap_Struct.  Be
   *  careful not to call this function more than once for the same
   *  pointer (in general, it should only be called for newly
   *  constructed objects that need to be managed by Ruby's garbage
   *  collector).
   *  \param obj the object to wrap.
   *  \param klass the Ruby class to use for the newly created Ruby
   *  object.
   *  \param mark_func a function that gets called by the garbage
   *  collector to mark the object's children.
   *  \param free_func a function that gets called by the garbage
   *  collector to free the object.
   */
  Data_Object(
      T * obj,
      VALUE klass = Data_Type<T>::klass(),
      Ruby_Data_Func mark_func = Default_Mark_Function<T>::mark,
      Ruby_Data_Func free_func = Default_Free_Function<T>::free);

  //! Unwrap a Ruby object.
  /*! This constructor is analogous to calling Data_Get_Struct.  Uses
   *  Data_Type<T>::klass as the class of the object.
   *  \param value the Ruby object to unwrap.
   */
  Data_Object(
      Object value);

  //! Unwrap a Ruby object.
  /*! This constructor is analogous to calling Data_Get_Struct.  Will
   *  throw an exception if the class of the object differs from the
   *  specified class.
   *  \param value the Ruby object to unwrap.
   *  \param klass the expected class of the object.
   */
  template<typename U>
  Data_Object(
      Object value,
      Data_Type<U> const & klass);

  // Enable copying
  Data_Object(const Data_Object& other) = default;
  Data_Object& operator=(const Data_Object& other) = default;

  // Enable moving
  Data_Object(Data_Object&& other);
  Data_Object& operator=(Data_Object&& other);

  T& operator*() const; //!< Return a reference to obj_
  T* operator->() const; //!< Return a pointer to obj_
  T* get() const;        //!< Return a pointer to obj_

private:
  static void check_cpp_type(Data_Type<T> const & klass);
  static void check_ruby_type(VALUE value, VALUE klass, bool include_super);

private:
  T * obj_;
};

} // namespace Rice

#endif // Rice__Data_Object_defn__hpp_


// ---------   Data_Object.ipp   ---------
#ifndef Rice__Data_Object__ipp_
#define Rice__Data_Object__ipp_


#include <algorithm>

template<typename T>
const typename Rice::Default_Mark_Function<T>::Ruby_Data_Func
Rice::Default_Mark_Function<T>::mark = ruby_mark<T>;

namespace Rice
{

namespace detail
{

inline VALUE data_wrap_struct(
    VALUE klass,
    RUBY_DATA_FUNC mark,
    RUBY_DATA_FUNC free,
    void * obj)
{
  return Data_Wrap_Struct(klass, mark, free, obj);
}

template<typename T>
inline VALUE wrap(
    VALUE klass,
    typename Data_Object<T>::Ruby_Data_Func mark,
    typename Data_Object<T>::Ruby_Data_Func free,
    T * obj)
{
  // We cast to obj void* here before passing to Data_Wrap_Struct,
  // becuase otherwise compilation will fail if obj is const.  It's safe
  // to do this, because unwrap() will always add the const back when
  // the object is unwrapped.
  return Rice::protect(data_wrap_struct,
      klass,
      reinterpret_cast<RUBY_DATA_FUNC>(mark),
      reinterpret_cast<RUBY_DATA_FUNC>(free),
      (void *)obj);
}

template<typename T>
inline VALUE data_get_struct(VALUE value, T * * obj)
{
  Data_Get_Struct(value, T, *obj);
  return Qnil;
}

template<typename T>
inline T * unwrap(VALUE value)
{
  T * obj;
  Rice::protect(data_get_struct<T>, value, &obj);
  return obj;
}

} // namespace detail

} // namespace Rice

template<typename T>
inline Rice::Data_Object<T>::
Data_Object(
    T * obj,
    VALUE klass,
    Ruby_Data_Func mark_func,
    Ruby_Data_Func free_func)
  : obj_(obj)
{
  VALUE value = detail::wrap(klass, mark_func, free_func, obj);
  this->set_value(value);
}

template<typename T>
inline Rice::Data_Object<T>::
Data_Object(
    Object value)
  : Object(value)
  , obj_(detail::unwrap<T>(value))
{  
  Data_Type<T> klass;
  check_cpp_type(klass);
  check_ruby_type(value, klass, true);
}

template<typename T>
template<typename U>
inline Rice::Data_Object<T>::
Data_Object(
    Object value,
    Data_Type<U> const & klass)
  : Object(value)
  , obj_(detail::unwrap<T>(value))
{  
  check_cpp_type(klass);
  check_ruby_type(value, klass, true);
}

template<typename T>
inline Rice::Data_Object<T>::
Data_Object(Data_Object&& other): Object(other)
{
  this->obj_ = other.obj_;
  other.obj_ = nullptr;
}

template<typename T>
inline Rice::Data_Object<T>& Rice::Data_Object<T>::
operator=(Data_Object&& other)
{
  Object::operator=(other);

  this->obj_ = other.obj_;
  other.obj_ = nullptr;

  return *this;
}

template<typename T>
inline void Rice::Data_Object<T>::
check_cpp_type(Data_Type<T> const & /* klass */)
{
}

template<typename T>
inline void Rice::Data_Object<T>::
check_ruby_type(VALUE value, VALUE klass, bool include_super)
{
  if (!rb_obj_is_kind_of(value, klass) || (!include_super && !rb_obj_is_instance_of(value, klass)))
  {
    // Not sure why this stuff can't be chained
    VALUE gotV = protect(rb_class_name, rb_obj_class(value));
    char* got = StringValueCStr(gotV);
    VALUE exptV = protect(rb_class_name, klass);
    char* expected = StringValueCStr(exptV);

    throw Exception(
      rb_eTypeError,
      "wrong argument type %s (expected %s)",
      got, expected
    );
  }
}

template<typename T>
inline T& Rice::Data_Object<T>::
operator*() const
{
  return *obj_;
}

template<typename T>
inline T* Rice::Data_Object<T>::
operator->() const
{
  return obj_;
}

template<typename T>
inline T* Rice::Data_Object<T>::
get() const
{
  return obj_;
}

#endif // Rice__Data_Object__ipp_




// =========   Iterator.ipp   =========
#ifndef Rice_Iterator__ipp_
#define Rice_Iterator__ipp_

#include <iterator>
#include <functional>


namespace Rice
{
  namespace detail
  {

    template <typename T, typename Iterator_T>
    inline Iterator<T, Iterator_T>::
      Iterator(Iterator_T(T::* begin)(), Iterator_T(T::* end)()) :
      begin_(begin), end_(end)
    {
    }

    template<typename T, typename Iterator_T>
    inline VALUE Iterator<T, Iterator_T>::
      call(VALUE self)
    {
      using Iter_T = Iterator<T, Iterator_T>;
      Iter_T* iterator = detail::MethodData::data<Iter_T*>();
      return iterator->operator()(self);
    }

    template<typename T, typename Iterator_T>
    inline VALUE Iterator<T, Iterator_T>::
      operator()(VALUE self)
    {
      using Value_T = typename std::iterator_traits<Iterator_T>::value_type;

      Data_Object<T> obj(self);
      Iterator_T it = std::invoke(this->begin_, *obj);
      Iterator_T end = std::invoke(this->end_, *obj);

      for (; it != end; ++it)
      {
        Rice::protect(rb_yield, detail::To_Ruby<Value_T>::convert(*it));
      }

      return self;
    }
  } // namespace detail
} // namespace Rice

#endif // Rice_Iterator__ipp_


// =========   default_allocation_func.ipp   =========

template<typename T>
VALUE Rice::detail::
default_allocation_func(VALUE klass)
{
  Data_Object<T> m(nullptr, klass);
  return m.value();
} 



// =========   Wrapped_Function.ipp   =========

#include <array>
#include <algorithm>


namespace Rice
{

namespace detail
{

template<typename Function_T, typename Return_T, typename Receiver_T, typename... Arg_Ts>
VALUE Wrapped_Function<Function_T, Return_T, Receiver_T, Arg_Ts...>::
call(int argc, VALUE* argv, VALUE self)
{
  using Wrapper_T = Wrapped_Function<Function_T, Return_T, Receiver_T, Arg_Ts...>;
  Wrapper_T* wrapper = detail::MethodData::data<Wrapper_T*>();
  return wrapper->operator()(argc, argv, self);
}

template<typename Function_T, typename Return_T, typename Receiver_T, typename... Arg_Ts>
Wrapped_Function<Function_T, Return_T, Receiver_T, Arg_Ts...>::
Wrapped_Function(Function_T func, std::shared_ptr<Exception_Handler> handler, Arguments* arguments)
    : func_(func), handler_(handler), arguments_(arguments)
{
  if (!arguments_)
  {
    arguments_ = std::make_unique<Arguments>();
  }
}

template<typename Function_T, typename Return_T, typename Receiver_T, typename... Arg_Ts>
std::vector<VALUE> Wrapped_Function<Function_T, Return_T, Receiver_T, Arg_Ts...>::
getRubyValues(int argc, VALUE* argv)
{
  // Setup a tuple to contain required arguments to rb_scan_args
  std::string scanFormat = this->arguments_->formatString(sizeof...(Arg_Ts));
  std::tuple<int, VALUE*, const char*> rbScanMandatory = std::forward_as_tuple(argc, argv, scanFormat.c_str());

  // Create a vector to store the variable number of Ruby Values
  std::vector<VALUE> rbScanArgsOptional(sizeof...(Arg_Ts), Qnil);

  // Convert the vector to an array so it can then be concatenated to a tuple
  std::array<VALUE*, sizeof...(Arg_Ts)> rbScanArgsOptionalPointers;
  std::transform(rbScanArgsOptional.begin(), rbScanArgsOptional.end(), rbScanArgsOptionalPointers.begin(),
    [](VALUE& value)
    {
      return &value;
    });

  // Combine the tuples and call rb_scan_args
  auto rbScanArgs = std::tuple_cat(rbScanMandatory, rbScanArgsOptionalPointers);
  std::apply(rb_scan_args, rbScanArgs);

  return rbScanArgsOptional;
}

template<typename Function_T, typename Return_T, typename Receiver_T, typename... Arg_Ts>
template<std::size_t... I>
std::tuple<Arg_Ts...> Wrapped_Function<Function_T, Return_T, Receiver_T, Arg_Ts...>::
getNativeValues(std::vector<VALUE>& values, std::tuple<NativeArg<Arg_Ts>...>& nativeArgs, std::index_sequence<I...>& indices)
{
  // Convert each Ruby value to its native value. Check each Ruby nil value to see if it has
  // a default argument, and if yes, use that. Otherwise use NativeArg<Arg_Ts> to convert
  // the Ruby value to a native value. Note that for fundamental types NativeArg<Arg_Ts> 
  // will keep a copy of the native value so it can be passed by reference or pointer to a
  // native function.
  return std::forward_as_tuple((values[I] == Qnil && this->arguments_->isOptional(I) ?
                                this->arguments_->template defaultValue<Arg_Ts>(I) :
                                std::get<I>(nativeArgs).nativeValue(values[I]))...);
}

template<typename Function_T, typename Return_T, typename Receiver_T, typename... Arg_Ts>
Receiver_T Wrapped_Function<Function_T, Return_T, Receiver_T, Arg_Ts...>::
getReceiver(VALUE receiver)
{
  // Return the receiver.
  if constexpr (std::is_same<Receiver_T, Object>::value)
  {
    // This is used for Rice::Constructor
    return Object(receiver);
  }
  else if constexpr (std::is_same<Receiver_T, Class>::value)
  {
    // This is used for Rice::Enum
    return Class(receiver);
  }
  else if constexpr (std::is_same<Receiver_T, void>::value)
  {
    // This is used for functions and static members
    return nullptr;
  }
  else
  {
    // This is used for member functions - we are returning a pointer to an object
    return From_Ruby<Receiver_T>::convert(receiver);
  }
}

template<typename Function_T, typename Return_T, typename Receiver_T, typename... Arg_Ts>
VALUE Wrapped_Function<Function_T, Return_T, Receiver_T, Arg_Ts...>::
invokeNative(NativeTypes& nativeArgs)
{
  // Call the native function
  if constexpr (std::is_void_v<Return_T>)
  {
    std::apply(this->func_, nativeArgs);
    return Qnil;
  }
  else
  {
    Return_T result = std::apply(this->func_, nativeArgs);
    return To_Ruby<Return_T>::convert(result);
  }
}

template<typename Function_T, typename Return_T, typename Receiver_T, typename... Arg_Ts>
VALUE Wrapped_Function<Function_T, Return_T, Receiver_T, Arg_Ts...>::
operator()(int argc, VALUE* argv, VALUE self)
{
  try
  {
    // Get the ruby values
    std::vector<VALUE> rubyValues = this->getRubyValues(argc, argv);

    // Create a tuple of NativeArgs that will convert the Ruby values to native values. For 
    // fundamental types NativeArgs will also keep a copy of the native value so that it 
    // can be passed by reference or pointer to the native function.
    std::tuple<NativeArg<Arg_Ts>...> nativeArgs;

    // Convert the Ruby values to native values
    auto indices = std::make_index_sequence<sizeof...(Arg_Ts)>{};
    std::tuple<Arg_Ts...> nativeValues = this->getNativeValues(rubyValues, nativeArgs, indices);

    // Now call the native method
    if constexpr (std::is_same_v<Receiver_T, std::nullptr_t>)
    {
      return this->invokeNative(nativeValues);
    }
    else
    {
      Receiver_T receiver = this->getReceiver(self);
      std::tuple<Receiver_T, Arg_Ts...> nativeArgs = std::tuple_cat(std::tuple(receiver), nativeValues);
      return this->invokeNative(nativeArgs);
    }
    return Qnil;
  }
  catch (...)
  {
    RUBY_TRY
    {
      return this->handler_->handle_exception();
    }
    RUBY_CATCH
  }
}
} // detail
} // Rice


// =========   Enum.hpp   =========


namespace Rice
{
  // The C++ struct that we use to store enum information and is wrapped
  // via Ruby Objects
  template<typename Enum_T>
  struct Enum_Storage
  {
    Enum_Storage(std::string name, Enum_T value);
    bool operator==(const Enum_Storage& other);
    int32_t compare(const Enum_Storage& other);

    std::string enumName;
    Enum_T enumValue;
  };

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
class Enum
  : public Data_Type<Enum_Storage<Enum_T>>
{
public:
  using Storage_T = Enum_Storage<Enum_T>;
  using Value_T = Data_Object<Enum_Storage<Enum_T>>;

  //! Default constructor.
  Enum() = default;

  //! Construct and initialize.
  Enum(
      char const * name,
      Module module = rb_cObject);

  //! Define a new enum value.
  /*! \param name the name of the enum value.
   *  \param value the value to associate with name.
   *  \return *this
   */
  Enum<Enum_T> & define_value(
      std::string name,
      Enum_T value);

private:
  //! Initialize the enum type.
  /*! Must be called only once.
   *  \param name the name of the class to define
   *  \param module the module in which to place the enum class.
   *  \return *this
   */
  Enum<Enum_T> & initialize(
      char const * name,
      Module module = rb_cObject);

private:
  static Object each(Object self);
  static Object to_s(Object self);
  static Object to_i(Object self);
  static Object inspect(Object self);
  static Object compare(Object lhs, Object rhs);
  static Object eql(Object lhs, Object rhs);
  static Object hash(Object self);
  static Object from_int(Class klass, Object i);
};

template<typename T>
Enum<T> define_enum(
    char const * name,
    Module module = rb_cObject);

} // namespace Rice


// ---------   Enum.ipp   ---------

template<typename Enum_T>
inline Rice::Enum_Storage<Enum_T>::
Enum_Storage(std::string name, Enum_T value) : enumName(name), enumValue(value)
{
}

template<typename Enum_T>
inline bool Rice::Enum_Storage<Enum_T>::
operator==(const Enum_Storage<Enum_T>& other)
{
  return this->enumValue == other.enumValue;
}

template<typename Enum_T>
inline int32_t Rice::Enum_Storage<Enum_T>::
compare(const Rice::Enum_Storage<Enum_T>& other)
{
  if (this->enumValue == other.enumValue)
  {
    return 0;
  }
  else if (this->enumValue < other.enumValue)
  {
    return -1;
  }
  else
  {
    return 1;
  }
}

template<typename Enum_T>
Rice::Enum<Enum_T>::
Enum(
    char const * name,
    Module module)
  : Data_Type<Enum_Storage<Enum_T>>()
{
  this->template bind<void>(initialize(name, module));
}

template<typename Enum_T>
Rice::Enum<Enum_T> & Rice::Enum<Enum_T>::
initialize(
    char const * name,
    Module module)
{
  Class c = Rice::define_class_under<Enum_T>(module, name)
    .define_method("to_s", to_s)
    .define_method("to_i", to_i)
    .define_method("inspect", inspect)
    .define_method("<=>", compare)
    .define_method("hash", hash)
    .define_method("eql?", eql)
    .define_method("==", eql)
    .define_method("===", eql)
    .define_singleton_method("each", each)
    .define_singleton_method("from_int", from_int)
    .include_module(rb_mComparable);

  // Create a Ruby array that we will use later to store enum values
  // and attach it to the class
  Array enums;
  protect(rb_iv_set, c, "enums", enums);

  // Since we just changed the underlying class we need to rebind this enum
  // before returning. Otherwise this->value() will bind the wrong class to 
  // the datatype
  this->set_value(c);
  return *this;
}

template<typename Enum_T>
Rice::Enum<Enum_T> & Rice::Enum<Enum_T>::
define_value(
    std::string name,
    Enum_T value)
{
  // Create a new storage struct for this value
  Storage_T* storage = new Storage_T(name, value);

  // Now wrap it and store it to the class enums field
  Value_T wrapper(storage, *this);
  Array enums = rb_iv_get(this->value(), "enums");
  enums.push(wrapper);

  // And store it as class constant
  this->const_set(name, wrapper);

  return *this;
}

template<typename Enum_T>
Rice::Object Rice::Enum<Enum_T>::
each(Object self)
{
  Array enums = rb_iv_get(self, "enums");
  Check_Type(enums, T_ARRAY);
  for(long i = 0; i < enums.size(); ++i)
  {
    rb_yield(enums[i].value());
  }
  return Qnil;
}

template<typename Enum_T>
Rice::Object Rice::Enum<Enum_T>::
to_s(Object self)
{
  Value_T wrapper(self);
  return detail::to_ruby(wrapper->enumName);
}

template<typename Enum_T>
Rice::Object Rice::Enum<Enum_T>::
inspect(Object self)
{
  Value_T wrapper(self);

  return String::format(
      "#<%s::%s>",
      String(self.class_of().name()).c_str(),
      wrapper->enumName.c_str());
}

template<typename Enum_T>
Rice::Object Rice::Enum<Enum_T>::
compare(Object lhs, Object rhs)
{
  if(lhs.class_of() != rhs.class_of())
  {
    String lhs_name(lhs.class_of().name());
    String rhs_name(rhs.class_of().name());
    rb_raise(
        rb_eTypeError,
        "Cannot compare %s to %s",
        lhs_name.c_str(),
        rhs_name.c_str());
  }

  Value_T left(lhs);
  Value_T right(rhs);

  int32_t result = left->compare(*right);
  return detail::To_Ruby<int>::convert(result);
}

template<typename Enum_T>
Rice::Object Rice::Enum<Enum_T>::
eql(Object lhs, Object rhs)
{
  Value_T left(lhs);
  Value_T right(rhs);
  bool is_equal = (*left == *right);
  return detail::To_Ruby<bool>::convert(is_equal);
}

template<typename Enum_T>
Rice::Object Rice::Enum<Enum_T>::
to_i(Object self)
{
  using Integral_T = std::underlying_type_t<Enum_T>;
  Value_T wrapper(self);
  return detail::To_Ruby<Integral_T>::convert((Integral_T)wrapper->enumValue);
}

template<typename Enum_T>
Rice::Object Rice::Enum<Enum_T>::
hash(Object self)
{
  return to_i(self);
}

template<typename Enum_T>
Rice::Object Rice::Enum<Enum_T>::
from_int(Class klass, Object i)
{
  Enum_T enumValue = static_cast<Enum_T>(detail::From_Ruby<long>::convert(i));
  Array enums = rb_iv_get(klass, "enums");

  auto iter = std::find_if(enums.begin(), enums.end(),
    [enumValue](const Object& object)
    {
      Value_T dataObject(object);
      return dataObject->enumValue == enumValue;
    });

  if (iter == enums.end())
  {
    rb_raise(
      rb_eArgError,
      "Invalid Enum value %i", (int)enumValue);
  }

  return *iter;
}

template<typename T>
Rice::Enum<T> Rice::
define_enum(
    char const * name,
    Module module)
{
  return Enum<T>(name, module);
}

// =========   Struct.hpp   =========


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
  : public Class
{
public:
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

  //! Get the members in Struct.
  Array members() const;

  class Instance;
  friend class Instance;
  friend Struct Rice::define_struct();

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
class Struct::Instance
  : public Builtin_Object<T_STRUCT>
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

private:
  Struct type_;
};

} // namespace Rice


// ---------   Struct.ipp   ---------

inline Rice::Struct& Rice::Struct::
initialize(
    Module module,
    Identifier name)
{
  Class struct_class(rb_cStruct);

  Object type = struct_class.vcall("new", this->members());

  set_value(type);
  module.const_set(name, type);

  return *this;
}

inline Rice::Struct& Rice::Struct::
define_member(
    Identifier name)
{
  if (value() != rb_cObject)
  {
    throw std::runtime_error("struct is already initialized");
  }

  members_.push_back(name.to_sym());

  return *this;
}

inline Rice::Array Rice::Struct::
members() const
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

inline Rice::Struct::Instance
Rice::Struct::
  new_instance(Array args) const
{
  Object instance = const_cast<Struct*>(this)->vcall("new", args);
  return Instance(*this, instance);
}

inline Rice::Struct::Instance::
Instance(
    Struct const& type,
    Array args)
  : Builtin_Object<T_STRUCT>(type.new_instance(args))
  , type_(type)
{
}

inline Rice::Struct::Instance::
Instance(
    Struct const& type,
    Object s)
  : Builtin_Object<T_STRUCT>(s)
  , type_(type)
{
}

inline Rice::Struct Rice::
define_struct()
{
  return Struct();
}

template<typename T>
inline Rice::Object Rice::Struct::Instance::
operator[](T index)
{
  return rb_struct_aref(value(), ULONG2NUM(index));
}

template<>
inline Rice::Object Rice::Struct::Instance::
operator[]<Rice::Identifier>(Rice::Identifier member)
{
  return rb_struct_aref(value(), Symbol(member));
}

template<>
inline Rice::Object Rice::Struct::Instance::
operator[]<char const *>(char const * name)
{
  return (*this)[Identifier(name)];
}



// =========   forward_declares.ipp   =========

// These methods cannot be defined where they are declared due to circular dependencies
inline Rice::Class Rice::Object::
class_of() const
{
  return rb_class_of(value_);
}

inline Rice::String Rice::Object::
to_s() const
{
  return call("to_s");
}

inline Rice::String Rice::Object::
inspect() const
{
  return call("inspect");
}

inline Rice::Object Rice::Object::
instance_eval(String const& s)
{
  VALUE argv[] = { s.value() };
  return protect(rb_obj_instance_eval, 1, &argv[0], *this);
}

inline Rice::Object Rice::Object::
vcall(
  Identifier id,
  Array args)
{
  std::vector<VALUE> a(args.size());

  Array::const_iterator it = args.begin();
  Array::const_iterator end = args.end();

  for (int i = 0; it != end; i++, ++it)
  {
    a[i] = it->value();
  }

  return protect(rb_funcall3, *this, id, (int)args.size(), a.data());
}

inline std::ostream& Rice::
operator<<(std::ostream& out, Rice::Object const& obj)
{
  String s(obj.to_s());
  out << s.c_str();
  return out;
}

inline Rice::Identifier::
Identifier(Symbol const& symbol)
  : id_(SYM2ID(symbol.value()))
{
}

inline
Rice::String Rice::Module::
name() const
{
  Object name = rb_mod_name(*this);
  if (name.is_nil())
  {
    // 1.9
    return String("");
  }
  else
  {
    return name;
  }
}

inline
Rice::Array
Rice::Module::
ancestors() const
{
  return protect(rb_mod_ancestors, *this);
}

inline Rice::Class
Rice::Module::
singleton_class() const
{
  return CLASS_OF(value());
}

inline
Rice::Class
Rice::Module::
define_class(
  char const* name,
  Object superclass)
{
  return Rice::define_class_under(*this, name, superclass);
}

template<typename T>
inline
Rice::Data_Type<T>
Rice::Module::
define_class_with_object_as_base(
  char const* name)
{
  return Rice::define_class_under<T>(*this, name);
}

template<typename T, typename T_Base_T>
inline
Rice::Data_Type<T>
Rice::Module::
define_class(
  char const* name)
{
  return Rice::define_class_under<T, T_Base_T>(
    *this,
    name);
}

template<typename Fun_T>
inline void Rice::Module::define_method_and_auto_wrap(VALUE klass, Identifier name, Fun_T function,
  std::shared_ptr<detail::Exception_Handler> handler,
  Arguments* arguments)
{
  auto* wrapper = detail::wrap_function(function, handler, arguments);
  using Wrapper_T = typename std::remove_pointer_t<decltype(wrapper)>;

  Rice::protect(detail::MethodData::define_method, klass, name.id(),
                RUBY_METHOD_FUNC(&Wrapper_T::call),
    -1, 
    wrapper);
}

