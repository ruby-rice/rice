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
  template<typename T>
  Data_Type<T> define_pointer_view()
  {
    std::string name = detail::typeName(typeid(T));
    std::string klassName = detail::makeClassName(name);
    Module rb_mRice = define_module("Rice");

    Data_Type<T> result = define_class_under<T>(rb_mRice, klassName).
      define_attr("size", &T::size).
      template define_method<VALUE(T::*)(size_t, size_t)>("read", &T::read, Return().setValue()).
      template define_method<VALUE(T::*)()>("read", &T::read, Return().setValue()).
      template define_method<Array(T::*)(size_t, size_t)>("to_a", &T::toArray, Return().setValue()).
      template define_method<Array(T::*)()>("to_a", &T::toArray, Return().setValue());

    return result;
  }

  template<typename T>
  struct Type<PointerView<T>>
  {
    static bool verify()
    {
      Type<intrinsic_type<T>>::verify();

      if (!detail::Registries::instance.types.isDefined<PointerView<T>>())
      {
        define_pointer_view<PointerView<T>>();
      }

      return true;
    }
  };
}
