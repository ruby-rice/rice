#include <sstream>
#include "from_ruby_defn.hpp"

namespace Rice
{
  template <typename...Arg_Ts>
  inline MethodInfo::MethodInfo(size_t argCount, const Arg_Ts&...args)
  {
    // Process the passed in arguments
    (this->processArg(args), ...);

    // Fill in any missing arguments
    for (int i = sizeof...(Arg_Ts); i < argCount; i++)
    {
      Arg arg("arg_" + std::to_string(i));
      this->args_.emplace_back(arg);
    }

    // TODO - so hacky but update the Arg positions
    for (uint32_t i = 0; i < this->args_.size(); i++)
    {
      this->args_[i].position = i;
    }
  }

  template <typename Arg_T>
  inline void MethodInfo::processArg(const Arg_T& arg)
  {
    if constexpr (std::is_same_v<Arg_T, Arg>)
    {
      this->addArg(arg);
    }
    else
    {
      this->returnInfo = arg;
    }
  }

  inline void MethodInfo::addArg(const Arg& arg)
  {
    this->args_.push_back(arg);
  }

  inline std::string MethodInfo::formatString()
  {
    size_t required = 0;
    size_t optional = 0;

    for (const Arg& arg : this->args_)
    {
      if (arg.hasDefaultValue())
      {
        optional++;
      }
      else
      {
        required++;
      }
    }

    return std::to_string(required) + std::to_string(optional);
  }

  template<typename Arg_T>
  inline Arg_T& MethodInfo::defaultValue(size_t pos)
  {
    return args_[pos].defaultValue<Arg_T>();
  }

  inline Arg& MethodInfo::arg(size_t pos)
  {
    return args_[pos];
  }

  inline bool MethodInfo::isOwner()
  {
    return this->returnInfo.isOwner();
  }

  inline void MethodInfo::takeOwnership()
  {
    this->returnInfo.takeOwnership();
  }

  inline std::vector<Arg>::iterator MethodInfo::begin()
  {
    return this->args_.begin();
  }

  inline std::vector<Arg>::iterator MethodInfo::end()
  {
    return this->args_.end();
  }
}