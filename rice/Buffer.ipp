namespace Rice
{
  // ----  Buffer<T> ------- 
  template<typename T>
  inline Buffer<T>::Buffer(T* pointer) : m_buffer(pointer)
  {
  }

  template<typename T>
  inline Buffer<T>::Buffer(T* pointer, size_t size) : m_buffer(pointer), m_size(size)
  {
  }

  template <typename T>
  inline Buffer<T>::Buffer(VALUE value)
  {
    if constexpr (std::is_fundamental_v<T>)
    {
      this->fromRubyType(value);
    }
    else
    {
      this->fromDataType(value);
    }
  }

  template <typename T>
  inline void Buffer<T>::fromRubyType(VALUE value)
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

        String packed = array.pack<Intrinsic_T>();
        memcpy(this->m_buffer, RSTRING_PTR(packed.value()), RSTRING_LEN(packed.value()));

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
        memcpy(this->m_buffer, RSTRING_PTR(value), this->m_size);

        this->m_owner = true;
        break;
      }
      case RUBY_T_DATA:
      {
        if (Data_Type<T>::is_descendant(value))
        {
          this->m_size = 1;
          this->m_buffer = new T[this->m_size]();
          this->m_buffer[0] = *detail::unwrap<T>(value, Data_Type<T>::ruby_data_type(), false);
          this->m_owner = false;
          break;
        }
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
          memcpy(this->m_buffer, &data, sizeof(T));
          this->m_owner = true;
          break;
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

  template <typename T>
  inline void Buffer<T>::fromDataType(VALUE value)
  {
    using Intrinsic_T = typename detail::intrinsic_type<T>;

    switch (rb_type(value))
    {
      case RUBY_T_ARRAY:
      {
        Array array(value);
        this->m_size = array.size();

        // Use operator new[] to allocate memory but not call constructors
        this->m_buffer = static_cast<T*>(operator new[](sizeof(T)* this->m_size));

        detail::From_Ruby<Intrinsic_T> fromRuby;

        for (int i = 0; i < this->m_size; i++)
        {
          this->m_buffer[i] = fromRuby.convert(array[i].value());
        }
        break;
      }
      default:
      {
        std::string typeName = detail::typeName(typeid(T));
        throw Exception(rb_eTypeError, "wrong argument type %s (expected % s*)",
          detail::protect(rb_obj_classname, value), typeName.c_str());
      }
    }
  }

  template <typename T>
  inline Buffer<T>::~Buffer()
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
  inline Buffer<T>::Buffer(Buffer<T>&& other) : m_buffer(other.m_buffer), m_size(other.m_size), m_owner(other.m_owner)
  {
    other.m_buffer = nullptr;
    other.m_size = 0;
    other.m_owner = false;
  }

  template <typename T>
  inline Buffer<T>& Buffer<T>::operator=(Buffer<T>&& other)
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
  inline size_t Buffer<T>::size() const
  {
    return this->m_size;
  }

  template <typename T>
  void  Buffer<T>::setSize(size_t value)
  {
    this->m_size = value;
  }

  template <typename T>
  inline T* Buffer<T>::ptr()
  {
    return this->m_buffer;
  }

  template <typename T>
  inline T& Buffer<T>::reference()
  {
    return *this->m_buffer;
  }

  template <typename T>
  inline bool Buffer<T>::isOwner() const
  {
    return this->m_owner;
  }

  template <typename T>
  inline void Buffer<T>::setOwner(bool value)
  {
    this->m_owner = value;
  }

  template <typename T>
  inline void Buffer<T>::release()
  {
    this->m_owner = false;
  }

 /* template<typename T>
  inline VALUE Buffer<T>::toString() const
  {
    std::string name = detail::typeName(typeid(T));
    std::string result = "Buffer<type: " + detail::cppClassName(name) + ", size: " + std::to_string(this->m_size) + ">";
    return detail::To_Ruby<std::string>().convert(result);
  }*/

  template<typename T>
  inline VALUE Buffer<T>::bytes(size_t count) const
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
  inline VALUE Buffer<T>::bytes() const
  {
    return this->bytes(this->m_size);
  }

  template<typename T>
  inline Array Buffer<T>::toArray(size_t count) const
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
        result.push(*ptr);
      }
      return result;
    }
  }

  template<typename T>
  inline Array Buffer<T>::toArray() const
  {
    return this->toArray(this->m_size);
  }

  template<typename T>
  inline T Buffer<T>::get(size_t index) const
  {
    if (index >= this->m_size)
    {
      throw Exception(rb_eIndexError, "index %ld outside of bounds: 0..%ld", index, this->m_size);
    }

    return this->m_buffer[index];
  }

  template<typename T>
  inline void Buffer<T>::set(size_t index, T element)
  {
    if (index >= this->m_size)
    {
      throw Exception(rb_eIndexError, "index %ld outside of bounds: 0..%ld", index, this->m_size);
    }

    this->m_buffer[index] = element;
  }

  // ----  Buffer<T*> ------- 
  template<typename T>
  inline Buffer<T*>::Buffer(T** pointer) : m_outer(pointer)
  {
  }

  template<typename T>
  inline Buffer<T*>::Buffer(T** pointer, size_t size) : m_outer(pointer), m_size(size)
  {
  }

  template <typename T>
  inline Buffer<T*>::Buffer(VALUE value)
  {
    using Intrinsic_T = typename detail::intrinsic_type<T>;
    using RubyType_T = typename detail::RubyType<Intrinsic_T>;
    ruby_value_type valueType = rb_type(value);

    switch (valueType)
    {
      case RUBY_T_ARRAY:
      {
        Array outer(value);
        this->m_size = outer.size();
        this->m_outer = new T * [this->m_size]();

        for (int i = 0; i < this->m_size; i++)
        {
          // Check the inner value is also an array
          Array inner(outer[i].value());

          // Wrap it with a buffer and add it our list of inner buffers
          Buffer<T> buffer(inner.value());

          // And update the outer array
          this->m_outer[i] = buffer.ptr();

          // Now move the buffer into the affer, not the buffer pointer is still valid (it just got moved)
          this->m_inner.push_back(std::move(buffer));
        }

        this->m_owner = true;
        break;
      }
      default:
      {
        std::string typeName = detail::typeName(typeid(T));
        throw Exception(rb_eTypeError, "wrong argument type %s (expected % s*)",
          detail::protect(rb_obj_classname, value), typeName.c_str());
      }
    }
  }

  template <typename T>
  inline Buffer<T*>::~Buffer()
  {
    if (this->m_owner)
    {
      delete[] this->m_outer;
    }
  }

  template <typename T>
  inline Buffer<T*>::Buffer(Buffer<T*>&& other) : m_outer(other.m_outer), m_inner(std::move(other.m_inner)),
    m_size(other.m_size), m_owner(other.m_owner)
  {
    other.m_outer = nullptr;
    other.m_inner.clear();
    other.m_size = 0;
    other.m_owner = false;
  }

  template <typename T>
  inline Buffer<T*>& Buffer<T*>::operator=(Buffer<T*>&& other)
  {
    this->m_outer = other.m_outer;
    other.m_outer = nullptr;

    this->m_inner = std::move(other.m_inner);
    other.m_inner.clear();

    this->m_size = other.m_size;
    other.m_size = 0;

    this->m_owner = other.m_owner;
    other.m_owner = false;

    return *this;
  }

  template <typename T>
  inline const Buffer<T>& Buffer<T*>::operator[](size_t index)
  {
    return this->m_inner[index];
  }

  template <typename T>
  inline size_t Buffer<T*>::size() const
  {
    return this->m_size;
  }

  template <typename T>
  void  Buffer<T*>::setSize(size_t value)
  {
    this->m_size = value;
  }

  template <typename T>
  inline T** Buffer<T*>::ptr()
  {
    return this->m_outer;
  }

  template <typename T>
  inline bool Buffer<T*>::isOwner() const
  {
    return this->m_owner;
  }

  template <typename T>
  inline void Buffer<T*>::setOwner(bool value)
  {
    this->m_owner = value;
  }

  template <typename T>
  inline void Buffer<T*>::release()
  {
    this->m_owner = false;
  }

  /*template<typename T>
  inline VALUE Buffer<T*>::toString() const
  {
    std::string name = detail::typeName(typeid(T*));
    std::string result = "Buffer<type: " + detail::cppClassName(name) + ", size: " + std::to_string(this->m_size) + ">";
    return detail::To_Ruby<std::string>().convert(result);
  }*/

  template<typename T>
  inline VALUE Buffer<T*>::bytes(size_t count) const
  {
    if (!this->m_outer)
    {
      return Qnil;
    }
    else
    {
      T** begin = this->m_outer;
      long length = (long)(count * sizeof(T*));
      return detail::protect(rb_str_new_static, (const char*)*begin, length);
    }
  }

  template<typename T>
  inline VALUE Buffer<T*>::bytes() const
  {
    return this->bytes(this->m_size);
  }

  template<typename T>
  inline Array Buffer<T*>::toArray(size_t count) const
  {
    if (!this->m_outer)
    {
      return Qnil;
    }
    else
    {
      Array result;

      T** ptr = this->m_outer;
      T** end = this->m_outer + count;

      for (; ptr < end; ptr++)
      {
        Buffer<T> buffer(*ptr);
        result.push(std::move(buffer));
      }
      return result;
    }
  }

  template<typename T>
  inline Array Buffer<T*>::toArray() const
  {
    return this->toArray(this->m_size);
  }

  // ----  Buffer<void> ------- 
  inline Buffer<void>::Buffer(void* pointer) : m_buffer(pointer)
  {
  }

  inline Buffer<void>::Buffer(Buffer<void>&& other) : m_buffer(other.m_buffer)
  {
    other.m_buffer = nullptr;
  }

  inline Buffer<void>& Buffer<void>::operator=(Buffer<void>&& other)
  {
    this->m_buffer = other.m_buffer;
    other.m_buffer = nullptr;

    return *this;
  }

  inline void* Buffer<void>::ptr()
  {
    return this->m_buffer;
  }

  // ------  define_buffer ----------
  template<typename T>
  inline Data_Type<Buffer<T>> define_buffer(std::string klassName)
  {
    using Buffer_T = Buffer<T>;

    if (klassName.empty())
    {
      std::string typeName = detail::typeName(typeid(Buffer_T));
      klassName = detail::rubyClassName(typeName);
    }

    Module rb_mRice = define_module("Rice");

    if constexpr (std::is_void_v<T>)
    {
      return define_class_under<Buffer_T>(rb_mRice, klassName);
    }
    else
    {
      Data_Type<Buffer_T> klass = define_class_under<Buffer_T>(rb_mRice, klassName).
        define_constructor(Constructor<Buffer_T, VALUE>(), Arg("value").setValue()).
        define_method("size", &Buffer_T::size).
        define_method("size=", &Buffer_T::setSize).
        define_method("ptr", &Buffer_T::ptr).
       // template define_method<VALUE(Buffer_T::*)() const>("to_s", &Buffer_T::toString, Return().setValue()).
        template define_method<VALUE(Buffer_T::*)(size_t) const>("bytes", &Buffer_T::bytes, Return().setValue()).
        template define_method<VALUE(Buffer_T::*)() const>("bytes", &Buffer_T::bytes, Return().setValue()).
        template define_method<Array(Buffer_T::*)(size_t) const>("to_ary", &Buffer_T::toArray, Return().setValue()).
        template define_method<Array(Buffer_T::*)() const>("to_ary", &Buffer_T::toArray, Return().setValue());

      if constexpr (!std::is_pointer_v<T>)
      {
        klass.
          define_method("[]", [](const Buffer_T& self, size_t index) -> T
          {
            return self.get(index);
          }).
          define_method("[]=", [](Buffer_T& self, size_t index, T element) -> void
          {
            self.set(index, element);
          });
      }

      return klass;
    }
  }

  inline void define_fundamental_buffer_types()
  {
    define_buffer<bool>();
    define_buffer<int>();
    define_buffer<int*>();
    define_buffer<unsigned int>();
    define_buffer<unsigned int*>();
    define_buffer<char>();
    define_buffer<char*>();
    define_buffer<unsigned char>();
    define_buffer<unsigned char*>();
    define_buffer<signed char>();
    define_buffer<signed char*>();
    define_buffer<double>();
    define_buffer<double*>();
    define_buffer<float>();
    define_buffer<float*>();
    define_buffer<long>();
    define_buffer<long*>();
    define_buffer<unsigned long>();
    define_buffer<unsigned long*>();
    define_buffer<long long>();
    define_buffer<long long*>();
    define_buffer<unsigned long long>();
    define_buffer<unsigned long long*>();
    define_buffer<short>();
    define_buffer<short*>();
    define_buffer<unsigned short>();
    define_buffer<unsigned short*>();
    define_buffer<void>();
  }
}

namespace Rice::detail
{
  template<typename T>
  struct Type<Buffer<T>>
  {
    static bool verify()
    {
      Type<intrinsic_type<T>>::verify();

      if (!Data_Type<Buffer<T>>::is_defined())
      {
        define_buffer<T>();
      }

      return true;
    }
  };
}
