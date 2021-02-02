#include <sstream>
#include "from_ruby_defn.hpp"

namespace Rice
{

template <typename...Arg_Ts>
inline Arguments::Arguments(Arg_Ts...args)
{
  (this->add(args), ...);
}

inline int Arguments::count()
{
  if (required_ == 0 && optional_ == 0)
  {
    return -1;
  }
  else
  {
    return required_ + optional_;
  }
}

inline std::string Arguments::formatString(size_t fullArgCount)
{
  std::stringstream s;
  if (required_ == 0 && optional_ == 0)
  {
    s << fullArgCount << 0;
  }
  else
  {
    s << required_ << optional_;
  }

  return s.str();
}

inline void Arguments::add(const Arg& arg)
{
  args_.push_back(arg);

  if (arg.hasDefaultValue())
  {
    optional_++;
  }
  else
  {
    required_++;
  }
}

inline bool Arguments::isOptional(unsigned int pos)
{
  if (required_ == 0 && optional_ == 0)
  {
    return false;
  }
  if (pos >= args_.size())
  {
    return false;
  }
  return args_[pos].hasDefaultValue();
}

template<typename Arg_T>
inline Arg_T& Arguments::defaultValue(int pos)
{
  return args_[pos].defaultValue<Arg_T>();
}

} // Rice
