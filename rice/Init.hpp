#ifndef Rice__Init__hpp_
#define Rice__Init__hpp_

namespace Rice
{
  // This class is used to initialize Rice when a Rice extension is loaded.
  // It defines template instantiations for the Buffer and RubyTypes classes.
  // To use it add the following code to your Init_ function when loading a 
  // Rice extension:
  //
  //    static Rice::Init init;
  class Init
  {
  public:
    Init();
  };
}
#endif // Rice__Init__hpp_