require 'open-uri'
require 'libxml-ruby'

module Rice
	module Doc
		class CppReference
			ROOT = "https://en.cppreference.com/w"
			INDEX = "https://raw.githubusercontent.com/p12tic/cppreference-doc/refs/heads/master/index-functions-cpp.xml"
			INDEX = "c:/Users/cfis/Downloads/index-functions-cpp.xml"

			OPERATORS = {"assign" => "operator=",
									 "assign_plus" => "operator+=",
									 "assign_minus" => "operator-=",
									 "assign_multiply" => "operator*=",
									 "assign_divide" => "operator/=",
									 "call" => "operator()",
									 "decrement" => "operator--",
									 "dereference" => "operator*",
									 "increment" => "operator++",
									 "==" => "operator==",
									 "!=" => "operator!=",
									 "+" => "operator+",
									 "-" => "operator-",
									 "*" => "operator*",
									 "/" => "operator/",
									 "&" => "operator&",
									 "|" => "operator|",
									 "<" => "operator<",
									 ">" => "operator>",
									 "<=" => "operator<=",
									 "<<" => "operator<<",
									 "[]" => "operator[]",
									 "[]=" => "operator[]"}

			def initialize
				@type_mappings = {/^enum\s*/i => '',
													/^union\s*/i => '',
													/<.*>/ => ''}

				@method_mappings = {'std::exception' => {'message' => 'what'},
														'std::map' => {'delete' => 'erase',
 																					 'include?' => 'find'},
														'std::multimap' => {'delete' => 'erase',
																								'include?' => 'find'},
														'std::runtime_error' => {'message' => 'what'},
														'std::unorderedmap' => {'delete' => 'erase',
																					 					'include?' => 'find'},
														'std::vector' => {'delete_at' => 'erase',
																							'first' => 'front',
																							'last' => 'back',
																							'pop' => 'pop_back',
																							'push' => 'push_back'}}

														@cache = Hash.new
				data = URI.open(INDEX).read
				parser = LibXML::XML::Parser.string(data)
				@doc = parser.parse
			end

			def camelize(content, capitalize = true)
				result = content.gsub(/(?:_)(.)/) do |matched|
					$1.capitalize
				end

				result[0] = capitalize ? result[0].upcase : result[0].downcase
				result
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

			def class_base(klass)
				node = class_node(klass)
				if node
					path = node.attributes["link"]
					name = cpp_name(klass)
					parts = name.split("::")
					"#{ROOT}/#{path}"
				end
			end

			def class_url(klass)
				node = class_node(klass)
				if node
					"#{class_base(klass)}.html"
				end
			end

			def module_url(klass)
				a = 1
			end

			def method_url(klass, native)
				node = class_node(klass)
				if node
					ruby_name = native.name
					method_names = Array.new
					if ruby_name == "initialize" || ruby_name == "initialize_copy"
						return "#{class_base(klass)}.html"
					elsif native_name = OPERATORS[ruby_name]
						method_names << native_name
					elsif native_name = @method_mappings.dig(cpp_name(klass), ruby_name)
						method_names << native_name
					elsif ruby_name.match(/\?$/)
						native_name = ruby_name.gsub(/\?$/, "")
						method_names << camelize(native_name, false)
						method_names << camelize(native_name, true)
						method_names << "is#{camelize(native_name, true)}"
					else
						method_names << ruby_name
						method_names << camelize(ruby_name, false)
						method_names << camelize(ruby_name, true)
					end

					pattern = method_names.map do |method_name|
						"@name='#{method_name}'"
					end.join(" or ")

					xpath = "function[#{pattern}]"
					member_node = node&.find_first(xpath)
					if member_node && member_node.attributes["link"]
  					"#{class_base(klass)}/#{member_node.attributes['link']}.html"
					elsif member_node
						"#{class_base(klass)}/#{member_node.attributes['name']}.html"
					end
				end
			end

			def attribute_url(klass, native)
				node = class_node(klass)

				# Find the member node
				attribute_name = camelize(native.name, false)

				xpath = "variable[@name='#{attribute_name}']"
				member_node = node&.find_first(xpath)
				link = member_node.attributes["link"]
				if member_node && link && link != "."
					"#{class_base(klass)}/#{member_node.attributes['link']}.html"
  			elsif member_node && link && link == "."
	  			"#{class_base(klass)}.html"
				elsif member_node
					"#{class_base(klass)}/#{member_node.attributes['name']}.html"
				end
			end
		end
	end
end