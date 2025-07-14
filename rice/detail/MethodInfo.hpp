#ifndef Rice__MethodInfo__hpp_
#define Rice__MethodInfo__hpp_

#include <vector>

namespace Rice
{
  class MethodInfo
  {
  public:
    MethodInfo() = default;

    template <typename...Arg_Ts>
    MethodInfo(size_t argCount, const Arg_Ts&...args);

    /**
      * Add a defined Arg to this list of Arguments
      */
    void addArg(const Arg& arg);

    /**
      * Get argument by position
      */
    Arg* arg(size_t pos);

    /**
      * Get argument by name
      */
    Arg* arg(std::string name);

    int argCount();
    Return* returnInfo();

    // Iterator support
    std::vector<Arg>::iterator begin();
    std::vector<Arg>::iterator end();

  private:
    template <typename Arg_T>
    void processArg(const Arg_T& arg);

    std::vector<Arg> args_;
    Return returnInfo_;
  };
}
#endif // Rice__MethodInfo__hpp_