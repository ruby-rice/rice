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
    virtual ~ParameterAbstract() = default;

    ParameterAbstract(ParameterAbstract&& other) = default;
    ParameterAbstract& operator=(ParameterAbstract&& other) = default;

    virtual Convertible matches(std::optional<VALUE>& valueOpt) = 0;
    virtual std::string cppTypeName() = 0;
    virtual VALUE klass() = 0;

  public:
    Arg* arg = nullptr;
  };

  template<typename T>
  class Parameter: public ParameterAbstract
  {
   public:
     using Type = T;

     Parameter() = default;
     Parameter(Arg* arg);
     Parameter(Parameter&& other) = default;
     Parameter& operator=(Parameter&& other) = default;

     T convertToNative(std::optional<VALUE>& valueOpt);
     Convertible matches(std::optional<VALUE>& valueOpt) override;
     std::string cppTypeName() override;
     VALUE klass() override;

    // std::string typeName() override;
  private:
    From_Ruby<remove_cv_recursive_t<T>> fromRuby_;
  };
}
#endif // Rice__detail__Parameter__hpp_
