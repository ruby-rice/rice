module Rice
	class NativeRegistry
		def native_attributes(klass)
			self.native_by_kind(klass, [Rice::NativeKind::AttributeReader, Rice::NativeKind::AttributeWriter])
		end

		def native_methods(klass)
			self.native_by_kind(klass, [Rice::NativeKind::Method])
		end

		def native_functions(klass)
			self.native_by_kind(klass, [Rice::NativeKind::Function])
		end

		def native_by_kind(klass, kinds)
			self.lookup(klass).find_all do |native_method|
				kinds.include?(native_method.kind)
			end
		end
	end
end