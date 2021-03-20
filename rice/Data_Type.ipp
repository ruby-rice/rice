#ifndef Rice__Data_Type__ipp_
#define Rice__Data_Type__ipp_

#include "detail/method_data.hpp"
#include "detail/Native_Attribute.hpp"
#include "detail/default_allocation_func.hpp"
#include "detail/Caster.hpp"
#include "detail/TypeRegistry.hpp"
#include "detail/Wrapper.hpp"
#include "detail/Iterator.hpp"
#include "Class.hpp"
#include "String.hpp"

#include <stdexcept>

namespace Rice
{
  template<typename T>
  void ruby_mark_internal(detail::Wrapper* wrapper)
  {
    // Tell the wrapper to mark the objects its keeping alive
    wrapper->ruby_mark();

    // Get the underlying data and call custom mark function (if any)
    T* data = static_cast<T*>(wrapper->get());
    ruby_mark<T>(data);
  }

  template<typename T>
  void ruby_free_internal(detail::Wrapper* wrapper)
  {
    delete wrapper;
  }

  template<typename T>
  size_t ruby_size_internal(const T* data)
  {
    return sizeof(T);
  }

  template<typename T>
  template <typename Base_T>
  inline Data_Type<T> Data_Type<T>::bind(Module const& klass)
  {
    if (klass.value() == klass_)
    {
      return Data_Type<T>();
    }

    if (is_bound())
    {
      std::string message = "Type " + detail::typeName(typeid(T)) + " is already bound to a different type";
      throw std::runtime_error(message.c_str());
    }

    // TODO: Make sure base type is bound; throw an exception otherwise.
    // We can't do this just yet, because we don't have a specialization
    // for binding to void.
    klass_ = klass;

    // TODO: do we need to unregister when the program exits?  we have to
    // be careful if we do, because the ruby interpreter might have
    // already shut down.  The correct behavior is probably to register an
    // exit proc with the interpreter, so the proc gets called before the
    // GC shuts down.
    detail::protect(rb_gc_register_address, &klass_);

    rb_type_ = new rb_data_type_t();
    rb_type_->wrap_struct_name = strdup(Rice::detail::protect(rb_class2name, klass_));
    rb_type_->function.dmark = reinterpret_cast<void(*)(void*)>(&Rice::ruby_mark_internal<T>);
    rb_type_->function.dfree = reinterpret_cast<void(*)(void*)>(&Rice::ruby_free_internal<T>);
    rb_type_->function.dsize = reinterpret_cast<size_t(*)(const void*)>(&Rice::ruby_size_internal<T>);
    rb_type_->data = nullptr;
    rb_type_->flags = RUBY_TYPED_FREE_IMMEDIATELY;

    if constexpr (!std::is_void_v<Base_T>)
    {
      rb_type_->parent = Data_Type<Base_T>::rb_type();
    }

    // Now register with the type registry
    detail::TypeRegistry::add<T>(klass_, rb_type_);

    for (typename Instances::iterator it = unbound_instances().begin(),
      end = unbound_instances().end();
      it != end;
      unbound_instances().erase(it++))
    {
      (*it)->set_value(klass);
    }

    return Data_Type<T>();
  }

  template<typename T>
  inline void Data_Type<T>::unbind()
  {
    if (klass_ != Qnil)
    {
      detail::protect(rb_gc_unregister_address, &klass_);
      klass_ = Qnil;
    }

    // There could be objects floating around using the existing rb_type so 
    // do not delete it. This is of course a memory leak.
    rb_type_ = nullptr;
  }

  template<typename T>
  void Data_Type<T>::verify()
  {
    if (!isDefined)
    {
      std::string message = "Type is not defined with Rice: " + detail::typeName(typeid(T));
      throw std::invalid_argument(message);
    }
  }

  template<typename T>
  inline Data_Type<T>::Data_Type() : Class(klass_ == Qnil ? rb_cObject : klass_)
  {
    if (!is_bound())
    {
      unbound_instances().insert(this);
    }
  }

  template<typename T>
  inline Data_Type<T>::Data_Type(Module const& klass) : Class(klass)
  {
    this->bind(klass);
  }

  template<typename T>
  inline Data_Type<T>::~Data_Type()
  {
    unbound_instances().erase(this);
  }

  template<typename T>
  inline rb_data_type_t* Data_Type<T>::rb_type()
  {
    check_is_bound();
    return rb_type_;
  }

  template<typename T>
  inline Class Data_Type<T>::klass()
  {
    check_is_bound();
    return klass_;
  }

  template<typename T>
  inline Data_Type<T>& Data_Type<T>::operator=(Module const& klass)
  {
    this->bind(klass);
    return *this;
  }

  template<typename T>
  template<typename Constructor_T>
  inline Data_Type<T>& Data_Type<T>::define_constructor(Constructor_T, Arguments* arguments)
  {
    check_is_bound();

    // Normal constructor pattern with new/initialize
    detail::protect(rb_define_alloc_func, static_cast<VALUE>(*this), detail::default_allocation_func<T>);
    this->define_method("initialize", &Constructor_T::construct, arguments);

    return *this;
  }

  template<typename T>
  template<typename Constructor_T, typename...Arg_Ts>
  inline Data_Type<T>& Data_Type<T>::define_constructor(Constructor_T constructor, Arg_Ts const& ...args)
  {
    check_is_bound();

    // Define a Ruby allocator which creates the Ruby object
    detail::protect(rb_define_alloc_func, static_cast<VALUE>(*this), detail::default_allocation_func<T>);

    // Define an initialize function that will create the C++ object
    this->define_method("initialize", &Constructor_T::construct, args...);

    return *this;
  }

  template<typename T>
  template<typename Director_T>
  inline Data_Type<T>& Data_Type<T>::define_director()
  {
    Data_Type<Director_T>::bind(*this);

    // TODO - hack to fake Ruby into thinking that a Director is
    // the same as the base data type
    Data_Type<Director_T>::rb_type_ = Data_Type<T>::rb_type_;
    return *this;
  }

  template<typename T>
  inline bool Data_Type<T>::is_bound()
  {
    return klass_ != Qnil;
  }

  template<typename T>
  inline bool Data_Type<T>::is_descendant(VALUE value)
  {
    check_is_bound();
    return detail::protect(rb_obj_is_kind_of, value, klass_) == Qtrue;
  }

  template<typename T>
  inline void Data_Type<T>::check_is_bound()
  {
    if (!is_bound())
    {
      std::string message = "Type " + detail::typeName(typeid(T)) + " is not bound";
      throw std::runtime_error(message.c_str());
    }
  }

  template<typename T, typename Base_T>
  inline Data_Type<T> define_class_under(Object module, char const* name)
  {
    Data_Type<T>::isDefined = true;

    Class superKlass;

    if constexpr (std::is_void_v<Base_T>)
    {
      superKlass = rb_cObject;
    }
    else
    {
      superKlass = Data_Type<Base_T>::klass();
    }
    
    Class c = define_class_under(module, name, superKlass);
    c.undef_creation_funcs();
    return Data_Type<T>::template bind<Base_T>(c);
  }

  template<typename T, typename Base_T>
  inline Data_Type<T> define_class(char const* name)
  {
    Data_Type<T>::isDefined = true;

    Class superKlass;
    if constexpr (std::is_void_v<Base_T>)
    {
      superKlass = rb_cObject;
    }
    else
    {
      superKlass = Data_Type<Base_T>::klass();
    }

    Class c = define_class(name, superKlass);
    c.undef_creation_funcs();
    return Data_Type<T>::template bind<Base_T>(c);
  }

  template<typename From_T, typename To_T>
  void define_implicit_cast()
  {
    Class from_class = Data_Type<From_T>::klass().value();
    Class to_class = Data_Type<To_T>::klass().value();
    detail::CasterRegistry::add<From_T, To_T>(from_class, to_class);
  }

  template<typename T>
  template<typename U, typename Iterator_T>
  inline Data_Type<T>& Data_Type<T>::define_iterator(Iterator_T(U::* begin)(), Iterator_T(U::* end)(), Identifier name)
  {
    using Iter_T = detail::Iterator<U, Iterator_T>;
    Iter_T* iterator = new Iter_T(begin, end);
    detail::MethodData::define_method(Data_Type<T>::klass(), name,
      (RUBY_METHOD_FUNC)iterator->call, 0, iterator);

    return *this;
  }

  template <typename T>
  template <typename Attr_T>
  inline Data_Type<T>& Data_Type<T>::define_attr(std::string name, Attr_T&& attr, AttrAccess access)
  {
    auto* native = detail::Make_Native_Attribute(attr, access);
    using Native_T = typename std::remove_pointer_t<decltype(native)>;

    detail::verifyType<typename Native_T::Native_Return_T>();

    if (access == AttrAccess::ReadWrite || access == AttrAccess::Read)
    {
      detail::MethodData::define_method( klass_, Identifier(name).id(),
        RUBY_METHOD_FUNC(&Native_T::get), 0, native);
    }

    if (access == AttrAccess::ReadWrite || access == AttrAccess::Write)
    {
      if (std::is_const_v<std::remove_pointer_t<Attr_T>>)
      {
        throw std::runtime_error(name + " is readonly");
      }

      detail::MethodData::define_method( klass_, Identifier(name + "=").id(),
        RUBY_METHOD_FUNC(&Native_T::set), 1, native);
    }

    return *this;
  }

  template <typename T>
  template <typename Attr_T>
  inline Data_Type<T>& Data_Type<T>::define_singleton_attr(std::string name, Attr_T&& attr, AttrAccess access)
  {
    auto* native = detail::Make_Native_Attribute(attr, access);
    using Native_T = typename std::remove_pointer_t<decltype(native)>;

    detail::verifyType<typename Native_T::Native_Return_T>();

    if (access == AttrAccess::ReadWrite || access == AttrAccess::Read)
    {
      VALUE singleton = detail::protect(rb_singleton_class, this->value());
      detail::MethodData::define_method(singleton, Identifier(name).id(),
        RUBY_METHOD_FUNC(&Native_T::get), 0, native);
    }

    if (access == AttrAccess::ReadWrite || access == AttrAccess::Write)
    {
      if (std::is_const_v<std::remove_pointer_t<Attr_T>>)
      {
        throw std::runtime_error(name  + " is readonly");
      }

      VALUE singleton = detail::protect(rb_singleton_class, this->value());
      detail::MethodData::define_method(singleton, Identifier(name + "=").id(),
        RUBY_METHOD_FUNC(&Native_T::set), 1, native);
    }

    return *this;
  }

  namespace detail
  {
    template<typename T, typename std::enable_if_t<!is_primitive_v<T> && !std::is_enum_v<T>>>
    constexpr void verifyType()
    {
      if (!Data_Type<intrinsic_type<T>>::isDefined)
      {
        std::string message = "Type not defined with Rice: " + detail::typeName(typeid(T));
        throw std::invalid_argument(message);
      }
    }
  }
}
#endif
