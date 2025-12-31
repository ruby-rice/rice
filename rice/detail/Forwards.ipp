namespace Rice::detail
{
  inline void define_forwarding(VALUE wrapper_klass, VALUE wrapped_klass)
  {
    protect(rb_require, "forwardable");
    Object forwardable = Object(rb_cObject).const_get("Forwardable");
    Object(wrapper_klass).extend(forwardable.value());

    // Get wrapper class's method names to avoid conflicts
    std::set<std::string> wrapperMethodSet;
    std::vector<std::string> wrapperMethods = Registries::instance.natives.lookup(wrapper_klass, NativeKind::Method);
    wrapperMethodSet.insert(wrapperMethods.begin(), wrapperMethods.end());
    std::vector<std::string> wrapperReaders = Registries::instance.natives.lookup(wrapper_klass, NativeKind::AttributeReader);
    wrapperMethodSet.insert(wrapperReaders.begin(), wrapperReaders.end());
    std::vector<std::string> wrapperWriters = Registries::instance.natives.lookup(wrapper_klass, NativeKind::AttributeWriter);
    for (const std::string& writer : wrapperWriters)
    {
      wrapperMethodSet.insert(writer + "=");
    }

    // Get wrapped class's method names from the registry, including ancestor classes
    std::set<std::string> wrappedMethodSet;
    Class klass(wrapped_klass);
    while (klass.value() != rb_cObject && klass.value() != Qnil)
    {
      std::vector<std::string> methods = Registries::instance.natives.lookup(klass.value(), NativeKind::Method);
      wrappedMethodSet.insert(methods.begin(), methods.end());

      std::vector<std::string> readers = Registries::instance.natives.lookup(klass.value(), NativeKind::AttributeReader);
      wrappedMethodSet.insert(readers.begin(), readers.end());

      std::vector<std::string> writers = Registries::instance.natives.lookup(klass.value(), NativeKind::AttributeWriter);
      for (const std::string& writer : writers)
      {
        wrappedMethodSet.insert(writer + "=");
      }

      klass = klass.superclass();
    }

    // Build the arguments array for def_delegators: [:get, :method1, :method2, ...]
    // Skip methods that are already defined on the wrapper class
    Array args;
    args.push(Symbol("get"));
    for (const std::string& method : wrappedMethodSet)
    {
      if (wrapperMethodSet.find(method) == wrapperMethodSet.end())
      {
        args.push(Symbol(method));
      }
    }

    // Call def_delegators(*args)
    Object(wrapper_klass).vcall("def_delegators", args);
  }
}
