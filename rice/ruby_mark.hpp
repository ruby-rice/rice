#ifndef Rice__ruby_mark__hpp_
#define Rice__ruby_mark__hpp_

//! Default function to call to mark a data object.
/*! This function can be specialized for a particular type to override
 *  the default behavior (which is to not mark any additional objects).
 */
RICE_BEGIN_NAMESPACE
  template<typename T>
  void ruby_mark(T*)
  {
    // Do nothing by default
  }
RICE_END_NAMESPACE

#endif // Rice__ruby_mark__hpp_

