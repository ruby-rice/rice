#include <iostream>
#include <stdexcept>
#include <sstream>
#include <typeindex>


namespace Rice::detail
{
  template <typename T>
  inline void TypeRegistry::add(VALUE klass, rb_data_type_t* rbType)
  {
    std::type_index key(typeid(T));
    registry_[key] = std::pair(klass, rbType);
  }

  /* Special case void. Rice defines classes using the class name not a pointer to the
     class. Thus define_class<void> is more consistent with Rice then 
     define_class<void*>. However, the types of void and void* are different so we need
     this special case.
     
     It is possible to support define_class<void*>, but it requires changing the static
     assertions on Data_Type and Data_Object and thus seems less desirable (and less 
     consistent as mentioned above).*/
  template <>
  inline void TypeRegistry::add<void>(VALUE klass, rb_data_type_t* rbType)
  {
    // The special case, use void*
    std::type_index key(typeid(void*));
    registry_[key] = std::pair(klass, rbType);
  }

  template <typename T>
  inline void TypeRegistry::remove()
  {
    std::type_index key(typeid(T));
    registry_.erase(key);
  }

  template <typename T>
  inline bool TypeRegistry::isDefined()
  {
    std::type_index key(typeid(T));
    auto iter = registry_.find(key);
    return iter != registry_.end();
  }

  template <typename T>
  std::pair<VALUE, rb_data_type_t*> TypeRegistry::getType()
  {
    std::type_index key(typeid(T));
    auto iter = registry_.find(key);
    if (iter != registry_.end())
    {
      return iter->second;
    }
    else
    {
      this->raiseUnverifiedType(typeid(T).name());
      // Make compiler happy
      return std::make_pair(Qnil, nullptr);
    }
  }

  // Special case void. See comment for add above.
  template <>
  inline bool TypeRegistry::isDefined<void>()
  {
    std::type_index key(typeid(void*));
    auto iter = registry_.find(key);
    return iter != registry_.end();
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
      const std::type_info& typeInfo = typeid(T);
      std::type_index key(typeInfo);
      this->unverified_.insert(key);
      return false;
    }
  }

  inline std::optional<std::pair<VALUE, rb_data_type_t*>> TypeRegistry::lookup(const std::type_info& typeInfo)
  {
    std::type_index key(typeInfo);
    auto iter = registry_.find(key);

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
    // First check and see if the actual type of the object is registered
    std::optional<std::pair<VALUE, rb_data_type_t*>> result = lookup(typeid(object));

    if (result)
    {
      return result.value();
    }

    // If not, then we are willing to accept an ancestor class specified by T. This is needed
    // to support Directors. Classes inherited from Directors are never actually registered
    // with Rice - and what we really want it to return the C++ class they inherit from.
    const std::type_info& typeInfo = typeid(T);
    result = lookup(typeInfo);
    if (result)
    {
      return result.value();
    }

    detail::TypeMapper<T> typeMapper;
    raiseUnverifiedType(typeMapper.name());
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
      detail::TypeMapper<int> typeMapper;
      stream << "  " << typeMapper.name(typeIndex) << "\n";
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