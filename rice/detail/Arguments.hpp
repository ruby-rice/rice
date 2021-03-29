#ifndef Rice__Arguments__hpp_
#define Rice__Arguments__hpp_

#include <vector>
#include "../Arg.hpp"
#include "../Return.hpp"

namespace Rice
{
  class Arguments
  {
  public:
    template <typename...Arg_Ts>
    Arguments(const Arg_Ts...args);

    /**
      * Get the full argument count of this
      * list of arguments.
      * Returns -1 no defined arguments
      */
    int count();

    /**
      * Get the rb_scan_args format string for this
      * list of arguments.
      * In the case of no Args (default case), this
      * method uses the passed in full argument count
      */
    std::string formatString(size_t fullArgCount);

    /**
      * Add a defined Arg to this list of Arguments
      */
    void add(const Arg& arg);

    /**
      * Is the argument at the request location an optional
      * argument?
      */
    bool isOptional(unsigned int pos);

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
    Arg_T& defaultValue(int pos);

    // Iterator support
    std::vector<Arg>::iterator begin();
    std::vector<Arg>::iterator end();

  private:

    template <typename Arg_T>
    void processArg(const Arg_T& arg);

    std::vector<Arg> args_;
    Return return_;

    /** Keep counts of required and optional parameters */
    int required_ = 0;
    int optional_ = 0;
  };
}
#include "Arguments.ipp"

#endif // Rice__Arguments__hpp_