
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
