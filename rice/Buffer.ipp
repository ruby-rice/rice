namespace Rice
{
  template<typename T>
  Buffer<T>::Buffer(T* pointer) : pointer(pointer)
  {
  }

  template<typename T>
  Buffer<T>::Buffer(T* pointer, size_t size) : pointer(pointer), size(size)
  {
  }

  template<typename T>
  Buffer<T>::Buffer(const Buffer& other)
  {
    this->pointer = other.pointer;
    this->size = other.size;
  }

  template<typename T>
  Buffer<std::remove_pointer_t<T>> Buffer<T>::operator*()
  {
    if constexpr (std::is_pointer_v<T>)
    {
      // This assumes that the value stored in this->pointer is another pointer of the same type
      return Buffer<std::remove_pointer_t<T>>(*this->pointer);
    }
    else
    {
      throw std::runtime_error("Can only dereference a pointer");
    }
  }
  
  template<typename T>
  inline VALUE Buffer<T>::read(size_t offset, size_t count)
  {
    if (!this->pointer)
    {
      return Qnil;
    }
    else
    {
      T* start = this->pointer + offset;
      long length = (long)(count * sizeof(T));
      return detail::protect(rb_str_new_static, (const char*)start, length);
    }
  }

  template<typename T>
  inline VALUE Buffer<T>::read()
  {
    return this->read(0, this->size);
  }

  template<typename T>
  inline Array Buffer<T>::toArray(size_t offset, size_t count)
  {
    if (!this->pointer)
    {
      return Qnil;
    }
    else if constexpr (std::is_fundamental_v<T>)
    {
      VALUE string = this->read(offset, count);
      return String(string).unpack<T>();
    }
    else
    {
      // Is this a pointer to a pointer, so T**?
      constexpr bool isPointerToPointer = std::is_pointer_v<T>;

      Array result;

      T* begin = this->pointer + offset;
      T* end = this->pointer + offset + count;

      for (T* ptr = begin; ptr < end; ptr++)
      {
        T& object = *ptr;

        if constexpr (isPointerToPointer)
        {
          Buffer<std::remove_pointer_t<T>> buffer(object);
          result.push(buffer);
        }
        else
        {
          result.push(object);
        }
      }
      return result;
    }
  }

  template<typename T>
  inline Array Buffer<T>::toArray()
  {
    return this->toArray(0, this->size);
  }

  // Specializations to handle void
  template<>
  inline VALUE Buffer<void>::read(size_t offset, size_t count)
  {
    return Qnil;
  }

  template<>
  inline Array Buffer<void>::toArray(size_t offset, size_t count)
  {
    return Qnil;
  }
}

namespace Rice::detail
{
  template<typename Buffer_T>
  Data_Type<Buffer_T> define_buffer()
  {
    std::string name = detail::typeName(typeid(Buffer_T));
    std::string klassName = detail::makeClassName(name);
    Module rb_mRice = define_module("Rice");

    Data_Type<Buffer_T> result = define_class_under<Buffer_T>(rb_mRice, klassName).
      define_constructor(Constructor<Buffer_T, typename Buffer_T::type*>()).
      define_attr("size", &Buffer_T::size).
      define_method("dereference", &Buffer_T::operator*).
      template define_method<VALUE(Buffer_T::*)(size_t, size_t)>("read", &Buffer_T::read, Return().setValue()).
      template define_method<VALUE(Buffer_T::*)()>("read", &Buffer_T::read, Return().setValue()).
      template define_method<Array(Buffer_T::*)(size_t, size_t)>("to_a", &Buffer_T::toArray, Return().setValue()).
      template define_method<Array(Buffer_T::*)()>("to_a", &Buffer_T::toArray, Return().setValue());

    return result;
  }

  template<typename T>
  struct Type<Buffer<T>>
  {
    static bool verify()
    {
      Type<intrinsic_type<T>>::verify();

      if (!Data_Type<Buffer<T>>::is_defined())
      {
        define_buffer<Buffer<T>>();
      }

      return true;
    }
  };
}
