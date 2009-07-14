#ifndef Rice__Arguments__hpp_
#define Rice__Arguments__hpp_

#include <vector>

namespace Rice {

  class Arg;

  class Arguments
  {
    public: 
      Arguments() {}

      /**
       * Add a defined Arg to this list of Arguments
       */
      void add(const Arg& arg) 
      {
        args_.push_back(arg);
      }

    private:
      std::vector<Arg> args_;
  };

}

#endif // Rice__Arguments__hpp_
