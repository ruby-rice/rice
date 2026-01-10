#include <limits>
#include <optional>
#include <stdexcept>

/* This file implements conversions from Ruby to native values fo fundamental types
   such as bool, int, float, etc. It also includes conversions for chars and strings */
namespace Rice::detail
{
  // Get precision bits for a Ruby numeric value
  inline int rubyPrecisionBits(VALUE value)
  {
    switch (rb_type(value))
    {
      // Ruby fixnums fit in long long (63 bits)
      case RUBY_T_FIXNUM:
        return std::numeric_limits<long long>::digits;

      // Ruby bignums can be arbitrarily large - return actual size
      case RUBY_T_BIGNUM:
      {
        int nlz = 0;
        size_t bytes = protect(rb_absint_size, value, &nlz);
        return static_cast<int>(bytes * CHAR_BIT - nlz);
      }

      // Ruby floats are C doubles (53 bit mantissa)
      case RUBY_T_FLOAT:
        return std::numeric_limits<double>::digits;
      
      // Everything else...
      default:
        return 0;
    }
  }

  // Precision score for converting Ruby numeric value to C++ type T
  template<typename T>
  inline double precisionScore(VALUE value)
  {
    int sourceBits = rubyPrecisionBits(value);
    if (sourceBits == 0)
    {
      return Convertible::None;
    }

    constexpr int targetBits = std::numeric_limits<T>::digits;
    return (targetBits >= sourceBits) ? Convertible::Exact : static_cast<double>(targetBits) / sourceBits;
  }

  // Primary template for integral types
  template<typename T, typename Enable = void>
  class FromRubyFundamental
  {
  public:
    static double is_convertible(VALUE value)
    {
      double score = precisionScore<T>(value);
      if (score > Convertible::None)
      {
        switch (rb_type(value))
        {
          case RUBY_T_BIGNUM:
          {
            constexpr int targetBits = std::numeric_limits<T>::digits;
            int sourceBits = rubyPrecisionBits(value);
            if (sourceBits > targetBits)
            {
              return Convertible::None;
            }
            [[fallthrough]];
          }
          case RUBY_T_FIXNUM:
          {
            if constexpr (std::is_unsigned_v<T>)
            {
              score *= Convertible::SignedToUnsigned;
            }
            break;
          }
          case RUBY_T_FLOAT:
          {
            score *= Convertible::FloatToInt;
            break;
          }
          default:
            break;
        }

        return score;
      }

      if constexpr (is_char_type_v<T>)
      {
        if (rb_type(value) == RUBY_T_STRING)
        {
          return Convertible::Exact;
        }
      }

      return Convertible::None;
    }

    static T convert(VALUE value)
    {
      return (T)protect(RubyType<T>::fromRuby, value);
    }
  };

  // Specialization for floating point types
  template<typename T>
  class FromRubyFundamental<T, std::enable_if_t<std::is_floating_point_v<T>>>
  {
  public:
    static double is_convertible(VALUE value)
    {
      double score = precisionScore<T>(value);
      if (score > Convertible::None && rb_type(value) != RUBY_T_FLOAT)
      {
        score *= Convertible::IntToFloat;
      }
      return score;
    }

    static T convert(VALUE value)
    {
      return (T)protect(RubyType<T>::fromRuby, value);
    }
  };

  template<typename T>
  class FromRubyFundamental<T*>
  {
  public:
    static double is_convertible(VALUE value)
    {
      ruby_value_type valueType = rb_type(value);

      switch (valueType)
      {
        case RUBY_T_NIL:
        {
          return Convertible::Exact;
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

    static T* convert(VALUE value)
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
          detail::TypeDetail<Pointer<T>> typeDetail;
          std::string expected = typeDetail.rubyName();
          throw Exception(rb_eTypeError, "wrong argument type %s (expected %s)",
            detail::protect(rb_obj_classname, value), expected.c_str());
        }
      }
    }
  };

  template<typename T>
  class FromRubyFundamental<T**>
  {
  public:
    static double is_convertible(VALUE value)
    {
      ruby_value_type valueType = rb_type(value);

      switch (valueType)
      {
        case RUBY_T_NIL:
        {
          return Convertible::Exact;
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

    static T** convert(VALUE value)
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
          detail::TypeDetail<Pointer<T*>> typeDetail;
          std::string expected = typeDetail.rubyName();
          throw Exception(rb_eTypeError, "wrong argument type %s (expected %s)",
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

    double is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_TRUE:
        case RUBY_T_FALSE:
        case RUBY_T_NIL:
          return Convertible::Exact;
        default:
          return Convertible::None;
      }
    }

    bool convert(VALUE value)
    {
      return protect(RubyType<bool>::fromRuby, value);
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<bool&>
  {
  public:
    using Reference_T = Reference<bool>;

    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    double is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Reference_T>::is_descendant(value))
          {
            return Convertible::Exact;
          }
          [[fallthrough]];
        }
        default:
        {
          return this->from_.is_convertible(value);
        }
      }
    }

    bool& convert(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Reference_T>::is_descendant(value))
          {
            Reference_T* reference = unwrap<Reference_T>(value, Data_Type<Reference_T>::ruby_data_type(), false);
            return reference->get();
          }
          [[fallthrough]];
        }
        default:
        {
          this->reference_ = Reference<bool>(value);
          return this->reference_.get();
        }
      }
    }

  private:
    From_Ruby<bool> from_;
    Arg* arg_ = nullptr;
    Reference<bool> reference_;
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

    double is_convertible(VALUE value)
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
    using Reference_T = Reference<char>;

    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    double is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Reference_T>::is_descendant(value))
          {
            return Convertible::Exact;
          }
          [[fallthrough]];
        }
        default:
        {
          return FromRubyFundamental<char>::is_convertible(value);
        }
      }
    }

    char& convert(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Reference_T>::is_descendant(value))
          {
            Reference_T* reference = unwrap<Reference_T>(value, Data_Type<Reference_T>::ruby_data_type(), false);
            return reference->get();
          }
          [[fallthrough]];
        }
        default:
        {
          this->reference_ = Reference<char>(value);
          return this->reference_.get();
        }
      }
    }

  private:
    Arg* arg_ = nullptr;
    Reference<char> reference_;
  };

  template<>
  class From_Ruby<char*>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    double is_convertible(VALUE value)
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
          return FromRubyFundamental<char*>::convert(value);
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

    double is_convertible(VALUE value)
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
    using Reference_T = Reference<unsigned char>;

    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    double is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Reference_T>::is_descendant(value))
          {
            return Convertible::Exact;
          }
          [[fallthrough]];
        }
        default:
        {
          return FromRubyFundamental<unsigned char>::is_convertible(value);
        }
      }
    }

    unsigned char& convert(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Reference_T>::is_descendant(value))
          {
            Reference_T* reference = unwrap<Reference_T>(value, Data_Type<Reference_T>::ruby_data_type(), false);
            return reference->get();
          }
          [[fallthrough]];
        }
        default:
        {
          this->reference_ = Reference<unsigned char>(value);
          return this->reference_.get();
        }
      }
    }

  private:
    Arg* arg_ = nullptr;
    Reference<unsigned char> reference_;
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

    double is_convertible(VALUE value)
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
    using Reference_T = Reference<signed char>;

    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    double is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Reference_T>::is_descendant(value))
          {
            return Convertible::Exact;
          }
          [[fallthrough]];
        }
        default:
        {
          return FromRubyFundamental<signed char>::is_convertible(value);
        }
      }
    }

    signed char& convert(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Reference_T>::is_descendant(value))
          {
            Reference_T* reference = unwrap<Reference_T>(value, Data_Type<Reference_T>::ruby_data_type(), false);
            return reference->get();
          }
          [[fallthrough]];
        }
        default:
        {
          this->reference_ = Reference<signed char>(value);
          return this->reference_.get();
        }
      }
    }

  private:
    Arg* arg_ = nullptr;
    Reference<signed char> reference_;
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

    double is_convertible(VALUE value)
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
    using Reference_T = Reference<double>;

    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    double is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Reference_T>::is_descendant(value))
          {
            return Convertible::Exact;
          }
          [[fallthrough]];
        }
        default:
        {
          return FromRubyFundamental<double>::is_convertible(value);
        }
      }
    }

    double& convert(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Reference_T>::is_descendant(value))
          {
            Reference_T* reference = unwrap<Reference_T>(value, Data_Type<Reference_T>::ruby_data_type(), false);
            return reference->get();
          }
          [[fallthrough]];
        }
        default:
        {
          this->reference_ = Reference<double>(value);
          return this->reference_.get();
        }
      }
    }

  private:
    Arg* arg_ = nullptr;
    Reference<double> reference_;
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

    double is_convertible(VALUE value)
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
    using Reference_T = Reference<float>;

    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    double is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Reference_T>::is_descendant(value))
          {
            return Convertible::Exact;
          }
          [[fallthrough]];
        }
        default:
        {
          return FromRubyFundamental<float>::is_convertible(value);
        }
      }
    }

    float& convert(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Reference_T>::is_descendant(value))
          {
            Reference_T* reference = unwrap<Reference_T>(value, Data_Type<Reference_T>::ruby_data_type(), false);
            return reference->get();
          }
          [[fallthrough]];
        }
        default:
        {
          this->reference_ = Reference<float>(value);
          return this->reference_.get();
        }
      }
    }

  private:
    Arg* arg_ = nullptr;
    Reference<float> reference_;
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

    double is_convertible(VALUE value)
    {
      double result = FromRubyFundamental<int>::is_convertible(value);

      // Is this an enum? If so we want to support converting it to an integer
      if (result == Convertible::None && rb_type(value) == RUBY_T_DATA)
      {
        static ID id = protect(rb_intern, "to_int");
        if (protect(rb_respond_to, value, id))
        {
          result = Convertible::Exact;
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
    using Reference_T = Reference<int>;

    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    double is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Reference_T>::is_descendant(value))
          {
            return Convertible::Exact;
          }
          [[fallthrough]];
        }
        default:
        {
          return FromRubyFundamental<int>::is_convertible(value);
        }
      }
    }

    int& convert(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Reference_T>::is_descendant(value))
          {
            Reference_T* reference = unwrap<Reference_T>(value, Data_Type<Reference_T>::ruby_data_type(), false);
            return reference->get();
          }
          [[fallthrough]];
        }
        default:
        {
          this->reference_ = Reference<int>(value);
          return this->reference_.get();
        }
      }
    }

  private:
    Arg* arg_ = nullptr;
    Reference<int> reference_;
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

    double is_convertible(VALUE value)
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
    using Reference_T = Reference<unsigned int>;

    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    double is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Reference_T>::is_descendant(value))
          {
            return Convertible::Exact;
          }
          [[fallthrough]];
        }
        default:
        {
          return FromRubyFundamental<unsigned int>::is_convertible(value);
        }
      }
    }

    unsigned int& convert(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Reference_T>::is_descendant(value))
          {
            Reference_T* reference = unwrap<Reference_T>(value, Data_Type<Reference_T>::ruby_data_type(), false);
            return reference->get();
          }
          [[fallthrough]];
        }
        default:
        {
          this->reference_ = Reference<unsigned int>(value);
          return this->reference_.get();
        }
      }
    }

  private:
    Arg* arg_ = nullptr;
    Reference<unsigned int> reference_;
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

    double is_convertible(VALUE value)
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
    using Reference_T = Reference<long>;

    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    double is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Reference_T>::is_descendant(value))
          {
            return Convertible::Exact;
          }
          [[fallthrough]];
        }
        default:
        {
          return FromRubyFundamental<long>::is_convertible(value);
        }
      }
    }

    long& convert(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Reference_T>::is_descendant(value))
          {
            Reference_T* reference = unwrap<Reference_T>(value, Data_Type<Reference_T>::ruby_data_type(), false);
            return reference->get();
          }
          [[fallthrough]];
        }
        default:
        {
          this->reference_ = Reference<long>(value);
          return this->reference_.get();
        }
      }
    }

  private:
    Arg* arg_ = nullptr;
    Reference<long> reference_;
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

    double is_convertible(VALUE value)
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
    using Reference_T = Reference<unsigned long>;

    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    double is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Reference_T>::is_descendant(value))
          {
            return Convertible::Exact;
          }
          [[fallthrough]];
        }
        default:
        {
          return FromRubyFundamental<unsigned long>::is_convertible(value);
        }
      }
    }

    unsigned long& convert(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Reference_T>::is_descendant(value))
          {
            Reference_T* reference = unwrap<Reference_T>(value, Data_Type<Reference_T>::ruby_data_type(), false);
            return reference->get();
          }
          [[fallthrough]];
        }
        default:
        {
          this->reference_ = Reference<unsigned long>(value);
          return this->reference_.get();
        }
      }
    }

  private:
    Arg* arg_ = nullptr;
    Reference<unsigned long> reference_;
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

    double is_convertible(VALUE value)
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
    using Reference_T = Reference<unsigned long long>;

    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    double is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Reference_T>::is_descendant(value))
          {
            return Convertible::Exact;
          }
          [[fallthrough]];
        }
        default:
        {
          return FromRubyFundamental<unsigned long long>::is_convertible(value);
        }
      }
    }

    unsigned long long& convert(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Reference_T>::is_descendant(value))
          {
            Reference_T* reference = unwrap<Reference_T>(value, Data_Type<Reference_T>::ruby_data_type(), false);
            return reference->get();
          }
          [[fallthrough]];
        }
        default:
        {
          this->converted_ = FromRubyFundamental<unsigned long long>::convert(value);
          return this->converted_;
        }
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

    double is_convertible(VALUE value)
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
    using Reference_T = Reference<long long>;

    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    double is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Reference_T>::is_descendant(value))
          {
            return Convertible::Exact;
          }
          [[fallthrough]];
        }
        default:
        {
          return FromRubyFundamental<long long>::is_convertible(value);
        }
      }
    }

    long long& convert(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Reference_T>::is_descendant(value))
          {
            Reference_T* reference = unwrap<Reference_T>(value, Data_Type<Reference_T>::ruby_data_type(), false);
            return reference->get();
          }
          [[fallthrough]];
        }
        default:
        {
          this->reference_ = Reference<long long>(value);
          return this->reference_.get();
        }
      }
    }

  private:
    Arg* arg_ = nullptr;
    Reference<long long> reference_;
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

    double is_convertible(VALUE value)
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
    using Reference_T = Reference<short>;

    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    double is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Reference_T>::is_descendant(value))
          {
            return Convertible::Exact;
          }
          [[fallthrough]];
        }
        default:
        {
          return FromRubyFundamental<short>::is_convertible(value);
        }
      }
    }

    short& convert(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Reference_T>::is_descendant(value))
          {
            Reference_T* reference = unwrap<Reference_T>(value, Data_Type<Reference_T>::ruby_data_type(), false);
            return reference->get();
          }
          [[fallthrough]];
        }
        default:
        {
          this->reference_ = Reference<short>(value);
          return this->reference_.get();
        }
      }
    }

  private:
    Arg* arg_ = nullptr;
    Reference<short> reference_;
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

    double is_convertible(VALUE value)
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
    using Reference_T = Reference<unsigned short>;

    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    double is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Reference_T>::is_descendant(value))
          {
            return Convertible::Exact;
          }
          [[fallthrough]];
        }
        default:
        {
          return FromRubyFundamental<unsigned short>::is_convertible(value);
        }
      }
    }

    unsigned short& convert(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
        {
          if (Data_Type<Reference_T>::is_descendant(value))
          {
            Reference_T* reference = unwrap<Reference_T>(value, Data_Type<Reference_T>::ruby_data_type(), false);
            return reference->get();
          }
          [[fallthrough]];
        }
        default:
        {
          this->reference_ = Reference<unsigned short>(value);
          return this->reference_.get();
        }
      }
    }

  private:
    Arg* arg_ = nullptr;
    Reference<unsigned short> reference_;
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

    double is_convertible(VALUE value)
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
          throw Exception(rb_eTypeError, "wrong argument type %s (expected %s)",
            detail::protect(rb_obj_classname, value), "nil");
        }
      }
    }
  private:
    Arg* arg_ = nullptr;
  };

  // ===========  void  ============
  template<>
  class From_Ruby<void>
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

    double is_convertible(VALUE value)
    {
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

    void convert(VALUE)
    {
      // Nothing to do
    }
  private:
    Arg* arg_ = nullptr;
  };

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

    double is_convertible(VALUE value)
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
        }
        case RUBY_T_STRING:
        {
          if (RB_ENCODING_IS_ASCII8BIT(value))
          {
            return Convertible::Exact;
          }
          else
          {
            return Convertible::None;
          }
        }
        case RUBY_T_NIL:
        {
          return Convertible::Exact;
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
          throw Exception(rb_eTypeError, "wrong argument type %s (expected %s)",
            detail::protect(rb_obj_classname, value), "pointer");
        }
      }
    }
  private:
    Arg* arg_ = nullptr;
  };
}