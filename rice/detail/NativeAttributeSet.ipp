#include <array>
#include <algorithm>


namespace Rice::detail
{
  template<typename Attribute_T>
  template<typename...Arg_Ts>
  void NativeAttributeSet<Attribute_T>::define(VALUE klass, std::string name, Attribute_T attribute, Arg_Ts&...args)
  {
    // Extract Arg from Arg_Ts if present, otherwise create default
    using Arg_Tuple = std::tuple<Arg_Ts...>;
    constexpr std::size_t index = tuple_element_index_v<Arg_Tuple, Arg, ArgBuffer>;

    std::unique_ptr<Arg> arg;
    if constexpr (index < std::tuple_size_v<Arg_Tuple>)
    {
      using Arg_T_Local = std::decay_t<std::tuple_element_t<index, Arg_Tuple>>;
      const Arg_T_Local& argInfo = std::get<index>(std::forward_as_tuple(std::forward<Arg_Ts>(args)...));
      arg = std::make_unique<Arg_T_Local>(argInfo);
    }
    else
    {
      arg = std::make_unique<Arg>("value");
    }

    // Create the parameter
    auto parameter = std::make_unique<Parameter<T_Unqualified>>(std::move(arg));

    // Create a NativeAttributeSet that Ruby will call to write C++ variables
    NativeAttribute_T* nativeAttribute = new NativeAttribute_T(klass, name, std::forward<Attribute_T>(attribute), std::move(parameter));
    std::unique_ptr<Native> native(nativeAttribute);

    // Define the write method name
    std::string setter = name + "=";

    // Tell Ruby to invoke the static method resolve to set the attribute value
    detail::protect(rb_define_method, klass, setter.c_str(), (RUBY_METHOD_FUNC)&Native::resolve, -1);

    // Add to native registry
    Identifier identifier(setter);
    detail::Registries::instance.natives.replace(klass, identifier.id(), native);
  }

  template<typename Attribute_T>
  NativeAttributeSet<Attribute_T>::NativeAttributeSet(VALUE klass, std::string name, Attribute_T attribute, std::unique_ptr<Parameter<T_Unqualified>> parameter)
    : Native(name), klass_(klass), attribute_(attribute), parameter_(std::move(parameter))
  {
  }

  template<typename Attribute_T>
  inline Resolved NativeAttributeSet<Attribute_T>::matches(std::map<std::string, VALUE>& values)
  {
    if (values.size() == 1)
      return Resolved{ Convertible::Exact, this };
    else
      return Resolved{ Convertible::None, this };
  }

  template<typename Attribute_T>
  inline VALUE NativeAttributeSet<Attribute_T>::operator()(std::map<std::string, VALUE>& values, VALUE self)
  {
    if (values.size() != 1)
    {
      throw std::runtime_error("Incorrect number of parameters for setting attribute. Attribute: " + this->name_);
    }

    // Get the Ruby value and convert to native
    VALUE value = values.begin()->second;
    std::optional<VALUE> valueOpt(value);
    T_Unqualified nativeValue = this->parameter_->convertToNative(valueOpt);

    if constexpr (!std::is_null_pointer_v<Receiver_T>)
    {
      Receiver_T* nativeSelf = From_Ruby<Receiver_T*>().convert(self);
      nativeSelf->*attribute_ = nativeValue;
    }
    else
    {
      *attribute_ = nativeValue;
    }

    // Check if we need to prevent the value from being garbage collected
    if (this->parameter_->arg()->isKeepAlive())
    {
      WrapperBase::addKeepAlive(self, value);
    }

    return value;
  }

  template<typename Attribute_T>
  inline std::string NativeAttributeSet<Attribute_T>::toString()
  {
    return "";
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
