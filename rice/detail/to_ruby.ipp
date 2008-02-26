#include "../Data_Object.hpp"

template<typename T>
Rice::Object
Rice::detail::to_ruby_<T>::
convert(T & x)
{
  if(Data_Type<T>::is_bound())
  {
    Data_Object<T> obj(&x);
    return obj;
  }
  else
  {
    std::string s("Unable to convert ");
    s += demangle(typeid(T *).name());
    throw std::invalid_argument(s.c_str());
  }
}   

template<typename T>
Rice::Object
Rice::detail::to_ruby_<T *>::
convert(T * x)
{
  if(Data_Type<T>::is_bound())
  {
    Data_Object<T> obj(x);
    return obj;
  }
  else
  {
    std::string s("Unable to convert ");
    s += demangle(typeid(T *).name());
    throw std::invalid_argument(s.c_str());
  }
}
