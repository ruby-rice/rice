module Rice
	module Doc
		class Ruby
			ROOT = "https://docs.ruby-lang.org/en/master"

			def initialize
			end

			def class_url(klass)
				"#{ROOT}/#{klass.name.split('::').last.capitalize}.html"
			end

			def module_url(klass)
			end

			def union_url(klass)
			end

			def enum_url(klass)
			end

			def enum_value_url(klass, enum_value)
			end

			def singleton_method_url(klass, native)
			end

			def method_url(klass, native)
			end

			def attribute_url(klass, native)
			end
		end
	end
end
