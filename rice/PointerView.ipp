namespace Rice
{
  template<typename T>
  PointerView<T>::PointerView(T* pointer) : pointer(pointer)
  {
  }

  template<typename T>
  PointerView<T>::PointerView(T* pointer, size_t size) : pointer(pointer), size(size)
  {
  }

  template<typename T>
  PointerView<T>::PointerView(const PointerView& other)
  {
    this->pointer = other.pointer;
    this->size = other.size;
  }

  template<typename T>
  PointerView<std::remove_pointer_t<T>> PointerView<T>::operator*()
  {
    if constexpr (std::is_pointer_v<T>)
    {
      // This assumes that the value stored in this->pointer is another pointer of the same type
      return PointerView<std::remove_pointer_t<T>>(*this->pointer);
    }
    else
    {
      throw std::runtime_error("Can only dereference a pointer");
    }
  }
  
  template<typename T>
  inline VALUE PointerView<T>::read(size_t offset, size_t count)
  {
    if (!this->pointer)
    {
      return Qnil;
    }
    else
    {
      T* start = this->pointer + offset;
      long length = count * sizeof(T);
      return detail::protect(rb_str_new_static, (const char*)start, length);
    }
  }

  template<typename T>
  inline VALUE PointerView<T>::read()
  {
    return this->read(0, this->size);
  }

  template<typename T>
  inline Array PointerView<T>::toArray(size_t offset, size_t count)
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
          PointerView<std::remove_pointer_t<T>> pointerView(object);
          result.push(pointerView);
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
  inline Array PointerView<T>::toArray()
  {
    return this->toArray(0, this->size);
  }

  // Specializations to handle void
  template<>
  inline VALUE PointerView<void>::read(size_t offset, size_t count)
  {
    return Qnil;
  }

  template<>
  inline Array PointerView<void>::toArray(size_t offset, size_t count)
  {
    return Qnil;
  }
}

namespace Rice::detail
{
  template<typename PointerView_T>
  Data_Type<PointerView_T> define_pointer_view()
  {
    std::string name = detail::typeName(typeid(PointerView_T));
    std::string klassName = detail::makeClassName(name);
    Module rb_mRice = define_module("Rice");

    Data_Type<PointerView_T> result = define_class_under<PointerView_T>(rb_mRice, klassName).
      define_constructor(Constructor<PointerView_T, PointerView_T::type*>()).
      define_attr("size", &PointerView_T::size).
      define_method("dereference", &PointerView_T::operator*).
      template define_method<VALUE(PointerView_T::*)(size_t, size_t)>("read", &PointerView_T::read, Return().setValue()).
      template define_method<VALUE(PointerView_T::*)()>("read", &PointerView_T::read, Return().setValue()).
      template define_method<Array(PointerView_T::*)(size_t, size_t)>("to_a", &PointerView_T::toArray, Return().setValue()).
      template define_method<Array(PointerView_T::*)()>("to_a", &PointerView_T::toArray, Return().setValue());

    return result;
  }

  template<typename T>
  struct Type<PointerView<T>>
  {
    static bool verify()
    {
      Type<intrinsic_type<T>>::verify();

      if (!Data_Type<PointerView<T>>::is_defined())
      {
        define_pointer_view<PointerView<T>>();
      }

      return true;
    }
  };
}
