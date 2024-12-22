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

      if (valueType == RubyType_T::valueType)
      {
        return Convertible::Exact;
      }
      else if (RubyType_T::castableTypes.find(valueType) != RubyType_T::castableTypes.end())
      {
        return Convertible::TypeCast;
      }
      else
      {
        return Convertible::None;
      }
    }

    static T convert(VALUE value)
    {
      return protect(RubyType_T::fromRuby, value);
    }
  };

  template<typename T>
  class FromRubyFundamental<T*>
  {
  public:
    using RubyType_T = RubyType<T>;

    static Convertible is_convertible(VALUE value)
    {
      ruby_value_type valueType = rb_type(value);

      if (valueType == RubyType_T::valueType)
      {
        return Convertible::Exact;
      }
      else if (RubyType_T::castableTypes.find(valueType) != RubyType_T::castableTypes.end())
      {
        return Convertible::TypeCast;
      }
      else if (valueType == RUBY_T_ARRAY)
      {
        return Convertible::Exact;
      }
      else if (valueType == RUBY_T_STRING)
      {
        return Convertible::Exact;
      }
      else
      {
        return Convertible::None;
      }
    }

    static std::unique_ptr<T[]> convert(VALUE value)
    {
      ruby_value_type valueType = rb_type(value);
      switch (valueType)
      {
        case RUBY_T_NIL:
        {
          return nullptr;
          break;
        }
        case RUBY_T_ARRAY:
        {
          return Array(value).pack<T>();
          break;
        }
        case RUBY_T_STRING:
        {
          long size = RSTRING_LEN(value);
          // Put in null character
          std::unique_ptr<T[]> dest = std::make_unique<T[]>(size + 1);
          dest.get()[size] = 0;
          memcpy(dest.get(), RSTRING_PTR(value), size);

          return std::move(dest);
          break;
        }
        case RubyType_T::valueType:
        {
          std::unique_ptr<T[]> dest = std::make_unique<T[]>(1);
          *(dest.get()) = protect(RubyType_T::fromRuby, value);
          return std::move(dest);
        }
        default:
        {
          std::string typeName = detail::typeName(typeid(T));
          throw Exception(rb_eTypeError, "wrong argument type %s (expected % s*)",
            detail::protect(rb_obj_classname, value), typeName.c_str());
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
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<bool>();
      }
      else
      {
        return FromRubyFundamental<bool>::convert(value);
      }
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<bool&>
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

    bool& convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<bool>();
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

  template<>
  class From_Ruby<bool*>
  {
  public:
    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<bool>::is_convertible(value);
    }

    bool* convert(VALUE value)
    {
      this->converted_ = FromRubyFundamental<bool*>::convert(value);
      return this->converted_.get();
    }

  private:
    std::unique_ptr<bool[]> converted_;
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
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<char>();
      }
      else
      {
        return FromRubyFundamental<char>::convert(value);
      }
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<char&>
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

    char& convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<char>();
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
    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<char*>::is_convertible(value);
    }

    char* convert(VALUE value)
    {
      this->converted_ = FromRubyFundamental<char*>::convert(value);
      return this->converted_.get();
    }

  private:
    std::unique_ptr<char[]> converted_;
  };
    
  template<>
  class From_Ruby<const char*>
  {
  public:
    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<char*>::is_convertible(value);
    }

    char* convert(VALUE value)
    {
      this->converted_ = FromRubyFundamental<char*>::convert(value);
      return this->converted_.get();
    }

  private:
    std::unique_ptr<char[]> converted_;
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
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<unsigned char>();
      }
      else
      {
        return FromRubyFundamental<unsigned char>::convert(value);
      }
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<unsigned char&>
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

    unsigned char& convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<unsigned char>();
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

  template<>
  class From_Ruby<unsigned char*>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned char*>::is_convertible(value);
    }

    unsigned char* convert(VALUE value)
    {
      this->converted_ = FromRubyFundamental<unsigned char*>::convert(value);
      return this->converted_.get();
    }

  private:
    std::unique_ptr<unsigned char[]> converted_;
    Arg* arg_ = nullptr;
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
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<signed char>();
      }
      else
      {
        return FromRubyFundamental<signed char>::convert(value);
      }
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<signed char*>
  {
  public:
    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<signed char*>::is_convertible(value);
    }

    signed char* convert(VALUE value)
    {
      this->converted_ = FromRubyFundamental<signed char*>::convert(value);
      return this->converted_.get();
    }

  private:
    std::unique_ptr<signed char[]> converted_;
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
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<double>();
      }
      else
      {
        return FromRubyFundamental<double>::convert(value);
      }
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<double&>
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

    double& convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<double>();
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

  template<>
  class From_Ruby<double*>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    ~From_Ruby()
    {
      if (this->arg_ && this->arg_->isTransfer())
      {
        this->converted_.release();
      }
    }

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<double*>::is_convertible(value);
    }

    double* convert(VALUE value)
    {
      this->converted_ = FromRubyFundamental<double*>::convert(value);
      return this->converted_.get();
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<double[]> converted_;
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
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<float>();
      }
      else
      {
        return FromRubyFundamental<float>::convert(value);
      }
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<float&>
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

    float& convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<float>();
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

  template<>
  class From_Ruby<float*>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    ~From_Ruby()
    {
      if (this->arg_ && this->arg_->isTransfer())
      {
        this->converted_.release();
      }
    }

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<float*>::is_convertible(value);
    }

    float* convert(VALUE value)
    {
      this->converted_ = FromRubyFundamental<float*>::convert(value);
      return this->converted_.get();
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<float[]> converted_;
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
      return FromRubyFundamental<int>::is_convertible(value);

/*        // This case is for Enums which are defined as Ruby classes. Some C++ apis
        // will take a int parameter but really what we have is an Enum
        case RUBY_T_DATA:
        {
          static ID id = protect(rb_intern, "to_int"); 
          if (protect(rb_respond_to, value, id))
          {
            return Convertible::TypeCast;
          }
          else
          {
            return Convertible::None;
          }*/
    }

    int convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<int>();
      }
      else
      {
        return FromRubyFundamental<int>::convert(value);
      }
    }
  
  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<int&>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<int>::is_convertible(value);
    }

    int& convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<int>();
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

  template<>
  class From_Ruby<int*>
  {
  public:
    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<int*>::is_convertible(value);
    }

    int* convert(VALUE value)
    {
      this->converted_ = FromRubyFundamental<int*>::convert(value);
      return this->converted_.get();
    }

  private:
    std::unique_ptr<int[]> converted_;
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
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<unsigned int>();
      }
      else
      {
        return FromRubyFundamental<unsigned int>::convert(value);
      }
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<unsigned int&>
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

    unsigned int& convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<unsigned int>();
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

  template<>
  class From_Ruby<unsigned int*>
  {
  public:
    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned int*>::is_convertible(value);
    }

    unsigned int* convert(VALUE value)
    {
      this->converted_ = FromRubyFundamental<unsigned int*>::convert(value);
      return this->converted_.get();
    }

  private:
    std::unique_ptr<unsigned int[]> converted_;;
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
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<long>();
      }
      else
      {
        return FromRubyFundamental<long>::convert(value);
      }
    }
  
  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<long&>
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

    long& convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<long>();
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

  template<>
  class From_Ruby<long*>
  {
  public:
    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<long*>::is_convertible(value);
    }

    long* convert(VALUE value)
    {
      this->converted_ = FromRubyFundamental<long*>::convert(value);
      return this->converted_.get();
    }

  private:
    std::unique_ptr<long[]> converted_;
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
      else if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<unsigned long>();
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
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned long>::is_convertible(value);
    }

    unsigned long& convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<unsigned long>();
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

  template<>
  class From_Ruby<unsigned long*>
  {
  public:
    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned long*>::is_convertible(value);
    }

    unsigned long* convert(VALUE value)
    {
      this->converted_ = FromRubyFundamental<unsigned long*>::convert(value);
      return this->converted_.get();
    }

  private:
    std::unique_ptr<unsigned long[]> converted_;
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
      else if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<unsigned long long>();
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
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned long long>::is_convertible(value);
    }

    unsigned long long& convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<unsigned long long>();
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

  template<>
  class From_Ruby<unsigned long long*>
  {
  public:
    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned long long*>::is_convertible(value);
    }

    unsigned long long* convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }
      else
      {
        this->converted_ = FromRubyFundamental<unsigned long long*>::convert(value);
        return this->converted_.get();
      }
    }

  private:
    std::unique_ptr<unsigned long long[]> converted_;
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
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<long long>();
      }
      else
      {
        return FromRubyFundamental<long long>::convert(value);
      }
    }
  
  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<long long&>
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

    long long& convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<long long>();
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

  template<>
  class From_Ruby<long long*>
  {
  public:
    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<long long*>::is_convertible(value);
    }

    long long* convert(VALUE value)
    {
      this->converted_ = FromRubyFundamental<long long*>::convert(value);
      return this->converted_.get();
    }

  private:
    std::unique_ptr<long long[]> converted_;
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
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<short>();
      }
      else
      {
        return FromRubyFundamental<short>::convert(value);
      }
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<short&>
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

    short& convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<short>();
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

  template<>
  class From_Ruby<short*>
  {
  public:
    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<short*>::is_convertible(value);
    }

    short* convert(VALUE value)
    {
      this->converted_ = FromRubyFundamental<short*>::convert(value);
      return this->converted_.get();
    }

  private:
    std::unique_ptr<short[]> converted_;
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
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<unsigned short>();
      }
      else
      {
        return FromRubyFundamental<unsigned short>::convert(value);
      }
    }
  
  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<unsigned short&>
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

    unsigned short& convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<unsigned short>();
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

  template<>
  class From_Ruby<unsigned short*>
  {
  public:
    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned short*>::is_convertible(value);
    }

    unsigned short* convert(VALUE value)
    {
      this->converted_ = FromRubyFundamental<unsigned short*>::convert(value);
      return this->converted_.get();
    }

  private:
    std::unique_ptr<unsigned short[]> converted_;
  };
}