
RICE_DETAIL_BEGIN_NAMESPACE
  inline void NativeRegistry::add(VALUE klass, ID methodId, std::unique_ptr<Native>& native)
  {
    if (rb_type(klass) == T_ICLASS)
    {
      klass = detail::protect(rb_class_of, klass);
    }

    // Create the key
    std::pair<VALUE, ID> key = std::make_pair(klass, methodId);

    // Lookup items for method
    std::vector<std::unique_ptr<Native>>& natives = this->natives_[key];

    natives.push_back(std::move(native));
  }

  inline void NativeRegistry::reset(VALUE klass)
  {
    for (auto iter = this->natives_.begin(); iter != this->natives_.end();)
    {
      // Iter points to a std::pair<std::pair<VALUE, ID>, std::vector<NativeRegistryItem>
      if (iter->first.first == klass)
      {
        iter = this->natives_.erase(iter);
      }
      else
      {
        ++iter;
      }
    }
  }
  
  inline const std::vector<Native*> NativeRegistry::lookup(VALUE klass)
  {
    std::vector<Native*> result;

    if (rb_type(klass) == T_ICLASS)
    {
      klass = detail::protect(rb_class_of, klass);
    }

    for (auto& pair : this->natives_)
    {
      const std::pair<VALUE, ID>& key = pair.first;

      if (klass == key.first)
      {
        const std::vector<std::unique_ptr<Native>>& value = pair.second;
        for (auto& native : value)
        {
          result.push_back(native.get());
        }
      }
    }

    return result;
  }

  inline const std::vector<std::unique_ptr<Native>>& NativeRegistry::lookup(VALUE klass, ID methodId)
  {
    if (rb_type(klass) == T_ICLASS)
    {
      klass = detail::protect(rb_class_of, klass);
    }

    // Create the key
    std::pair<VALUE, ID> key = std::make_pair(klass, methodId);

    // Lookup items for method
    return this->natives_[key];
  }
RICE_DETAIL_END_NAMESPACE
