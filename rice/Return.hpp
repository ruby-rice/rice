#ifndef Rice__Return__hpp_
#define Rice__Return__hpp_

namespace Rice
{
  //! Helper for defining Return argument of a method

  class Return: public Arg
  {
  public:
    Return();
    Return& keepAlive() override;
    Return& setValue() override;
    Return& setOpaque() override;
    Return& takeOwnership() override;
    Return& setArray();
    bool isArray();
  private:
    bool isArray_ = false;
  };
} // Rice

#endif // Rice__Return__hpp_
