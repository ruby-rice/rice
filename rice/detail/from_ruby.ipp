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
    using RubyType_T = RubyType<T>;

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
        case RUBY_T_ARRAY:
        {
          return Convertible::Exact;
          break;
        }
        case RUBY_T_STRING:
        {
          return Convertible::Exact;
          break;
        }
        default:
        {
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

          return Convertible::None;
        }
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
          Array array(value);
          return array.pack<T>();
          break;
        }
        case RUBY_T_STRING:
        {
          long size = RSTRING_LEN(value);
          // Add null character to end
          std::unique_ptr<T[]> dest = std::make_unique<T[]>(size + 1);
          dest.get()[size] = 0;
          memcpy(dest.get(), RSTRING_PTR(value), size);

          return std::move(dest);
          break;
        }
        default:
        {
          if (RubyType_T::Exact.find(valueType) != RubyType_T::Exact.end() ||
              RubyType_T::Castable.find(valueType) != RubyType_T::Castable.end() ||
              RubyType_T::Narrowable.find(valueType) != RubyType_T::Narrowable.end())
          {
            T data = protect(RubyType_T::fromRuby, value);
            std::unique_ptr<T[]> dest = std::make_unique<T[]>(1);
            dest[0] = *(new T(data));
            return std::move(dest);
          }
          else
          {
            std::string typeName = detail::typeName(typeid(T));
            throw Exception(rb_eTypeError, "wrong argument type %s (expected % s*)",
              detail::protect(rb_obj_classname, value), typeName.c_str());
          }
        }
      }
    }
  };

  template<typename T>
  class FromRubyFundamental<T**>
  {
  public:
    using RubyType_T = RubyType<T>;

    static Convertible is_convertible(VALUE value)
    {
      ruby_value_type valueType = rb_type(value);
      
      if (valueType == RUBY_T_ARRAY)
      {
        std::vector<detail::Convertible> convertibles;
        Array array(value);
        std::transform(array.begin(), array.end(), std::back_inserter(convertibles),
          [](Object item)
          {
            return FromRubyFundamental<T*>::is_convertible(item.value());
          });

        auto iter = std::min_element(convertibles.begin(), convertibles.end());

        if (iter == convertibles.end())
        {
          return Convertible::None;
        }
        else
        {
          return *iter;
        }
      }
      else
      {
        return Convertible::None;
      }
    }

    static std::tuple<std::unique_ptr<T*[]>, std::vector<std::unique_ptr<T[]>>> convert(VALUE value)
    {
      std::unique_ptr<T*[]> outer;
      std::vector<std::unique_ptr<T[]>> inner;
      
      ruby_value_type valueType = rb_type(value);
      switch (valueType)
      {
        case RUBY_T_NIL:
        {
          return { std::move(outer), std::move(inner) };
          break;
        }
        case RUBY_T_ARRAY:
        {
          Array array(value);
          outer = std::make_unique<T*[]>(array.size());

          for (int i = 0; i < array.size(); i++)
          {
            std::unique_ptr<T[]> item = FromRubyFundamental<T*>::convert(array[i].value());
            inner.push_back(std::move(item));
            outer[i] = inner[i].get();
          }

          return { std::move(outer), std::move(inner) };
          break;
        }
        default:
        {
          std::string typeName = detail::typeName(typeid(T));
          throw Exception(rb_eTypeError, "wrong argument type %s - must be an Array of Arrays (expected % s*)",
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
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<bool>::is_convertible(value);
    }

    bool* convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }

      this->converted_ = FromRubyFundamental<bool*>::convert(value);

      if (this->arg_ && this->arg_->isOwner())
      {
        bool* result = this->converted_.get();
        this->converted_.release();
        return result;
      }
      else
      {
        return this->converted_.get();
      }
    }

  private:
    Arg* arg_ = nullptr;
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
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<char*>::is_convertible(value);
    }

    char* convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }

      this->converted_ = FromRubyFundamental<char*>::convert(value);

      if (this->arg_ && this->arg_->isOwner())
      {
        char* result = this->converted_.get();
        this->converted_.release();
        return result;
      }
      else
      {
        return this->converted_.get();
      }
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<char[]> converted_;
  };
    
  template<>
  class From_Ruby<const char*>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<char*>::is_convertible(value);
    }

    char* convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }

      this->converted_ = FromRubyFundamental<char*>::convert(value);

      if (this->arg_ && this->arg_->isOwner())
      {
        char* result = this->converted_.get();
        this->converted_.release();
        return result;
      }
      else
      {
        return this->converted_.get();
      }
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<char[]> converted_;
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

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    ~From_Ruby()
    {
      if (this->arg_ && this->arg_->isOwner())
      {
        for (std::unique_ptr<char[]>& ptr : this->inner_)
        {
          ptr.release();
        }
        this->outer_.release();
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<char**>::is_convertible(value);
    }

    char** convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }

      auto arrays = FromRubyFundamental<char**>::convert(value);
      this->outer_ = std::move(std::get<0>(arrays));
      this->inner_ = std::move(std::get<1>(arrays));
      return this->outer_.get();
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<char* []> outer_;
    std::vector<std::unique_ptr<char[]>> inner_;
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

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned char*>::is_convertible(value);
    }

    unsigned char* convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }

      this->converted_ = FromRubyFundamental<unsigned char*>::convert(value);

      if (this->arg_ && this->arg_->isOwner())
      {
        unsigned char* result = this->converted_.get();
        this->converted_.release();
        return result;
      }
      else
      {
        return this->converted_.get();
      }
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<unsigned char[]> converted_;
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

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    ~From_Ruby()
    {
      if (this->arg_ && this->arg_->isOwner())
      {
        for (std::unique_ptr<unsigned char[]>& ptr : this->inner_)
        {
          ptr.release();
        }
        this->outer_.release();
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned char**>::is_convertible(value);
    }

    unsigned char** convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }

      auto arrays = FromRubyFundamental<unsigned char**>::convert(value);
      this->outer_ = std::move(std::get<0>(arrays));
      this->inner_ = std::move(std::get<1>(arrays));
      return this->outer_.get();
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<unsigned char* []> outer_;
    std::vector<std::unique_ptr<unsigned char[]>> inner_;
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

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<signed char*>::is_convertible(value);
    }

    signed char* convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }

      this->converted_ = FromRubyFundamental<signed char*>::convert(value);

      if (this->arg_ && this->arg_->isOwner())
      {
        signed char* result = this->converted_.get();
        this->converted_.release();
        return result;
      }
      else
      {
        return this->converted_.get();
      }
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<signed char[]> converted_;
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

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    ~From_Ruby()
    {
      if (this->arg_ && this->arg_->isOwner())
      {
        for (std::unique_ptr<signed char[]>& ptr : this->inner_)
        {
          ptr.release();
        }
        this->outer_.release();
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<signed char**>::is_convertible(value);
    }

    signed char** convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }

      auto arrays = FromRubyFundamental<signed char**>::convert(value);
      this->outer_ = std::move(std::get<0>(arrays));
      this->inner_ = std::move(std::get<1>(arrays));
      return this->outer_.get();
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<signed char* []> outer_;
    std::vector<std::unique_ptr<signed char[]>> inner_;
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

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<double*>::is_convertible(value);
    }

    double* convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }

      this->converted_ = FromRubyFundamental<double*>::convert(value);

      if (this->arg_ && this->arg_->isOwner())
      {
        double* result = this->converted_.get();
        this->converted_.release();
        return result;
      }
      else
      {
        return this->converted_.get();
      }
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<double[]> converted_;
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

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    ~From_Ruby()
    {
      if (this->arg_ && this->arg_->isOwner())
      {
        for (std::unique_ptr<double[]>& ptr : this->inner_)
        {
          ptr.release();
        }
        this->outer_.release();
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<double**>::is_convertible(value);
    }

    double** convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }

      auto arrays = FromRubyFundamental<double**>::convert(value);
      this->outer_ = std::move(std::get<0>(arrays));
      this->inner_ = std::move(std::get<1>(arrays));
      return this->outer_.get();
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<double* []> outer_;
    std::vector<std::unique_ptr<double[]>> inner_;
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

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<float*>::is_convertible(value);
    }

    float* convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }

      this->converted_ = FromRubyFundamental<float*>::convert(value);

      if (this->arg_ && this->arg_->isOwner())
      {
        float* result = this->converted_.get();
        this->converted_.release();
        return result;
      }
      else
      {
        return this->converted_.get();
      }
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<float[]> converted_;
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

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    ~From_Ruby()
    {
      if (this->arg_ && this->arg_->isOwner())
      {
        for (std::unique_ptr<float[]>& ptr : this->inner_)
        {
          ptr.release();
        }
        this->outer_.release();
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<float**>::is_convertible(value);
    }

    float** convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }

      auto arrays = FromRubyFundamental<float**>::convert(value);
      this->outer_ = std::move(std::get<0>(arrays));
      this->inner_ = std::move(std::get<1>(arrays));
      return this->outer_.get();
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<float*[]> outer_;
    std::vector<std::unique_ptr<float[]>> inner_;
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

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<int*>::is_convertible(value);
    }

    int* convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }

      this->converted_ = FromRubyFundamental<int*>::convert(value);

      if (this->arg_ && this->arg_->isOwner())
      {
        int* result = this->converted_.get();
        this->converted_.release();
        return result;
      }
      else
      {
        return this->converted_.get();
      }
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<int[]> converted_;
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

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<int**>::is_convertible(value);
    }

    int** convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }

      auto arrays = FromRubyFundamental<int**>::convert(value);
      this->outer_ = std::move(std::get<0>(arrays));
      this->inner_ = std::move(std::get<1>(arrays));
      return this->outer_.get();
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<int* []> outer_;
    std::vector<std::unique_ptr<int[]>> inner_;
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

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned int*>::is_convertible(value);
    }

    unsigned int* convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }

      this->converted_ = FromRubyFundamental<unsigned int*>::convert(value);

      if (this->arg_ && this->arg_->isOwner())
      {
        unsigned int* result = this->converted_.get();
        this->converted_.release();
        return result;
      }
      else
      {
        return this->converted_.get();
      }
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<unsigned int[]> converted_;;
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

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    ~From_Ruby()
    {
      if (this->arg_ && this->arg_->isOwner())
      {
        for (std::unique_ptr<unsigned int[]>& ptr : this->inner_)
        {
          ptr.release();
        }
        this->outer_.release();
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned int**>::is_convertible(value);
    }

    unsigned int** convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }

      auto arrays = FromRubyFundamental<unsigned int**>::convert(value);
      this->outer_ = std::move(std::get<0>(arrays));
      this->inner_ = std::move(std::get<1>(arrays));
      return this->outer_.get();
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<unsigned int* []> outer_;
    std::vector<std::unique_ptr<unsigned int[]>> inner_;
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

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<long*>::is_convertible(value);
    }

    long* convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }

      this->converted_ = FromRubyFundamental<long*>::convert(value);

      if (this->arg_ && this->arg_->isOwner())
      {
        long* result = this->converted_.get();
        this->converted_.release();
        return result;
      }
      else
      {
        return this->converted_.get();
      }
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<long[]> converted_;
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

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    ~From_Ruby()
    {
      if (this->arg_ && this->arg_->isOwner())
      {
        for (std::unique_ptr<long[]>& ptr : this->inner_)
        {
          ptr.release();
        }
        this->outer_.release();
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<long**>::is_convertible(value);
    }

    long** convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }

      auto arrays = FromRubyFundamental<long**>::convert(value);
      this->outer_ = std::move(std::get<0>(arrays));
      this->inner_ = std::move(std::get<1>(arrays));
      return this->outer_.get();
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<long* []> outer_;
    std::vector<std::unique_ptr<long[]>> inner_;
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

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned long*>::is_convertible(value);
    }

    unsigned long* convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }

      this->converted_ = FromRubyFundamental<unsigned long*>::convert(value);

      if (this->arg_ && this->arg_->isOwner())
      {
        unsigned long* result = this->converted_.get();
        this->converted_.release();
        return result;
      }
      else
      {
        return this->converted_.get();
      }
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<unsigned long[]> converted_;
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

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    ~From_Ruby()
    {
      if (this->arg_ && this->arg_->isOwner())
      {
        for (std::unique_ptr<unsigned long[]>& ptr : this->inner_)
        {
          ptr.release();
        }
        this->outer_.release();
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned long**>::is_convertible(value);
    }

    unsigned long** convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }

      auto arrays = FromRubyFundamental<unsigned long**>::convert(value);
      this->outer_ = std::move(std::get<0>(arrays));
      this->inner_ = std::move(std::get<1>(arrays));
      return this->outer_.get();
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<unsigned long* []> outer_;
    std::vector<std::unique_ptr<unsigned long[]>> inner_;
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

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

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

      this->converted_ = FromRubyFundamental<unsigned long long*>::convert(value);

      if (this->arg_ && this->arg_->isOwner())
      {
        unsigned long long* result = this->converted_.get();
        this->converted_.release();
        return result;
      }
      else
      {
        return this->converted_.get();
      }
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<unsigned long long[]> converted_;
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

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    ~From_Ruby()
    {
      if (this->arg_ && this->arg_->isOwner())
      {
        for (std::unique_ptr<unsigned long long[]>& ptr : this->inner_)
        {
          ptr.release();
        }
        this->outer_.release();
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned long long**>::is_convertible(value);
    }

    unsigned long long** convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }

      auto arrays = FromRubyFundamental<unsigned long long**>::convert(value);
      this->outer_ = std::move(std::get<0>(arrays));
      this->inner_ = std::move(std::get<1>(arrays));
      return this->outer_.get();
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<unsigned long long* []> outer_;
    std::vector<std::unique_ptr<unsigned long long[]>> inner_;
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

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<long long*>::is_convertible(value);
    }

    long long* convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }

      this->converted_ = FromRubyFundamental<long long*>::convert(value);

      if (this->arg_ && this->arg_->isOwner())
      {
        long long* result = this->converted_.get();
        this->converted_.release();
        return result;
      }
      else
      {
        return this->converted_.get();
      }
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<long long[]> converted_;
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

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    ~From_Ruby()
    {
      if (this->arg_ && this->arg_->isOwner())
      {
        for (std::unique_ptr<long long[]>& ptr : this->inner_)
        {
          ptr.release();
        }
        this->outer_.release();
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<long long**>::is_convertible(value);
    }

    long long** convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }

      auto arrays = FromRubyFundamental<long long**>::convert(value);
      this->outer_ = std::move(std::get<0>(arrays));
      this->inner_ = std::move(std::get<1>(arrays));
      return this->outer_.get();
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<long long* []> outer_;
    std::vector<std::unique_ptr<long long[]>> inner_;
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

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<short*>::is_convertible(value);
    }

    short* convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }

      this->converted_ = FromRubyFundamental<short*>::convert(value);

      if (this->arg_ && this->arg_->isOwner())
      {
        short* result = this->converted_.get();
        this->converted_.release();
        return result;
      }
      else
      {
        return this->converted_.get();
      }
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<short[]> converted_;
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

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    ~From_Ruby()
    {
      if (this->arg_ && this->arg_->isOwner())
      {
        for (std::unique_ptr<short[]>& ptr : this->inner_)
        {
          ptr.release();
        }
        this->outer_.release();
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<short**>::is_convertible(value);
    }

    short** convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }

      auto arrays = FromRubyFundamental<short**>::convert(value);
      this->outer_ = std::move(std::get<0>(arrays));
      this->inner_ = std::move(std::get<1>(arrays));
      return this->outer_.get();
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<short* []> outer_;
    std::vector<std::unique_ptr<short[]>> inner_;
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

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned short*>::is_convertible(value);
    }

    unsigned short* convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }

      this->converted_ = FromRubyFundamental<unsigned short*>::convert(value);

      if (this->arg_ && this->arg_->isOwner())
      {
        unsigned short* result = this->converted_.get();
        this->converted_.release();
        return result;
      }
      else
      {
        return this->converted_.get();
      }
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<unsigned short[]> converted_;
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

    // Need move constructor and assignment due to std::unique_ptr
    From_Ruby(From_Ruby&& other) = default;
    From_Ruby& operator=(From_Ruby&& other) = default;

    ~From_Ruby()
    {
      if (this->arg_ && this->arg_->isOwner())
      {
        for (std::unique_ptr<unsigned short[]>& ptr : this->inner_)
        {
          ptr.release();
        }
        this->outer_.release();
      }
    }

    Convertible is_convertible(VALUE value)
    {
      return FromRubyFundamental<unsigned short**>::is_convertible(value);
    }

    unsigned short** convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }

      auto arrays = FromRubyFundamental<unsigned short**>::convert(value);
      this->outer_ = std::move(std::get<0>(arrays));
      this->inner_ = std::move(std::get<1>(arrays));
      return this->outer_.get();
    }

  private:
    Arg* arg_ = nullptr;
    std::unique_ptr<unsigned short* []> outer_;
    std::vector<std::unique_ptr<unsigned short[]>> inner_;
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
      detail::Type<PointerView<void>>::verify();
    };

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
      detail::Type<PointerView<void>>::verify();

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

          // Is this a PointerView? It could also be a pointer to any other object being passed to 
          // a C++ paramter that takes void*
          if (rb_type == Data_Type<PointerView<void>>::ruby_data_type())
          {
            Data_Object<PointerView<void>> pointerView(value);
            return pointerView->pointer;
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