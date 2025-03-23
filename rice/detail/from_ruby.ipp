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
          return Data_Type<Buffer<T>>::is_descendant(value) ? Convertible::Exact : Convertible::None;
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
          if (arg && arg->hasDefaultValue())
          {
            return arg->defaultValue<T*>();
          }
          else
          {
            return nullptr;
          }
        }
        case RUBY_T_DATA:
        {
          if (Data_Type<Buffer<T>>::is_descendant(value))
          {
            Buffer<T>* buffer = unwrap<Buffer<T>>(value, Data_Type<Buffer<T>>::ruby_data_type(), false);
            if (arg && arg->isOwner())
            {
              buffer->release();
            }
            return buffer->get();
          }
          [[fallthrough]];
        }
        default:
        {
          std::string name = typeName(typeid(Buffer<intrinsic_type<T>>));
          std::string expected = rubyClassName(name);
          throw Exception(rb_eTypeError, u8"wrong argument type %s (expected % s)",
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
          return Data_Type<Buffer<T*>>::is_descendant(value) ? Convertible::Exact : Convertible::None;
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
          if (arg && arg->hasDefaultValue())
          {
            return arg->defaultValue<T**>();
          }
          else
          {
            return nullptr;
          }
        }
        case RUBY_T_DATA:
        {
          if (Data_Type<Buffer<T*>>::is_descendant(value))
          {
            Buffer<T*>* buffer = unwrap<Buffer<T*>>(value, Data_Type<Buffer<T*>>::ruby_data_type(), false);
            if (arg && arg->isOwner())
            {
              buffer->release();
            }
            return buffer->get();
          }
          [[fallthrough]];
        }
        default:
        {
          std::string name = typeName(typeid(Buffer<intrinsic_type<T>*>));
          std::string expected = rubyClassName(name);
          throw Exception(rb_eTypeError, u8"wrong argument type %s (expected % s)",
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
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<bool>::is_convertible(value);
    }

    bool* convert(VALUE value)
    {
      return FromRubyFundamental<bool*>::convert(value, this->arg_);
    }

  private:
    Arg* arg_ = nullptr;
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
    
  template<>
  class From_Ruby<const char*> : public From_Ruby<char*>
  {
  };

  template<>
  class From_Ruby<char**>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
      if (this->arg_ && this->arg_->isOwner())
      {
        throw std::runtime_error("Cannot transfer ownership of an array of pointers to a fundamental type");
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<char**>::is_convertible(value);
    }

    char** convert(VALUE value)
    {
      return FromRubyFundamental<char**>::convert(value, this->arg_);
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
      return FromRubyFundamental<unsigned char*>::convert(value, this->arg_);
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<unsigned char**>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
      if (this->arg_ && this->arg_->isOwner())
      {
        throw std::runtime_error("Cannot transfer ownership of an array of pointers to a fundamental type");
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned char**>::is_convertible(value);
    }

    unsigned char** convert(VALUE value)
    {
      return FromRubyFundamental<unsigned char**>::convert(value, this->arg_);
    }

  private:
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
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<signed char*>::is_convertible(value);
    }

    signed char* convert(VALUE value)
    {
      return FromRubyFundamental<signed char*>::convert(value, this->arg_);
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<signed char**>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
      if (this->arg_ && this->arg_->isOwner())
      {
        throw std::runtime_error("Cannot transfer ownership of an array of pointers to a fundamental type");
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<signed char**>::is_convertible(value);
    }

    signed char** convert(VALUE value)
    {
      return FromRubyFundamental<signed char**>::convert(value, this->arg_);
    }

  private:
    Arg* arg_ = nullptr;
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

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<double*>::is_convertible(value);
    }

    double* convert(VALUE value)
    {
      return FromRubyFundamental<double*>::convert(value, this->arg_);
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<double**>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
      if (this->arg_ && this->arg_->isOwner())
      {
        throw std::runtime_error("Cannot transfer ownership of an array of pointers to a fundamental type");
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<double**>::is_convertible(value);
    }

    double** convert(VALUE value)
    {
      return FromRubyFundamental<double**>::convert(value, this->arg_);
    }

  private:
    Arg* arg_ = nullptr;
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

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<float*>::is_convertible(value);
    }

    float* convert(VALUE value)
    {
      return FromRubyFundamental<float*>::convert(value, this->arg_);
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<float**>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
      if (this->arg_ && this->arg_->isOwner())
      {
        throw std::runtime_error("Cannot transfer ownership of an array of pointers to a fundamental type");
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<float**>::is_convertible(value);
    }

    float** convert(VALUE value)
    {
      return FromRubyFundamental<float**>::convert(value, this->arg_);
    }

  private:
    Arg* arg_ = nullptr;
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
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<int*>::is_convertible(value);
    }

    int* convert(VALUE value)
    {
      return FromRubyFundamental<int*>::convert(value, this->arg_);
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<int**>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
      if (this->arg_ && this->arg_->isOwner())
      {
        throw std::runtime_error("Cannot transfer ownership of an array of pointers to a fundamental type");
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<int**>::is_convertible(value);
    }

    int** convert(VALUE value)
    {
      return FromRubyFundamental<int**>::convert(value, this->arg_);
    }

  private:
    Arg* arg_ = nullptr;
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
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned int*>::is_convertible(value);
    }

    unsigned int* convert(VALUE value)
    {
      return FromRubyFundamental<unsigned int*>::convert(value, this->arg_);
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<unsigned int**>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
      if (this->arg_ && this->arg_->isOwner())
      {
        throw std::runtime_error("Cannot transfer ownership of an array of pointers to a fundamental type");
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned int**>::is_convertible(value);
    }

    unsigned int** convert(VALUE value)
    {
      return FromRubyFundamental<unsigned int**>::convert(value, this->arg_);
    }

  private:
    Arg* arg_ = nullptr;
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
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<long*>::is_convertible(value);
    }

    long* convert(VALUE value)
    {
      return FromRubyFundamental<long*>::convert(value, this->arg_);
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<long**>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
      if (this->arg_ && this->arg_->isOwner())
      {
        throw std::runtime_error("Cannot transfer ownership of an array of pointers to a fundamental type");
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<long**>::is_convertible(value);
    }

    long** convert(VALUE value)
    {
      return FromRubyFundamental<long**>::convert(value, this->arg_);
    }

  private:
    Arg* arg_ = nullptr;
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
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned long*>::is_convertible(value);
    }

    unsigned long* convert(VALUE value)
    {
      return FromRubyFundamental<unsigned long*>::convert(value, this->arg_);
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<unsigned long**>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
      if (this->arg_ && this->arg_->isOwner())
      {
        throw std::runtime_error("Cannot transfer ownership of an array of pointers to a fundamental type");
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned long**>::is_convertible(value);
    }

    unsigned long** convert(VALUE value)
    {
      return FromRubyFundamental<unsigned long**>::convert(value, this->arg_);
    }

  private:
    Arg* arg_ = nullptr;
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
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned long long*>::is_convertible(value);
    }

    unsigned long long* convert(VALUE value)
    {
      return FromRubyFundamental<unsigned long long*>::convert(value, this->arg_);
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<unsigned long long**>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
      if (this->arg_ && this->arg_->isOwner())
      {
        throw std::runtime_error("Cannot transfer ownership of an array of pointers to a fundamental type");
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned long long**>::is_convertible(value);
    }

    unsigned long long** convert(VALUE value)
    {
      return FromRubyFundamental<unsigned long long**>::convert(value, this->arg_);
    }

  private:
    Arg* arg_ = nullptr;
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
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<long long*>::is_convertible(value);
    }

    long long* convert(VALUE value)
    {
      return FromRubyFundamental<long long*>::convert(value, this->arg_);
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<long long**>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
      if (this->arg_ && this->arg_->isOwner())
      {
        throw std::runtime_error("Cannot transfer ownership of an array of pointers to a fundamental type");
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<long long**>::is_convertible(value);
    }

    long long** convert(VALUE value)
    {
      return FromRubyFundamental<long long**>::convert(value, this->arg_);
    }

  private:
    Arg* arg_ = nullptr;
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
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<short*>::is_convertible(value);
    }

    short* convert(VALUE value)
    {
      return FromRubyFundamental<short*>::convert(value, this->arg_);
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<short**>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
      if (this->arg_ && this->arg_->isOwner())
      {
        throw std::runtime_error("Cannot transfer ownership of an array of pointers to a fundamental type");
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<short**>::is_convertible(value);
    }

    short** convert(VALUE value)
    {
      return FromRubyFundamental<short**>::convert(value, this->arg_);
    }
  private:
    Arg* arg_ = nullptr;
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
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned short*>::is_convertible(value);
    }

    unsigned short* convert(VALUE value)
    {
      return FromRubyFundamental<unsigned short*>::convert(value, this->arg_);
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<unsigned short**>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
      if (this->arg_ && this->arg_->isOwner())
      {
        throw std::runtime_error("Cannot transfer ownership of an array of pointers to a fundamental type");
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned short**>::is_convertible(value);
    }

    unsigned short** convert(VALUE value)
    {
      return FromRubyFundamental<unsigned short**>::convert(value, this->arg_);
    }

  private:
    Arg* arg_ = nullptr;
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
      detail::Type<Buffer<void>>::verify();
    };

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
      detail::Type<Buffer<void>>::verify();

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
          return Convertible::Exact;
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

          // Is this a Buffer? It could also be a pointer to any other object being passed to 
          // a C++ paramter that takes void*
          if (rb_type == Data_Type<Buffer<void>>::ruby_data_type())
          {
            Data_Object<Buffer<void>> buffer(value);
            return buffer->get();
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