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
      this->returnInfo = arg;
    }
    else
    {
      this->addArg(arg);
    }
  }

  inline void MethodInfo::addArg(const Arg& arg)
  {
    this->args_.push_back(arg);
  }

  inline int MethodInfo::requiredArgCount()
  {
    int result = 0;

    for (const Arg& arg : this->args_)
    {
      if (!arg.hasDefaultValue())
      {
        result++;
      }
    }

    return result;
  }

  inline int MethodInfo::optionalArgCount()
  {
    int result = 0;

    for (const Arg& arg : this->args_)
    {
      if (arg.hasDefaultValue())
      {
        result++;
      }
    }

    return result;
  }

  inline void MethodInfo::verifyArgCount(int argc)
  {
    int requiredArgCount = this->requiredArgCount();
    int optionalArgCount = this->optionalArgCount();

    if (argc < requiredArgCount || argc > requiredArgCount + optionalArgCount)
    {
      std::string message;

      if (optionalArgCount > 0)
      {
        message = "wrong number of arguments (given " +
          std::to_string(argc) + ", expected " +
          std::to_string(requiredArgCount) + ".." + std::to_string(requiredArgCount + optionalArgCount) + ")";
      }
      else
      {
        message = "wrong number of arguments (given " +
          std::to_string(argc) + ", expected " + std::to_string(requiredArgCount) + ")";
      }
      throw std::invalid_argument(message);
    }
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

  inline std::vector<Arg>::iterator MethodInfo::begin()
  {
    return this->args_.begin();
  }

  inline std::vector<Arg>::iterator MethodInfo::end()
  {
    return this->args_.end();
  }
}
