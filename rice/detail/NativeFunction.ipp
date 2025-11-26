#include <algorithm>
#include <array>
#include <stdexcept>
#include <sstream>
#include <tuple>

namespace Rice::detail
{
  template<typename Function_T, bool NoGVL>
  template<typename ...Arg_Ts>
  void NativeFunction<Function_T, NoGVL>::define(VALUE klass, std::string method_name, Function_T function, Arg_Ts&& ...args)
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

  template<typename Function_T, bool NoGVL>
  NativeFunction<Function_T, NoGVL>::NativeFunction(VALUE klass, std::string method_name, Function_T function, std::unique_ptr<Return>&& returnInfo, std::vector<std::unique_ptr<ParameterAbstract>>&& parameters)
    : Native(std::move(returnInfo), std::move(parameters)),
      klass_(klass), method_name_(method_name), function_(function), toRuby_(returnInfo_.get())
  {
  }

  template<typename Function_T, bool NoGVL>
  template<std::size_t... I>
  std::vector<std::string> NativeFunction<Function_T, NoGVL>::argTypeNames(std::ostringstream& stream, const std::index_sequence<I...>& indices)
  {
    std::vector<std::string> result;
    for (std::unique_ptr<ParameterAbstract>& parameter : this->parameters_)
    {
      result.push_back(parameter->cppTypeName());
    }
    return result;
  }

  template<typename Function_T, bool NoGVL>
  std::string NativeFunction<Function_T, NoGVL>::toString()
  {
    std::ostringstream result;

    detail::TypeIndexParser typeIndexParser(typeid(Return_T), std::is_fundamental_v<detail::intrinsic_type<Return_T>>);
    result << typeIndexParser.simplifiedName() << " ";
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
    
  template<typename Function_T, bool NoGVL>
  template<std::size_t... I>
  typename NativeFunction<Function_T, NoGVL>::Parameter_Ts NativeFunction<Function_T, NoGVL>::getNativeValues(std::vector<std::optional<VALUE>>& values,
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

  template<typename Function_T, bool NoGVL>
  VALUE NativeFunction<Function_T, NoGVL>::invoke(Parameter_Ts&& nativeArgs)
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

  template<typename Function_T, bool NoGVL>
  VALUE NativeFunction<Function_T, NoGVL>::invokeNoGVL(Parameter_Ts&& nativeArgs)
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

  template<typename Function_T, bool NoGVL>
  VALUE NativeFunction<Function_T, NoGVL>::operator()(size_t argc, const VALUE* argv, VALUE self)
  {
    // Get the ruby values and make sure we have the correct number
    std::vector<std::optional<VALUE>> rubyValues = this->getRubyValues(argc, argv, true);

    auto indices = std::make_index_sequence<std::tuple_size_v<Parameter_Ts>>{};

    // Convert the Ruby values to native values
    Parameter_Ts nativeValues = this->getNativeValues(rubyValues, indices);

    VALUE result = Qnil;

    if constexpr (NoGVL)
    {
      result = this->invokeNoGVL(std::forward<Parameter_Ts>(nativeValues));
    }
    else
    {
      result = this->invoke(std::forward<Parameter_Ts>(nativeValues));
    }

    // Check if any function arguments or return values need to have their lifetimes tied to the receiver
    this->checkKeepAlive(self, result, rubyValues);

    return result;
  }

  template<typename Function_T, bool NoGVL>
  inline std::string NativeFunction<Function_T, NoGVL>::name()
  {
    return this->method_name_;
  }

  template<typename Function_T, bool NoGVL>
  inline NativeKind NativeFunction<Function_T, NoGVL>::kind()
  {
    return NativeKind::Function;
  }

  template<typename Function_T, bool NoGVL>
  inline VALUE NativeFunction<Function_T, NoGVL>::returnKlass()
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
