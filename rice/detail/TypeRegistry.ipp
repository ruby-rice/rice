#include <iostream>
#include <stdexcept>
#include <sstream>
#include <typeindex>


namespace Rice::detail
{
  template <typename T>
  inline std::type_index TypeRegistry::key()
  {
    if constexpr (is_complete_v<T>)
    {
      return std::type_index(typeid(T));
    }
    else
    {
      return std::type_index(typeid(T*));
    }
  }

  template <typename T>
  inline void TypeRegistry::add(VALUE klass, rb_data_type_t* rbType)
  {
    registry_[key<T>()] = std::pair(klass, rbType);
  }

  template <typename T>
  inline void TypeRegistry::remove()
  {
    registry_.erase(key<T>());
  }

  template <typename T>
  inline bool TypeRegistry::isDefined()
  {
    auto iter = registry_.find(key<T>());
    return iter != registry_.end();
  }

  template <typename T>
  std::pair<VALUE, rb_data_type_t*> TypeRegistry::getType()
  {
    auto iter = registry_.find(key<T>());
    if (iter != registry_.end())
    {
      return iter->second;
    }
    else
    {
      this->raiseUnverifiedType(TypeIndexParser::name<T>());
      // Make compiler happy
      return std::make_pair(Qnil, nullptr);
    }
  }

  template <typename T>
  inline bool TypeRegistry::verify()
  {
    if (isDefined<T>())
    {
      return true;
    }
    else
    {
      this->unverified_.insert(key<T>());
      return false;
    }
  }

  inline std::optional<std::pair<VALUE, rb_data_type_t*>> TypeRegistry::lookup(std::type_index typeIndex)
  {
    auto iter = registry_.find(typeIndex);

    if (iter == registry_.end())
    {
      return std::nullopt;
    }
    else
    {
      return iter->second;
    }
  }

  template <typename T>
  inline std::pair<VALUE, rb_data_type_t*> TypeRegistry::figureType(const T& object)
  {
    std::optional<std::pair<VALUE, rb_data_type_t*>> result;

    // First check and see if the actual type of the object is registered.
    // This requires a complete type for typeid to work.
    if constexpr (is_complete_v<T>)
    {
      result = lookup(std::type_index(typeid(object)));

      if (result)
      {
        return result.value();
      }
    }

    // If not, then we are willing to accept an ancestor class specified by T. This is needed
    // to support Directors. Classes inherited from Directors are never actually registered
    // with Rice - and what we really want it to return the C++ class they inherit from.
    result = lookup(key<T>());
    if (result)
    {
      return result.value();
    }

    raiseUnverifiedType(TypeIndexParser::name<T>());

    // Make the compiler happy
    return std::pair<VALUE, rb_data_type_t*>(Qnil, nullptr);
  }

  inline void TypeRegistry::validateTypes()
  {
    // Loop over the unverified types and delete each on that is found in the registry
    // the registry and raise an exception for the first one that is not
    for (auto iter = this->unverified_.begin(); iter != this->unverified_.end(); )
    {
      const std::type_index& typeIndex = *iter;
      bool isDefined = this->registry_.find(typeIndex) != this->registry_.end();
      if (isDefined)
      {
        iter = this->unverified_.erase(iter);
      }
      else
      {
        iter++;
      }
    }

    if (this->unverified_.empty())
    {
      return;
    }

    std::stringstream stream;
    stream << "The following types are not registered with Rice:" << "\n";

    for (const std::type_index& typeIndex : this->unverified_)
    {
      detail::TypeIndexParser typeIndexParser(typeIndex);
      stream << "  " << typeIndexParser.name() << "\n";
    }

    throw std::invalid_argument(stream.str());
  }

  inline void TypeRegistry::clearUnverifiedTypes()
  {
    this->unverified_.clear();
  }

  inline void TypeRegistry::raiseUnverifiedType(const std::string& typeName)
  {
    std::string message = "Type is not registered with Rice: " + typeName;
    throw std::invalid_argument(message);
  }

  inline VALUE TypeRegistry::klasses()
  {
    Array result;

    for (auto& pair : this->registry_)
    {
      std::pair<VALUE, rb_data_type_t*>& value = pair.second;
      Class klass = value.first;
      result.push(klass, false);
    }
    return result;
  }
}