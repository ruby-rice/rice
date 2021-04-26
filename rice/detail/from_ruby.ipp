#ifndef Rice__detail__from_ruby__ipp_
#define Rice__detail__from_ruby__ipp_

#include <optional>
#include <stdexcept>
#include "../Exception_defn.hpp"
#include "RubyFunction.hpp"

/* This file implements conversions from Ruby to native values fo fundamental types 
   such as bool, int, float, etc. It also includes conversions for chars and strings */
namespace Rice::detail
{
  // ===========  short  ============
  template<>
  class From_Ruby<short>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(short defaultValue) : defaultValue_(defaultValue)
    {
    }

    short convert(VALUE value)
    {
      if (value == Qnil && this->defaultValue_)
      {
        return this->defaultValue_.value();
      }
      else
      {
        return protect(rb_num2short_inline, value);
      }
    }
  
  private:
    std::optional<short> defaultValue_;
  };

  template<>
  class From_Ruby<short&>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(short defaultValue) : defaultValue_(defaultValue)
    {
    }

    short& convert(VALUE value)
    {
      if (value == Qnil && this->defaultValue_)
      {
        return this->defaultValue_.value();
      }
      else
      {
        this->converted_ = protect(rb_num2short_inline, value);
        return this->converted_;
      }
    }

  private:
    std::optional<short> defaultValue_;
    short converted_ = 0;
  };

  template<>
  class From_Ruby<short*>
  {
  public:
    short* convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }
      else
      {
        this->converted_ = protect(rb_num2short_inline, value);
        return &this->converted_;
      }
    }

  private:
    short converted_ = 0;
  };

  // ===========  int  ============
  template<>
  class From_Ruby<int>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(int defaultValue) : defaultValue_(defaultValue)
    {
    }

    int convert(VALUE value)
    {
      if (value == Qnil && this->defaultValue_)
      {
        return this->defaultValue_.value();
      }
      else
      {
        return (int)protect(rb_num2long_inline, value);
      }
    }
  
  private:
    std::optional<int> defaultValue_;
  };

  template<>
  class From_Ruby<int&>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(int defaultValue) : defaultValue_(defaultValue)
    {
    }

    int& convert(VALUE value)
    {
      if (value == Qnil && this->defaultValue_)
      {
        return this->defaultValue_.value();
      }
      else
      {
        this->converted_ = (int)protect(rb_num2long_inline, value);
        return this->converted_;
      }
    }

  private:
    std::optional<int> defaultValue_;
    int converted_ = 0;
  };

  template<>
  class From_Ruby<int*>
  {
  public:
    int* convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }
      else
      {
        this->converted_ = (int)protect(rb_num2long_inline, value);
        return &this->converted_;
      }
    }

  private:
    int converted_;
  };

  // ===========  long  ============
  template<>
  class From_Ruby<long>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(long defaultValue) : defaultValue_(defaultValue)
    {
    }

    long convert(VALUE value)
    {
      if (value == Qnil && this->defaultValue_)
      {
        return this->defaultValue_.value();
      }
      else
      {
        return protect(rb_num2long_inline, value);
      }
    }
  
  private:
    std::optional<long> defaultValue_;
  };

  template<>
  class From_Ruby<long&>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(long defaultValue) : defaultValue_(defaultValue)
    {
    }

    long& convert(VALUE value)
    {
      if (value == Qnil && this->defaultValue_)
      {
        return this->defaultValue_.value();
      }
      else
      {
        this->converted_ = protect(rb_num2long_inline, value);
        return this->converted_;
      }
    }

  private:
    std::optional<long> defaultValue_;
    long converted_ = 0;
  };

  template<>
  class From_Ruby<long*>
  {
  public:
    long* convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }
      else
      {
        this->converted_ = protect(rb_num2long_inline, value);
        return &this->converted_;
      }
    }

  private:
    long converted_ = 0;
  };

  // ===========  long long  ============
  template<>
  class From_Ruby<long long>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(long long defaultValue) : defaultValue_(defaultValue)
    {
    }

    long long convert(VALUE value)
    {
      if (value == Qnil && this->defaultValue_)
      {
        return this->defaultValue_.value();
      }
      else
      {
        return protect(rb_num2ll_inline, value);
      }
    }
  
  private:
    std::optional<long long> defaultValue_;
  };

  template<>
  class From_Ruby<long long&>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(long long defaultValue) : defaultValue_(defaultValue)
    {
    }

    long long& convert(VALUE value)
    {
      if (value == Qnil && this->defaultValue_)
      {
        return this->defaultValue_.value();
      }
      else
      {
        this->converted_ = protect(rb_num2ll_inline, value);
        return this->converted_;
      }
    }

  private:
    std::optional<long long> defaultValue_;
    long long converted_ = 0;
  };

  template<>
  class From_Ruby<long long*>
  {
  public:
    long long* convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }
      else
      {
        this->converted_ = protect(rb_num2ll_inline, value);
        return &this->converted_;
      }
    }

  private:
    long long converted_ = 0;
  };

  // ===========  unsigned short  ============
  template<>
  class From_Ruby<unsigned short>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(unsigned short defaultValue) : defaultValue_(defaultValue)
    {
    }

    unsigned short convert(VALUE value)
    {
      if (value == Qnil && this->defaultValue_)
      {
        return this->defaultValue_.value();
      }
      else
      {
        return protect(rb_num2ushort, value);
      }
    }
  
  private:
    std::optional<unsigned short> defaultValue_;
  };

  template<>
  class From_Ruby<unsigned short&>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(unsigned short defaultValue) : defaultValue_(defaultValue)
    {
    }

    unsigned short& convert(VALUE value)
    {
      if (value == Qnil && this->defaultValue_)
      {
        return this->defaultValue_.value();
      }
      else
      {
        this->converted_ = protect(rb_num2ushort, value);
        return this->converted_;
      }
    }

  private:
    std::optional<unsigned short> defaultValue_;
    unsigned short converted_ = 0;
  };

  template<>
  class From_Ruby<unsigned short*>
  {
  public:
    unsigned short* convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }
      else
      {
        this->converted_ = protect(rb_num2ushort, value);
        return &this->converted_;
      }
    }

  private:
    unsigned short converted_ = 0;
  };

  // ===========  unsigned int  ============
  template<>
  class From_Ruby<unsigned int>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(unsigned int defaultValue) : defaultValue_(defaultValue)
    {
    }

    unsigned int convert(VALUE value)
    {
      if (value == Qnil && this->defaultValue_)
      {
        return this->defaultValue_.value();
      }
      else
      {
        return (unsigned int)protect(rb_num2ulong_inline, value);
      }
    }
  
  private:
    std::optional<unsigned int> defaultValue_;
  };

  template<>
  class From_Ruby<unsigned int&>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(unsigned int defaultValue) : defaultValue_(defaultValue)
    {
    }

    unsigned int& convert(VALUE value)
    {
      if (value == Qnil && this->defaultValue_)
      {
        return this->defaultValue_.value();
      }
      else
      {
        this->converted_ = (unsigned int)protect(rb_num2ulong_inline, value);
        return this->converted_;
      }
    }

  private:
    std::optional<unsigned int> defaultValue_;
    unsigned int converted_ = 0;
  };

  template<>
  class From_Ruby<unsigned int*>
  {
  public:
    unsigned int* convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }
      else
      {
        this->converted_ = (unsigned int)protect(rb_num2ulong_inline, value);
        return &this->converted_;
      }
    }

  private:
    unsigned int converted_ = 0;
  };

  // ===========  unsigned long  ============
  template<>
  class From_Ruby<unsigned long>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(unsigned long defaultValue) : defaultValue_(defaultValue)
    {
    }

    explicit From_Ruby(bool isValue) : isValue_(isValue)
    {
    }

    unsigned long convert(VALUE value)
    {
      if (this->isValue_)
      {
        return (unsigned long)value;
      }
      else if (value == Qnil && this->defaultValue_)
      {
        return this->defaultValue_.value();
      }
      else
      {
        return protect(rb_num2ulong_inline, value);
      }
    }
  
  private:
    std::optional<unsigned long> defaultValue_;
    unsigned long isValue_;
  };

  template<>
  class From_Ruby<unsigned long&>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(unsigned long defaultValue) : defaultValue_(defaultValue)
    {
    }

    unsigned long& convert(VALUE value)
    {
      if (value == Qnil && this->defaultValue_)
      {
        return this->defaultValue_.value();
      }
      else
      {
        this->converted_ = protect(rb_num2ulong_inline, value);
        return this->converted_;
      }
    }

  private:
    std::optional<unsigned long> defaultValue_;
    unsigned long converted_ = 0;
  };

  template<>
  class From_Ruby<unsigned long*>
  {
  public:
    unsigned long* convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }
      else
      {
        this->converted_ = protect(rb_num2ulong_inline, value);
        return &this->converted_;
      }
    }

  private:
    unsigned long converted_ = 0;
  };

  // ===========  unsigned long long  ============
  template<>
  class From_Ruby<unsigned long long>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(unsigned long long defaultValue) : defaultValue_(defaultValue)
    {
    }

    explicit From_Ruby(bool isValue) : isValue_(isValue)
    {
    }

    unsigned long long convert(VALUE value)
    {
      if (this->isValue_)
      {
        return value;
      }
      else if (value == Qnil && this->defaultValue_)
      {
        return this->defaultValue_.value();
      }
      else
      {
        return protect(rb_num2ull, value);
      }
    }
  
  private:
    std::optional<unsigned long long> defaultValue_;
    bool isValue_ = false;
  };

  template<>
  class From_Ruby<unsigned long long&>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(unsigned long long defaultValue) : defaultValue_(defaultValue)
    {
    }

    unsigned long long& convert(VALUE value)
    {
      if (value == Qnil && this->defaultValue_)
      {
        return this->defaultValue_.value();
      }
      else
      {
        this->converted_ = protect(rb_num2ull, value);
        return this->converted_;
      }
    }

  private:
    std::optional<unsigned long long> defaultValue_;
    unsigned long long converted_ = 0;
  };

  template<>
  class From_Ruby<unsigned long long*>
  {
  public:
    unsigned long long* convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }
      else
      {
        this->converted_ = protect(rb_num2ull, value);
        return &this->converted_;
      }
    }

  private:
    unsigned long long converted_ = 0;
  };

  // ===========  bool  ============
  template<>
  class From_Ruby<bool>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(bool defaultValue) : defaultValue_(defaultValue)
    {
    }

    bool convert(VALUE value)
    {
      if (value == Qnil && this->defaultValue_)
      {
        return this->defaultValue_.value();
      }
      else
      {
        return RTEST(value);
      }
    }
  
  private:
    std::optional<bool> defaultValue_;
  };

  template<>
  class From_Ruby<bool&>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(bool defaultValue) : defaultValue_(defaultValue)
    {
    }

    bool& convert(VALUE value)
    {
      if (value == Qnil && this->defaultValue_)
      {
        return this->defaultValue_.value();
      }
      else
      {
        this->converted_ = RTEST(value);
        return this->converted_;
      }
    }

  private:
    std::optional<bool> defaultValue_;
    bool converted_ = false;
  };

  template<>
  class From_Ruby<bool*>
  {
  public:
    bool* convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }
      else
      {
        this->converted_ = RTEST(value);
        return &this->converted_;
      }
    }

  private:
    bool converted_ = false;
  };

  // ===========  char  ============
  template<typename T>
  inline T charFromRuby(VALUE value)
  {
    switch (rb_type(value))
    {
      case T_STRING:
      {
        if (RSTRING_LEN(value) == 1)
        {
          return RSTRING_PTR(value)[0];
        }
        else
        {
          throw std::invalid_argument("from_ruby<char>: string must have length 1");
        }
        break;
      }
      case T_FIXNUM:
      {
        return From_Ruby<long>().convert(value) & 0xff;
        break;
      }
      default:
      {
        throw Exception(rb_eTypeError, "wrong argument type %s (expected % s)",
          detail::protect(rb_obj_classname, value), "char type");
      }
    }
  }
  
  template<>
  class From_Ruby<char>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(char defaultValue) : defaultValue_(defaultValue)
    {
    }

    char convert(VALUE value)
    {
      if (value == Qnil && this->defaultValue_)
      {
        return this->defaultValue_.value();
      }
      else
      {
        return charFromRuby<char>(value);
      }
    }
  
  private:
    std::optional<char> defaultValue_;
  };

  template<>
  class From_Ruby<char&>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(char defaultValue) : defaultValue_(defaultValue)
    {
    }

    char& convert(VALUE value)
    {
      if (value == Qnil && this->defaultValue_)
      {
        return this->defaultValue_.value();
      }
      else
      {
        this->converted_ = charFromRuby<char>(value);
        return this->converted_;
      }
    }

  private:
    std::optional<char> defaultValue_;
    char converted_ = 0;
  };

  template<>
  class From_Ruby<char*>
  {
  public:
    char* convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }
      else
      {
        detail::protect(rb_check_type, value, (int)T_STRING);
        return RSTRING_PTR(value);
      }
    }
  };
  
  // This is mostly for testing. NativeFunction removes const before calling From_Ruby
  template<>
  class From_Ruby<char const*>
  {
  public:
    char const* convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }
      else
      {
        detail::protect(rb_check_type, value, (int)T_STRING);
        return RSTRING_PTR(value);
      }
    }
  };

  // ===========  unsinged char  ============
  template<>
  class From_Ruby<unsigned char>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(unsigned char defaultValue) : defaultValue_(defaultValue)
    {
    }

    unsigned char convert(VALUE value)
    {
      return charFromRuby<unsigned char>(value);
    }
  
  private:
    std::optional<unsigned char> defaultValue_;
  };

  // ===========  signed char  ============
  template<>
  class From_Ruby<signed char>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(signed char defaultValue) : defaultValue_(defaultValue)
    {
    }

    signed char convert(VALUE value)
    {
      return charFromRuby<signed char>(value);
    }
  
  private:
    std::optional<signed char> defaultValue_;
  };

  // ===========  double  ============
  template<>
  class From_Ruby<double>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(double defaultValue) : defaultValue_(defaultValue)
    {
    }

    double convert(VALUE value)
    {
      if (value == Qnil && this->defaultValue_)
      {
        return this->defaultValue_.value();
      }
      else
      {
        return protect(rb_num2dbl, value);
      }
    }
  
  private:
    std::optional<double> defaultValue_;
  };

  template<>
  class From_Ruby<double&>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(double defaultValue) : defaultValue_(defaultValue)
    {
    }

    double& convert(VALUE value)
    {
      if (value == Qnil && this->defaultValue_)
      {
        return this->defaultValue_.value();
      }
      else
      {
        this->converted_ = protect(rb_num2dbl, value);
        return this->converted_;
      }
    }

  private:
    std::optional<double> defaultValue_;
    double converted_;
  };

  template<>
  class From_Ruby<double*>
  {
  public:
    double* convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }
      else
      {
        this->converted_ = protect(rb_num2dbl, value);
        return &this->converted_;
      }
    }

  private:
    double converted_;
  };

  // ===========  float  ============
  template<>
  class From_Ruby<float>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(float defaultValue) : defaultValue_(defaultValue)
    {
    }

    float convert(VALUE value)
    {
      if (value == Qnil && this->defaultValue_)
      {
        return this->defaultValue_.value();
      }
      else
      {
        return (float)protect(rb_num2dbl, value);
      }
    }
  
  private:
    std::optional<float> defaultValue_;
  };

  template<>
  class From_Ruby<float&>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(float defaultValue) : defaultValue_(defaultValue)
    {
    }

    float& convert(VALUE value)
    {
      if (value == Qnil && this->defaultValue_)
      {
        return this->defaultValue_.value();
      }
      else
      {
        this->converted_ = (float)protect(rb_num2dbl, value);
        return this->converted_;
      }
    }

  private:
    std::optional<float> defaultValue_;
    float converted_;
  };

  template<>
  class From_Ruby<float*>
  {
  public:
    float* convert(VALUE value)
    {
      if (value == Qnil)
      {
        return nullptr;
      }
      else
      {
        this->converted_ = (float)protect(rb_num2dbl, value);
        return &this->converted_;
      }
    }

  private:
    float converted_;
  };
}
#endif // Rice__detail__from_ruby__ipp_
