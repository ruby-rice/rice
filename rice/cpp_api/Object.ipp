namespace Rice
{
  inline Object::Object() : value_(Qnil)
  {
  }

  inline Object::Object(VALUE value) : value_(value)
  {
  }

  inline VALUE Object::value() const
  {
    return this->value_.value();
  }

  inline Object::operator VALUE() const
  {
    return this->value();
  }

  inline VALUE Object::validated_value() const
  {
    VALUE result = this->value();

    if (result == Qnil)
    {
      std::string message = "Rice Object does not wrap a Ruby object";
      throw std::runtime_error(message.c_str());
    }

    return result;
  }

  inline Object::operator bool() const
  {
    return RTEST(this->value());
  }

  inline bool Object::is_nil() const
  {
    return NIL_P(this->value());
  }

  template<typename ...Parameter_Ts>
  inline Object Object::call(Identifier id, Parameter_Ts... args) const
  {
    /* IMPORTANT - We store VALUEs in an array that is a local variable.
       That allows the Ruby garbage collector to find them when scanning
       the stack and thus mark them. If instead we use a vector, then Ruby's GC
       can't find the VALUEs and may garbage collect them before they are sent
       to the destination method resulting in a segmentation fault. This is
       easy to duplicate by setting GC.stress to true and calling a constructor
       that takes multiple values like a std::pair wrapper. */
    std::array<VALUE, sizeof...(Parameter_Ts)> values = { detail::To_Ruby<detail::remove_cv_recursive_t<Parameter_Ts>>().convert(std::forward<Parameter_Ts>(args))... };
    return detail::protect(rb_funcallv, this->validated_value(), id.id(), (int)values.size(), (const VALUE*)values.data());
  }

  template<typename ...Parameter_Ts>
  inline Object Object::call_kw(Identifier id, Parameter_Ts... args) const
  {
    /* IMPORTANT - See call() above */
    std::array<VALUE, sizeof...(Parameter_Ts)> values = { detail::To_Ruby<detail::remove_cv_recursive_t<Parameter_Ts>>().convert(args)... };
    return detail::protect(rb_funcallv_kw, this->validated_value(), id.id(), (int)values.size(), (const VALUE*)values.data(), RB_PASS_KEYWORDS);
  }

  template<typename T>
  inline void Object::iv_set(Identifier name, T const& value)
  {
    detail::protect(rb_ivar_set, this->validated_value(), name.id(), detail::To_Ruby<T>().convert(value));
  }

  inline int Object::compare(Object const& other) const
  {
    Object result = call("<=>", other);
    return detail::From_Ruby<int>().convert(result);
  }

  inline bool Object::is_equal(const Object& other) const
  {
    if (this->is_nil() || other.is_nil())
    {
      return this->is_nil() && other.is_nil();
    }

    VALUE result = detail::protect(rb_equal, this->validated_value(), other.validated_value());
    return RB_TEST(result);
  }

  inline bool Object::is_eql(const Object& other) const
  {
    if (this->is_nil() || other.is_nil())
    {
      return this->is_nil() && other.is_nil();
    }

    VALUE result = detail::protect(rb_eql, this->validated_value(), other.validated_value());
    return RB_TEST(result);
  }

  inline void Object::freeze()
  {
    detail::protect(rb_obj_freeze, this->validated_value());
  }

  inline bool Object::is_frozen() const
  {
    return RB_OBJ_FROZEN(this->validated_value());
  }

  inline int Object::rb_type() const
  {
    return ::rb_type(this->validated_value());
  }

  inline VALUE Object::object_id() const
  {
    return detail::protect(rb_obj_id, this->validated_value());
  }

  inline bool Object::is_a(Object klass) const
  {
    VALUE result = detail::protect(rb_obj_is_kind_of, this->validated_value(), klass.validated_value());
    return RB_TEST(result);
  }

  inline void Object::extend(Module const& mod)
  {
    detail::protect(rb_extend_object, this->validated_value(), mod.validated_value());
  }

  inline bool Object::respond_to(Identifier id) const
  {
    return bool(rb_respond_to(this->validated_value(), id.id()));
  }

  inline bool Object::is_instance_of(Object klass) const
  {
    VALUE result = detail::protect(rb_obj_is_instance_of, this->validated_value(), klass.validated_value());
    return RB_TEST(result);
  }

  inline Object Object::iv_get(Identifier name) const
  {
    return detail::protect(rb_ivar_get, this->validated_value(), name.id());
  }

  inline Object Object::attr_get(Identifier name) const
  {
    return detail::protect(rb_attr_get, this->validated_value(), name.id());
  }

  inline void Object::set_value(VALUE value)
  {
    this->value_ = Pin(value);
  }

  inline Object Object::const_get(Identifier name) const
  {
    return detail::protect(rb_const_get, this->validated_value(), name.id());
  }

  inline bool Object::const_defined(Identifier name) const
  {
    size_t result = detail::protect(rb_const_defined, this->validated_value(), name.id());
    return bool(result);
  }

  inline Object Object::const_set(Identifier name, Object value)
  {
    // We will allow setting constants to Qnil, or the decimal value of 4. This happens
    // in C++ libraries with enums. Thus use value() instead of validated_value
    detail::protect(rb_const_set, this->validated_value(), name.id(), value.value());
    return value;
  }

  inline Object Object::const_set_maybe(Identifier name, Object value)
  {
    if (!this->const_defined(name))
    {
      this->const_set(name, value);
    }
    return value;
  }

  inline void Object::remove_const(Identifier name)
  {
    detail::protect(rb_mod_remove_const, this->validated_value(), name.to_sym());
  }

  inline bool operator==(Object const& lhs, Object const& rhs)
  {
    return lhs.is_equal(rhs);
  }

  inline bool operator!=(Object const& lhs, Object const& rhs)
  {
    return !(lhs == rhs);
  }

  inline bool operator<(Object const& lhs, Object const& rhs)
  {
    Object result = lhs.call("<", rhs);
    return result;
  }

  inline bool operator>(Object const& lhs, Object const& rhs)
  {
    Object result = lhs.call(">", rhs);
    return result;
  }
}

namespace Rice::detail
{
  template<>
  struct Type<Object>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cObject;
    }
  };

  template<>
  class To_Ruby<Object>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Arg* arg) : arg_(arg)
    {
    }

    VALUE convert(Object const& x)
    {
      return x.value();
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class To_Ruby<Object&>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Arg* arg) : arg_(arg)
    {
    }

    VALUE convert(Object const& x)
    {
      return x.value();
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<Object>
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
        case RUBY_T_OBJECT:
          return Convertible::Exact;
          break;
        default:
          return Convertible::None;
      }
    }

    Object convert(VALUE value)
    {
      return Object(value);
    }

  private:
    Arg* arg_ = nullptr;
  };
}
