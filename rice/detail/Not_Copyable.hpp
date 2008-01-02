#ifndef Rice__Not_Copyable__hpp_
#define Rice__Not_Copyable__hpp_

namespace Rice
{

namespace detail
{

// Inherit from Not_Copyable to prevent copying instances of a class.
class Not_Copyable
{
public:
  Not_Copyable() { }

private:
  Not_Copyable(Not_Copyable const &);
  Not_Copyable & operator=(Not_Copyable const &);
};

} // namespace detail

} // namespace Rice

#endif // Rice__Not_Copyable__hpp_
