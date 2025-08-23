require 'open-uri'
require 'libxml-ruby'

module Rice
	module Doc
		class CppReference
			ROOT = "https://en.cppreference.com/w"
			INDEX = "https://raw.githubusercontent.com/p12tic/cppreference-doc/refs/heads/master/index-functions-cpp.xml"

			def initialize
				data = URI.open(INDEX).read
				parser = LibXML::XML::Parser.string(data)
				@doc = parser.parse
				@cache = Hash.new
			end

			def cpp_name(klass)
				case klass.cpp_class
					when /^enum/i
						klass.cpp_class.gsub(/enum\s+/i, '')
					when /^union/i
						klass.cpp_class.gsub(/union\s+/i, '')
					when /<.*>/
						pos = klass.cpp_class.index('<')
						klass.cpp_class[0..pos-1]
					else
						klass.cpp_class
				end
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