#include <sstream>

namespace Rice
{
  template <typename...Arg_Ts>
  inline MethodInfo::MethodInfo(size_t argCount, const Arg_Ts&...args)
  {
    // Process the passed in arguments
    (this->processArg(args), ...);

    // Fill in any missing arguments
    for (size_t i = this->args_.size(); i < argCount; i++)
    {
      Arg arg("arg_" + std::to_string(i));
      this->addArg(arg);
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
    if constexpr (std::is_same_v<Arg_T, Return>)
    {
      this->returnInfo_ = arg;
    }
    else if constexpr (std::is_same_v<Arg_T, Function>)
    {
      this->function_ = arg;
    }
    else if constexpr (std::is_same_v<Arg_T, Arg>)
    {
      this->addArg(arg);
    }
    else
    {
      static_assert(true, "Unknown argument type");
    }
  }

  inline void MethodInfo::addArg(const Arg& arg)
  {
    this->args_.push_back(arg);
  }

  inline int MethodInfo::argCount()
  {
    return this->args_.size();
  }

  inline Arg* MethodInfo::arg(size_t pos)
  {
    if (pos < this->args_.size())
    {
      return &this->args_[pos];
    }
    else
    {
      return nullptr;
    }
  }

  inline Arg* MethodInfo::arg(std::string name)
  {
    for (Arg& arg : this->args_)
    {
      if (arg.name == name)
      {
        return &arg;
      }
    }
    return nullptr;
  }

  inline Return* MethodInfo::returnInfo()
  {
    return &this->returnInfo_;
  }
  
  inline Function* MethodInfo::function()
  {
    return &this->function_;
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
