#ifndef Rice__detail__native_invoker__hpp_
#define Rice__detail__native_invoker__hpp_

#include <optional>

RICE_DETAIL_BEGIN_NAMESPACE
  template<typename Return_T>
  class ResultWrapper
  {
  public:
    std::exception_ptr exception;
    Return_T getResult();
		void setResult(Return_T&& value);
  private:
    std::optional<Return_T> result_;
  };

  template<typename Return_T>
  class ResultWrapper<Return_T&>
  {
  public:
    std::exception_ptr exception;
    Return_T& getResult();
    void setResult(Return_T& value);
  private:
    std::optional<std::reference_wrapper<Return_T>> result_;
  };

  template<typename Return_T>
  class ResultWrapper<Return_T*>
  {
  public:
    std::exception_ptr exception;

    Return_T* getResult();
    void setResult(Return_T* value);
  private:
    Return_T* result_;
  };

  template<>
  class ResultWrapper<void>
  {
  public:
    std::exception_ptr exception;
  };

  template<typename Return_T, typename Function_T, typename Tuple_T>
  class NativeInvoker
  {
  public:
    const Function_T func;
    Tuple_T args;
  public:
    NativeInvoker(const Function_T func, Tuple_T&& args);
    void invoke();
    Return_T result();
    std::exception_ptr exception();
  private:
    ResultWrapper<Return_T> resultWrapper;
  };

  template<typename Function_T, typename ...Parameter_Ts>
  auto protect(Function_T func, Parameter_Ts...args);

  template<typename Function_T, typename ...Parameter_Ts>
  typename function_traits<Function_T>::return_type no_gvl(Function_T func, Parameter_Ts...args);

  template<typename Function_T, typename Tuple_T>
  typename function_traits<Function_T>::return_type no_gvl(Function_T func, Tuple_T&& args);
RICE_DETAIL_END_NAMESPACE

#endif // Rice__detail__native_invoker__hpp_
