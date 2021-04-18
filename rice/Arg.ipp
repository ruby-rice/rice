namespace Rice
{
  inline Arg::Arg(std::string name) : name(name)
  {
  }

  template<typename Arg_Type>
  inline Arg& Arg::operator=(Arg_Type val)
  {
    this->defaultValue_ = val;
    return *this;
  }

  //! Check if this Arg has a default value associated with it
  inline bool Arg::hasDefaultValue() const
  {
    return this->defaultValue_.has_value();
  }

  //! Return a reference to the default value associated with this Arg
  /*! \return the type saved to this Arg
    */
  template<typename Arg_Type>
  inline Arg_Type& Arg::defaultValue()
  {
    return std::any_cast<Arg_Type&>(this->defaultValue_);
  }

  inline Arg& Arg::keepAlive()
  {
    this->isKeepAlive = true;
    return *this;
  }

  inline Arg& Arg::isValue()
  {
    isValue_ = true;
    return *this;
  }

  inline bool Arg::getIsValue()
  {
    return isValue_;
  }
} // Rice
