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
    std::array<VALUE, sizeof...(Arg_Ts)> values = { detail::To_Ruby<Arg_Ts>::convert(args, false)... };
    return detail::protect(rb_funcall2, value(), id.id(), (int)values.size(), (const VALUE*)values.data());
  }

  template<typename T>
  inline void Object::iv_set(Identifier name, T const& value)
  {
    detail::protect(rb_ivar_set, this->value(), name.id(), detail::To_Ruby<T>::convert(value));
  }

  inline int Object::compare(Object const& other) const
  {
    Object result = call("<=>", other);
    return detail::From_Ruby<int>::convert(result);
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
    return ::rb_type(*this);
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
  template <typename T>
  struct is_builtin<T, std::enable_if_t<std::is_base_of_v<Rice::Object, intrinsic_type<T>>>> :
    public std::true_type
  {
  };

  template<>
  struct From_Ruby<Object>
  {
    static Object convert(VALUE value)
    {
      return Object(value);
    }
  };

  template<>
  struct To_Ruby<Object>
  {
    static VALUE convert(Object const& x, bool takeOwnership = false)
    {
      return x.value();
    }
  };
}
#endif // Rice__Object__ipp_