#include <algorithm>
#include <array>
#include <stdexcept>
#include <sstream>
#include <tuple>

namespace Rice::detail
{
  template<typename Function_T>
  template<typename ...Arg_Ts>
  void NativeFunction<Function_T>::define(VALUE klass, std::string method_name, Function_T function, Arg_Ts&& ...args)
  {
    // Verify return type
    using Arg_Tuple = std::tuple<Arg_Ts...>;
    constexpr bool isBuffer = tuple_element_index_v<Arg_Tuple, ReturnBuffer> < std::tuple_size_v<Arg_Tuple>;
    Native::verify_type<Return_T, isBuffer>();

    // Have we defined this method yet in Ruby?
    Identifier identifier(method_name);
    const std::vector<std::unique_ptr<Native>>& natives = Registries::instance.natives.lookup(klass, identifier.id());
    if (natives.empty())
    {
      // Tell Ruby to invoke the static resolved method defined above
      detail::protect(rb_define_method, klass, method_name.c_str(), (RUBY_METHOD_FUNC)&Native::resolve, -1);
    }

    // Create function parameters - this will also validate their types
    std::vector<std::unique_ptr<ParameterAbstract>> parameters = Native::create_parameters<Parameter_Ts>(args...);

    // Create return info
    std::unique_ptr<Return> returnInfo = Native::create_return<Arg_Ts...>(args...);

    // Create native method
    NativeFunction_T* nativeFunction = new NativeFunction_T(klass, method_name, std::forward<Function_T>(function), std::move(returnInfo), std::move(parameters));
    std::unique_ptr<Native> native(nativeFunction);

    // Register the native function
    detail::Registries::instance.natives.add(klass, identifier.id(), native);
  }

  template<typename Function_T>
  NativeFunction<Function_T>::NativeFunction(VALUE klass, std::string method_name, Function_T function, std::unique_ptr<Return>&& returnInfo, std::vector<std::unique_ptr<ParameterAbstract>>&& parameters)
    : Native(std::move(returnInfo), std::move(parameters)),
      klass_(klass), method_name_(method_name), function_(function), toRuby_(returnInfo_.get())
  {
  }

  template<typename Function_T>
  template<std::size_t... I>
  std::vector<std::string> NativeFunction<Function_T>::argTypeNames(std::ostringstream& stream, const std::index_sequence<I...>& indices)
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

    auto indices = std::make_index_sequence<std::tuple_size_v<Parameter_Ts>>{};
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
  typename NativeFunction<Function_T>::Parameter_Ts NativeFunction<Function_T>::getNativeValues(std::vector<std::optional<VALUE>>& values,
     std::index_sequence<I...>& indices)
  {
    /* Loop over each value returned from Ruby and convert it to the appropriate C++ type based
       on the arguments (Parameter_Ts) required by the C++ function. Arg_T may have const/volatile while
       the associated From_Ruby<T> template parameter will not. Thus From_Ruby produces non-const values 
       which we let the compiler convert to const values as needed. This works except for 
       T** -> const T**, see comment in convertToNative method. */
    //return std::forward_as_tuple(this->getNativeValue<std::tuple_element_t<I, Parameter_Ts>, I>(values)...);
    return std::forward_as_tuple(
      (dynamic_cast<Parameter<std::tuple_element_t<I, Parameter_Ts>>*>(this->parameters_[I].get()))->
               convertToNative(values[I])...);
  }

  template<typename Function_T>
  VALUE NativeFunction<Function_T>::invoke(Parameter_Ts&& nativeArgs)
  {
    if constexpr (std::is_void_v<Return_T>)
    {
      std::apply(this->function_, std::forward<Parameter_Ts>(nativeArgs));
      return Qnil;
    }
    else
    {
      // Call the native method and get the result
      Return_T nativeResult = std::apply(this->function_, std::forward<Parameter_Ts>(nativeArgs));

      // Return the result
      return this->toRuby_.convert(nativeResult);
    }
  }

  template<typename Function_T>
  VALUE NativeFunction<Function_T>::invokeNoGVL(Parameter_Ts&& nativeArgs)
  {
    if constexpr (std::is_void_v<Return_T>)
    {
      no_gvl(this->function_, std::forward<Parameter_Ts>(nativeArgs));
      return Qnil;
    }
    else
    {
      // Call the native method and get the result
      Return_T nativeResult = no_gvl(this->function_, std::forward<Parameter_Ts>(nativeArgs));

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
    for (size_t i = 0; i < this->parameters_.size(); i++)
    {
      Arg* arg = parameters_[i]->arg();
      if (arg->isKeepAlive())
      {
        if (selfWrapper == nullptr)
        {
          noWrapper(self, "self");
        }
        selfWrapper->addKeepAlive(rubyValues[i].value());
      }
    }

    // Check return value
    if (this->returnInfo_->isKeepAlive())
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

    auto indices = std::make_index_sequence<std::tuple_size_v<Parameter_Ts>>{};

    // Convert the Ruby values to native values
    Parameter_Ts nativeValues = this->getNativeValues(rubyValues, indices);

    bool noGvl = false; // this->methodInfo_->function()->isNoGvl();

    VALUE result = Qnil;

    //if (noGvl)
    //{
     // result = this->invokeNoGVL(std::forward<Parameter_Ts>(nativeValues));
    //}
    //else
    //{
      result = this->invoke(std::forward<Parameter_Ts>(nativeValues));
    //}

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
    bool isBuffer = dynamic_cast<ReturnBuffer*>(this->returnInfo_.get()) ? true : false;
    if (isBuffer)
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
