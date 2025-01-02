namespace Rice
{
  inline Identifier::Identifier(ID id) : id_(id)
  {
  }

  inline Identifier::Identifier(char const* name, Encoding encoding)
  {
    this->id_ = detail::protect(rb_intern3, name, (long)strlen(name), encoding);
  }

  inline Identifier::Identifier(const std::string& name, Encoding encoding)
  {
    this->id_ = detail::protect(rb_intern3, name.c_str(), (long)name.size(), encoding);
  }

  inline char const* Identifier::c_str() const
  {
    return detail::protect(rb_id2name, id_);
  }

  inline std::string Identifier::str() const
  {
    return c_str();
  }

  inline VALUE Identifier::to_sym() const
  {
    return ID2SYM(id_);
  }
}