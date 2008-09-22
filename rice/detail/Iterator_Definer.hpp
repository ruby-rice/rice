#ifndef Rice__detail__Iterator_Definer__hpp_
#define Rice__detail__Iterator_Definer__hpp_

#include "method_data.hpp"
#include "ruby.hpp"
#include "../protect.hpp"
#include "../Data_Object.hpp"

namespace Rice
{

namespace detail
{

template<typename Derived_T>
class Iterator_Definer
{
public:
  virtual ~Iterator_Definer()
  {
  }

  class Iterator
  {
  public:
    virtual ~Iterator() { }

    virtual VALUE call_impl(VALUE self) = 0;

    static VALUE call(VALUE self)
    {
      void * data = method_data();
      Iterator * iterator = static_cast<Iterator *>(data);
      return iterator->call_impl(self);
    }
  };

  template<typename T, typename Iterator_T>
  class Iterator_Impl
    : public Iterator
  {
  public:
    Iterator_Impl(
        Iterator_T (T::*begin)(),
        Iterator_T (T::*end)(),
        Data_Type<T> data_type)
      : begin_(begin)
      , end_(end)
      , data_type_(data_type)
    {
    }

    virtual VALUE call_impl(VALUE self)
    {
      Data_Object<T> obj(self, data_type_);
      Iterator_T it = obj->begin();
      Iterator_T end = obj->end();
      for(; it != end; ++it)
      {
        Rice::protect(rb_yield, to_ruby(*it));
      }
      return self;
    }

  private:
    Iterator_T (T::*begin_)();
    Iterator_T (T::*end_)();
    Data_Type<T> data_type_;
  };

  template<typename T, typename Iterator_T>
  Derived_T & define_iterator(
      char const * name,
      Iterator_T (T::*begin)(),
      Iterator_T (T::*end)())
  {
    Derived_T & self = static_cast<Derived_T &>(*this);
    // TODO: memory leak!!!!!!!
    Iterator * iterator = new Iterator_Impl<T, Iterator_T>(
        begin,
        end,
        Data_Type<T>(self));
    define_method_with_data(
        static_cast<VALUE>(self),
        name,
        (RUBY_METHOD_FUNC)iterator->call,
        0,
        iterator);
    return self;
  }
};

} // namespac edetail

} // namespace Rice

#endif // Rice__detail__Iterator_Definer__hpp_

