#ifndef Rice__Arguments__hpp_
#define Rice__Arguments__hpp_

#include "../Arg_impl.hpp"
#include <sstream>
#include <vector>
#include "../to_from_ruby_defn.hpp"

namespace Rice {

  class Arguments
  {
    public:
      Arguments() {
        required_ = 0;
        optional_ = 0;
      }

      ~Arguments() {
      }

      /**
       * Get the full argument count of this
       * list of arguments.
       * Returns -1 no defined arguments
       */
      int count() {
        if(required_ == 0 && optional_ == 0) {
          return -1;
        } else {
          return required_ + optional_;
        }
      }

      /**
       * Get the rb_scan_args format string for this
       * list of arguments.
       * In the case of no Args (default case), this
       * method uses the passed in full argument count
       */
      std::string formatString(int fullArgCount)
      {
        std::stringstream s;
        if(required_ == 0 && optional_ == 0)
        {
          s << fullArgCount << 0;
        }
        else
        {
          s << required_ << optional_;
        }

        return s.str();
      }

      /**
       * Add a defined Arg to this list of Arguments
       */
      void add(const Arg& arg)
      {
        args_.push_back(arg);

        if(arg.hasDefaultValue())
        {
          optional_++;
        }
        else
        {
          required_++;
        }
      }

      /**
       * Is the argument at the request location an optional
       * argument?
       */
      bool isOptional(unsigned int pos)
      {
        if(required_ == 0 && optional_ == 0)
        {
          return false;
        }
        if(pos >= args_.size())
        {
          return false;
        }
        return args_[pos].hasDefaultValue();
      }

      /**
       * Given a position, a type, and a ruby VALUE, figure out
       * what argument value we need to return according to
       * defaults and if that VALUE is nil or not
       */
      template<typename Arg_T>
      Arg_T getArgumentOrDefault(int pos, VALUE in)
      {
        if(isOptional(pos) && NIL_P(in))
        {
          return args_[pos].getDefaultValue<Arg_T>();
        }
        else
        {
          return from_ruby<Arg_T>(in);
        }
      }

    private:
      std::vector<Arg> args_;

      /** Keep counts of required and optional parameters */
      int required_;
      int optional_;
  };

}

#endif // Rice__Arguments__hpp_
