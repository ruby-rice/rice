module Rice
  class NativeRegistry
    def native_attributes(klass)
      self.lookup_by_kind(klass, Rice::NativeKind::AttributeReader) +
        self.lookup_by_kind(klass, Rice::NativeKind::AttributeWriter)
    end

    def native_methods(klass)
      self.lookup_by_kind(klass, Rice::NativeKind::Method)
    end

    def native_functions(klass)
      self.lookup_by_kind(klass, Rice::NativeKind::Function)
    end
  end
end