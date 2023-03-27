#ifndef Rice__Object__ipp_
#define Rice__Object__ipp_

namespace Rice
{
  inline const Object Nil(Qnil);
  inline const Object True(Qtrue);
  inline const Object False(Qfalse);
  inline const Object Undef(Qundef);

  inline Object::Object(Object&& other)
  {
    this->value_ = other.value_;
    other.value_ = Qnil;
  }

  // Ruby auto detects VALUEs in the stack, so when an Object gets deleted make sure
  // to clean up in case it is on the stack
  inline Object::~Object()
  {
    this->value_ = Qnil;
  }

  inline Object& Object::operator=(Object&& other)
  {
    this->value_ = other.value_;
    other.value_ = Qnil;
    return *this;
  }

  template<typename ...Arg_Ts>
  inline Object Object::call(Identifier id, Arg_Ts... args) const
  {
    /* IMPORTANT - We store VALUEs in an array that is a local variable.
       That allows the Ruby garbage collector to find them when scanning
       the stack and thus mark them. If instead we use a vector, then Ruby's GC
       can't find the VALUEs and may garbage collect them before they are sent
       to the destination method resulting in a segmentation fault. This is
       easy to duplicate by setting GC.stress to true and calling a constructor
       that takes multiple values like a std::pair wrapper. */
    std::array<VALUE, sizeof...(Arg_Ts)> values = { detail::To_Ruby<detail::remove_cv_recursive_t<Arg_Ts>>().convert(args)... };
    return detail::protect(rb_funcall2, value(), id.id(), (int)values.size(), (const VALUE*)values.data());
  }

  template<typename T>
  inline void Object::iv_set(Identifier name, T const& value)
  {
    detail::protect(rb_ivar_set, this->value(), name.id(), detail::To_Ruby<T>().convert(value));
  }

  inline int Object::compare(Object const& other) const
  {
    Object result = call("<=>", other);
    return detail::From_Ruby<int>().convert(result);
  }

  inline bool Object::is_equal(const Object& other) const
  {
    return detail::protect(rb_equal, this->value_, other.value_);
  }

  inline bool Object::is_eql(const Object& other) const
  {
    return detail::protect(rb_eql, this->value_, other.value_);
  }

  inline void Object::freeze()
  {
    detail::protect(rb_obj_freeze, value());
  }

  inline bool Object::is_frozen() const
  {
    return bool(OBJ_FROZEN(value()));
  }

  inline int Object::rb_type() const
  {
    return ::rb_type(this->value());
  }

  inline VALUE Object::object_id() const
  {
    return detail::protect(rb_obj_id, this->value());
  }

  inline bool Object::is_a(Object klass) const
  {
    Object result = detail::protect(rb_obj_is_kind_of, this->value(), klass.value());
    return result.test();
  }

  inline bool Object::respond_to(Identifier id) const
  {
    return bool(rb_respond_to(this->value(), id.id()));
  }

  inline bool Object::is_instance_of(Object klass) const
  {
    Object result = detail::protect(rb_obj_is_instance_of, this->value(), klass.value());
    return result.test();
  }

  inline Object Object::iv_get(Identifier name) const
  {
    return detail::protect(rb_ivar_get, this->value(), name.id());
  }

  inline Object Object::attr_get(Identifier name) const
  {
    return detail::protect(rb_attr_get, this->value(), name.id());
  }

  inline void Object::set_value(VALUE v)
  {
    value_ = v;
  }

  inline Object Object::const_get(Identifier name) const
  {
    return detail::protect(rb_const_get, this->value(), name.id());
  }

  inline bool Object::const_defined(Identifier name) const
  {
    size_t result = detail::protect(rb_const_defined, this->value(), name.id());
    return bool(result);
  }

  inline Object Object::const_set(Identifier name, Object value)
  {
    detail::protect(rb_const_set, this->value(), name.id(), value.value());
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
    detail::protect(rb_mod_remove_const, this->value(), name.to_sym());
  }

  inline bool operator==(Object const& lhs, Object const& rhs)
  {
    return detail::protect(rb_equal, lhs.value(), rhs.value()) == Qtrue;
  }

  inline bool operator!=(Object const& lhs, Object const& rhs)
  {
    return !(lhs == rhs);
  }

  inline bool operator<(Object const& lhs, Object const& rhs)
  {
    Object result = lhs.call("<", rhs);
    return result.test();
  }

  inline bool operator>(Object const& lhs, Object const& rhs)
  {
    Object result = lhs.call(">", rhs);
    return result.test();
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
  };

  template<>
  class To_Ruby<Object>
  {
  public:
    static VALUE convert(Object const& x)
    {
      return x.value();
    }
  };

  template<>
  class To_Ruby<Object&>
  {
  public:
    static VALUE convert(Object const& x)
    {
      return x.value();
    }
  };

  template<>
  class From_Ruby<Object>
  {
  public:
    Object convert(VALUE value)
    {
      return Object(value);
    }
  };
}
#endif // Rice__Object__ipp_