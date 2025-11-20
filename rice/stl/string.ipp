#include <string>

namespace Rice::detail
{
  template<>
  struct Type<std::string>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };

  template<int N>
  struct Type<std::string[N]>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };

  template<>
  struct Type<std::string*>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      using Pointer_T = Pointer<std::string>;
      std::pair<VALUE, rb_data_type_t*> pair = Registries::instance.types.getType<Pointer_T>();
      return pair.first;
    }
  };

  template<>
  struct Type<std::string**>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      using Pointer_T = Pointer<std::string*>;
      std::pair<VALUE, rb_data_type_t*> pair = Registries::instance.types.getType<Pointer_T>();
      return pair.first;
    }
  };

  template<>
  class To_Ruby<std::string>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Arg* arg) : arg_(arg)
    {
    }

    VALUE convert(const std::string& x)
    {
      return detail::protect(rb_external_str_new, x.data(), (long)x.size());
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class To_Ruby<std::string&>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Arg* arg) : arg_(arg)
    {
    }

    VALUE convert(const std::string& x)
    {
      return detail::protect(rb_external_str_new, x.data(), (long)x.size());
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<int N>
  class To_Ruby<std::string[N]>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Arg* arg) : arg_(arg)
    {
    }

    VALUE convert(std::string data[N])
    {
      Buffer<std::string> buffer(data, N);
      Data_Object<Buffer<std::string>> dataObject(std::move(buffer));
      return dataObject.value();
    }
  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class To_Ruby<std::string*>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Arg* arg) : arg_(arg)
    {
    }

    VALUE convert(const std::string* value)
    {
      bool isOwner = this->arg_ && this->arg_->isOwner();
      bool isBuffer = dynamic_cast<ReturnBuffer*>(this->arg_) ? true : false;

      if (isBuffer)
      {
        using Pointer_T = Pointer<std::string>;
        return detail::wrap(Data_Type<Pointer_T>::klass(), Data_Type<Pointer_T>::ruby_data_type(), value, isOwner);
      }
      else
      {
        return detail::protect(rb_external_str_new, value->data(), (long)value->size());
      }
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class To_Ruby<std::string*&>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Arg* arg) : arg_(arg)
    {
    }

    VALUE convert(const std::string* x)
    {
      return detail::protect(rb_external_str_new, x->data(), (long)x->size());
    }

  private:
    Arg* arg_ = nullptr;
  };

  /*template<>
  class To_Ruby<std::string**>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Arg* arg) : arg_(arg)
    {
    }

    VALUE convert(std::string** data)
    {
      Buffer<std::string*> buffer(data);
      Data_Object<Buffer<std::string*>> dataObject(std::move(buffer));
      return dataObject.value();
    }

  private:
    Arg* arg_ = nullptr;
  };*/

  template<>
  class From_Ruby<std::string>
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
        case RUBY_T_STRING:
          return Convertible::Exact;
          break;
        default:
          return Convertible::None;
      }
    }

    std::string convert(VALUE value)
    {
      detail::protect(rb_check_type, value, (int)RUBY_T_STRING);
      return std::string(RSTRING_PTR(value), RSTRING_LEN(value));
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<std::string&>
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
        case RUBY_T_STRING:
          return Convertible::Exact;
          break;
        default:
          return Convertible::None;
      }
    }

    std::string& convert(VALUE value)
    {
      detail::protect(rb_check_type, value, (int)RUBY_T_STRING);
      this->converted_ = std::string(RSTRING_PTR(value), RSTRING_LEN(value));
      return this->converted_;
    }

  private:
    Arg* arg_ = nullptr;
    std::string converted_ = "";
  };

  template<>
  class From_Ruby<std::string&&>
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
        case RUBY_T_STRING:
          return Convertible::Exact;
          break;
        default:
          return Convertible::None;
      }
    }

    std::string&& convert(VALUE value)
    {
      detail::protect(rb_check_type, value, (int)T_STRING);
      this->converted_ = std::string(RSTRING_PTR(value), RSTRING_LEN(value));
      return std::move(this->converted_);
    }

  private:
    Arg* arg_ = nullptr;
    std::string converted_ = "";
  };

  template<>
  class From_Ruby<std::string*>
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
        case RUBY_T_STRING:
          return Convertible::Exact;
          break;
        default:
          return Convertible::None;
      }
    }

    std::string* convert(VALUE value)
    {
      detail::protect(rb_check_type, value, (int)RUBY_T_STRING);
      this->converted_ = std::string(RSTRING_PTR(value), RSTRING_LEN(value));
      return &this->converted_;
    }

  private:
    Arg* arg_ = nullptr;
    std::string converted_;
  };
}