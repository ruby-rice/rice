#ifndef Rice__ruby_mark__hpp_
#define Rice__ruby_mark__hpp_

//! Default function to call to mark a data object.
/*! This function can be specialized for a particular type to override
 *  the default behavior (which is to not mark any additional objects).
 */
namespace Rice
{
  template<typename T>
  void ruby_mark(T* data)
  {
  }
}
#endif // Rice__ruby_mark__hpp_

