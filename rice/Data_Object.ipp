
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

    explicit To_Ruby(Return * returnInfo) : returnInfo_(returnInfo)
    {
    }

    template<typename U>
    VALUE convert(U& data)
    {
      // Note that T could be a pointer or reference to a base class while data is in fact a
      // child class. Lookup the correct type so we return an instance of the correct Ruby class
      std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::Registries::instance.types.figureType<T>(data);

      bool isOwner = (this->returnInfo_ && this->returnInfo_->isOwner());
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

    static_assert(!std::is_same_v<T, std::map<T, T>> && !std::is_same_v<T, std::unordered_map<T, T>> &&
                  !std::is_same_v<T, std::monostate> && !std::is_same_v<T, std::multimap<T, T>> &&
                  !std::is_same_v<T, std::optional<T>> && !std::is_same_v<T, std::pair<T, T>> &&
                  !std::is_same_v<T, std::set<T>> && !std::is_same_v<T, std::string> &&
                  !std::is_same_v<T, std::vector<T>>,
                  "Please include rice/stl.hpp header for STL support");

  public:
    To_Ruby() = default;

    explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
    {
    }

    template<typename U>
    VALUE convert(U* data)
    {
      bool isOwner = this->returnInfo_ && this->returnInfo_->isOwner();

      if (data == nullptr)
      {
        return Qnil;
      }
      else if (this->returnInfo_ && this->returnInfo_->isArray())
      {
        using Buffer_T = Buffer<T>;
        Buffer_T buffer((T*)data);
        buffer.setOwner(isOwner);
        return detail::wrap(Data_Type<Buffer_T>::klass(), Data_Type<Buffer_T>::ruby_data_type(), buffer, true);
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
    Return* returnInfo_ = nullptr;
  };

  template <typename T>
  class To_Ruby<T*&>
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

    explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
    {
    }

    template<typename U>
    VALUE convert(U* data)
    {
      bool isOwner = this->returnInfo_ && this->returnInfo_->isOwner();

      if (data == nullptr)
      {
        return Qnil;
      }
      else if (this->returnInfo_ && this->returnInfo_->isArray())
      {
        using Buffer_T = Buffer<intrinsic_type<T>>;
        Buffer_T buffer((T*)data);
        buffer.setOwner(isOwner);
        return detail::wrap(Data_Type<Buffer_T>::klass(), Data_Type<Buffer_T>::ruby_data_type(), buffer, true);
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
    Return* returnInfo_ = nullptr;
  };

  template <typename T>
  class To_Ruby<T**>
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

    explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
    {
    }

    template<typename U>
    VALUE convert(U** data)
    {
      if (data)
      {
        bool isOwner = this->returnInfo_ && this->returnInfo_->isOwner();
        Buffer<T*> buffer((T**)data);
        buffer.setOwner(isOwner);
        using Buffer_T = Buffer<intrinsic_type<T>*>;
        return detail::wrap(Data_Type<Buffer_T>::klass(), Data_Type<Buffer_T>::ruby_data_type(), buffer, true);
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

      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->template defaultValue<Intrinsic_T>();
      }
      else
      {
        return *detail::unwrap<Intrinsic_T>(value, Data_Type<Intrinsic_T>::ruby_data_type(), this->arg_ && this->arg_->isOwner()); 
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

      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return std::move(this->arg_->template defaultValue<Intrinsic_T>());
      }
      else
      {
        Intrinsic_T* object = detail::unwrap<Intrinsic_T>(value, Data_Type<Intrinsic_T>::ruby_data_type(), this->arg_ && this->arg_->isOwner());
        return std::move(*object);
      }
    }

  private:
    Arg* arg_ = nullptr;
  };

  // 99% of the time a T* represents a wrapped C++ object that we want to call methods on. However, T* 
  // could also be a pointer to an array of T objects, so T[]. OpenCV for example has API calls like this.
  // In that case, the Ruby VALUE will be a Buffer<T> instance
  template<typename T>
  class From_Ruby<T*>
  {
    static_assert(!std::is_fundamental_v<intrinsic_type<T>>,
                  "Data_Object cannot be used with fundamental types");

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
          if (this->arg_ && this->arg_->isArray())
          {
            return Data_Type<Buffer<T>>::is_descendant(value) ? Convertible::Exact : Convertible::None;
          }
          else
          {
            return Data_Type<T>::is_descendant(value) ? Convertible::Exact : Convertible::None;
          }
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
        case RUBY_T_NIL:
        {
          return nullptr;
          break;
        }
        case RUBY_T_DATA:
        {
          if (this->arg_ && this->arg_->isArray())
          {
            using Buffer_T = Buffer<Intrinsic_T>;
            Buffer_T* buffer = detail::unwrap<Buffer_T>(value, Data_Type<Buffer_T>::ruby_data_type(), this->arg_ && this->arg_->isOwner());
            return buffer->get();
          }
          else
          {
            return detail::unwrap<Intrinsic_T>(value, Data_Type<Intrinsic_T>::ruby_data_type(), this->arg_ && this->arg_->isOwner());
          }
        }
        default:
        {
          throw create_type_exception<Intrinsic_T>(value);
        }
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
    static_assert(!std::is_fundamental_v<intrinsic_type<T>>,
                  "Data_Object cannot be used with fundamental types");

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
          return Data_Type<Buffer<T*>>::is_descendant(value) ? Convertible::Exact : Convertible::None;
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
          Buffer<Intrinsic_T*>* buffer = detail::unwrap<Buffer<Intrinsic_T*>>(value, Data_Type<Buffer<Intrinsic_T*>>::ruby_data_type(), false);
          return buffer->get();
          break;
        }
        case RUBY_T_NIL:
        {
          return nullptr;
          break;
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
