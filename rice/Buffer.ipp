namespace Rice
{
  // ----  Buffer<T> ------- 
  template<typename T>
  inline Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::Buffer(T* pointer) : m_buffer(pointer)
  {
  }

  template<typename T>
  inline Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::Buffer(T* pointer, size_t size) : m_size(size), m_buffer(pointer)
  {
  }

  template <typename T>
  inline Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::Buffer(VALUE value) : Buffer(value, 0)
  {
  }

  template <typename T>
  inline Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::Buffer(VALUE value, size_t size)
  {
    if constexpr (std::is_fundamental_v<T>)
    {
      this->fromBuiltinType(value, size);
    }
    else
    {
      this->fromWrappedType(value, size);
    }
  }

  template <typename T>
  inline Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::~Buffer()
  {
    if constexpr (std::is_destructible_v<T>)
    {
      if (this->m_owner)
      {
        delete[] this->m_buffer;
      }
    }
  }

  template <typename T>
  inline void Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::fromBuiltinType(VALUE value, size_t size)
  {
    using Intrinsic_T = typename detail::intrinsic_type<T>;
    using RubyType_T = typename detail::RubyType<Intrinsic_T>;

    ruby_value_type valueType = rb_type(value);
    switch (valueType)
    {
      case RUBY_T_ARRAY:
      {
        Array array(value);
        this->m_size = array.size();
        this->m_buffer = new T[this->m_size]();

        if constexpr (std::is_fundamental_v<T>)
        {
          String packed = array.pack<Intrinsic_T>();
          memcpy((void*)this->m_buffer, RSTRING_PTR(packed.value()), RSTRING_LEN(packed.value()));
        }
        else
        {
          detail::From_Ruby<Intrinsic_T> fromRuby;
          for (int i = 0; i < array.size(); i++)
          {
            this->m_buffer[0] = fromRuby.convert(array[i]);
          }
        }
        this->m_owner = true;
        break;
      }
      case RUBY_T_STRING:
      {
        this->m_size = RSTRING_LEN(value);
        if constexpr (std::is_same_v<T, char>)
        {
          // Add 2 for null characters (string and wstring)
          this->m_buffer = new T[this->m_size + 2]();
        }
        else
        {
          this->m_buffer = new T[this->m_size]();
        }
        memcpy((void*)this->m_buffer, RSTRING_PTR(value), this->m_size);

        this->m_owner = true;
        break;
      }
      case RUBY_T_DATA:
      {
        if (Data_Type<Pointer<T>>::is_descendant(value))
        {
          this->m_buffer = detail::unwrap<T>(value, Data_Type<Pointer<T>>::ruby_data_type(), false);
          this->m_owner = false;
          this->m_size = size;
          break;
        }
        [[fallthrough]];
      }
      default:
      {
        if (RubyType_T::Exact.find(valueType) != RubyType_T::Exact.end() ||
          RubyType_T::Castable.find(valueType) != RubyType_T::Castable.end() ||
          RubyType_T::Narrowable.find(valueType) != RubyType_T::Narrowable.end())
        {
          T data = detail::protect(RubyType_T::fromRuby, value);
          this->m_size = 1;
          this->m_buffer = new T[this->m_size]();
          memcpy((void*)this->m_buffer, &data, sizeof(T));
          this->m_owner = true;
          break;
        }
        else
        {
          detail::TypeMapper<T> typeMapper;
          std::string typeName = typeMapper.name();
          throw Exception(rb_eTypeError, "wrong argument type %s (expected % s*)",
            detail::protect(rb_obj_classname, value), typeName.c_str());
        }
      }
    }
  }

  template <typename T>
  inline void Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::fromWrappedType(VALUE value, size_t size)
  {
    using Intrinsic_T = typename detail::intrinsic_type<T>;

    switch (rb_type(value))
    {
      case RUBY_T_ARRAY:
      {
        Array array(value);
        this->m_size = array.size();

        // Use operator new[] to allocate memory but not call constructors. Memset to 0 so that
        // if an object implements move assignment it won't blow up.
        size_t size = sizeof(T) * this->m_size;
        this->m_buffer = static_cast<T*>(operator new[](size));
        std::memset(this->m_buffer, 0, size);

        detail::From_Ruby<Intrinsic_T> fromRuby;

        for (size_t i = 0; i < this->m_size; i++)
        {
          this->m_buffer[i] = fromRuby.convert(array[i].value());
        }
        break;
      }
      case RUBY_T_DATA:
      {
        if (Data_Type<Intrinsic_T>::is_descendant(value))
        {
          this->m_buffer = detail::unwrap<T>(value, Data_Type<Intrinsic_T>::ruby_data_type(), false);
          this->m_owner = false;
          this->m_size = size;
          break;
        }
      }
      case RUBY_T_STRING:
      {
        // This special case is a bit ugly...
        if constexpr (std::is_same_v<detail::intrinsic_type<T>, std::string>)
        {
          // FromRuby owns the converted string so we need to keep it alive until we get to the copy constructor
          // two lines down
          detail::From_Ruby<T*> fromRuby;
          T* converted = fromRuby.convert(value);
          this->m_buffer = new T[1]{ *converted };
          this->m_owner = true;
          this->m_size = 1;
          break;
        }
        [[fallthrough]];
      }
      default:
      {
        detail::TypeMapper<T> typeMapper;
        std::string typeName = typeMapper.name();
        throw Exception(rb_eTypeError, "wrong argument type %s (expected % s*)",
          detail::protect(rb_obj_classname, value), typeName.c_str());
      }
    }
  }

  template <typename T>
  inline Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::Buffer(Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>&& other) : m_owner(other.m_owner), m_size(other.m_size), m_buffer(other.m_buffer)
  {
    other.m_buffer = nullptr;
    other.m_size = 0;
    other.m_owner = false;
  }

  template <typename T>
  inline Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>& Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::operator=(Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>&& other)
  {
    this->m_buffer = other.m_buffer;
    other.m_buffer = nullptr;

    this->m_size = other.m_size;
    other.m_size = 0;

    this->m_owner = other.m_owner;
    other.m_owner = false;

    return *this;
  }

  template <typename T>
  inline size_t Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::size() const
  {
    return this->m_size;
  }

  template <typename T>
  inline T* Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::ptr()
  {
    return this->m_buffer;
  }

  template <typename T>
  inline T& Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::reference()
  {
    return *this->m_buffer;
  }

  template <typename T>
  inline T* Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::release()
  {
    this->m_owner = false;
    return this->m_buffer;
  }

  template <typename T>
  inline bool Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::isOwner() const
  {
    return this->m_owner;
  }

  template <typename T>
  inline void Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::setOwner(bool value)
  {
    this->m_owner = value;
  }

  template<typename T>
  inline VALUE Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::toString() const
  {
    detail::TypeMapper<T*> typeMapper;
    std::string description = "Buffer<type: " + typeMapper.simplifiedName() + ", size: " + std::to_string(this->m_size) + ">";

    // We can't use To_Ruby because To_Ruby depends on Buffer - ie a circular reference
    return detail::protect(rb_utf8_str_new_cstr, description.c_str());
  }

  template<typename T>
  inline VALUE Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::bytes(size_t count) const
  {
    if (!this->m_buffer)
    {
      return Qnil;
    }
    else
    {
      long length = (long)(count * sizeof(T));
      return detail::protect(rb_str_new_static, (const char*)this->m_buffer, length);
    }
  }

  template<typename T>
  inline VALUE Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::bytes() const
  {
    return this->bytes(this->m_size);
  }

  template<typename T>
  inline Array Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::toArray(size_t count) const
  {
    if (!this->m_buffer)
    {
      return Qnil;
    }
    else if constexpr (std::is_fundamental_v<T>)
    {
      VALUE string = this->bytes(count);
      return String(string).unpack<T>();
    }
    else
    {
      Array result;

      T* ptr = this->m_buffer;
      T* end = this->m_buffer + count;

      for (; ptr < end; ptr++)
      {
        result.push(*ptr, false);
      }
      return result;
    }
  }

  template<typename T>
  inline Array Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::toArray() const
  {
    return this->toArray(this->m_size);
  }

  template<typename T>
  inline T& Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>::operator[](size_t index)
  {
    if (index >= this->m_size)
    {
      throw Exception(rb_eIndexError, "index %ld outside of bounds: 0..%ld", index, this->m_size);
    }

    return this->m_buffer[index];
  }

  // ----  Buffer<T*> - Builtin ------- 
  template<typename T>
  inline Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::Buffer(T** pointer) : m_buffer(pointer)
  {
  }

  template<typename T>
  inline Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::Buffer(T** pointer, size_t size) : m_buffer(pointer), m_size(size)
  {
  }

  template <typename T>
  inline Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::Buffer(VALUE value) : Buffer(value, 0)
  {
  }
 
  template <typename T>
  inline Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::Buffer(VALUE value, size_t size)
  {
    using Intrinsic_T = typename detail::intrinsic_type<T>;

    ruby_value_type valueType = rb_type(value);
    switch (valueType)
    {
      case RUBY_T_ARRAY:
      {
        Array outer(value);

        // Allocate outer buffer
        this->m_size = outer.size();
        this->m_buffer = new T*[this->m_size]();

        for (size_t i = 0; i < this->m_size; i++)
        {
          // Check the inner value is also an array
          Array inner(outer[i].value());

          // Allocate inner buffer
          this->m_buffer[i] = new T[inner.size()]();

          if constexpr (std::is_fundamental_v<Intrinsic_T>)
          {
            String packed = inner.pack<Intrinsic_T>();
            memcpy((void*)this->m_buffer[i], RSTRING_PTR(packed.value()), RSTRING_LEN(packed.value()));
          }
          else
          {
            detail::From_Ruby<Intrinsic_T*> fromRuby;
            for (int i = 0; i < inner.size(); i++)
            {
              this->m_buffer[0] = fromRuby.convert(inner[i].value());
            }
          }
        }

        this->m_owner = true;
        break;
      }
      case RUBY_T_DATA:
      {
        if (Data_Type<Pointer<T*>>::is_descendant(value))
        {
          this->m_buffer = detail::unwrap<T*>(value, Data_Type<Pointer<T*>>::ruby_data_type(), false);
          this->m_owner = false;
          this->m_size = size;
          break;
        }
        [[fallthrough]];
      }
      default:
      {
        detail::TypeMapper<T> typeMapper;
        std::string typeName = typeMapper.name();
        throw Exception(rb_eTypeError, "wrong argument type %s (expected % s*)",
          detail::protect(rb_obj_classname, value), typeName.c_str());
      }
    }
  }

  template <typename T>
  inline Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::~Buffer()
  {
    if (this->m_owner)
    {
      for (size_t i = 0; i < this->m_size; i++)
      {
        delete this->m_buffer[i];
      }

      delete[] this->m_buffer;
    }
  }

  template <typename T>
  inline Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::Buffer(Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>&& other) : m_owner(other.m_owner), m_size(other.m_size),
                                                  m_buffer(other.m_buffer)
  {
    other.m_buffer = nullptr;
    other.m_size = 0;
    other.m_owner = false;
  }

  template <typename T>
  inline Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>& Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::operator=(Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>&& other)
  {
    this->m_buffer = other.m_buffer;
    other.m_buffer = nullptr;

    this->m_size = other.m_size;
    other.m_size = 0;

    this->m_owner = other.m_owner;
    other.m_owner = false;

    return *this;
  }

  template <typename T>
  inline T* Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::operator[](size_t index)
  {
    return this->m_buffer[index];
  }

  template <typename T>
  inline size_t Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::size() const
  {
    return this->m_size;
  }

  template <typename T>
  inline T** Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::ptr()
  {
    return this->m_buffer;
  }

  template <typename T>
  inline T** Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::release()
  {
    this->m_owner = false;
    return this->m_buffer;
  }

  template <typename T>
  inline bool Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::isOwner() const
  {
    return this->m_owner;
  }

  template <typename T>
  inline void Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::setOwner(bool value)
  {
    this->m_owner = value;
  }

  template<typename T>
  inline VALUE Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::toString() const
  {
    detail::TypeMapper<T*> typeMapper;
    std::string description = "Buffer<type: " + typeMapper.simplifiedName() + ", size: " + std::to_string(this->m_size) + ">";

    // We can't use To_Ruby because To_Ruby depends on Buffer - ie a circular reference
    return detail::protect(rb_utf8_str_new_cstr, description.c_str());
  }

  template<typename T>
  inline VALUE Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::bytes(size_t count) const
  {
    if (!this->m_buffer)
    {
      return Qnil;
    }
    else
    {
      T** begin = this->m_buffer;
      long length = (long)(count * sizeof(T*));
      return detail::protect(rb_str_new_static, (const char*)*begin, length);
    }
  }

  template<typename T>
  inline VALUE Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::bytes() const
  {
    return this->bytes(this->m_size);
  }

  template<typename T>
  inline Array Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::toArray(size_t count) const
  {
    if (!this->m_buffer)
    {
      return Qnil;
    }
    else
    {
      Array result;

      T** ptr = this->m_buffer;
      T** end = this->m_buffer + count;

      for (; ptr < end; ptr++)
      {
        Buffer<T> buffer(*ptr);
        result.push(std::move(buffer), true);
      }
      return result;
    }
  }

  template<typename T>
  inline Array Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>::toArray() const
  {
    return this->toArray(this->m_size);
  }

  // ----  Buffer<T*> - Wrapped ------- 
  template<typename T>
  inline Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::Buffer(T** pointer) : m_buffer(pointer)
  {
  }

  template<typename T>
  inline Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::Buffer(T** pointer, size_t size) : m_buffer(pointer), m_size(size)
  {
  }

  template <typename T>
  inline Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::Buffer(VALUE value) : Buffer(value, 0)
  {
  }
  
  template <typename T>
  inline Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::Buffer(VALUE value, size_t size)
  {
    ruby_value_type valueType = rb_type(value);
    switch (valueType)
    {
      case RUBY_T_ARRAY:
      {
        Array array(value);
        this->m_size = array.size();
        this->m_buffer = new T * [this->m_size]();

        detail::From_Ruby<T*> fromRuby;

        for (size_t i = 0; i < this->m_size; i++)
        {
          this->m_buffer[i] = fromRuby.convert(array[i].value());
        }

        this->m_owner = true;
        break;
      }
      case RUBY_T_DATA:
      {
        if (Data_Type<Pointer<T*>>::is_descendant(value))
        {
          this->m_buffer = detail::unwrap<T*>(value, Data_Type<Pointer<T*>>::ruby_data_type(), false);
          this->m_owner = false;
          this->m_size = size;
          break;
        }
      }
      default:
      {
        detail::TypeMapper<T> typeMapper;
        std::string typeName = typeMapper.name();
        throw Exception(rb_eTypeError, "wrong argument type %s (expected % s*)",
          detail::protect(rb_obj_classname, value), typeName.c_str());
      }
    }
  }

  template <typename T>
  inline Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::~Buffer()
  {
    if (this->m_owner)
    {
      delete[] this->m_buffer;
    }
  }

  template <typename T>
  inline Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::Buffer(Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>&& other) : m_owner(other.m_owner), m_size(other.m_size),
    m_buffer(other.m_buffer)
  {
    other.m_buffer = nullptr;
    other.m_size = 0;
    other.m_owner = false;
  }

  template <typename T>
  inline Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>& Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::operator=(Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>&& other)
  {
    this->m_buffer = other.m_buffer;
    other.m_buffer = nullptr;

    this->m_size = other.m_size;
    other.m_size = 0;

    this->m_owner = other.m_owner;
    other.m_owner = false;

    return *this;
  }

  template <typename T>
  inline T* Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::operator[](size_t index)
  {
    if (index >= this->m_size)
    {
      throw Exception(rb_eIndexError, "index %ld outside of bounds: 0..%ld", index, this->m_size);
    }
    return this->m_buffer[index];
  }

  template <typename T>
  inline size_t Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::size() const
  {
    return this->m_size;
  }

  template <typename T>
  inline T** Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::ptr()
  {
    return this->m_buffer;
  }

  template <typename T>
  inline T** Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::release()
  {
    this->m_owner = false;
    return this->m_buffer;
  }

  template <typename T>
  inline bool Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::isOwner() const
  {
    return this->m_owner;
  }

  template <typename T>
  inline void Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::setOwner(bool value)
  {
    this->m_owner = value;
  }

  template<typename T>
  inline VALUE Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::toString() const
  {
    detail::TypeMapper<T*> typeMapper;
    std::string description = "Buffer<type: " + typeMapper.simplifiedName() + ", size: " + std::to_string(this->m_size) + ">";

    // We can't use To_Ruby because To_Ruby depends on Buffer - ie a circular reference
    return detail::protect(rb_utf8_str_new_cstr, description.c_str());
  }

  template<typename T>
  inline VALUE Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::bytes(size_t count) const
  {
    if (!this->m_buffer)
    {
      return Qnil;
    }
    else
    {
      T** begin = this->m_buffer;
      long length = (long)(count * sizeof(T*));
      return detail::protect(rb_str_new_static, (const char*)*begin, length);
    }
  }

  template<typename T>
  inline VALUE Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::bytes() const
  {
    return this->bytes(this->m_size);
  }

  template<typename T>
  inline Array Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::toArray(size_t count) const
  {
    if (!this->m_buffer)
    {
      return Qnil;
    }
    else
    {
      Array result;

      T** ptr = this->m_buffer;
      T** end = this->m_buffer + count;

      for (; ptr < end; ptr++)
      {
        result.push(*ptr, false);
      }
      return result;
    }
  }

  template<typename T>
  inline Array Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>::toArray() const
  {
    return this->toArray(this->m_size);
  }

  // ----  Buffer<void> ------- 
  template<typename T>
  inline Buffer<T, std::enable_if_t<std::is_void_v<T>>>::Buffer(VALUE value) : Buffer(value, 0)
  {
  }
  
  template<typename T>
  inline Buffer<T, std::enable_if_t<std::is_void_v<T>>>::Buffer(VALUE value, size_t size)
  {
    ruby_value_type valueType = rb_type(value);

    switch (valueType)
    {
      case RUBY_T_STRING:
      {
        this->m_size = RSTRING_LEN(value);
        this->m_buffer = ::operator new(this->m_size);
        memcpy((void*)this->m_buffer, RSTRING_PTR(value), this->m_size);

        this->m_owner = true;
        break;
      }
      default:
      {
        detail::TypeMapper<void> typeMapper;
        std::string typeName = typeMapper.name();
        throw Exception(rb_eTypeError, "wrong argument type %s (expected % s*)",
          detail::protect(rb_obj_classname, value), typeName.c_str());
      }
    }
  }

  template<typename T>
  inline Buffer<T, std::enable_if_t<std::is_void_v<T>>>::Buffer(T* pointer) : m_buffer(pointer)
  {
  }

  template<typename T>
  inline Buffer<T, std::enable_if_t<std::is_void_v<T>>>::Buffer(Buffer<T, std::enable_if_t<std::is_void_v<T>>>&& other) : m_owner(other.m_owner), m_size(other.m_size), m_buffer(other.m_buffer)
  {
    other.m_buffer = nullptr;
    other.m_size = 0;
    other.m_owner = false;
  }

  template<typename T>
  inline Buffer<T, std::enable_if_t<std::is_void_v<T>>>& Buffer<T, std::enable_if_t<std::is_void_v<T>>>::operator=(Buffer<T, std::enable_if_t<std::is_void_v<T>>>&& other)
  {
    this->m_buffer = other.m_buffer;
    other.m_buffer = nullptr;

    return *this;
  }

  template<typename T>
  inline size_t Buffer<T, std::enable_if_t<std::is_void_v<T>>>::size() const
  {
    return this->m_size;
  }

  template<typename T>
  inline T* Buffer<T, std::enable_if_t<std::is_void_v<T>>>::ptr()
  {
    return this->m_buffer;
  }

  template <typename T>
  inline T* Buffer<T, std::enable_if_t<std::is_void_v<T>>>::release()
  {
    this->m_owner = false;
    return this->m_buffer;
  }

  template<typename T>
  inline VALUE Buffer<T, std::enable_if_t<std::is_void_v<T>>>::bytes(size_t count) const
  {
    if (!this->m_buffer)
    {
      return Qnil;
    }
    else
    {
      return detail::protect(rb_usascii_str_new, (const char*)this->m_buffer, (long)count);
    }
  }

  template<typename T>
  inline VALUE Buffer<T, std::enable_if_t<std::is_void_v<T>>>::bytes() const
  {
    return this->bytes(this->m_size);
  }

  // ------  define_buffer ----------
  template<typename T>
  inline Data_Type<Buffer<T>> define_buffer(std::string klassName)
  {
    using Buffer_T = Buffer<T>;
    using Data_Type_T = Data_Type<Buffer_T>;

    if (klassName.empty())
    {
      detail::TypeMapper<Buffer_T> typeMapper;
      klassName = typeMapper.rubyName();
    }

    Module rb_mRice = define_module("Rice");

    if (Data_Type_T::check_defined(klassName, rb_mRice))
    {
      return Data_Type_T();
    }

    if constexpr (std::is_void_v<T>)
    {
      return define_class_under<Buffer_T>(rb_mRice, klassName).
        define_constructor(Constructor<Buffer_T, VALUE>(), Arg("value").setValue()).
        define_constructor(Constructor<Buffer_T, VALUE, size_t>(), Arg("value").setValue(), Arg("size")).
        define_method("size", &Buffer_T::size).
        template define_method<VALUE(Buffer_T::*)(size_t) const>("bytes", &Buffer_T::bytes, Return().setValue()).
        template define_method<VALUE(Buffer_T::*)() const>("bytes", &Buffer_T::bytes, Return().setValue()).
        define_method("data", &Buffer_T::ptr, Return().setArray()).
        define_method("release", &Buffer_T::release, Return().setArray());
    }
    else
    {
      Data_Type<Buffer_T> klass = define_class_under<Buffer_T>(rb_mRice, klassName).
        define_constructor(Constructor<Buffer_T, VALUE>(), Arg("value").setValue()).
        define_constructor(Constructor<Buffer_T, VALUE, size_t>(), Arg("value").setValue(), Arg("size")).
        define_method("size", &Buffer_T::size).
        template define_method<VALUE(Buffer_T::*)() const>("to_s", &Buffer_T::toString, Return().setValue()).
        template define_method<VALUE(Buffer_T::*)(size_t) const>("bytes", &Buffer_T::bytes, Return().setValue()).
        template define_method<VALUE(Buffer_T::*)() const>("bytes", &Buffer_T::bytes, Return().setValue()).
        template define_method<Array(Buffer_T::*)(size_t) const>("to_ary", &Buffer_T::toArray, Return().setValue()).
        template define_method<Array(Buffer_T::*)() const>("to_ary", &Buffer_T::toArray, Return().setValue()).
        define_method("[]", &Buffer_T::operator[], Arg("index")).
        define_method("data", &Buffer_T::ptr, Return().setArray()).
        define_method("release", &Buffer_T::release, Return().setArray());

      if constexpr (!std::is_pointer_v<T> && !std::is_void_v<T> && !std::is_const_v<T> && std::is_copy_assignable_v<T>)
      {
        klass.define_method("[]=", [](Buffer_T& self, size_t index, T& value) -> void
        {
          self[index] = value;
        });
      }
      else if constexpr (std::is_pointer_v<T> && !std::is_const_v<std::remove_pointer_t<T>> && std::is_copy_assignable_v<std::remove_pointer_t<T>>)
      {
        klass.define_method("[]=", [](Buffer_T& self, size_t index, T value) -> void
        {
          *self[index] = *value;
        });
      }

      return klass;
    }
  }
}