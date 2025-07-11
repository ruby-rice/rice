#ifndef Rice__detail__Parameter__hpp_
#define Rice__detail__Parameter__hpp_

#include <optional>

namespace Rice::detail
{
  class ParameterAbstract
  {
  public:
    ParameterAbstract() = default;
    ParameterAbstract(Arg* arg);
//    virtual std::string typeName() = 0;
  protected:
    Arg* arg_;
  };

  template<typename T>
  class Parameter: public ParameterAbstract
  {
   public:
     using Type = T;

     Parameter() = default;
     Parameter(Arg* arg);
     T convertToNative(std::optional<VALUE>& valueOpt);
     Convertible matches(std::optional<VALUE>& valueOpt);
     std::string cppType();

    // std::string typeName() override;
  private:
    From_Ruby<remove_cv_recursive_t<T>> fromRuby_;
  };

  template<typename Tuple_T>
  auto make_parameters_tuple(MethodInfo* methodInfo);
}
#endif // Rice__detail__Parameter__hpp_
