#ifndef Rice__detail__traits__hpp_
#define Rice__detail__traits__hpp_

namespace Rice
{
namespace detail
{

/**
 * Remove const from a type
 */
template<typename T>
struct remove_const { typedef T Type; };

template<typename T>
struct remove_const<const T> { typedef T Type; };

/**
 * Remove a reference from a type
 */
template<typename T>
struct remove_ref { typedef T Type; };

template<typename T>
struct remove_ref<T &> { typedef T Type; };

/**
 * Do both of the above in one easy step
 */
template<typename T>
struct sanitize
{
  typedef T Type;
  //typedef typename remove_const< 
    //typename remove_ref<T>::Type 
    //>::Type Type;
};

}
}


#endif // Rice__detail__traits__hpp_
