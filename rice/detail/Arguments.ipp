#include <sstream>
#include "from_ruby_defn.hpp"

namespace Rice
{
  template <typename...Arg_Ts>
  inline Arguments::Arguments(const Arg_Ts...args)
  {
    (this->processArg(args), ...);
    // TODO - so hacky but update the Arg positions
    for (uint32_t i = 0; i < this->args_.size(); i++)
    {
      this->args_[i].position = i;
    }
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

  template <typename Arg_T>
  inline void Arguments::processArg(const Arg_T& arg)
  {
    if constexpr (std::is_same_v<Arg_T, Arg>)
    {
      this->add(arg);
    }
    else
    {
      this->return_ = arg;
    }
  }

  inline void Arguments::add(const Arg& arg)
  {
    this->args_.push_back(arg);

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

  inline bool Arguments::isOwner()
  {
    return this->return_.isOwner();
  }

  inline void Arguments::takeOwnership()
  {
    this->return_.takeOwnership();
  }

  inline std::vector<Arg>::iterator Arguments::begin()
  {
    return this->args_.begin();
  }

  inline std::vector<Arg>::iterator Arguments::end()
  {
    return this->args_.end();
  }
}