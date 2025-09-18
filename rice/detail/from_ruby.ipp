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

    static T* convert(VALUE value, Arg* arg)
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

    static T** convert(VALUE value, Arg* arg)
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
      if (rb_type(value) == RUBY_T_DATA && Data_Type<Pointer_T>::is_descendant(value))
      {
        return Convertible::Exact;
      }
      else
      {
        return FromRubyFundamental<bool>::is_convertible(value);
      }
    }

    bool& convert(VALUE value)
    {
      if (rb_type(value) == RUBY_T_DATA && Data_Type<Pointer_T>::is_descendant(value))
      {
        return (bool&)*unwrap<Pointer_T>(value, Data_Type<Pointer_T>::ruby_data_type(), false);
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
      if (rb_type(value) == RUBY_T_DATA && Data_Type<Pointer_T>::is_descendant(value))
      {
        return Convertible::Exact;
      }
      else
      {
        return FromRubyFundamental<char>::is_convertible(value);
      }
    }

    char& convert(VALUE value)
    {
      if (rb_type(value) == RUBY_T_DATA && Data_Type<Pointer_T>::is_descendant(value))
      {
        return (char&)*unwrap<Pointer_T>(value, Data_Type<Pointer_T>::ruby_data_type(), false);
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
        return FromRubyFundamental<char*>::convert(value, this->arg_);
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
      if (rb_type(value) == RUBY_T_DATA && Data_Type<Pointer_T>::is_descendant(value))
      {
        return Convertible::Exact;
      }
      else
      {
        return FromRubyFundamental<unsigned char>::is_convertible(value);
      }
    }

    unsigned char& convert(VALUE value)
    {
      if (rb_type(value) == RUBY_T_DATA && Data_Type<Pointer_T>::is_descendant(value))
      {
        return (unsigned char&)*unwrap<Pointer_T>(value, Data_Type<Pointer_T>::ruby_data_type(), false);
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
      if (rb_type(value) == RUBY_T_DATA && Data_Type<Pointer_T>::is_descendant(value))
      {
        return Convertible::Exact;
      }
      else
      {
        return FromRubyFundamental<signed char>::is_convertible(value);
      }
    }

    signed char& convert(VALUE value)
    {
      if (rb_type(value) == RUBY_T_DATA && Data_Type<Pointer_T>::is_descendant(value))
      {
        return (signed char&)*unwrap<Pointer_T>(value, Data_Type<Pointer_T>::ruby_data_type(), false);
      }
      else
      {
        this->converted_ = FromRubyFundamental<signed char>::convert(value);
        return this->converted_;
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
      if (rb_type(value) == RUBY_T_DATA && Data_Type<Pointer_T>::is_descendant(value))
      {
        return Convertible::Exact;
      }
      else
      {
        return FromRubyFundamental<double>::is_convertible(value);
      }
    }

    double& convert(VALUE value)
    {
      if (rb_type(value) == RUBY_T_DATA && Data_Type<Pointer_T>::is_descendant(value))
      {
        return (double&)*unwrap<Pointer_T>(value, Data_Type<Pointer_T>::ruby_data_type(), false);
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
      if (rb_type(value) == RUBY_T_DATA && Data_Type<Pointer_T>::is_descendant(value))
      {
        return Convertible::Exact;
      }
      else
      {
        return FromRubyFundamental<float>::is_convertible(value);
      }
    }

    float& convert(VALUE value)
    {
      if (rb_type(value) == RUBY_T_DATA && Data_Type<Pointer_T>::is_descendant(value))
      {
        return (float&)*unwrap<Pointer_T>(value, Data_Type<Pointer_T>::ruby_data_type(), false);
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
      if (rb_type(value) == RUBY_T_DATA && Data_Type<Pointer_T>::is_descendant(value))
      {
        return Convertible::Exact;
      }
      else
      {
        return FromRubyFundamental<int>::is_convertible(value);
      }
    }

    int& convert(VALUE value)
    {
      if (rb_type(value) == RUBY_T_DATA && Data_Type<Pointer_T>::is_descendant(value))
      {
        return (int&)*unwrap<Pointer_T>(value, Data_Type<Pointer_T>::ruby_data_type(), false);
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
      if (rb_type(value) == RUBY_T_DATA && Data_Type<Pointer_T>::is_descendant(value))
      {
        return Convertible::Exact;
      }
      else
      {
        return FromRubyFundamental<unsigned int>::is_convertible(value);
      }
    }

    unsigned int& convert(VALUE value)
    {
      if (rb_type(value) == RUBY_T_DATA && Data_Type<Pointer_T>::is_descendant(value))
      {
        return (unsigned int&)*unwrap<Pointer_T>(value, Data_Type<Pointer_T>::ruby_data_type(), false);
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
      if (rb_type(value) == RUBY_T_DATA && Data_Type<Pointer_T>::is_descendant(value))
      {
        return Convertible::Exact;
      }
      else
      {
        return FromRubyFundamental<long>::is_convertible(value);
      }
    }

    long& convert(VALUE value)
    {
      if (rb_type(value) == RUBY_T_DATA && Data_Type<Pointer_T>::is_descendant(value))
      {
        return (long&)*unwrap<Pointer_T>(value, Data_Type<Pointer_T>::ruby_data_type(), false);
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
      if (rb_type(value) == RUBY_T_DATA && Data_Type<Pointer_T>::is_descendant(value))
      {
        return Convertible::Exact;
      }
      else
      {
        return FromRubyFundamental<unsigned long>::is_convertible(value);
      }
    }

    unsigned long& convert(VALUE value)
    {
      if (rb_type(value) == RUBY_T_DATA && Data_Type<Pointer_T>::is_descendant(value))
      {
        return (unsigned long&)*unwrap<Pointer_T>(value, Data_Type<Pointer_T>::ruby_data_type(), false);
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
      if (rb_type(value) == RUBY_T_DATA && Data_Type<Pointer_T>::is_descendant(value))
      {
        return Convertible::Exact;
      }
      else
      {
        return FromRubyFundamental<unsigned long long>::is_convertible(value);
      }
    }

    unsigned long long& convert(VALUE value)
    {
      if (rb_type(value) == RUBY_T_DATA && Data_Type<Pointer_T>::is_descendant(value))
      {
        return (unsigned long long&)*unwrap<Pointer_T>(value, Data_Type<Pointer_T>::ruby_data_type(), false);
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
      if (rb_type(value) == RUBY_T_DATA && Data_Type<Pointer_T>::is_descendant(value))
      {
        return Convertible::Exact;
      }
      else
      {
        return FromRubyFundamental<long long>::is_convertible(value);
      }
    }

    long long& convert(VALUE value)
    {
      if (rb_type(value) == RUBY_T_DATA && Data_Type<Pointer_T>::is_descendant(value))
      {
        return (long long&)*unwrap<Pointer_T>(value, Data_Type<Pointer_T>::ruby_data_type(), false);
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
      if (rb_type(value) == RUBY_T_DATA && Data_Type<Pointer_T>::is_descendant(value))
      {
        return Convertible::Exact;
      }
      else
      {
        return FromRubyFundamental<short>::is_convertible(value);
      }
    }

    short& convert(VALUE value)
    {
      if (rb_type(value) == RUBY_T_DATA && Data_Type<Pointer_T>::is_descendant(value))
      {
        return (short&)*unwrap<Pointer_T>(value, Data_Type<Pointer_T>::ruby_data_type(), false);
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
      if (rb_type(value) == RUBY_T_DATA && Data_Type<Pointer_T>::is_descendant(value))
      {
        return Convertible::Exact;
      }
      else
      {
        return FromRubyFundamental<unsigned short>::is_convertible(value);
      }
    }

    unsigned short& convert(VALUE value)
    {
      if (rb_type(value) == RUBY_T_DATA && Data_Type<Pointer_T>::is_descendant(value))
      {
        return (unsigned short&)*unwrap<Pointer_T>(value, Data_Type<Pointer_T>::ruby_data_type(), false);
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