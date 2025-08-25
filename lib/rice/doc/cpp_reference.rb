require 'open-uri'
require 'libxml-ruby'

module Rice
	module Doc
		class CppReference
			ROOT = "https://en.cppreference.com/w"
			INDEX = "https://raw.githubusercontent.com/p12tic/cppreference-doc/refs/heads/master/index-functions-cpp.xml"
			INDEX = "c:/Users/cfis/Downloads/index-functions-cpp.xml"

			def initialize(type_mappings = Hash.new, method_mappings = Hash.new)
				@type_mappings = type_mappings
				@method_mappings = method_mappings
				@cache = Hash.new
				data = URI.open(INDEX).read
				parser = LibXML::XML::Parser.string(data)
				@doc = parser.parse
			end

			def cpp_name(klass)
				result = klass.cpp_class
				@type_mappings.each do |key, value|
					result = result.gsub(key, value)
				end
				result
			end

			def get_node(klass, xpath)
				node = @cache[klass]
				if node.nil? && !@cache.has_key?(klass)
					node = @cache[klass] = @doc.find_first(xpath)
				end
				node
			end

			def class_node(klass)
				xpath = "//class[@name='#{cpp_name(klass)}']"
				get_node(klass, xpath)
			end

			def class_url(klass)
				node = class_node(klass)
				if node
					path = node.attributes["link"]
					"#{ROOT}/#{path}"
				end
			end

			def module_url(klass)
			end

			def method_url(klass, native)
			end
		end
	end
end