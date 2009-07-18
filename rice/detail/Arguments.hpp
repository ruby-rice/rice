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
        cout << "New arguments object" << endl;
      }

      ~Arguments() {
        cout << "Arguments getting destroyed" << endl;
      }

      /**
       * Add a defined Arg to this list of Arguments
       */
      void add(const Arg* arg) 
      {
        cout << "Adding arg with name " << arg->name() << endl;
        args_.push_back(arg);
        cout << "Count is now " << args_.size() << endl;
      }

    private:
      std::vector<const Arg*> args_;
  };

}

#endif // Rice__Arguments__hpp_
