#ifndef Rice__Default_Allocation_Strategy__hpp_
#define Rice__Default_Allocation_Strategy__hpp_

#include "detail/ruby.hpp"

/*! \file
 *  \brief Strategies for allocation/deallocation of objects.
 */

namespace Rice
{

template<typename T>
struct Default_Allocation_Strategy
{
  //! Allocate an object using operator new.
  static T * allocate() { return new T; }

  //! Delete obj using the delete operator.
  static void free(T * obj) { delete obj; }
};

template<typename T>
struct Xmalloc_Allocation_Strategy
{
  //! Allocate an array of objects using operator new and xmalloc.
  static T * allocate() { T * obj = static_cast<T *>(::xmalloc(sizeof(T))); new(obj) T; return obj; }

  //! Delete obj by calling the destructor explicitly and calling xfree.
  static void free(T * obj) { obj->~T(); ::xfree(obj); }
};

// TODO: array allocation

} // namespace Rice

#endif // Rice__Default_Allocation_Strategy__hpp_
