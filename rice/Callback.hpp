#ifndef Rice__Callback__hpp_
#define Rice__Callback__hpp_

namespace Rice
{
  //! Define a callback.
  /*! When C++ invokes a C style callback, Rice automatically converts the C++ arguments
   *  to Ruby. However, there may be cases where you need to specify how individual arguments
   *  should be handled. For example, callbacks often have a user data parameter which is 
   *  defined as a void pointer (void*). In this case, you need to tell Ruby that the parameter
   *  is opaque and should not be convered. For example:
   * 
   *  define_callback<void(*)(void*)>(Arg("user_data").setOpaque());
   *
   *  \param args a list of Arg instance used to define default parameters (optional)
   *  \return nothing
   */
  template<typename Callback_T, typename...Parameter_Ts>
  void define_callback(const Parameter_Ts&...args);
}
#endif // Rice__Callback__hpp_