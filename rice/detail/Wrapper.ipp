#include <memory>

namespace Rice::detail
{
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

  inline void WrapperBase::setOwner(bool value)
  {
    this->isOwner_ = value;
  }

  // ----  Wrapper -----
  template <typename T>
  inline Wrapper<T>::Wrapper(T& data): data_(data)
  {
  }

  template <typename T>
  inline Wrapper<T>::Wrapper(T&& data) : data_(std::move(data))
  {
  }

  template <typename T>
  inline Wrapper<T>::~Wrapper()
  {
    Registries::instance.instances.remove(this->get());
  }

  template <typename T>
  inline void* Wrapper<T>::Wrapper::get()
  {
    return (void*)&this->data_;
  }

  // ----  Wrapper& -----
  template <typename T>
  inline Wrapper<T&>::Wrapper(T& data): data_(data)
  {
  }

  template <typename T>
  inline Wrapper<T&>::~Wrapper()
  {
    Registries::instance.instances.remove(this->get());
  }

  template <typename T>
  inline void* Wrapper<T&>::get()
  {
    return (void*)&this->data_;
  }

  // ----  Wrapper* -----
  template <typename T>
  inline Wrapper<T*>::Wrapper(T* data, bool isOwner) : data_(data)
  {
    this->isOwner_ = isOwner;
  }

  template <typename T>
  inline Wrapper<T*>::~Wrapper()
  {
    Registries::instance.instances.remove(this->get());
    if constexpr (std::is_destructible_v<T>)
    {
      if (this->isOwner_)
      {
        delete this->data_;
      }
    }
  }

  template <typename T>
  inline void* Wrapper<T*>::get()
  {
    return (void*)this->data_;
  }

  // ---- Helper Functions -------
  template <typename T>
  inline VALUE wrap(VALUE klass, rb_data_type_t* rb_type, T& data, bool isOwner)
  {
    VALUE result = Registries::instance.instances.lookup(&data);

    if (result != Qnil)
      return result;

    WrapperBase* wrapper = nullptr;

    // If Ruby is not the owner then wrap the reference
    if (!isOwner)
    {
      wrapper = new Wrapper<T&>(data);
      result = TypedData_Wrap_Struct(klass, rb_type, wrapper);
    }

    // Ruby is the owner so copy data
    else if constexpr (std::is_copy_constructible_v<T> || std::is_move_constructible_v<T>)
    {
      wrapper = new Wrapper<T>(std::forward<T>(data));
      result = TypedData_Wrap_Struct(klass, rb_type, wrapper);
    }

    else
    {
      std::string message = "Ruby was directed to take ownership of a C++ object but it does not have an accessible copy or move constructor. Type: " +
        typeName(typeid(T));
      throw std::runtime_error(message);
    }

    Registries::instance.instances.add(wrapper->get(), result);

    return result;
  };

  template <typename T>
  inline VALUE wrap(VALUE klass, rb_data_type_t* rb_type, T* data, bool isOwner)
  {
    VALUE result = Registries::instance.instances.lookup(data);

    if (result != Qnil)
      return result;

    WrapperBase* wrapper = new Wrapper<T*>(data, isOwner);
    result = TypedData_Wrap_Struct(klass, rb_type, wrapper);

    Registries::instance.instances.add(wrapper->get(), result);
    return result;
  };

  template <typename T>
  inline T* unwrap(VALUE value, rb_data_type_t* rb_type, bool takeOwnership)
  {
    WrapperBase* wrapper = getWrapper(value, rb_type);

    if (wrapper == nullptr)
    {
      std::string message = "Wrapped C++ object is nil. Did you override " + 
                            std::string(detail::protect(rb_obj_classname, value)) + 
                            "#initialize and forget to call super?";

      throw std::runtime_error(message);
    }

    if (takeOwnership)
      wrapper->setOwner(false);

    return static_cast<T*>(wrapper->get());
  }
    
  inline WrapperBase* getWrapper(VALUE value, rb_data_type_t* rb_type)
  {
    WrapperBase* wrapper = nullptr;
    TypedData_Get_Struct(value, WrapperBase, rb_type, wrapper);
    return wrapper;
  }

  inline WrapperBase* getWrapper(VALUE value)
  {
    // Turn off spurious warning on g++ 12
    #if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Warray-bounds"
    #endif

    return RTYPEDDATA_P(value) ? static_cast<WrapperBase*>(RTYPEDDATA_DATA(value)) : nullptr;
    
    #if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic pop
    #endif
  }

  template <typename T>
  inline void wrapConstructed(VALUE value, rb_data_type_t* rb_type, T* data, bool isOwner)
  {
    using Wrapper_T = Wrapper<T*>;
    
    Wrapper_T* wrapper = nullptr;
    TypedData_Get_Struct(value, Wrapper_T, rb_type, wrapper);
    if (wrapper)
    {
      Registries::instance.instances.remove(wrapper->get());
      delete wrapper;
    }

    wrapper = new Wrapper_T(data, true);
    RTYPEDDATA_DATA(value) = wrapper;

    Registries::instance.instances.add(data, value);
  }
} // namespace
