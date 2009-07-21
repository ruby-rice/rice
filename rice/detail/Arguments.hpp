#ifndef Rice__Arguments__hpp_
#define Rice__Arguments__hpp_

#include "../Arg_impl.hpp"
#include <vector>

#include <iostream>
using namespace std;

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
       * Get the rb_scan_args format string for this
       * list of arguments.
       * In the case of no Args (default case), this
       * method uses the passed in full argument count
       */
      char* formatString(int fullArgCount) 
      {
        char* output = new char[2];
        cout << "In format string... required is " << required_ << " and optional is " << optional_ << endl;
        if(required_ == 0 && optional_ == 0) 
        {
          cout << "All required, full arg count is " << fullArgCount << endl;
          sprintf(output, "%d0", fullArgCount);
        }
        else 
        {
          cout << "Required and optional!" << endl;
          sprintf(output, "%d%d", required_ , optional_);
        }

        cout << "Returning " << output << endl;

        return output;
      }

      /**
       * Add a defined Arg to this list of Arguments
       */
      void add(const Arg* arg) 
      {
        args_.push_back(arg);

        if(arg->hasDefaultValue())
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
      bool isOptional(int pos) 
      {
        if(required_ == 0 && optional_ == 0) 
        {
          return false;
        }
        if(pos >= args_.size()) 
        {
          return false; 
        }
        return args_[pos]->hasDefaultValue();
      }

      /**
       * Get access to the Arg object at the given position
       */
      const Arg* get(int pos) 
      {
        return args_[pos];
      }

    private:
      std::vector<const Arg*> args_;

      /** Keep counts of required and optional parameters */
      int required_;
      int optional_;
  };

}

#endif // Rice__Arguments__hpp_
