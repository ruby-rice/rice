#ifndef Rice__Function__hpp_
#define Rice__Function__hpp_

namespace Rice
{
  class Function
  {
  public:
    Function& setNoGvl();
    bool isNoGvl() const;

  private:
    bool isNoGvl_ = false;
  };
} // Rice

#endif // Rice__Function__hpp_
