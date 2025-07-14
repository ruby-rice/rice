#ifndef Rice__detail__Native__hpp_
#define Rice__detail__Native__hpp_

namespace Rice::detail
{
  class Native;

  class Resolved
  {
  public:
    inline bool operator<(Resolved other);
    inline bool operator>(Resolved other);

    Convertible convertible;
    double parameterMatch;
    Native* native;
  };

  enum class NativeKind
  {
    Function,
    Method,
    Attribute_Read,
    Attribute_Write,
    Proc
  };

  class Native
  {
  public:
    static VALUE resolve(int argc, VALUE* argv, VALUE self);
  public:
    Native() = default;
    Native(std::vector<std::unique_ptr<ParameterAbstract>>&& parameters);
    virtual ~Native() = default;

    Native(const Native&) = delete;
    Native(Native&&) = delete;
    void operator=(const Native&) = delete;
    void operator=(Native&&) = delete;

    virtual Resolved matches(size_t argc, const VALUE* argv, VALUE self);
    virtual VALUE operator()(size_t argc, const VALUE* argv, VALUE self) = 0;
    virtual std::string toString() = 0;
    //virtual NativeKind nativeKind() = 0;
    //virtual std::vector<Parameter*> params() = 0;
    //virtual std::string name() = 0;
  protected:
    std::vector<std::optional<VALUE>> getRubyValues(size_t argc, const VALUE* argv, bool validate);
    ParameterAbstract* getParameterByName(std::string name);
    Convertible matchParameters(std::vector<std::optional<VALUE>>& values);

    template<typename Tuple_T>
    static std::vector<std::unique_ptr<ParameterAbstract>> create_parameters(MethodInfo* methodInfo);

    template<typename Tuple_T, std::size_t ...Indices>
    static inline void create_parameters_impl(std::vector<std::unique_ptr<ParameterAbstract>>& parameters, MethodInfo* methodInfo, std::index_sequence<Indices...> indices);

  protected:
    std::vector<std::unique_ptr<ParameterAbstract>> parameters_;

  };
}

#endif // Rice__detail__Native__hpp_
