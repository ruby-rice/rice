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
    Iterator,
    AttributeReader,
    AttributeWriter,
    Proc,
    Callback
  };

  class Native
  {
  public:
    static VALUE resolve(int argc, VALUE* argv, VALUE self);
  public:
    Native(std::string name);
    Native(std::string name, std::unique_ptr<Return>&& returnInfo);
    Native(std::string name, std::unique_ptr<Return>&& returnInfo, std::vector<std::unique_ptr<ParameterAbstract>>&& parameters);
    virtual ~Native() = default;

    Native(const Native&) = delete;
    Native(Native&&) = delete;
    void operator=(const Native&) = delete;
    void operator=(Native&&) = delete;

    virtual Resolved matches(size_t argc, const VALUE* argv);
    virtual VALUE operator()(size_t argc, const VALUE* argv, VALUE self) = 0;
    virtual std::string toString() = 0;

    // Ruby API access
    std::string name();
    std::vector<const ParameterAbstract*> parameters();
    virtual NativeKind kind() = 0;
    virtual VALUE returnKlass() = 0;

  protected:
    template<typename T, bool isBuffer>
    static void verify_type();

    std::vector<std::optional<VALUE>> getRubyValues(size_t argc, const VALUE* argv, bool validate);
    ParameterAbstract* getParameterByName(std::string name);
    Convertible matchParameters(std::vector<std::optional<VALUE>>& values);

    template<typename Parameter_Tuple, typename... Arg_Ts>
    static std::vector<std::unique_ptr<ParameterAbstract>> create_parameters(Arg_Ts&& ...args);

    template<typename... Arg_Ts>
    static std::unique_ptr<Return> create_return(Arg_Ts& ...args);

    // Do we need to keep alive any arguments?
    void checkKeepAlive(VALUE self, VALUE returnValue, std::vector<std::optional<VALUE>>& rubyValues);

  private:
    template<typename Parameter_Tuple, typename Arg_Tuple, std::size_t ...Indices>
    static inline void create_parameters_impl(std::vector<std::unique_ptr<ParameterAbstract>>& parameters, std::index_sequence<Indices...> indices, std::vector<std::unique_ptr<Arg>>&& args);

    template<typename Parameter_Tuple, typename Arg_Tuple, size_t I>
    static void verify_parameter();

  protected:
    std::string name_;
    std::unique_ptr<Return> returnInfo_;
    std::vector<std::unique_ptr<ParameterAbstract>> parameters_;
  };
}

#endif // Rice__detail__Native__hpp_
