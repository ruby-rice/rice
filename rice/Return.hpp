#ifndef Rice__Return__hpp_
#define Rice__Return__hpp_

RICE_BEGIN_NAMESPACE
  //! Helper for defining Return argument of a method

  class Return: public Arg
  {
  public:
    Return();
    Return& keepAlive() override;
    Return& setValue() override;
    Return& setOpaque() override;
    Return& takeOwnership() override;
  };

  class ReturnBuffer : public Return
  {
  };
RICE_END_NAMESPACE

#endif // Rice__Return__hpp_
