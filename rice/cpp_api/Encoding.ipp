namespace Rice
{
  inline Encoding Encoding::utf8()
  {
    return Encoding(rb_utf8_encoding());
  }

  inline Encoding::Encoding(rb_encoding* encoding) : encoding_(encoding)
  {
  }
}
