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
      detail::TypeIndexParser typeIndexParser(typeid(T), std::is_fundamental_v<detail::intrinsic_type<T>>);
      std::string message = "Type " + typeIndexParser.name() + " is already bound to a different type";
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
      detail::TypeIndexParser typeIndexParser(typeid(T), std::is_fundamental_v<detail::intrinsic_type<T>>);
      std::string cppClassName = typeIndexParser.simplifiedName();
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
      detail::TypeIndexParser typeIndexParser(typeid(T), std::is_fundamental_v<detail::intrinsic_type<T>>);
      std::string message = "Type is not defined with Rice: " + typeIndexParser.name();
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
  template <typename Attribute_T, typename...Arg_Ts>
  inline Data_Type<T>& Data_Type<T>::define_attr(std::string name, Attribute_T attribute, AttrAccess access, const Arg_Ts&...args)
  {
    return this->define_attr_internal<Attribute_T>(this->klass_, name, std::forward<Attribute_T>(attribute), access, args...);
  }

  template <typename T>
  template <typename Attribute_T, typename...Arg_Ts>
  inline Data_Type<T>& Data_Type<T>::define_singleton_attr(std::string name, Attribute_T attribute, AttrAccess access, const Arg_Ts&...args)
  {
    VALUE singleton = detail::protect(rb_singleton_class, this->value());
    return this->define_attr_internal<Attribute_T>(singleton, name, std::forward<Attribute_T>(attribute), access, args...);
  }

  template <typename T>
  template <typename Attribute_T, typename...Arg_Ts>
  inline Data_Type<T>& Data_Type<T>::define_attr_internal(VALUE klass, std::string name, Attribute_T attribute, AttrAccess access, const Arg_Ts&...args)
  {
    using Attr_T = typename detail::attribute_traits<Attribute_T>::attr_type;

    // Define attribute getter
    if (access == AttrAccess::ReadWrite || access == AttrAccess::Read)
    {
      detail::NativeAttributeGet<Attribute_T>::define(klass, name, std::forward<Attribute_T>(attribute), args...);
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
  template<typename Method_T, typename ...Arg_Ts>
  inline void Data_Type<T>::wrap_native_method(VALUE klass, std::string name, Method_T&& method, const Arg_Ts&...args)
  {
    Module::wrap_native_method<T, Method_T>(klass, name, std::forward<Method_T>(method), args...);
  }
}
