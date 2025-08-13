module Rice
	class NativeRegistry
		def native_attributes(klass)
			self.native_by_kind(klass, [Rice::NativeKind::AttributeReader, Rice::NativeKind::AttributeWriter])
		end

		def native_methods(klass)
			self.native_by_kind(klass, [Rice::NativeKind::Method])
		end

		def native_singleton_functions(klass)
			self.native_by_kind(klass.singleton_class, [Rice::NativeKind::Function])
		end

		def native_by_kind(klass, kinds)
			self.lookup(klass).find_all do |native|
				kinds.include?(native.kind)
			end
		end
	end
end