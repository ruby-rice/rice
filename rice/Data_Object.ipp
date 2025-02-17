
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

  template<typename T>
  inline T* Data_Object<T>::from_ruby(VALUE value, bool takeOwnership)
  {
    if (Data_Type<T>::is_descendant(value))
    {
      return detail::unwrap<T>(value, Data_Type<T>::ruby_data_type(), takeOwnership);
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
    static_assert(!std::is_fundamental_v<intrinsic_type<T>>,
                  "Data_Object cannot be used with fundamental types");
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
    static_assert(!std::is_fundamental_v<intrinsic_type<T>>,
                  "Data_Object cannot be used with fundamental types");
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
    static_assert(!std::is_fundamental_v<intrinsic_type<T>>,
                  "Data_Object cannot be used with fundamental types");
  public:
    To_Ruby()
    {
      detail::Type<PointerView<T>>::verify();
    };

    explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
    {
      detail::Type<PointerView<T>>::verify();
    }

    VALUE convert(T* data)
    {
      if (data == nullptr)
      {
        return Qnil;
      }
      else if (this->returnInfo_ && this->returnInfo_->isArray())
      {
        PointerView<T> pointerView(data);
        Data_Object<PointerView<T>> dataObject(pointerView, true);
        return dataObject.value();
      }
      else
      {
        // Note that T could be a pointer or reference to a base class while data is in fact a
        // child class. Lookup the correct type so we return an instance of the correct Ruby class
        std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::Registries::instance.types.figureType(*data);
        bool isOwner = this->returnInfo_ && this->returnInfo_->isOwner();
        return detail::wrap(rubyTypeInfo.first, rubyTypeInfo.second, data, isOwner);
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
  class To_Ruby<T*&>
  {
    static_assert(!std::is_fundamental_v<intrinsic_type<T>>,
                  "Data_Object cannot be used with fundamental types");
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
    static_assert(!std::is_fundamental_v<intrinsic_type<T>>,
                  "Data_Object cannot be used with fundamental types");
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
      // This expression checks to see if T has an explicit copy constructor
      // If it does then we have to call it directly. Not sure if this end ups
      // with an extra copy or not?
      // 
      // std::is_constructible_v<T, T> && !std::is_convertible_v<T, T>
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        if constexpr (std::is_constructible_v<T, T> && !std::is_convertible_v<T, T>)
        {
          return T(this->arg_->template defaultValue<T>());
        }
        else
        {
          return this->arg_->template defaultValue<T>();
        }
      }
      else
      {
        if constexpr (std::is_constructible_v<T, T> && !std::is_convertible_v<T, T>)
        {
          using Intrinsic_T = intrinsic_type<T>;
          return T(*Data_Object<Intrinsic_T>::from_ruby(value, this->arg_ && this->arg_->isOwner()));
        }
        else
        {
          using Intrinsic_T = intrinsic_type<T>;
          return *Data_Object<Intrinsic_T>::from_ruby(value, this->arg_ && this->arg_->isOwner());
        }
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
        return *Data_Object<Intrinsic_T>::from_ruby(value, this->arg_ && this->arg_->isOwner());
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
        return std::move(*Data_Object<Intrinsic_T>::from_ruby(value, this->arg_ && this->arg_->isOwner()));
      }
    }

  private:
    Arg* arg_ = nullptr;
  };

  // Helper class to convert a Ruby array of T to a C++ array of T (this happens when an API take T* as parameter
  // along with a second size parameter)
  template<typename T>
  class ArrayHelper
  {
  public:
    using Intrinsic_T = intrinsic_type<T>;

    T* convert(VALUE value)
    {
      this->vector_ = Array(value).to_vector<T>();
      return this->vector_.data();
    }

  private:
    std::vector<Intrinsic_T> vector_;
  };

  // 99% of the time a T* represents a wrapped C++ object that we want to call methods on. However, T* 
  // could also be a pointer to an array of T objects, so T[]. OpenCV for example has API calls like this.
  //
  // Therefore this From_Ruby implementation supports both uses cases which complicates the code. The problem
  // is for T[] to compile, a class needs to be constructible, destructible and not abstract. A further wrinkle
  // is if T has an explicit copy-constructor then that requires additional special handling in the code
  // (see From_Ruby<T>). Whether this extra complication is worth it is debatable, but it does mean that 
  // a Ruby array can be passed to any C++ API that takes a * including fundamental types (unsigned char)
  // and class types (T).
  //
  // Note that the From_Ruby<T[]> specialization never matches a parameter defined in function as T[] - the C++ 
  // compiler always picks T* instead. Not sure why...
  template<typename T>
  class From_Ruby<T*>
  {
    static_assert(!std::is_fundamental_v<intrinsic_type<T>>,
                  "Data_Object cannot be used with fundamental types");
    using Intrinsic_T = intrinsic_type<T>;

  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    ~From_Ruby()
    {
      if constexpr (std::is_destructible_v<Intrinsic_T>)
      {
        delete this->arrayHelper_;
      }
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
        case RUBY_T_ARRAY:
          return Convertible::Exact;
          break;
        default:
          return Convertible::None;
      }
    }

    T* convert(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          return Data_Object<Intrinsic_T>::from_ruby(value, this->arg_ && this->arg_->isOwner());
          break;
        }
        case RUBY_T_NIL:
        {
          return nullptr;
          break;
        }
        case RUBY_T_ARRAY:
        {
          if constexpr (std::is_copy_constructible_v<Intrinsic_T> && std::is_destructible_v<Intrinsic_T> && !std::is_abstract_v<Intrinsic_T>)
          {
            if (this->arrayHelper_ == nullptr)
            {
              this->arrayHelper_ = new ArrayHelper<T>();
            }
            return this->arrayHelper_->convert(value);
            break;
          }
          // Will fall through to the type exception if we get here
        }
        default:
        {
          throw create_type_exception<Intrinsic_T>(value);
        }
      }
    }

  private:
    Arg* arg_ = nullptr;
    ArrayHelper<T>* arrayHelper_ = nullptr;
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
        return Data_Object<Intrinsic_T>::from_ruby(value, this->arg_ && this->arg_->isOwner());
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
          return Data_Type<T>::is_descendant(value) ? Convertible::Exact : Convertible::None;
          break;
        case RUBY_T_NIL:
          return Convertible::Exact;
          break;
        case RUBY_T_ARRAY:
          return Convertible::Exact;
          break;
        default:
          return Convertible::None;
      }
    }

    T** convert(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          return detail::unwrap<Intrinsic_T*>(value, Data_Type<T>::ruby_data_type(), false);
          break;
        }
        case RUBY_T_NIL:
        {
          return nullptr;
          break;
        }
        case RUBY_T_ARRAY:
        {
          this->vector_ = Array(value).to_vector<Intrinsic_T*>();
          return this->vector_.data();
        }
        default:
        {
          throw create_type_exception<Intrinsic_T>(value);
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
