namespace Rice
{
  inline Function& Function::setNoGvl()
  {
    this->isNoGvl_ = true;
    return *this;
  }

  inline bool Function::isNoGvl() const
  {
    return this->isNoGvl_;
  }
}  // Rice
