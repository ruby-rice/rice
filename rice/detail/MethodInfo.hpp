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
      * Get the rb_scan_args format string for this
      * list of arguments.
      */
    std::string formatString();

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

    int requiredArgCount();
    int optionalArgCount();
    void verifyArgCount(int argc);

    // Iterator support
    std::vector<Arg>::iterator begin();
    std::vector<Arg>::iterator end();

    Return returnInfo;

  private:
    template <typename Arg_T>
    void processArg(const Arg_T& arg);

    std::vector<Arg> args_;
  };
}
#endif // Rice__MethodInfo__hpp_