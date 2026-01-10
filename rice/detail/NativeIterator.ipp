#include <iterator>
#include <functional>
#include <type_traits>

namespace Rice::detail
{
  template <typename T, typename Iterator_Func_T>
  inline void NativeIterator<T, Iterator_Func_T>::define(VALUE klass, std::string method_name, Iterator_Func_T begin, Iterator_Func_T end)
  {
    // Tell Ruby to invoke the resolveIterator static method defined in Native super class.
    detail::protect(rb_define_method, klass, method_name.c_str(), (RUBY_METHOD_FUNC)&Native::resolve, -1);

    // Create a NativeIterator instance and save it to the NativeRegistry. There may be multiple
    // NativeFunction instances for a specific method because C++ supports method overloading.
    NativeIterator_T* nativeIterator = new NativeIterator_T(klass, method_name, begin, end);
    std::unique_ptr<Native> native(nativeIterator);

    Identifier identifier(method_name);
    detail::Registries::instance.natives.add(klass, identifier.id(), native);
  }

  template <typename T, typename Iterator_Func_T>
  inline NativeIterator<T, Iterator_Func_T>::NativeIterator(VALUE klass, std::string method_name, Iterator_Func_T begin, Iterator_Func_T end) :
    Native(method_name), klass_(klass), begin_(begin), end_(end)
  {
  }

  template<typename T, typename Iterator_Func_T>
  inline Resolved NativeIterator<T, Iterator_Func_T>::matches(std::map<std::string, VALUE>&)
  {
    return Resolved{ Convertible::Exact, this };
  }

  template<typename T, typename Iterator_Func_T>
  inline VALUE NativeIterator<T, Iterator_Func_T>::createRubyEnumerator(VALUE self)
  {
    auto rb_size_function = [](VALUE recv, VALUE, VALUE eobj) -> VALUE
    {
      // Since we can't capture VALUE self from above (because then we can't send
      // this lambda to rb_enumeratorize_with_size), extract it from recv
      return cpp_protect([&]
      {
        // Get the iterator instance
        // Class is easy
        VALUE klass = protect(rb_class_of, recv);
        // Read the method_id from an attribute we added to the enumerator instance
        Identifier identifier = protect(rb_ivar_get, eobj, rb_intern("rice_method"));

        const std::vector<std::unique_ptr<Native>>& natives = detail::Registries::instance.natives.lookup(klass, identifier.id());
        NativeIterator_T* iterator = static_cast<NativeIterator_T*>(natives.back().get());

        // Get the wrapped C++ instance
        T* receiver = detail::From_Ruby<T*>().convert(recv);

        // Get the distance
        Iterator_T begin = std::invoke(iterator->begin_, *receiver);
        Iterator_T end = std::invoke(iterator->end_, *receiver);
        Difference_T distance = std::distance(begin, end);

        return detail::To_Ruby<Difference_T>().convert(distance);
      });
    };

    Identifier identifier(this->name());
    VALUE enumerator = protect(rb_enumeratorize_with_size, self, identifier.to_sym(), 0, nullptr, rb_size_function);
    
    // Hack the enumerator object by storing name_ on the enumerator object so
    // the rb_size_function above has access to it
    protect(rb_ivar_set, enumerator, rb_intern("rice_method"), identifier.id());

    return enumerator;
  }

  template<typename T, typename Iterator_Func_T>
  inline VALUE NativeIterator<T, Iterator_Func_T>::operator()(std::map<std::string, VALUE>&, VALUE self)
  {
    if (!protect(rb_block_given_p))
    {
      return createRubyEnumerator(self);
    }
    else
    {
      detail::From_Ruby<T*> fromRuby;
      T* receiver = fromRuby.convert(self);

      Iterator_T it = std::invoke(this->begin_, *receiver);
      Iterator_T end = std::invoke(this->end_, *receiver);

      detail::To_Ruby<To_Ruby_T> toRuby;
      for (; it != end; ++it)
      {
        protect(rb_yield, toRuby.convert(*it));
      }

      return self;
    }
  }

  template<typename T, typename Iterator_Func_T>
  inline std::string NativeIterator<T, Iterator_Func_T>::toString()
  {
    return "";
  }

  template<typename T, typename Iterator_Func_T>
  inline NativeKind NativeIterator<T, Iterator_Func_T>::kind()
  {
    return NativeKind::Iterator;
  }

  template<typename T, typename Iterator_Func_T>
  inline VALUE NativeIterator<T, Iterator_Func_T>::returnKlass()
  {
    // Check if an array is being returned
    bool isBuffer = dynamic_cast<ReturnBuffer*>(this->returnInfo_.get()) ? true : false;
    if (isBuffer)
    {
      TypeDetail<Pointer<detail::remove_cv_recursive_t<std::remove_pointer_t<Value_T>>>> typeDetail;
      return typeDetail.rubyKlass();
    }
    else
    {
      TypeDetail<Value_T> typeDetail;
      return typeDetail.rubyKlass();
    }
  }
}