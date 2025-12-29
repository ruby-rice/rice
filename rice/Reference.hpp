#ifndef Rice__Reference__hpp_
#define Rice__Reference__hpp_

namespace Rice
{
  template<typename T>
  class Reference
  {
    static_assert(!detail::is_wrapped_v<detail::intrinsic_type<T>>,
      "Reference can only be used with fundamental types");

  public:
    Reference();
    Reference(T& data);
    Reference(VALUE value);
    T& get();

  private:
    T data_;
  };

  // Specialization needed when VALUE type matches T, causing constructor ambiguity
  // between Reference(T&) and Reference(VALUE). VALUE is unsigned long when
  // SIZEOF_LONG == SIZEOF_VOIDP (Linux/macOS) and unsigned long long when
  // SIZEOF_LONG_LONG == SIZEOF_VOIDP (Windows x64).
#if SIZEOF_LONG == SIZEOF_VOIDP
  template<>
  class Reference<unsigned long>
  {
  public:
    Reference();
    Reference(unsigned long value, bool isValue = true);
    unsigned long& get();

  private:
    unsigned long data_;
  };
#else
  template<>
  class Reference<unsigned long long>
  {
  public:
    Reference();
    Reference(unsigned long long value, bool isValue = true);
    unsigned long long& get();

  private:
    unsigned long long data_;
  };
#endif

  template<typename T>
  Data_Type<Reference<T>> define_reference(std::string klassName = "");
}

#endif // Rice__Reference__hpp_
