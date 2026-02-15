#include <memory>

namespace Rice::detail
{
  inline void WrapperBase::addKeepAlive(VALUE object, VALUE keepAlive)
  {
    WrapperBase* wrapper = getWrapper(object);
    wrapper->addKeepAlive(keepAlive);
  }

  inline bool WrapperBase::isConst(VALUE object)
  {
    WrapperBase* wrapper = getWrapper(object);
    return wrapper->isConst();
  }

  inline WrapperBase::WrapperBase(rb_data_type_t* rb_data_type) : rb_data_type_(rb_data_type)
  {
  }

  inline bool WrapperBase::isConst()
  {
    return this->isConst_;
  }

  inline void WrapperBase::ruby_mark()
  {
    for (VALUE value : this->keepAlive_)
    {
      rb_gc_mark(value);
    }
  }

  inline void WrapperBase::addKeepAlive(VALUE value)
  {
    this->keepAlive_.push_back(value);
  }

  inline const std::vector<VALUE>& WrapperBase::getKeepAlive() const
  {
    return this->keepAlive_;
  }

  inline void WrapperBase::setKeepAlive(const std::vector<VALUE>& keepAlive)
  {
    this->keepAlive_ = keepAlive;
  }

  inline void WrapperBase::setOwner(bool value)
  {
    this->isOwner_ = value;
  }

  // ----  Wrapper -----
  template <typename T>
  inline Wrapper<T>::Wrapper(rb_data_type_t* rb_data_type, T& data) : WrapperBase(rb_data_type), data_(data)
  {
    this->isConst_ = std::is_const_v<std::remove_reference_t<T>>;
  }

  template <typename T>
  inline Wrapper<T>::Wrapper(rb_data_type_t* rb_data_type, T&& data) : WrapperBase(rb_data_type), data_(std::move(data))
  {
  }

  template <typename T>
  inline void* Wrapper<T>::get(rb_data_type_t* requestedType)
  {
    if (rb_typeddata_inherited_p(this->rb_data_type_, requestedType))
    {
      return (void*)&this->data_;
    }
    else
    {
      throw Exception(rb_eTypeError, "wrong argument type %s (expected %s)",
                      this->rb_data_type_->wrap_struct_name,
                      requestedType->wrap_struct_name);
    }
  }

  // ----  Wrapper& -----
  template <typename T>
  inline Wrapper<T&>::Wrapper(rb_data_type_t* rb_data_type, T& data) : WrapperBase(rb_data_type), data_(data)
  {
    this->isConst_ = std::is_const_v<std::remove_reference_t<T>>;
  }

  template <typename T>
  inline Wrapper<T&>::~Wrapper()
  {
    Registries::instance.instances.remove(this->get(this->rb_data_type_));
  }

  template <typename T>
  inline void* Wrapper<T&>::get(rb_data_type_t* requestedType)
  {
    if (rb_typeddata_inherited_p(this->rb_data_type_, requestedType))
    {
      return (void*)&this->data_;
    }
    else
    {
      throw Exception(rb_eTypeError, "wrong argument type %s (expected %s)",
                      this->rb_data_type_->wrap_struct_name,
                      requestedType->wrap_struct_name);
    }
  }

  // ----  Wrapper* -----
  template <typename T>
  inline Wrapper<T*>::Wrapper(rb_data_type_t* rb_data_type, T* data, bool isOwner) : WrapperBase(rb_data_type), data_(data)
  {
    this->isOwner_ = isOwner;
    this->isConst_ = std::is_const_v<std::remove_pointer_t<T>>;
  }

  template <typename T>
  inline Wrapper<T*>::~Wrapper()
  {
    Registries::instance.instances.remove(this->get(this->rb_data_type_));

    if constexpr (is_complete_v<T>)
    {
      if constexpr (std::is_destructible_v<T>)
      {
        if (this->isOwner_)
        {
          delete this->data_;
        }
      }
    }
  }

  template <typename T>
  inline void* Wrapper<T*>::get(rb_data_type_t* requestedType)
  {
    if (rb_typeddata_inherited_p(this->rb_data_type_, requestedType))
    {
      return (void*)this->data_;
    }
    else
    {
      throw Exception(rb_eTypeError, "wrong argument type %s (expected %s)",
                      this->rb_data_type_->wrap_struct_name,
                      requestedType->wrap_struct_name);
    }
  }

  // ----  Wrapper** -----
  template <typename T>
  inline Wrapper<T**>::Wrapper(rb_data_type_t* rb_data_type, T** data, bool isOwner) : WrapperBase(rb_data_type), data_(data)
  {
    this->isOwner_ = isOwner;
    this->isConst_ = std::is_const_v<std::remove_pointer_t<std::remove_pointer_t<T>>>;
  }

  template <typename T>
  inline Wrapper<T**>::~Wrapper()
  {
    Registries::instance.instances.remove(this->get(this->rb_data_type_));

    if constexpr (is_complete_v<T>)
    {
      if constexpr (std::is_destructible_v<T>)
      {
        if (this->isOwner_)
        {
          delete this->data_;
        }
      }
    }
  }

  template <typename T>
  inline void* Wrapper<T**>::get(rb_data_type_t* requestedType)
  {
    if (rb_typeddata_inherited_p(this->rb_data_type_, requestedType))
    {
      return (void*)this->data_;
    }
    else
    {
      throw Exception(rb_eTypeError, "wrong argument type %s (expected %s)",
                      this->rb_data_type_->wrap_struct_name,
                      requestedType->wrap_struct_name);
    }
  }

  // ---- Helper Functions -------
  template <typename T>
  inline VALUE wrapLvalue(VALUE klass, rb_data_type_t* rb_data_type, T& data)
  {
    WrapperBase* wrapper = new Wrapper<T>(rb_data_type, data);
    return TypedData_Wrap_Struct(klass, rb_data_type, wrapper);
  }

  template <typename T>
  inline VALUE wrapRvalue(VALUE klass, rb_data_type_t* rb_data_type, T& data)
  {
    WrapperBase* wrapper = new Wrapper<T>(rb_data_type, std::move(data));
    return TypedData_Wrap_Struct(klass, rb_data_type, wrapper);
  }

  template <typename T>
  inline VALUE wrapReference(VALUE klass, rb_data_type_t* rb_data_type, T& data, bool isOwner)
  {
    VALUE result = Registries::instance.instances.lookup(&data, isOwner);
    if (result == Qnil)
    {
      WrapperBase* wrapper = new Wrapper<T&>(rb_data_type, data);
      result = TypedData_Wrap_Struct(klass, rb_data_type, wrapper);
      Registries::instance.instances.add(&data, result, isOwner);
    }
    return result;
  }

  template <typename T>
  inline VALUE wrap(VALUE klass, rb_data_type_t* rb_data_type, T& data, bool isOwner)
  {
    // Incomplete types can't be copied/moved, just wrap as reference
    if constexpr (!is_complete_v<T>)
    {
      return wrapReference(klass, rb_data_type, data, isOwner);
    }
    // If Ruby is not the owner then wrap the reference
    else if (!isOwner)
    {
      return wrapReference(klass, rb_data_type, data, isOwner);
    }
    // std::is_copy_constructible_v<std::vector<std::unique_ptr<T>>>> returns true. Sigh.
    else if constexpr (detail::is_std_vector_v<T>)
    {
      if constexpr (std::is_copy_constructible_v<typename T::value_type>)
      {
        return wrapLvalue(klass, rb_data_type, data);
      }
      else
      {
        return wrapRvalue(klass, rb_data_type, data);
      }
    }

    // Ruby is the owner so copy data
    else if constexpr (std::is_copy_constructible_v<T>)
    {
      return wrapLvalue(klass, rb_data_type, data);
    }

    // Ruby is the owner so move data
    else if constexpr (std::is_move_constructible_v<T>)
    {
      return wrapRvalue(klass, rb_data_type, data);
    }

    else
    {
      detail::TypeDetail<T> typeDetail;
      std::string message = "Rice was directed to take ownership of a C++ object but it does not have an accessible copy or move constructor. Type: " +
        typeDetail.name();
      throw std::runtime_error(message);
    }
  };

  template <typename T>
  inline VALUE wrap(VALUE klass, rb_data_type_t* rb_data_type, T* data, bool isOwner)
  {
    VALUE result = Registries::instance.instances.lookup(data, isOwner);

    if (result != Qnil)
      return result;

    WrapperBase* wrapper = new Wrapper<T*>(rb_data_type, data, isOwner);
    result = TypedData_Wrap_Struct(klass, rb_data_type, wrapper);

    Registries::instance.instances.add(data, result, isOwner);
    return result;
  };

  template <typename T>
  inline T* unwrap(VALUE value, rb_data_type_t* rb_data_type, bool takeOwnership)
  {
    if (!RTYPEDDATA_P(value))
    {
      std::string message = "The Ruby object does not wrap a C++ object. It is actually a " +
        std::string(detail::protect(rb_obj_classname, value)) + ".";
      throw std::runtime_error(message);
    }

    if (protect(rb_obj_is_kind_of, value, rb_cProc))
    {
      std::string message = "The Ruby object is a proc or lambda and does not wrap a C++ object";
      throw std::runtime_error(message);
    }

    WrapperBase* wrapper = static_cast<WrapperBase*>(RTYPEDDATA_DATA(value));

    if (wrapper == nullptr)
    {
      std::string message = "Wrapped C++ object is nil. Did you override " +
                            std::string(detail::protect(rb_obj_classname, value)) +
                            "#initialize and forget to call super?";

      throw std::runtime_error(message);
    }

    if (takeOwnership)
    {
      wrapper->setOwner(false);
    }

    return static_cast<T*>(wrapper->get(rb_data_type));
  }
    
  template <typename Wrapper_T>
  inline Wrapper_T* getWrapper(VALUE value, rb_data_type_t* rb_data_type)
  {
    WrapperBase* wrapper = nullptr;
    TypedData_Get_Struct(value, WrapperBase, rb_data_type, wrapper);
    return dynamic_cast<Wrapper_T*>(wrapper);
  }

  inline WrapperBase* getWrapper(VALUE value)
  {
    // Turn off spurious warning on g++ 12
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif

    if (!RTYPEDDATA_P(value))
    {
      throw Exception(rb_eTypeError, "wrong argument type %s (expected %s)",
                      detail::protect(rb_obj_classname, value),
                      "wrapped C++ object");
    }

    return static_cast<WrapperBase*>(RTYPEDDATA_DATA(value));
    
    #if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic pop
    #endif
  }

  template <typename T>
  inline Wrapper<T*>* wrapConstructed(VALUE value, rb_data_type_t* rb_data_type, T* data, VALUE source)
  {
    using Wrapper_T = Wrapper<T*>;

    Wrapper_T* wrapper = nullptr;
    TypedData_Get_Struct(value, Wrapper_T, rb_data_type, wrapper);
    if (wrapper)
    {
      Registries::instance.instances.remove(wrapper->get(rb_data_type));
      delete wrapper;
    }

    wrapper = new Wrapper_T(rb_data_type, data, true);
    RTYPEDDATA_DATA(value) = wrapper;

    Registries::instance.instances.add(data, value, true);

    // Copy keepAlive references from the source object (used by initialize_copy
    // so that cloned containers directly protect the same Ruby objects)
    if (source != Qnil)
    {
      WrapperBase* sourceWrapper = getWrapper(source);
      wrapper->setKeepAlive(sourceWrapper->getKeepAlive());
    }

    return wrapper;
  }
}
