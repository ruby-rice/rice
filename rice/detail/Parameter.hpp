#ifndef Rice__detail__Parameter__hpp_
#define Rice__detail__Parameter__hpp_

#include <optional>

namespace Rice::detail
{
  class ParameterAbstract
  {
  public:
    ParameterAbstract() = default;
    ParameterAbstract(std::unique_ptr<Arg>&& arg);
    virtual ~ParameterAbstract() = default;

    ParameterAbstract(const ParameterAbstract& other);
    ParameterAbstract(ParameterAbstract&& other) = default;
    ParameterAbstract& operator=(ParameterAbstract&& other) = default;

    virtual Convertible matches(std::optional<VALUE>& valueOpt) = 0;
    virtual std::string cppTypeName() = 0;
    virtual VALUE klass() = 0;

    Arg* arg();

  private:
    std::unique_ptr<Arg> arg_;
  };

  template<typename T>
  class Parameter: public ParameterAbstract
  {
   public:
     using Type = T;

     Parameter() = default;
     Parameter(std::unique_ptr<Arg>&& arg);
     Parameter(const Parameter& other) = default;
     Parameter(Parameter&& other) = default;
     Parameter& operator=(Parameter&& other) = default;

     T convertToNative(std::optional<VALUE>& valueOpt);
     VALUE convertToRuby(T object);

     Convertible matches(std::optional<VALUE>& valueOpt) override;
     std::string cppTypeName() override;
     VALUE klass() override;

    // std::string typeName() override;
  private:
    From_Ruby<remove_cv_recursive_t<T>> fromRuby_;
    To_Ruby<remove_cv_recursive_t<T>> toRuby_;
  };
}
#endif // Rice__detail__Parameter__hpp_
