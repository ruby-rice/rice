require 'yaml'

module Rice
	module Doc
		class Config
			attr_reader :extension, :output, :resolvers

			def initialize(path)
				@config = YAML.load_file(path)
				@extension = @config['extension']
				@output = @config['output']
				@resolvers = build_resolvers(@config['namespaces'] || {})
			end

			private

			def build_resolvers(namespace_configs)
				# Always include Ruby resolver for core types
				result = { nil => Ruby.new }

				namespace_configs.each do |namespace, config|
					key = namespace == 'default' ? nil : namespace
					result[key] = build_resolver(config)
				end
				result
			end

			def build_resolver(config)
				resolver = config['resolver']
				case resolver
				when 'cppreference'
					build_cppreference_resolver(config)
				when 'doxygen'
					build_doxygen_resolver(config)
				when 'rice'
					build_rice_resolver(config)
				else
					raise "Unknown resolver: #{resolver}"
				end
			end

			def build_cppreference_resolver(config)
				CppReference.new
			end

			def build_doxygen_resolver(config)
				root = config['root']
				tagfile = config['tagfile']
				type_mappings = build_type_mappings(config['type_mappings'] || [])
				method_mappings = config['method_mappings'] || {}
				Doxygen.new(root, tagfile, type_mappings, method_mappings)
			end

			def build_rice_resolver(config)
				Rice.new
			end

			def build_type_mappings(mappings)
				result = {}
				mappings.each do |mapping|
					pattern = Regexp.new(mapping['pattern'], Regexp::IGNORECASE)
					replacement = mapping['replacement'] || ''
					result[pattern] = replacement
				end
				result
			end
		end
	end
end
