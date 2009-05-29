#ifndef Rice__Director__hpp_
#define Rice__Director__hpp_

#include "Object.hpp"

namespace Rice {

  /**
   * A Director works exactly as a SWIG %director works (thus the name).
   * You use this class to help build proxy classes so that polymorphism
   * works from C++ into Ruby.
   */
  class Director 
  {
    public:
      //! Construct new Director. Needs the Ruby object so that the 
      //  proxy class can call methods on that object.
      Director(Object self) { self_ = self; }

      virtual ~Director() { }

      //! Is the current method call path coming from Ruby?
      /*! This method allows one to choose the call chain according
       *  to the direction of the execution path. We need to do
       *  this to prevent infinite loops where super() calls could
       *  inadvertantly call methods back in Ruby
       */
      bool callIsFromRuby(const char* methodName);

      //! Get the Ruby object linked to this C++ instance
      Object getSelf() { return self_; }

    private:

      // Save the Ruby object related to the instance of this class
      Object self_;
  };
}

#endif // Rice__Director__hpp_
