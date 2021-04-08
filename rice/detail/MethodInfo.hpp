#ifndef Rice__MethodInfo__hpp_
#define Rice__MethodInfo__hpp_

#include <vector>
#include "../Arg.hpp"
#include "../ReturnInfo.hpp"

namespace Rice
{
  class MethodInfo
  {
  public:
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
      * Specifices if Ruby owns the returned data
      */
    bool isOwner();

    /**
      * Tell Ruby to take ownership of the returned data
      */
    void takeOwnership();

    /**
      * Given a position, a type, and a ruby VALUE, figure out
      * what argument value we need to return according to
      * defaults and if that VALUE is nil or not
      */
    template<typename Arg_T>
    Arg_T& defaultValue(size_t pos);

    Arg& arg(size_t pos);

    // Iterator support
    std::vector<Arg>::iterator begin();
    std::vector<Arg>::iterator end();

    ReturnInfo returnInfo;

  private:
    template <typename Arg_T>
    void processArg(const Arg_T& arg);

    std::vector<Arg> args_;
  };
}
#include "MethodInfo.ipp"

#endif // Rice__MethodInfo__hpp_